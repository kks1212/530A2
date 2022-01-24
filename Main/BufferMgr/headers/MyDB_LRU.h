//
// Created by Shawn Zeng on 2022/1/23.
//

#ifndef A1_MYDB_LRU_H
#define A1_MYDB_LRU_H
#include <list>
#include <unordered_map>
#include "MyDB_BufferManager.h"
#include "MyDB_Table.h"
#include "MyDB_Page.h"

using namespace std;
class MyDB_LRU {

    // node for LRU list
    struct Node{
        MyDB_PagePtr pagePtr;
        pair<MyDB_TablePtr, long> id;
        Node(pair<MyDB_TablePtr, long> id, MyDB_PagePtr pagePtr): pagePtr(pagePtr), id(id){}
    };

public:
    //constructor
    MyDB_LRU(int capacity, MyDB_BufferManager *myManager);

    ~MyDB_LRU();

    //get the value in the map
    MyDB_PagePtr getValue(pair<MyDB_TablePtr, long> key);

    //set key and value in the LRU
    void put(pair<MyDB_TablePtr, long> key, MyDB_PagePtr value);

    //check whether contains a key
    bool check(pair<MyDB_TablePtr, long> key);

    //map and list made up to LRU
    unordered_map<pair<MyDB_TablePtr, long>, list<Node>::iterator> cacheMap;
    list<Node> cacheList;

    //number of pin page in the LRU
    int pinCount;
private:
    int capacity;
    MyDB_BufferManager* myManager;
};


#endif //A1_MYDB_LRU_H
