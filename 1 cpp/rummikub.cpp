#include "rummikub.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <ctime>
#include <climits>
#include <set>

using namespace std;

// Tile Implementation
Tile::Tile() : color('-'), number(0), is_joker(false) {}
Tile::Tile(char c, int n) : color(c), number(n), is_joker(false) {}

char Tile::getColor() const { return color; }
int Tile::getNumber() const { return number; }
bool Tile::isJoker() const { return false; }
int Tile::getScore() const { return number; }
void Tile::setColor(char c) {}
void Tile::setNumber(int n) {}
void Tile::display() const {
    cout << color << number;
}

// Joker Implementation
Joker::Joker() : joker_set(false) {
    is_joker = true;
    color = '-';
    number = 0;
}

char Joker::getColor() const { return joker_set ? color : '-'; }
int Joker::getNumber() const { return joker_set ? number : 0; }
bool Joker::isJoker() const { return true; }
int Joker::getScore() const { return joker_set ? 0 : 30; }
void Joker::setColor(char c) { color = c; joker_set = true; }
void Joker::setNumber(int n) { number = n; joker_set = true; }
void Joker::reset() { joker_set = false; color = '-'; number = 0; }
void Joker::display() const {
    if (joker_set) {
        cout << "[" << color << number << "]";
    }
    else {
        cout << ":-)";
    }
}

// Pool Implementation
Pool::Pool() : rng(time(nullptr)) {
    generate();
}

Pool::~Pool() {
    for (Tile* tile : tiles) {
        delete tile;
    }
}

void Pool::generate() {
    for (Tile* tile : tiles) {
        delete tile;
    }
    tiles.clear();

    const char colors[] = { 'R', 'B', 'Y', 'K' };
    for (int c = 0; c < 4; c++) {
        for (int n = 1; n <= 13; n++) {
            tiles.push_back(new Tile(colors[c], n));
            tiles.push_back(new Tile(colors[c], n));
        }
    }
    tiles.push_back(new Joker());
    tiles.push_back(new Joker());
}

void Pool::shuffle() {
    std::shuffle(tiles.begin(), tiles.end(), rng);
}

Tile* Pool::draw() {
    if (tiles.empty()) return nullptr;
    Tile* tile = tiles.back();
    tiles.pop_back();
    return tile;
}

void Pool::put(Tile* tile) {
    tiles.push_back(tile);
}

void Pool::display() const {
    for (size_t i = 0; i < tiles.size(); i++) {
        tiles[i]->display();
        if (i < tiles.size() - 1) cout << " ";
    }
}

int Pool::size() const {
    return tiles.size();
}

// Rack Implementation
Rack::~Rack() {
    for (Tile* tile : tiles) {
        delete tile;
    }
}

void Rack::put(Tile* tile) {
    tiles.push_back(tile);
}

void Rack::insert(int pos, Tile* tile) {
    if (pos < 0 || pos > static_cast<int>(tiles.size())) {
        tiles.push_back(tile);
    }
    else {
        tiles.insert(tiles.begin() + pos, tile);
    }
}

Tile* Rack::pop(int pos) {
    if (pos < 0 || pos >= static_cast<int>(tiles.size()))
        return nullptr;

    Tile* tile = tiles[pos];
    tiles.erase(tiles.begin() + pos);
    return tile;
}

int Rack::getPoints() const {
    int total = 0;
    for (Tile* tile : tiles) {
        total += tile->getScore();
    }
    return total;
}

void Rack::display() const {
    for (size_t i = 0; i < tiles.size(); i++) {
        tiles[i]->display();
        if (i < tiles.size() - 1) cout << " ";
    }
    cout << endl;

    for (size_t i = 0; i < tiles.size(); i++) {
        cout << setw(3) << i;
        if (i < tiles.size() - 1) cout << " ";
    }
}

int Rack::size() const {
    return tiles.size();
}

// Set Implementation
Set::~Set() {
    for (Tile* tile : tiles) {
        delete tile;
    }
}

Tile* Set::pop(int pos) {
    if (pos < 0 || pos >= static_cast<int>(tiles.size()))
        return nullptr;

    Tile* tile = tiles[pos];
    tiles.erase(tiles.begin() + pos);

    if (tile->isJoker()) {
        Joker* joker = dynamic_cast<Joker*>(tile);
        if (joker) {
            joker->reset();
        }
    }

    return tile;
}

