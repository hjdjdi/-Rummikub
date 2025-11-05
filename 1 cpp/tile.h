#pragma once
#ifndef TILE_H
#define TILE_H

#include <iostream>

class Tile {
protected:
    char color;
    int number;
    bool is_joker;

public:
    Tile();
    Tile(char c, int n);
    virtual ~Tile() = default;

    virtual char getColor() const;
    virtual int getNumber() const;
    virtual bool isJoker() const;
    virtual int getScore() const;

    virtual void setColor(char c);
    virtual void setNumber(int n);

    virtual void display() const;
};

#endif // TILE_H
