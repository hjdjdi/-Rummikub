#pragma once
#ifndef GROUPSET_H
#define GROUPSET_H

#include "set.h"

class GroupSet : public Set {
public:
    GroupSet() = default;
    GroupSet(const std::vector<Tile*>& initial_tiles);

    bool put(Tile* tile) override;
    Set* split(int pos) override;
    bool merge(Set* other) override;
    bool isComplete() const override;
};

#endif // GROUPSET_H