void Set::display(bool showIndex) const {
    for (size_t i = 0; i < tiles.size(); i++) {
        tiles[i]->display();
        if (i < tiles.size() - 1) cout << " ";
    }

    if (showIndex) {
        cout << endl;
        for (size_t i = 0; i < tiles.size(); i++) {
            cout << setw(3) << i;
            if (i < tiles.size() - 1) cout << " ";
        }
    }
}

int Set::size() const {
    return tiles.size();
}

// GroupSet Implementation
GroupSet::GroupSet(const vector<Tile*>& initial_tiles) {
    for (Tile* tile : initial_tiles) {
        put(tile);
    }
}

bool GroupSet::put(Tile* tile) {
    if (tiles.empty()) {
        tiles.push_back(tile);
        return true;
    }

    int groupNum = tiles[0]->getNumber();
    if (tile->getNumber() != groupNum)
        return false;

    for (Tile* existing : tiles) {
        if (existing->getColor() == tile->getColor())
            return false;
    }

    tiles.push_back(tile);
    return true;
}

Set* GroupSet::split(int pos) {
    if (pos < 0 || pos >= static_cast<int>(tiles.size()) || tiles.size() < 2)
        return nullptr;

    vector<Tile*> newTiles;
    for (int i = pos; i < static_cast<int>(tiles.size()); i++) {
        newTiles.push_back(tiles[i]);
    }
    tiles.resize(pos);

    return new GroupSet(newTiles);
}

bool GroupSet::merge(Set* other) {
    GroupSet* otherGroup = dynamic_cast<GroupSet*>(other);
    if (!otherGroup) return false;

    if (tiles.empty() || otherGroup->tiles.empty())
        return false;

    int thisNum = tiles[0]->getNumber();
    int otherNum = otherGroup->tiles[0]->getNumber();
    if (thisNum != otherNum)
        return false;

    set<char> colors;
    for (Tile* tile : tiles) {
        colors.insert(tile->getColor());
    }
    for (Tile* tile : otherGroup->tiles) {
        if (colors.find(tile->getColor()) != colors.end())
            return false;
    }

    for (Tile* tile : otherGroup->tiles) {
        tiles.push_back(tile);
    }
    otherGroup->tiles.clear();

    return true;
}

bool GroupSet::isComplete() const {
    return tiles.size() >= 3 && tiles.size() <= 4;
}

// RunSet Implementation
void RunSet::sortTiles() {
    sort(tiles.begin(), tiles.end(),
        [](Tile* a, Tile* b) {
            return a->getNumber() < b->getNumber();
        });
}

bool RunSet::isValidRun() const {
    if (tiles.size() < 3) return false;

    char color = tiles[0]->getColor();
    int prevNum = tiles[0]->getNumber();

    for (size_t i = 1; i < tiles.size(); i++) {
        if (tiles[i]->getColor() != color)
            return false;

        if (tiles[i]->getNumber() != prevNum + 1)
            return false;

        prevNum = tiles[i]->getNumber();
    }
    return true;
}

RunSet::RunSet(const vector<Tile*>& initial_tiles) {
    for (Tile* tile : initial_tiles) {
        put(tile);
    }
    sortTiles();
}

bool RunSet::put(Tile* tile) {
    if (tiles.empty()) {
        tiles.push_back(tile);
        return true;
    }

    char setColor = tiles[0]->getColor();
    if (tile->getColor() != setColor)
        return false;

    tiles.push_back(tile);
    sortTiles();
    return isValidRun();
}

Set* RunSet::split(int pos) {
    if (pos < 0 || pos >= static_cast<int>(tiles.size()) || tiles.size() < 2)
        return nullptr;

    vector<Tile*> newTiles;
    for (int i = pos; i < static_cast<int>(tiles.size()); i++) {
        newTiles.push_back(tiles[i]);
    }
    tiles.resize(pos);

    return new RunSet(newTiles);
}

