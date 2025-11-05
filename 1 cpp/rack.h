#pragma once
#ifndef RACK_H
#define RACK_H

#include <vector>
#include "tile.h"

class Rack {
private:
    std::vector<Tile*> tiles;

public:
    ~Rack();

    void put(Tile* tile);
    void insert(int pos, Tile* tile);
    Tile* pop(int pos);
    int getPoints() const;
    void display() const;
    int size() const;
};

#endif // RACK_H