#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <stack>

using namespace std;

/**
    Remove a specified point from a vector of points
 
    @param1 p The to-be-removed point
    @param2 v The vector to remove the point from
 */
void removePoint(Point p, vector<Point>& v)
{
    for (int i = 0; i < v.size(); i++)
    {
        if (v[i].r == p.r && v[i].c == p.c)
            v.erase(v.begin() + i);
    }
}

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
public:
    // Constructor
    AwfulPlayer(string nm, const Game& g);
    
    // Destructror
    virtual ~AwfulPlayer() {}
    
    // Other
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId) { /* do nothing */ }
    virtual void recordAttackByOpponent(Point p) { /* do nothing */ }
private:
    Point m_lastCellAttacked;
};

/**
    Awful Player constructor
 */
AwfulPlayer::AwfulPlayer(string nm, const Game& g)
: Player(nm, g), m_lastCellAttacked(0, 0)
{}

/**
    placeShips for Awful Player
 
    @param1 b The board to place the ship on
    Awful Players systematically place ships in a cluster
 */
bool AwfulPlayer::placeShips(Board& b)
{
    // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
            return false;
    return true;
}

/**
    recommendAttack for Awful Player
 
    Awful Players systematically attack each row starting at the bottom
 */
Point AwfulPlayer::recommendAttack()
{
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}

class HumanPlayer : public Player
{
public:
    // Constructor
    HumanPlayer(string nm, const Game& g) : Player(nm, g) {}
    
    // Destructor
    virtual ~HumanPlayer() {}
    
    // Accessor
    virtual bool isHuman() const { return true; }
    
    // Other
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId) { /* do nothing */ }
    virtual void recordAttackByOpponent(Point p) { /* do nothing */ }
};

/**
    placeShips for Human Player
 
    @param1 b The board to place the ship
    Accepts user input to place the ship
 */