bool RunSet::merge(Set* other) {
    RunSet* otherRun = dynamic_cast<RunSet*>(other);
    if (!otherRun) return false;

    if (tiles.empty() || otherRun->tiles.empty())
        return false;

    char thisColor = tiles[0]->getColor();
    char otherColor = otherRun->tiles[0]->getColor();
    if (thisColor != otherColor)
        return false;

    int thisLast = tiles.back()->getNumber();
    int otherFirst = otherRun->tiles.front()->getNumber();
    if (thisLast + 1 != otherFirst)
        return false;

    for (Tile* tile : otherRun->tiles) {
        tiles.push_back(tile);
    }
    otherRun->tiles.clear();
    sortTiles();

    return true;
}

bool RunSet::isComplete() const {
    return tiles.size() >= 3 && isValidRun();
}

// Sets Implementation
Sets::~Sets() {
    for (Set* set : sets) {
        delete set;
    }
}

void Sets::put(Set* set) {
    sets.push_back(set);
}

void Sets::swap(int pos1, int pos2) {
    if (pos1 < 0 || pos1 >= static_cast<int>(sets.size()) ||
        pos2 < 0 || pos2 >= static_cast<int>(sets.size())) {
        return;
    }
    std::swap(sets[pos1], sets[pos2]);
}

void Sets::insert(int pos, Set* set) {
    if (pos < 0 || pos > static_cast<int>(sets.size())) {
        sets.push_back(set);
    }
    else {
        sets.insert(sets.begin() + pos, set);
    }
}

void Sets::cleanUp() {
    auto it = remove_if(sets.begin(), sets.end(),
        [](Set* set) { return set->size() == 0; });
    sets.erase(it, sets.end());
}

void Sets::display() const {
    for (size_t i = 0; i < sets.size(); i++) {
        cout << i << ": ";
        sets[i]->display(false);
        cout << endl;
    }
}

int Sets::size() const {
    return sets.size();
}

Set* Sets::getSet(int pos) const {
    if (pos < 0 || pos >= static_cast<int>(sets.size())) {
        return nullptr;
    }
    return sets[pos];
}

// Game Implementation
Game::Game(int players) : numPlayers(players), currentPlayer(0), iceBroken(false), gameEnded(false) {
    if (numPlayers < 2 || numPlayers > 4) {
        throw invalid_argument("Number of players must be between 2 and 4");
    }

    for (int i = 0; i < numPlayers; i++) {
        this->players.push_back(new Rack());
    }

    scores.resize(numPlayers, 0);
    pool.generate();
    pool.shuffle();
}

Game::~Game() {
    for (auto* rack : players) {
        delete rack;
    }
}

void Game::startGame() {
    for (int i = 0; i < players.size(); i++) {
        for (int j = 0; j < 14; j++) {
            Tile* tile = pool.draw();
            if (tile) players[i]->put(tile);
        }
    }

    while (!isGameOver()) {
        displayGameState();
        nextTurn();
    }

    calculateScores();
    displayWinner();
}

void Game::nextTurn() {
    cout << "\nPlayer " << currentPlayer + 1 << "'s turn\n";
    cout << "Options:\n";
    cout << "1. Play tiles\n";
    cout << "2. Rearrange table\n";
    cout << "3. Draw tiles\n";
    cout << "4. Pass\n";
    cout << "Enter choice: ";

    int choice;
    cin >> choice;

    switch (choice) {
    case 1:
        handlePlay();
        break;
    case 2:
        rearrangeTable();
        break;
    case 3:
        handleDraw();
        break;
    case 4:
        break;
    default:
        cout << "Invalid choice. Try again.\n";
        return;
    }

    if (players[currentPlayer]->size() == 0) {
        gameEnded = true;
        return;
    }

    currentPlayer = (currentPlayer + 1) % players.size();
}

