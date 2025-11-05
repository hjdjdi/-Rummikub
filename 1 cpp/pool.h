#pragma once
#ifndef POOL_H
#define POOL_H

#include <vector>
#include <random>
#include "tile.h"

class Pool {
private:
    std::vector<Tile*> tiles;
    std::mt19937 rng;

public:
    Pool();
    ~Pool();

    void generate();
    void shuffle();
    Tile* draw();
    void put(Tile* tile);
    void display() const;
    int size() const;
};

#endif // POOL_H