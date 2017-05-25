#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <vector>

using namespace std;

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

class GameImpl
{
public:
    // Constructor
    GameImpl(int nRows, int nCols) : m_rows(nRows), m_cols(nCols), m_nShips(0), m_ships({}) {}
    // Destructor
    ~GameImpl();
    
    // Accessors
    int rows() const { return m_rows; }
    int cols() const { return m_cols; }
    int nShips() const { return m_nShips; }
    int shipLength(int shipId) const { return m_ships[shipId]->m_len; }
    char shipSymbol(int shipId) const { return m_ships[shipId]->m_symbol; }
    string shipName(int shipId) const { return m_ships[shipId]->m_name; }
    
    // Other
    bool addShip(int length, char symbol, string name);
    bool isValid(Point p) const { return p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols(); }
    Point randomPoint() const { return Point(randInt(rows()), randInt(cols())); }
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
    
private:
    int m_rows, m_cols, m_nShips;
    // Ship struct -- stores necessary ship data
    struct Ship
    {
        // Initialize m_id to global id which is incremented each time a ship is added
        Ship(int i, int l, char s, string n) : m_id(i), m_len(l), m_symbol(s), m_name(n) {}
        int m_id, m_len;
        char m_symbol;
        string m_name;
    };
    // Store ships using a vector of Ship pointers
    // Ships vector index corresponds to its id
    // Using pointers saves memory and allows it to run faster because it does not have to duplicate values
    vector<Ship*> m_ships;
};

/**
    Destructor
 
    Since pointers are used in the private members the destructor must explicitly delete them
    Implemented using a range based for loop
 */
GameImpl::~GameImpl()
{
    for (auto d : m_ships)
        delete d;
}

/**
    Adds new ship to storage and returns true
 
    @param1 length The length of the ship
    @param2 symbol The symbol of the ship
    @param3 name The name of the ship
    @return True if the ship is successfully added
 */
bool GameImpl::addShip(int length, char symbol, string name)
{
    Ship* new_ship = new Ship(m_nShips++, length, symbol, name);
    m_ships.push_back(new_ship);
    return true;
}

/**
    Runs a complete game between two indicated players
 
    @param1 p1 Pointer to the first player -- either human, awful, mediocre, or good
    @param2 p2 Pointer to the second player -- either human, awful, mediocre, or good
    @param3 b1 Reference to first players board
    @param4 b2 Reference to second players board
    @param5 shouldPause If true program will wait for user to press enter before continuing to subsequent turns
    @return Pointer to the winning player -- either p1 or p2
 */
Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
    // Player points and board pointer
    Player *t1, *t2;
    Board* b;
    // P1s turn is first
    bool p1Turn = true;
    bool human;
    
    // Check to make sure both players are not human
    if (p1->isHuman() && p2->isHuman())
    {
        cout << "This game does not support 2-player." << endl;
        return nullptr;
    }
    // If ships cannot be placed return nullptr
    if (!p1->placeShips(b1)) return nullptr;
    if (!p2->placeShips(b2)) return nullptr;
    
    // Play game until one of the players ships are destroyed
    while (!b1.allShipsDestroyed() && !b2.allShipsDestroyed())
    {
        // Set booleans to false
        bool shotHit = false, shipDestroyed = false, validShot = false;
        // Garbage value
        int shipId = 100;
        // Set pointer variables depending on whos turn it is
        if (p1Turn)
        {
            t1 = p1;
            t2 = p2;
            b = &b2;
            human = p1->isHuman();
        }
        else
        {
            t1 = p2;
            t2 = p1;
            b = &b1;
            human = p2->isHuman();
        }
        // Display board
        cout << t1->name() << "'s turn. Board for " << t2->name() << ":" << endl;
        b->display(human);
        // Get attack from player
        Point attackCoord = t1->recommendAttack();
        // Attack and set validShot to result
        validShot = b->attack(attackCoord, shotHit, shipDestroyed, shipId);
        // Record the attack result
        t1->recordAttackResult(attackCoord, validShot, shotHit, shipDestroyed, shipId);
        // If human and shot was invalid print special message
        // Computers cannot waste shots
        if (human && shipId == -1)
            cout << t1->name() << " wasted a shot at (" << attackCoord.r << "," << attackCoord.c << ")." << endl;
        // Shot is valid
        else
        {
            // Display message depending on result of attack
            cout << t1->name() << " attacked (" << attackCoord.r << "," << attackCoord.c << ") and ";
            if (shotHit && shipDestroyed)
                cout << "destroyed the " << this->shipName(shipId);
            else if (shotHit)
                cout << "hit something";
            else
                cout << "missed";
            cout << ", resulting in:" << endl;
            // Display results -- board displayed depends on whether the player is a human or not
            b->display(human);
        }
        // Switch turn to P2
        p1Turn = !p1Turn;
        
        // If one of the player's ships are destroyed break loop
        if (b1.allShipsDestroyed() || b2.allShipsDestroyed())
            break;
        // If pause parameter is true waitForEnter after each turn
        if (shouldPause)
            waitForEnter();
    }

    // Set t1 to the winnder
    if (b1.allShipsDestroyed())
        t1 = p2;
    else
        t1 = p1;
    
    // Output name and return winner
    cout << t1->name() << " wins!" << endl;
    return t1;
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1  ||  nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1  ||  nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows()  &&  length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
        << endl;
        return false;
    }
    if (!isascii(symbol)  ||  !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
        << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
        << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
            << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr  ||  p2 == nullptr  ||  nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}
