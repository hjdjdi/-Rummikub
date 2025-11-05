#pragma once
#ifndef SET_H
#define SET_H

#include <vector>
#include "tile.h"

class Set {
protected:
    std::vector<Tile*> tiles;

public:
    virtual ~Set();

    virtual bool put(Tile* tile) = 0;
    Tile* pop(int pos);
    virtual Set* split(int pos) = 0;
    virtual bool merge(Set* other) = 0;
    virtual bool isComplete() const = 0;

    void display(bool showIndex = false) const;
    int size() const;
};

#endif // SET_H