void Game::handlePlay() {
    players[currentPlayer]->display();
    cout << "\nEnter tile indices to play (e.g., '0 2 4'), or -1 to cancel: ";

    string input;
    cin.ignore();
    getline(cin, input);

    if (input == "-1") return;

    vector<int> indices;
    istringstream iss(input);
    int index;

    while (iss >> index) {
        if (index >= 0 && index < players[currentPlayer]->size()) {
            indices.push_back(index);
        }
    }

    sort(indices.rbegin(), indices.rend());

    vector<Tile*> tilesToPlay;
    for (int idx : indices) {
        Tile* popped = players[currentPlayer]->pop(idx);
        if (popped) {
            tilesToPlay.push_back(popped);
        }
    }

    if (!iceBroken && !isValidInitialMeld(tilesToPlay)) {
        cout << "Invalid initial meld! Must be at least 30 points and no jokers.\n";
        for (auto* tile : tilesToPlay) {
            players[currentPlayer]->put(tile);
        }
        return;
    }

    if (!isValidSet(tilesToPlay)) {
        cout << "Invalid set! Must be a valid group or run.\n";
        for (auto* tile : tilesToPlay) {
            players[currentPlayer]->put(tile);
        }
        return;
    }

    addTilesToTable(tilesToPlay);

    if (!iceBroken) iceBroken = true;
}

void Game::addTilesToTable(const vector<Tile*>& tiles) {
    GroupSet* groupSet = new GroupSet();
    bool isGroup = true;

    for (auto* tile : tiles) {
        if (!groupSet->put(tile)) {
            isGroup = false;
            break;
        }
    }

    if (isGroup && groupSet->isComplete()) {
        table.put(groupSet);
        return;
    }
    else {
        delete groupSet;
    }

    RunSet* runSet = new RunSet();
    for (auto* tile : tiles) {
        if (!runSet->put(tile)) {
            delete runSet;
            cout << "Failed to create a valid set\n";
            for (auto* t : tiles) {
                players[currentPlayer]->put(t);
            }
            return;
        }
    }

    if (runSet->isComplete()) {
        table.put(runSet);
    }
    else {
        delete runSet;
        cout << "Invalid set! Must be a valid group or run.\n";
        for (auto* t : tiles) {
            players[currentPlayer]->put(t);
        }
    }
}

void Game::rearrangeTable() {
    if (!iceBroken) {
        cout << "You must break the ice before rearranging the table.\n";
        return;
    }

    cout << "Table sets:\n";
    table.display();

    cout << "Enter set index and tile index to pop (e.g., '1 2'), or -1 to cancel: ";
    string input;
    cin.ignore();
    getline(cin, input);

    if (input == "-1") return;

    int setIndex, tileIndex;
    istringstream iss(input);
    iss >> setIndex >> tileIndex;

    if (setIndex < 0 || setIndex >= table.size()) {
        cout << "Invalid set index.\n";
        return;
    }

    Set* selectedSet = table.getSet(setIndex);
    if (!selectedSet || tileIndex < 0 || tileIndex >= selectedSet->size()) {
        cout << "Invalid tile index.\n";
        return;
    }

    Tile* tile = selectedSet->pop(tileIndex);
    if (tile) {
        players[currentPlayer]->put(tile);
        table.cleanUp();
        cout << "Tile moved to rack: ";
        tile->display();
        cout << endl;
    }

    cout << "Enter tile indices to play back (e.g., '0 2 4'), or -1 to cancel: ";
    getline(cin, input);

    if (input == "-1") return;

    vector<int> indices;
    istringstream iss2(input);
    int index;

    while (iss2 >> index) {
        if (index >= 0 && index < players[currentPlayer]->size()) {
            indices.push_back(index);
        }
    }

    sort(indices.rbegin(), indices.rend());

    vector<Tile*> tilesToPlay;
    for (int idx : indices) {
        Tile* popped = players[currentPlayer]->pop(idx);
        if (popped) {
            tilesToPlay.push_back(popped);
        }
    }

    if (!isValidSet(tilesToPlay)) {
        cout << "Invalid set! Must be a valid group or run.\n";
        for (auto* tile : tilesToPlay) {
            players[currentPlayer]->put(tile);
        }
        return;
    }

    addTilesToTable(tilesToPlay);
}

void Game::handleDraw() {
    int drawCount = iceBroken ? 3 : 1;

    for (int i = 0; i < drawCount; i++) {
        Tile* tile = pool.draw();
        if (tile) {
            players[currentPlayer]->put(tile);
        }
        else {
            cout << "Pool is empty!\n";
            break;
        }
    }

    cout << "Drew " << drawCount << " tile(s).\n";
}

bool Game::isValidInitialMeld(const vector<Tile*>& tiles) const {
    if (iceBroken) return true;

    int totalPoints = 0;

    for (const auto* tile : tiles) {
        if (tile->isJoker()) {
            return false;
        }
        totalPoints += tile->getScore();
    }

    return totalPoints >= 30;
}

