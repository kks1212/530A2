//
// Created by Shawn Zeng on 2022/1/23.
//

#ifndef A1_MYDB_PAGE_H
#define A1_MYDB_PAGE_H
#include <memory>

#include "MyDB_Table.h"
//#include "MyDB_LRU.h"

using namespace std;
class MyDB_BufferManager;

//smart pointer for pages
class MyDB_Page;
typedef shared_ptr <MyDB_Page> MyDB_PagePtr;

class MyDB_Page {

public:
    //constructor
    MyDB_Page(MyDB_BufferManager* myManager, MyDB_TablePtr myTable, long offset);

    //destructor
    ~MyDB_Page();

    //access the raw bytes in this page
    void *getBytes ();

    // let the page know that we have written to the bytes.
    void wroteBytes ();

    //whether this page is pinned
    bool isPin;
    //whether this page is dirty
    bool isDirty;

    //whether this page is anonymous
    bool isAnonymous;

    //this page is from which table(*pageid[0]) and which page(*pageid[1])
    pair<MyDB_TablePtr, long> pageId;

    //reference counts
    int refCount;

    //read from the disk
//    void* readFromDisk(MyDB_TablePtr whichTable, long offset);

    //write back to disk
    void writeBack();

    /* add a reference to this page */
    void addRef();

    /* reduce a reference to this page */
    void reduceRef();

private:
    friend class MyDB_BufferManager;
    friend class MyDB_LRU;
    //point to the actual bytes
    void* bytes;
    MyDB_BufferManager* myManager;


};


#endif //A1_MYDB_PAGE_H