bool HumanPlayer::placeShips(Board& b)
{
    string d;
    bool valid = false;
    int r = -1, c = -1;
    Direction dir = HORIZONTAL;
    // Continue prompting user until all ships are placed
    for (int i = 0; i < game().nShips(); i++)
    {
        cout << Player::name() << " must place " << game().nShips()-i << " ship";
        if (game().nShips()-i > 1)
            cout << "s.";
        else
            cout << ".";
        cout << endl;
        b.display(false);
        // Prompt user for direction until valid
        while (!valid)
        {
            cout << "Enter h or v for direction of " << game().shipName(i) << " (length " << game().shipLength(i) << "): ";
            cin >> d;
            if (d != "h" && d != "v")
                cout << "Direction must be h or v." << endl;
            else
                valid = true;
            // Clear buffer
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        valid = false;
        if (d == "v")
            dir = VERTICAL;
        // Prompt user for point of placement until valid
        while (!valid)
        {
            cout << "Enter row and column of ";
            if (dir == VERTICAL)
                cout << "topmost";
            else
                cout << "leftmost";
            cout << " cell (e.g. 3 5): ";
            cin >> r >> c;
            if (!b.placeShip(Point(r,c), i, dir))
                cout << "The ship cannot be placed there." << endl;
            else
                valid = true;
            // Clear buffer
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        valid = false;
    }
    return true;
}

/**
    recommendAttack for Human Player
 
    Accepts user input for where to attack
 */
Point HumanPlayer::recommendAttack()
{
    int r, c;
    cout << "Enter the row and column to attack (e.g. 3 5): ";
    cin >> r >> c;
    Point p(r, c);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return p;
}

//*********************************************************************
//  MediocrePlayer
//*********************************************************************

class MediocrePlayer : public Player
{
public:
    // Constructor
    MediocrePlayer(string nm, const Game& g);
    
    // Destructor
    ~MediocrePlayer() {}
    
    // Other
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p) { /* do nothing */ }
    
    // Helper functions
    bool auxPlaceShips(Board& b, int shipsLeft, int r, int c, int id, bool backTrack, vector<Point> added, vector<Direction> dirs);
    Point calculateShot();
    void buildCalculatedPoints(Point p);
    
private:
    // Stores last cell HIT -- only set when a shot hits
    Point m_lastCellHit;
    // Stores state of player -- two states: randomly firing and calculated firing
    int m_state;
    // Stores the points available on the board to shoot
    vector<Point> m_points;
    // Stores the calculated points available when set to state 2
    vector<Point> m_calculatedPoints;
    // Stores history of shots -- misses and hits
    vector<vector<char> > m_hist;
    // Allows the player to know when to build the calculated points
    bool buildCPoints;
};

/**
    Mediocre Player Constructor
 
    Initializes m_points to all the points on the board
    Initializes m_hist to blank i.e. all '.'s
 */
MediocrePlayer::MediocrePlayer(string nm, const Game& g)
: Player(nm, g), m_state(1), m_lastCellHit(0, 0), m_calculatedPoints({}), buildCPoints(false)
{
    m_hist.resize(game().rows());
    for (int r = 0; r < game().rows(); r++)
    {
        m_hist[r].resize(game().cols());
        for (int c = 0; c < game().cols(); c++)
        {
            m_points.push_back(Point(r,c));
            m_hist[r][c] = '.';
        }
    }
}

/**
    placeShips for Mediocre Player
 
    @param1 b The board to place ships on
    Mediocre Players block ~50% of the map before placing ships
    Use recursive algorithm to place the ships
    Unblocks the board before returning
 */
bool MediocrePlayer::placeShips(Board& b)
{
    bool valid = false;
    int counter = 0;
    // Attempt to place ships 50 times
    while (!valid && counter < 50)
    {
        // Block ~50% of board before placement
        b.block();
        valid = auxPlaceShips(b, game().nShips(), 0, 0, 0, false, {}, {});
        // Unblock board after attempting to place
        b.unblock();
        counter++;
    }
    return valid;
}

/**
    Auxillary function for Mediocre Player to place ships
 
    @param1 b The board to place ships on
    @param2 shipsLeft The number of ships left to be placed
    @param3 r The starting row for the algorithm
    @param4 c The starting column for the algorithm
    @param5 id The id of the ship currently being placed
    @param5 backTrack True if ship does not fit and needs to unplace the last ship placed
    @param6 added Vector storing the topmost of leftmost points of ships added to the board
    @param7 dirs Vector storing the directionns of ships added to the board
 
    This is a recursive function that places ships on the board. It recursively iterates through the board
    attempting to place ships (either HORIZONTALLy or VERTICALLY) whenever possible. If it reaches the end
    of the board and all ships have not yet been placed, it backTracks by unplacing the last placed ship.
    It will do this until it unplaces the first ship if need be. This is all performed by recursively calling
    itself with updated parameters.
 */
bool MediocrePlayer::auxPlaceShips(Board& b, int shipsLeft, int r, int c, int id, bool backTrack, vector<Point> added, vector<Direction> dirs)
{
    // Set to true if ship is placed false otherwise
    bool valid;
    
    // Base Case 1: If all ships placed our job is done
    if (shipsLeft == 0) return true;
    // If c exceeds game columns set it to 0 and increment row count
    if (c > game().cols()-1)
    {
        c = 0;
        r++;
    }
    // Base Case 2: If r exceeds game rows start backtracking
    if (r > game().rows()-1 && c == 0) backTrack = true;
    // Base Case 3: If no ships in added and backtrack is true return false -- no viable arrangment for ships
    if (added.size() == 0 && backTrack) return false;
    
    // If not backtracking
    if (!backTrack)
    {
        // Attempt to place ship horizontally
        valid = b.placeShip(Point(r, c), id, HORIZONTAL);
        // If successfully placed horizontally
        if (valid)
        {
            // Add point of placement
            added.push_back(Point(r,c));
            // Add direction or placedment
            dirs.push_back(HORIZONTAL);
            // Recursively call with adjusted parameters
            return auxPlaceShips(b, shipsLeft-1, 0, 0, id+1, backTrack, added, dirs);
        }
        // Cannot place ship horizontally
        else
        {
            // Attempt to place ship vertically
            valid = b.placeShip(Point(r, c), id, VERTICAL);
            // If successfully placed vertically
            if (valid)
            {
                // Add points of placement
                added.push_back(Point(r,c));
                // Add direction of placement
                dirs.push_back(VERTICAL);
                // Recursively call with adjusted parameters
                return auxPlaceShips(b, shipsLeft-1, 0, 0, id+1, backTrack, added, dirs);
            }
            // Not able to place horizontally or vertically
            else
            {
                // Recursively call with updated parameters -- only increment column
                return auxPlaceShips(b, shipsLeft, r, c+1, id, backTrack, added, dirs);
            }
        }
    }
    else // backtracking
    {
        // Get row and column of last added ship
        r = added.back().r;
        c = added.back().c;
        // Get direction of last added ship
        Direction dir = dirs.back();
        // Remove ship from added
        added.pop_back();
        // Remove direciton from added
        dirs.pop_back();
        // Unplace ship
        valid = b.unplaceShip(Point(r, c), id-1, dir);
        // No longer backtracking
        backTrack = false;
        // Recursively call with updated parameters -- increment shipsLeft, columns, decrement id
        return auxPlaceShips(b, shipsLeft+1, r, c+1, id-1, backTrack, added, dirs);
    }
}

/**
    Recommends attack for a mediocre player
 
    This function recommends an attack for the mediocre player depending on its state.
        State 1: randomly selects a point left on our map i.e. in the m_points vector
        State 2: randomly selects a point from calculated points vector. The calculated
    points vector is initialized with points that are within either 4 rows, or 4 columns
    of the last hit point.
 */
Point MediocrePlayer::recommendAttack()
{
    // m_points should not be empty
    if (m_points.empty())
        cerr << "Error MediocrePlayer::recommendAttack() -- someone should have one" << endl;
    // Randomly select point on board to shoot
    if (m_state == 1)
    {
        // Randomly select point from points
        int i = randInt(m_points.size());
        Point p(m_points[i].r, m_points[i].c);
        removePoint(p, m_points);
        return p;
    }
    // Select point randomly from calculated points
    else // state 2
    {
        Point p = calculateShot();
        removePoint(p, m_points);
        return p;
    }
}

/**
    Records attack results for Mediocre Player
 
    @param1 p The point of the last shot
    @param2 validShot True of the last shot is valid i.e. in bounds and not already shot
    @param3 shotHit True if the last shot hit a ship
    @param4 shipDestroyed True if the last shot destroyed a ship
    @param5 shipId The id of a ship that is hit/destroyed
 */
void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
    // If shot hit mark it in players data members
    if (shotHit)
        m_hist[p.r][p.c] = 'X';
    else
        m_hist[p.r][p.c] = 'o';
    
    if (!validShot)
        cerr << "Error MediocrePlayer::recordAttackResult -- computer should not be shooting invalid shots" << endl;
    
    // Switch to state 2 if shot hit but ship was not destroyed
    if (m_state == 1)
    {
        if (shotHit && !shipDestroyed)
        {
            m_state = 2;
            m_lastCellHit = p;
            buildCPoints = true;
        }
    }
    // Switch to state 1 if shot hit and ship was destroyed
    else // state 2
    {
        if (shotHit && shipDestroyed)
            m_state = 1;
    }
}