bool Game::isValidSet(const vector<Tile*>& tiles) const {
    if (tiles.empty()) return false;

    GroupSet groupSet;
    for (auto* tile : tiles) {
        if (!groupSet.put(tile)) {
            break;
        }
    }
    if (groupSet.isComplete()) return true;

    RunSet runSet;
    for (auto* tile : tiles) {
        if (!runSet.put(tile)) {
            break;
        }
    }
    return runSet.isComplete();
}

bool Game::isGameOver() const {
    return gameEnded || pool.size() == 0;
}

void Game::calculateScores() {
    int winnerPoints = 0;
    int winnerIndex = -1;

    for (int i = 0; i < players.size(); i++) {
        if (players[i]->size() == 0) {
            winnerIndex = i;
            continue;
        }

        int playerPoints = players[i]->getPoints();
        scores[i] -= playerPoints;
        winnerPoints += playerPoints;
    }

    if (winnerIndex != -1) {
        scores[winnerIndex] += winnerPoints;
    }
}

void Game::displayGameState() const {
    cout << "\n=== Game State ===\n";
    cout << "Current player: " << currentPlayer + 1 << "\n";
    cout << "Ice broken: " << (iceBroken ? "Yes" : "No") << "\n";

    cout << "\nTable sets:\n";
    table.display();

    cout << "\nYour rack:\n";
    players[currentPlayer]->display();
    cout << "\n";

    cout << "Tiles in pool: " << pool.size() << "\n";
}

void Game::displayWinner() const {
    cout << "\n=== Game Over ===\n";

    int winnerIndex = -1;
    int maxScore = INT_MIN;

    for (int i = 0; i < players.size(); i++) {
        cout << "Player " << i + 1 << " score: " << scores[i] << "\n";

        if (scores[i] > maxScore) {
            maxScore = scores[i];
            winnerIndex = i;
        }
    }

    if (winnerIndex != -1) {
        cout << "\nPlayer " << winnerIndex + 1 << " wins with " << maxScore << " points!\n";
    }
    else {
        cout << "\nNo winner!\n";
    }
}

void Game::resetGame() {
    pool = Pool();
    table = Sets();
    iceBroken = false;
    gameEnded = false;
    currentPlayer = 0;

    for (auto* rack : players) {
        delete rack;
    }
    players.clear();

    for (int i = 0; i < numPlayers; i++) {
        players.push_back(new Rack());
    }

    for (int i = 0; i < players.size(); i++) {
        for (int j = 0; j < 14; j++) {
            Tile* tile = pool.draw();
            if (tile) players[i]->put(tile);
        }
    }
}

// Test Functions Implementation
void tileTest() {
    cout << "=== Tiles and Jokers Test ===" << endl;
    Tile* t1 = new Tile('R', 13);
    Tile* t2 = new Tile('K', 1);

    cout << "Tile t1: R13" << endl;
    cout << "T1: " << t1->getColor() << " (Expected: R)" << endl;
    cout << "T2: " << t1->getNumber() << " (Expected: 13)" << endl;
    cout << "T3: " << (t1->isJoker() ? "Yes" : "No") << " (Expected: No)" << endl;
    cout << "T4: " << t1->getScore() << " (Expected: 13)" << endl;
    cout << "T6: "; t1->display(); cout << " (Expected: R13)" << endl;
    cout << endl;

    cout << "Tile t2: K1" << endl;
    cout << "T1: " << t2->getColor() << " (Expected: K)" << endl;
    cout << "T2: " << t2->getNumber() << " (Expected: 1)" << endl;
    cout << "T3: " << (t2->isJoker() ? "Yes" : "No") << " (Expected: No)" << endl;
    cout << "T4: " << t2->getScore() << " (Expected: 1)" << endl;
    cout << "T6: "; t2->display(); cout << " (Expected: K1)" << endl;
    cout << endl;

    Joker* j1 = new Joker();
    cout << "Tile j1: Joker" << endl;
    cout << "T1: " << j1->getColor() << " (Expected: -)" << endl;
    cout << "T2: " << j1->getNumber() << " (Expected: 0)" << endl;
    cout << "T3: " << (j1->isJoker() ? "Yes" : "No") << " (Expected: Yes)" << endl;
    cout << "T4: " << j1->getScore() << " (Expected: 30)" << endl;
    cout << "T6: "; j1->display(); cout << " (Expected: :-))" << endl;
    cout << endl;

    cout << "Setting color and number of joker to R6" << endl;
    j1->setColor('R');
    j1->setNumber(6);
    cout << "T5: " << j1->getColor() << " (Expected: R)" << endl;
    cout << "T5: " << j1->getNumber() << " (Expected: 6)" << endl;
    cout << "T3: " << (j1->isJoker() ? "Yes" : "No") << " (Expected: Yes)" << endl;
    cout << "T4: " << j1->getScore() << " (Expected: 0)" << endl;
    cout << "T6: "; j1->display(); cout << " (Expected: [R6])" << endl;
    cout << endl;

    delete t1;
    delete t2;
    delete j1;
}

