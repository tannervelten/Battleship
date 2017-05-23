#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <vector>
#include <map>

using namespace std;

class BoardImpl
{
public:
    // Constructor
    BoardImpl(const Game& g);
    
    // Other
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const { return m_shipsInPlay.empty(); }
    
private:
    vector<vector<char> > m_board;
    map<int, int> m_shipsInPlay;
    const Game& m_game;
};

/**
    BoardImpl constructor
 
    @param1 g Game object which holds rows, columns, and other pieces of game info
    Utilizes a 2D vector to store the board
    Utilizes an unordered set to store ship ids added to the board
 */
BoardImpl::BoardImpl(const Game& g)
: m_game(g), m_shipsInPlay({})
{
    // Resize the outer vector to the number of rows in the game
    m_board.resize(g.rows());
    for (int r = 0; r < g.rows(); r++)
    {
        // Resize the inner vector to the number of columns in the game
        m_board[r].resize(g.cols());
        for (int c = 0; c < g.cols(); c++)
        {
            m_board[r][c] = '.';
        }
    }
}

/**
    Clears the board
    Iterates through board and sets cells back to .
 */
void BoardImpl::clear()
{
    for (int r = 0; r < m_game.rows(); r++)
    {
        for (int c = 0; c < m_game.cols(); c++)
        {
            m_board[r][c] = '.';
        }
    }
}

/**
    Blocks the board
    Marks ~50% of the board as blocked by randomly placing X's
    Only used by mediocre player
 */
void BoardImpl::block()
{
    // Block cells with 50% probability
    for (int r = 0; r < m_game.rows(); r++)
    {
        for (int c = 0; c < m_game.cols(); c++)
        {
            if (randInt(2) == 0)
            {
                m_board[r][c] = 'X';
            }
        }
    }
}

/**
    Unblocks the board
    Iterates through board and sets X's back to .'s
    Only used by mediocre player
 */
void BoardImpl::unblock()
{
    for (int r = 0; r < m_game.rows(); r++)
    {
        for (int c = 0; c < m_game.cols(); c++)
        {
            if (m_board[r][c] == 'X')
            {
                m_board[r][c] = '.';
            }
        }
    }
}

/**
    Places a ship on the board
 
    @param1 topOrLeft The coordinate of the topmost of leftmost segment of the ship
    @param2 shipId The id of the ship being placed
    @param3 dir The orientation of the ship being placed -- VERTICAL or HORIZONTAL
    @return True if the ship is successfully placed else false
 */
bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    if (shipId < 0 || shipId > m_game.nShips() - 1)
        return false;
    if (topOrLeft.r < 0 || topOrLeft.r > m_game.rows() - 1 || topOrLeft.c < 0 || topOrLeft.c > m_game.cols() - 1)
        return false;
    if (m_shipsInPlay.find(shipId) != m_shipsInPlay.end())
        return false;
    int len = m_game.shipLength(shipId);
    if (dir == HORIZONTAL)
    {
        Point left = topOrLeft;
        if (left.c + len > m_game.cols())
            return false;
        for (int i = 0; i < len; i++)
        {
            if (m_board[left.r][left.c+i] != '.')
                return false;
        }
        for (int i = 0; i < len; i++)
            m_board[left.r][left.c+i] = shipId+'0';
    }
    else // dir == VERTICAL
    {
        Point top = topOrLeft;
        if (top.r + len > m_game.rows())
            return false;
        for (int i = 0; i < len; i++)
        {
            if (m_board[top.r+i][top.c] != '.')
                return false;
        }
        for (int i = 0; i < len; i++)
            m_board[top.r+i][top.c] = shipId+'0';
    }
    m_shipsInPlay.insert(make_pair(shipId, m_game.shipLength(shipId)));
    return true;
}

/** Remove a ship on the board
 
    @param1 topOrLeft The coordinate of the topmost of leftmost segment of the ship
    @param2 shipId The id of the ship being removed
    @param3 dir The orientation of the ship being removed -- VERTICAL or HORIZONTAL
    @return True if the ship is successfully removed else false
 */
bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    if (shipId < 0 || shipId > m_game.nShips() - 1)
        return false;
    if (dir == VERTICAL && topOrLeft.r + m_game.shipLength(shipId) > m_game.rows())
        return false;
    if (dir == HORIZONTAL && topOrLeft.c + m_game.shipLength(shipId) > m_game.cols())
        return false;
    if (topOrLeft.r < 0 || topOrLeft.r > m_game.rows() - 1 || topOrLeft.c < 0 || topOrLeft.c > m_game.cols() - 1)
        return false;
    if (m_shipsInPlay.find(shipId) == m_shipsInPlay.end())
        return false;
    int len = m_game.shipLength(shipId);
    if (dir == HORIZONTAL)
    {
        Point left = topOrLeft;
        for (int i = 0; i < len; i++)
        {
            if (m_board[left.r][left.c+i]-48 != shipId)
                return false;
        }
        for (int i = 0; i < len; i++)
            m_board[left.r][left.c+i] = '.';
    }
    else // dir == VERTICAL
    {
        Point top = topOrLeft;
        for (int i = 0; i < len; i++)
        {
            if (m_board[top.r+i][top.c]-48 != shipId)
                return false;
        }
        for (int i = 0; i < len; i++)
            m_board[top.r+i][top.c] = '.';
    }
    m_shipsInPlay.erase(shipId);
    return true;
}

/** Displays the board
 
    @param1 shotsOnly If true board only displays both missed and hit shots
 */
void BoardImpl::display(bool shotsOnly) const
{
    // Output top row
    cout << "  ";
    for (int n = 0; n < m_game.cols(); n++)
        cout << n;
    cout << endl;
    
    // Output remainder of the board
    for (int r = 0; r < m_game.rows(); r++)
    {
        cout << r << " ";
        for (int c = 0; c < m_game.cols(); c++)
        {
            if (m_board[r][c] == '.' || m_board[r][c] == 'o' || m_board[r][c] == 'X')
                cout << m_board[r][c];
            else // board cell contains an unattacked boat segment
            {
                if (shotsOnly)
                    cout << '.';
                else
                    cout << m_game.shipSymbol(m_board[r][c]-48);
            }
        }
        cout << endl;
    }
}

/**
    Attacks coordinate on the board
 
    @param1 p Point to attack
    @param2 shotHit Set to true if shot hits a ship
    @param3 shipDestroyed Set to true of a ship is destroyed
    @param4 shipId Set to shipId of ship hit
    @return True if shot is valid -- meaning point is inbounds and has not already been shot
 */
bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    if (p.r < 0 || p.r > m_game.rows() || p.c < 0 || p.c > m_game.rows())
    {
        // Used to let Game::play() know user wasted shot
        shipId = -1;
        return false;
    }
    if (m_board[p.r][p.c] == 'o' || m_board[p.r][p.c] == 'X')
    {
        shipId = -1;
        return false;
    }
    else if (m_board[p.r][p.c] == '.')
    {
        shotHit = false;
        shipDestroyed = false;
        m_board[p.r][p.c] = 'o';
    }
    else // hit a ship
    {
        shotHit = true;
        shipId = m_board[p.r][p.c]-48;
        m_board[p.r][p.c] = 'X';
        m_shipsInPlay[shipId]--;
        if (m_shipsInPlay[shipId] == 0)
        {
            shipDestroyed = true;
            m_shipsInPlay.erase(shipId);
        }
    }
    return true;
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}