/**
    Calculates shot for Mediocre Player -- helper function
 
    This function runs when the player is in state 2
    @returns Point for player to attack based on where the shot hit a ship
 */
Point MediocrePlayer::calculateShot()
{
    if (buildCPoints)
        buildCalculatedPoints(m_lastCellHit);
    int i = randInt(m_calculatedPoints.size());
    Point r(m_calculatedPoints[i].r, m_calculatedPoints[i].c);
    removePoint(r, m_calculatedPoints);
    if (m_calculatedPoints.empty())
        m_state = 1;
    return r;
}

/**
    Builds m_calculatedPoints vector for Mediocre Player -- helper function
 
    This function fills the vector with calculated points for the player to attack
 */
void MediocrePlayer::buildCalculatedPoints(Point p)
{
    // Clear old points before adding new ones
    m_calculatedPoints.clear();
    // Check points for validity and add them to vector
    for (int d = 1; d < 5; d++)
    {
        if (p.r-d >= 0 && m_hist[p.r-d][p.c] == '.')
            m_calculatedPoints.push_back(Point(p.r-d, p.c));
        if (p.r+d <= game().rows()-1 && m_hist[p.r+d][p.c] == '.')
            m_calculatedPoints.push_back(Point(p.r+d, p.c));
        if (p.c-d >= 0 && m_hist[p.r][p.c-d] == '.')
            m_calculatedPoints.push_back(Point(p.r, p.c-d));
        if (p.c+d <= game().cols()-1 && m_hist[p.r][p.c+d] == '.')
            m_calculatedPoints.push_back(Point(p.r, p.c+d));
    }
    buildCPoints = false;
}



//*********************************************************************
//  GoodPlayer
//*********************************************************************

class GoodPlayer : public Player
{
public:
    // Constructor
    GoodPlayer(string nm, const Game& g);
    
    // Destructor
    ~GoodPlayer() {}
    
    // Other
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p) { /* do nothing */ }
    
    // Helpers
    void addAttackPoints(Point p);
    
private:
    // Stores points left on the board
    vector<Point> m_points;
    // State of the player -- randomly firing and shooting surrounding cells
    int m_state;
    // Stack storing the points surrounding a hit attack
    stack<Point> m_attackPoints;
    // Stores history of shots -- misses and hits
    vector<vector<char> > m_hist;
};

/** 
    GoodPlayer Constructor
 
    Initializes m_hist to empty board and m_points with all points on the board
 */