void poolTest() {
    cout << "=== Pool Test ===" << endl;
    Pool* p = new Pool();
    cout << "P1 and P5 Test:" << endl;
    p->display();
    cout << endl << "Number of tiles: " << p->size() << " (Expected: 106)" << endl;
    cout << endl;

    cout << "P3 Test: ";
    Tile* t = p->draw();
    t->display();
    cout << " (Expected: Any tile)" << endl;
    cout << "Pool size after draw: " << p->size() << " (Expected: 105)" << endl;
    cout << endl;

    cout << "P4 Test:" << endl;
    p->put(t);
    cout << "Pool size after put: " << p->size() << " (Expected: 106)" << endl;
    cout << endl;

    cout << "P2 Test:" << endl;
    p->shuffle();
    p->display();
    cout << endl << "Number of tiles: " << p->size() << " (Expected: 106)" << endl;
    cout << endl;

    delete p;
}

void rackTest() {
    cout << "=== Rack Test ===" << endl;
    Pool* p = new Pool();

    cout << "R1 and R5 Test: " << endl;
    Rack* r = new Rack();
    for (int i = 0; i < 14; i++) {
        Tile* t = p->draw();
        r->put(t);
    }
    r->display();
    cout << endl << "Rack size: " << r->size() << " (Expected: 14)" << endl;
    cout << endl;

    cout << "R2 Test (with Joker added to pos 2): " << endl;
    r->insert(2, new Joker());
    r->display();
    cout << endl << "Rack size: " << r->size() << " (Expected: 15)" << endl;
    cout << endl;

    cout << "R3 Test (pop pos 3): " << endl;
    Tile* popped = r->pop(3);
    cout << "Popped tile: ";
    popped->display();
    cout << endl;
    r->display();
    cout << endl << "Rack size: " << r->size() << " (Expected: 14)" << endl;
    cout << endl;

    cout << "R4 Test: " << r->getPoints() << " (Expected: Sum of tile points)" << endl;
    cout << endl;

    delete popped;
    delete r;
    delete p;
}

