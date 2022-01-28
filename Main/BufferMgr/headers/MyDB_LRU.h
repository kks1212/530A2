//
// Created by Shawn Zeng on 2022/1/23.
//

#ifndef A1_MYDB_LRU_H
#define A1_MYDB_LRU_H
#include <list>
#include <map>
#include "MyDB_BufferManager.h"
//#include "MyDB_Table.h"
//#include "MyDB_Page.h"

class MyDB_Table;
class MyDB_Page;
class MyDB_BufferManager;

using namespace std;
// node for LRU list
struct DLinkedNode {
    MyDB_PagePtr pagePtr;
    pair<MyDB_TablePtr, long> id;
    DLinkedNode* prev;
    DLinkedNode* next;
    DLinkedNode(): pagePtr(nullptr), prev(nullptr), next(nullptr) {
        id = make_pair(nullptr, 0);
    }
    DLinkedNode(pair<MyDB_TablePtr, long> id, MyDB_PagePtr pagePtr): pagePtr(pagePtr), id(id), prev(nullptr), next(nullptr){}
};

class MyDB_LRU {

public:
    //constructor
    MyDB_LRU(size_t capacity, MyDB_BufferManager *myManager);

    ~MyDB_LRU();

    //get the value in the map
    MyDB_PagePtr get(pair<MyDB_TablePtr, long> key);

    //set key and value in the LRU
    void put(pair<MyDB_TablePtr, long> key, MyDB_PagePtr value);

    //check whether contains a key
    bool check(pair<MyDB_TablePtr, long> key);

    //kill a node
    void killNode(pair<MyDB_TablePtr, long> key);

    //unpin a page, decrease the pinCount
    void updatePinCount(pair<MyDB_TablePtr, long> key);

    //map and list made up to LRU
    map<pair<MyDB_TablePtr, long>, DLinkedNode*> cacheMap;

    //number of pin page in the LRU
    int pinCount;

    //overall count
    int size;

    //helper function for LRU
    void moveToHead(DLinkedNode* node);
    void addToHead(DLinkedNode* node);
    void removeNode(DLinkedNode* node);
    void removeTail();

private:
    size_t capacity;
    DLinkedNode* head;
    DLinkedNode* tail;
    MyDB_BufferManager* myManager;
};

#endif //A1_MYDB_LRU_H
