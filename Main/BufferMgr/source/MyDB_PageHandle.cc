
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include "MyDB_PageHandle.h"

void *MyDB_PageHandleBase :: getBytes () {
    return this->myPage->getBytes();
}

void MyDB_PageHandleBase :: wroteBytes () {
    return this->myPage->wroteBytes();
}

MyDB_PageHandleBase::MyDB_PageHandleBase(MyDB_PagePtr page) {
    this->myPage = page;
    //todo:    this->myPage.addRef;
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
    //todo:    this->myPage.decreaseRef;
}

MyDB_PagePtr MyDB_PageHandleBase::getMyPage() {
    return this->myPage;
}



#endif

