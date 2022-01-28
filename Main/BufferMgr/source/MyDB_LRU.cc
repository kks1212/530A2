//
// Created by Shawn Zeng on 2022/1/23.
//

#include "MyDB_BufferManager.h"
#include <iostream>

MyDB_LRU::MyDB_LRU(size_t capacity, MyDB_BufferManager *myManager) {
    this->capacity = capacity;
    this->myManager = myManager;
    //dummy head and tail
    head = new DLinkedNode();
    tail = new DLinkedNode();
    head->next = tail;
    tail->prev = head;
    pinCount = 0;
    size = 0;
}

MyDB_PagePtr MyDB_LRU::get(pair<MyDB_TablePtr, long> key) {
    auto value = cacheMap.find(key);
    // if not in the map, return null
    if(value == cacheMap.end()){
        return nullptr;
    }else{
        //if contains, move to the head of the list
        DLinkedNode* node = cacheMap[key];
        moveToHead(node);
        return node->pagePtr;
    }
}

void MyDB_LRU::put(pair<MyDB_TablePtr, long> key, MyDB_PagePtr value) {
    auto kv = cacheMap.find(key);
//    cout<<"Size:";
//    cout<<size<<endl;
    //new element
    if(kv == cacheMap.end()){
        //if LRU is full, pop last first
        if(size == capacity){
            removeTail();
        }
        //create new item
        size++;
        if(key.first == nullptr) pinCount++;

        DLinkedNode* node = new DLinkedNode(key, value);
        cacheMap[key] = node;
        addToHead(node);
    }else{
        //update the value and move to the front
        cacheMap[key]->pagePtr = value;
        moveToHead(cacheMap[key]);
    }
}

void MyDB_LRU::removeTail() {
    if(pinCount == capacity){
        cout << "pinCount == capacity\n";
        exit(1);
    }
    DLinkedNode* temp = tail->prev;
    //for loop find the first page which is not pinned
    for(int i=0; i<size; i++){
        if(!temp->pagePtr->isPin){
            //write the bytes back to disk if the bytes is dirty
            MyDB_PagePtr page = temp->pagePtr;
            page->writeBack();

            //if ref == 0, also delete from the map
            if(page->refCount == 0){
                myManager->killPage(page);
            }

            //seems duplicate with killpage
            killNode(page->pageId);
            break;
        }
        temp = temp->prev;
    }

}

//move a key from map and list
void MyDB_LRU::killNode(pair<MyDB_TablePtr, long> key) {
    if(cacheMap.find(key) != cacheMap.end()){
        DLinkedNode* deNode = cacheMap[key];
        //recover the buffervector
        myManager->bufferVector.push_back(deNode->pagePtr->bytes);
        deNode->pagePtr->bytes = nullptr;
        cacheMap.erase(key);
        removeNode(deNode);
        delete deNode;
        size--;
    }
}

bool MyDB_LRU::check(pair<MyDB_TablePtr, long> key) {
    return cacheMap.find(key) != cacheMap.end();
}

void MyDB_LRU::moveToHead(DLinkedNode *node) {
    removeNode(node);
    addToHead(node);
}

void MyDB_LRU::addToHead(DLinkedNode *node) {
    node->prev = head;
    node->next = head->next;
    head->next->prev = node;
    head->next = node;
}

void MyDB_LRU::removeNode(DLinkedNode *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

MyDB_LRU::~MyDB_LRU() {
    //clean the memory
    DLinkedNode* node = head;
    while(node != nullptr){
        DLinkedNode* next = node->next;
        delete node;
        node = next;
    }
}

void MyDB_LRU::updatePinCount(pair<MyDB_TablePtr, long> key) {
    if(check(key)){
        pinCount--;
    }
}