GoodPlayer::GoodPlayer(string nm, const Game& g)
: Player(nm, g), m_state(1)
{
    m_hist.resize(game().rows());
    for (int r = 0; r < game().rows(); r++)
    {
        m_hist[r].resize(game().cols());
        for (int c = 0; c < game().cols(); c++)
        {
            m_points.push_back(Point(r,c));
            m_hist[r][c] = '.';
        }
    }
}

/**
    placeShips for Good Player
 
    @param1 b The board to place ships on
 */
bool GoodPlayer::placeShips(Board& b)
{
    int id = 0;
    bool valid;
    int shipsLeft = game().nShips();
    while (shipsLeft > 0)
    {
        int i = randInt(m_points.size());
        Point p(m_points[i].r, m_points[i].c);
        valid = b.placeShip(p, id, HORIZONTAL);
        if (!valid)
            valid = b.placeShip(p, id, VERTICAL);
        if (valid)
        {
            removePoint(p, m_points);
            shipsLeft--;
            id++;
        }
    }
    // Once ships are placed clear points and reinitiate them for attacking phase
    m_points.clear();
    for (int r = 0; r < game().rows(); r++)
        for (int c = 0; c < game().cols(); c++)
            m_points.push_back(Point(r,c));
    return true;
}

/**
    recommendAttack for Good Player
 
    If m_state == 1 selects a random point left on the board to attack
    If m_state == 2 selects point from stack to attack
 */
Point GoodPlayer::recommendAttack()
{
    // Randomly select one of the points left
    if (m_state == 1)
    {
        // Randomly select point from points
        int i = randInt(m_points.size());
        Point p(m_points[i].r, m_points[i].c);
        // Remove the selected point from points remaining
        removePoint(p, m_points);
        return p;
    }
    // Attack the next point on the stack
    else // m_state == 2
    {
        Point attack;
        if (!m_attackPoints.empty())
            attack = m_attackPoints.top();
        // Make sure stack is not empty
        else
            cerr << "Error GoodPlayer::recomendAttack -- stack should not be empty" << endl;
        m_attackPoints.pop();
        // Remove the selected point from points remaining
        removePoint(attack, m_points);
        return attack;
    }
}

/**
    recordAttackResult for Good Player
 
    @param1 p The point last attacked
    @param2 validShot True if the last attack is valid
    @param3 shotHit True if the last attack hit a ship
    @param4 shipDestroyed True if the last attack destroyed a ship
    @param5 shipId Id of the ship last hit
 */
void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
    // Check if shot was valid
    if (!validShot)
        cerr << "Error GoodPlayer::recordAttackResult -- computer should not be shooting invalid shots" << endl;
    
    // If shot hit mark it and add to the stack
    if (shotHit)
    {
        m_hist[p.r][p.c] = 'X';
        addAttackPoints(p);
    }
    // Mark if shot did not hit
    else
        m_hist[p.r][p.c] = 'o';
    
    // Switch to state 2 if shot hit
    if (m_state == 1)
    {
        if (shotHit) m_state = 2;
    }
    // Switch to state 1 if stack is empty
    else // m_state == 2
    {
        if (m_attackPoints.empty())
            m_state = 1;
    }
}

/**
    Adds attack points to stack for Good Player
 
    @param1 p The point last attacked
    When a shot hits the four surrounding cells are added to the stack to attack next
 */
void GoodPlayer::addAttackPoints(Point p)
{
    // If cell above p is valid add it to the stack
    if (p.r-1 >= 0 && m_hist[p.r-1][p.c] == '.')
    {
        m_hist[p.r-1][p.c] = 'a';
        m_attackPoints.push(Point(p.r-1, p.c));
    }
    // If cell below p is valid add it to the stack
    if (p.r+1 <= game().rows()-1 && m_hist[p.r+1][p.c] == '.')
    {
        m_hist[p.r+1][p.c] = 'a';
        m_attackPoints.push(Point(p.r+1, p.c));
    }
    // If cell to the left of p is valid add it to the stack
    if (p.c-1 >= 0 && m_hist[p.r][p.c-1] == '.')
    {
        m_hist[p.r][p.c-1] = 'a';
        m_attackPoints.push(Point(p.r, p.c-1));
    }
    // If cell to the right of p is valid add it to the stack
    if (p.c+1 <= game().cols()-1 && m_hist[p.r][p.c+1] == '.')
    {
        m_hist[p.r][p.c+1] = 'a';
        m_attackPoints.push(Point(p.r, p.c+1));
    }
}

//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };
    
    int pos;
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
         type != types[pos]; pos++)
        ;
    switch (pos)
    {
        case 0:  return new HumanPlayer(nm, g);
        case 1:  return new AwfulPlayer(nm, g);
        case 2:  return new MediocrePlayer(nm, g);
        case 3:  return new GoodPlayer(nm, g);
        default: return nullptr;
    }
}
