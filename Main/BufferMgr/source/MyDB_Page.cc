//
// Created by Shawn Zeng on 2022/1/23.
//

#include "MyDB_Page.h"
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include "MyDB_BufferManager.h"

//constructor
MyDB_Page::MyDB_Page(MyDB_BufferManager* myManager, MyDB_TablePtr myTable, long offset) {
    this->bytes = nullptr;
    this->myManager = myManager;
    this->pageId = make_pair(myTable, offset);
    this->refCount = 0;
    this->isPin = false;
    this->isDirty = false;
    if(myTable == nullptr){
        this->isAnonymous = true;
    }else{
        this->isAnonymous = false;
    }

}

//access the raw bytes in this page, if not in the buffer, then loaded from the secondary storage
void *MyDB_Page::getBytes() {
    if(this->bytes == nullptr){
        //allocate memory
        myManager->allocateRAM(this);


        //read from disc
        int file_descriptor;
        if(pageId.first == nullptr){
            //todo:O_FSYNC?
            file_descriptor = open(myManager->tempFile.c_str (), O_CREAT | O_RDWR | O_FSYNC, 0666);
        }else{
            file_descriptor = open (pageId.first->getStorageLoc ().c_str (), O_CREAT | O_RDWR | O_FSYNC, 0666);
        }
        lseek (file_descriptor, pageId.second * this->myManager->pageSize, SEEK_SET);
        read (file_descriptor, this->bytes, this->myManager->pageSize);
        close (file_descriptor);
    }
    return this->bytes;
}

void MyDB_Page::addRef() {
    this->refCount += 1;
}

void MyDB_Page::reduceRef() {
    this->refCount -= 1;
    //anonymous page refCount == 0, kill it
    if(this->refCount == 0 && pageId.first == nullptr){
        myManager->killPage(make_shared<MyDB_Page>(myManager, pageId.first, pageId.second));
    }
}

MyDB_Page::~MyDB_Page() {

}

void MyDB_Page::wroteBytes() {
    this->isDirty = true;
}

//write the bytes back to disk if the bytes is dirty
void MyDB_Page::writeBack() {
    if(isDirty){
        int file_descriptor;
        if (pageId.first == nullptr) {
            file_descriptor = open (myManager->tempFile.c_str (), O_CREAT | O_RDWR | O_SYNC, 0666);
        } else {
            file_descriptor = open (pageId.first->getStorageLoc().c_str (), O_CREAT | O_RDWR | O_SYNC, 0666);
        }
        lseek (file_descriptor, pageId.second * this->myManager->pageSize, SEEK_SET);
        write (file_descriptor, bytes, this->myManager->pageSize);
        close (file_descriptor);
        isDirty = false;
    }

}