void setTest() {
    cout << "=== Set and Sets Test ===" << endl;
    Tile* t1 = new Tile('R', 3);
    Tile* t2 = new Tile('Y', 3);
    Tile* t3 = new Tile('B', 3);
    Tile* t4 = new Tile('K', 3);
    Tile* t5 = new Tile('K', 4);
    Tile* t6 = new Tile('K', 5);
    Tile* t7 = new Tile('K', 6);
    Tile* t8 = new Tile('K', 7);
    Tile* t9 = new Tile('K', 8);
    Joker* j1 = new Joker();

    cout << "-- Group Set Test --" << endl;
    GroupSet* gs1 = new GroupSet();
    cout << "S1, S2, S8: Adding R3 to empty group set: ";
    bool b1 = gs1->put(t1);
    cout << (b1 ? "Success" : "Failed") << " (Expected: Success)" << endl;
    gs1->display(true); cout << endl;

    cout << "S2: Adding K4 to group set: ";
    bool b2 = gs1->put(t5);
    cout << (b2 ? "Success" : "Failed") << " (Expected: Failed)" << endl;
    gs1->display(true); cout << endl;

    cout << "S2: Adding Y3, B3, K3 to group set: " << endl;
    gs1->put(t2);
    gs1->put(t3);
    gs1->put(t4);
    gs1->display(true); cout << endl;
    cout << "S7: Is complete: " << (gs1->isComplete() ? "Yes" : "No") << " (Expected: Yes)" << endl;

    cout << "S3: Pop tile at pos 2: ";
    Tile* popped1 = gs1->pop(2);
    popped1->display(); cout << " (Expected: B3)" << endl;
    gs1->display(true); cout << endl;

    cout << "S4: Split at pos 2: ";
    Set* gs1s = gs1->split(2);
    gs1->display(false); cout << endl;
    gs1s->display(false); cout << endl;
    cout << "S6: Size of split set: " << gs1s->size() << " (Expected: 1)" << endl;

    cout << "S5: Merge split set back: ";
    bool merged = gs1->merge(gs1s);
    cout << (merged ? "Success" : "Failed") << " (Expected: Success)" << endl;
    gs1->display(false); cout << endl;

    cout << "-- Run Set Test --" << endl;
    RunSet* rs1 = new RunSet();
    cout << "S1, S2, S8: Adding K5 to empty run set: ";
    bool b3 = rs1->put(t6);
    cout << (b3 ? "Success" : "Failed") << " (Expected: Success)" << endl;
    rs1->display(true); cout << endl;

    cout << "S2: Adding K4, K6, K7: " << endl;
    rs1->put(t5);
    rs1->put(t7);
    rs1->put(t8);
    rs1->display(true); cout << endl;
    cout << "S7: Is complete: " << (rs1->isComplete() ? "Yes" : "No") << " (Expected: Yes)" << endl;

    cout << "S3: Pop tile at pos 2: ";
    Tile* popped2 = rs1->pop(2);
    popped2->display(); cout << " (Expected: K6)" << endl;
    rs1->display(true); cout << endl;

    cout << "S4: Split at pos 2: ";
    Set* rs1s = rs1->split(2);
    rs1->display(false); cout << endl;
    rs1s->display(false); cout << endl;

    cout << "S5: Add joker as K6 and merge: ";
    j1->setColor('K');
    j1->setNumber(6);
    rs1s->put(j1);
    bool merged2 = rs1->merge(rs1s);
    cout << (merged2 ? "Success" : "Failed") << " (Expected: Success)" << endl;
    rs1->display(false); cout << endl;

    cout << "-- Sets Test --" << endl;
    Sets* sets = new Sets();
    sets->put(gs1);
    sets->put(rs1);
    cout << "Ss4: Display sets: " << endl;
    sets->display();

    cout << "Ss1: Clean up empty sets: " << endl;
    sets->cleanUp();
    sets->display();

    cout << "Ss2: Swap positions 0 and 1: " << endl;
    sets->swap(0, 1);
    sets->display();

    cout << "Ss3: Insert new run set at pos 1: " << endl;
    RunSet* rs2 = new RunSet();
    rs2->put(new Tile('K', 9));
    rs2->put(new Tile('K', 10));
    rs2->put(new Tile('K', 11));
    sets->insert(1, rs2);
    sets->display();

    delete sets;
    delete popped1;
    delete popped2;
}

void runTests() {
    cout << "=== Running Tests ===" << endl;
    tileTest();
    cout << endl;
    poolTest();
    cout << endl;
    rackTest();
    cout << endl;
    setTest();
    cout << endl;
    cout << "=== Tests Completed ===" << endl;
}

void playGame() {
    cout << "=== Rummikub Game ===\n";

    int numPlayers;
    do {
        cout << "Enter number of players (2-4): ";
        cin >> numPlayers;
    } while (numPlayers < 2 || numPlayers > 4);

    try {
        Game game(numPlayers);
        game.startGame();

        char playAgain;
        cout << "\nPlay again? (y/n): ";
        cin >> playAgain;

        if (tolower(playAgain) == 'y') {
            game.resetGame();
            game.startGame();
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

// Main Function
int main() {
    int choice;

    do {
        cout << "=== Rummikub System ===\n";
        cout << "1. Run tests\n";
        cout << "2. Play game\n";
        cout << "3. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            runTests();
            break;
        case 2:
            playGame();
            break;
        case 3:
            cout << "Goodbye!\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 3);

    return 0;
}