
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include <string>
#include <utility>
#include <iostream>
#include <unistd.h>

using namespace std;

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr whichTable, long i) {
    if(whichTable == nullptr){
        cout << "whichTable == nullptr1\n";
        exit(1);
    }

    pair<MyDB_TablePtr, long> index = make_pair(whichTable, i);
    MyDB_PagePtr newPage;
    if(lookupTable.find(index) == lookupTable.end()){
        //if not found, create a new one, and store in lookuptable
        newPage = make_shared<MyDB_Page>(this, whichTable, i);
        lookupTable[index] = newPage;

        //put in LRU, LRU handle the situation when no more places in RAM
        myDbLRU->put(index, newPage);

        //allocate spaces
        allocateRAM(newPage);

        return make_shared<MyDB_PageHandleBase>(newPage);
    }else{
        // if found, return handle, through LRU
        if(myDbLRU->check(index)){
            return make_shared<MyDB_PageHandleBase>(myDbLRU->get(index));
        }else{
            newPage = lookupTable[index];
            myDbLRU->put(index, newPage);
            allocateRAM(newPage);
            return make_shared<MyDB_PageHandleBase>(newPage);
        }
    }
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
    //create new page with null table
    pair<MyDB_TablePtr, long> index = make_pair(nullptr, this->anonymousCount);
    MyDB_PagePtr newPage = make_shared<MyDB_Page>(this, nullptr, this->anonymousCount);
    lookupTable[index] = newPage;
    myDbLRU->put(index, newPage);
    allocateRAM(newPage);
    this->anonymousCount += 1;
	return make_shared<MyDB_PageHandleBase>(newPage);
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr whichTable, long i) {
    if(whichTable == nullptr){
        cout << "whichTable == nullptr2\n";
        exit(1);
    }
    //the page we need to return
    MyDB_PagePtr resPage;
    pair<MyDB_TablePtr, long> index = make_pair(whichTable, i);
    //search in the lookuptable
    if(lookupTable.find(index) == lookupTable.end()){
        //if not found, create a new one, and store in lookuptable
        resPage = make_shared<MyDB_Page>(this, whichTable, i);
        lookupTable[index] = resPage;
        myDbLRU->put(index, resPage);
        allocateRAM(resPage);
    }else{
        if(myDbLRU->check(index)){
            resPage = myDbLRU->get(index);
        }else{
            resPage = lookupTable[index];
            myDbLRU->put(index, resPage);
            allocateRAM(resPage);
        }
    }

    //set to pin page
    if(!resPage->isPin){
        resPage->isPin = true;
        myDbLRU->pinCount += 1;
    }
    return make_shared<MyDB_PageHandleBase>(resPage);
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
    MyDB_PageHandle res = getPage();
    res->getMyPage()->isPin = true;
    myDbLRU->pinCount += 1;
    return res;
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
    unpinMe->getMyPage()->isPin = false;
    myDbLRU->updatePinCount(unpinMe->getMyPage()->pageId);
}

//constructor
MyDB_BufferManager :: MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile) {
    this->numPages = numPages;
    this->pageSize = pageSize;
    this->tempFile = tempFile;
    this->anonymousCount = 1;

    //create memory
    for(int i=0; i<numPages; i++){
        bufferVector.push_back((void *) malloc(pageSize));
    }

    this->myDbLRU = new MyDB_LRU(numPages, this);
}

//getPage already check there is enough place
void MyDB_BufferManager::allocateRAM(MyDB_PagePtr page) {
    if(bufferVector.empty()){
        cout << "bufferVector.empty(\n";
        exit(1);
    }
    page->bytes = bufferVector[bufferVector.size() - 1];
    bufferVector.pop_back();

    //read ahead
    page->readFromDisk();
}

void MyDB_BufferManager::killPage(MyDB_PagePtr page) {
    pair<MyDB_TablePtr, long> index = page->pageId;
    //remove in lookuptable
    if(lookupTable.find(index) != lookupTable.end()){
        lookupTable.erase(index);
    }
    //remove in LRU
    myDbLRU->killNode(page->pageId);
}

MyDB_BufferManager :: ~MyDB_BufferManager () {
    //delete all page in the map
    for(auto pagePair : lookupTable){
        MyDB_PagePtr page = pagePair.second;
        if(page->bytes != nullptr){
            page->writeBack();
        }
        //free memory
        free(page->bytes);
        page->bytes = nullptr;
    }
    for(auto buffer : bufferVector){
        free(buffer);
    }
    unlink(tempFile.c_str ());
}

void MyDB_BufferManager::manage(pair<MyDB_TablePtr, long> pageId) {
    MyDB_PagePtr page = lookupTable[pageId];
    if(!myDbLRU->check(pageId)){
        myDbLRU->put(pageId, page);
        allocateRAM(page);
    }
}

#endif


