
#ifndef JOKER_H
#define JOKER_H

#include "tile.h"

class Joker : public Tile {
private:
    bool joker_set;

public:
    Joker();
    char getColor() const override;
    int getNumber() const override;
    bool isJoker() const override;
    int getScore() const override;
    void setColor(char c) override;
    void setNumber(int n) override;
    void reset();
    void display() const override;
};

#endif // JOKER_H#pragma once
