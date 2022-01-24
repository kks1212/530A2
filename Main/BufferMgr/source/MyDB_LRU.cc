//
// Created by Shawn Zeng on 2022/1/23.
//

#include "MyDB_LRU.h"

MyDB_LRU::MyDB_LRU(int capacity, MyDB_BufferManager *myManager) {
    this->capacity = capacity;
    this->myManager = myManager;
}

MyDB_PagePtr MyDB_LRU::getValue(pair<MyDB_TablePtr, long> key) {
    auto value = cacheMap.find(key);
    // if not in the map, return null
    if(value == cacheMap.end()){
        return nullptr;
    }else{
        //if contains, move to the head of the list
        cacheList.splice(cacheList.begin(), cacheList, cacheMap[key]);
        cacheMap[key] = cacheList.begin();
        return cacheMap[key]->pagePtr;
    }
}

void MyDB_LRU::put(pair<MyDB_TablePtr, long> key, MyDB_PagePtr value) {
    auto kv = cacheMap.find(key);
    //new element
    if(kv == cacheMap.end()){
        //if LRU is full
        if(cacheList.size() == capacity){
            auto toRemove = cacheList.back();
            //remove from the map, delete the last item in the list
            //todo: 通知manager need to evict a node
            cacheMap.erase(toRemove.id);
            cacheList.pop_back();
        }
        //add to the head of the list
        cacheList.push_front(Node(key, value));
        cacheMap[key] = cacheList.begin();
    }else{
        //update the value and add to the front
        cacheMap[key]->pagePtr = value;
        cacheList.splice(cacheList.begin(), cacheList, cacheMap[key]);
        cacheMap[key] = cacheList.begin();
    }
}

MyDB_LRU::~MyDB_LRU() {
    //todo: clean the memory?
}

bool MyDB_LRU::check(pair<MyDB_TablePtr, long> key) {
    return cacheMap.find(key) != cacheMap.end();
}
