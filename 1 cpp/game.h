#pragma once
#ifndef GAME_H
#define GAME_H

#include "pool.h"
#include "rack.h"
#include "sets.h"

class Game {
private:
    Pool pool;
    std::vector<Rack*> players;
    Sets table;
    int currentPlayer;
    bool iceBroken;
    bool gameEnded;
    std::vector<int> scores;
    int numPlayers;

public:
    Game(int players);
    ~Game();

    void startGame();
    void nextTurn();
    void handlePlay();
    void rearrangeTable();
    void handleDraw();
    bool isValidInitialMeld(const std::vector<Tile*>& tiles) const;
    bool isValidSet(const std::vector<Tile*>& tiles) const;
    bool isGameOver() const;
    void calculateScores();
    void displayGameState() const;
    void displayWinner() const;
    void resetGame();
    void addTilesToTable(const std::vector<Tile*>& tiles);
};

#endif // GAME_H