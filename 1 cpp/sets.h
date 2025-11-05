#pragma once
#ifndef SETS_H
#define SETS_H

#include <vector>
#include "set.h"

class Sets {
private:
    std::vector<Set*> sets;

public:
    ~Sets();
    void put(Set* set);
    void swap(int pos1, int pos2);
    void insert(int pos, Set* set);
    void cleanUp();
    void display() const;
    int size() const;  // Return the number of collections     
    Set* getSet(int pos) const; //Returns the collection at the specified index
};

#endif // SETS_H