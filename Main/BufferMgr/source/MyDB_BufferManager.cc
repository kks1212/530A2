
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include <string>
#include "MyDB_Page.h"

using namespace std;

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr whichTable, long i) {
    if(whichTable == nullptr){
        exit(1);
    }

    pair<MyDB_TablePtr, long> index = make_pair(whichTable, i);
    if(lookupTable.find(index) == lookupTable.end()){
        //if not found, create a new one, and store in lookuptable
        MyDB_PagePtr newPage = make_shared<MyDB_Page>(this, whichTable, i);
        lookupTable[index] = newPage;
        return make_shared<MyDB_PageHandleBase>(newPage);
    }else{
        // if found, return handle
        return make_shared<MyDB_PageHandleBase>(lookupTable[index]);
    }
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
    //create new page with null table
    pair<MyDB_TablePtr, long> index = make_pair(nullptr, this->anonymousCount);
    MyDB_PagePtr newPage = make_shared<MyDB_Page>(this, nullptr, this->anonymousCount);
    lookupTable[index] = newPage;
    this->anonymousCount += 1;
	return make_shared<MyDB_PageHandleBase>(newPage);
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr whichTable, long i) {
    if(whichTable == nullptr){
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
    }else{
        resPage = lookupTable[index];
    }

    //check if resPage is in LRU;
    if(!myDbLRU->check(index)){
        myDbLRU->put(index, resPage);
    }

    //set to pin page
    if(!resPage->isPin){
        resPage->isPin = true;
        myDbLRU->pinCount += 1;
    }

    //todo: read from file?
    return make_shared<MyDB_PageHandleBase>(resPage);
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
    //create new page with null table
    pair<MyDB_TablePtr, long> index = make_pair(nullptr, this->anonymousCount);
    MyDB_PagePtr newPage = make_shared<MyDB_Page>(this, nullptr, this->anonymousCount);
    lookupTable[index] = newPage;
    this->anonymousCount += 1;

    if(!myDbLRU->check(index)){
        myDbLRU->put(index, newPage);
    }

    if(!newPage->isPin){
        newPage->isPin = true;
        myDbLRU->pinCount += 1;
    }

    return make_shared<MyDB_PageHandleBase>(newPage);
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
    unpinMe->getMyPage()->isPin = false;
}

//constructor
MyDB_BufferManager :: MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile) {
    this->numPages = numPages;
    this->pageSize = pageSize;
    this->tempFile = tempFile;
    this->anonymousCount = 0;

    //create memory
    for(int i=0; i<numPages; i++){
        bufferVector.push_back((void *) malloc(pageSize));
    }

    this->myDbLRU = new MyDB_LRU(numPages, this);
}

MyDB_BufferManager :: ~MyDB_BufferManager () {
}
	
#endif


