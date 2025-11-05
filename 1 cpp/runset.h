#pragma once
#ifndef RUNSET_H
#define RUNSET_H

#include "set.h"

class RunSet : public Set {
private:
    void sortTiles();
    bool isValidRun() const;

public:
    RunSet() = default;
    RunSet(const std::vector<Tile*>& initial_tiles);

    bool put(Tile* tile) override;
    Set* split(int pos) override;
    bool merge(Set* other) override;
    bool isComplete() const override;
};

#endif // RUNSET_H