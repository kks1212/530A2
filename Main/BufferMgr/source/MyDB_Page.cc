//
// Created by Shawn Zeng on 2022/1/23.
//

#include "MyDB_Page.h"

//constructor
MyDB_Page::MyDB_Page(MyDB_BufferManager* myManager, MyDB_TablePtr myTable, long offset) {
    this->bytes = nullptr;
    this->myManager = myManager;
    this->pageId = make_pair(myTable, offset);
    this->refCount = 0;
    this->isPin = false;
    this->isDirty = false;
//    this->isAnonymous = false;
}

//access the raw bytes in this page, if not in the buffer, then loaded from the secondary storage
void *MyDB_Page::getBytes() {
    //todo: operate LRU
    if(myManager->myDbLRU->check(pageId)){
        MyDB_PagePtr temp = myManager->myDbLRU->getValue(this->pageId);
    }

    return this->bytes;
}

void MyDB_Page::addRef() {
    this->refCount += 1;
}

void MyDB_Page::reduceRef() {
    this->refCount -= 1;
    if(this->refCount == 0){
        //todo
    }
}
