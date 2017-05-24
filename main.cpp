#include "Game.h"
#include "Player.h"
#include <iostream>
#include <string>
#include <vector>

// not in original skeleton
#include "Board.h"
#include <cassert>
#include <unordered_set>
#include <map>
#include <algorithm>
#include <stdlib.h>

using namespace std;

bool addStandardShips(Game& g)
{
    return g.addShip(5, 'A', "aircraft carrier")  &&
    g.addShip(4, 'B', "battleship")  &&
    g.addShip(3, 'D', "destroyer")  &&
    g.addShip(3, 'S', "submarine")  &&
    g.addShip(2, 'P', "patrol boat");
}

int main()
{
    Game g(10, 10);
//    g.addShip(5, 'A', "aircraft carrier");
//    g.addShip(2, 'L', "latrol boat");
//    g.addShip(3, 'S', "submarine");
    addStandardShips(g);
    Player* p1 = createPlayer("good", "Conman", g);
    Player* p2 = createPlayer("mediocre", "Tanman", g);
    if (g.play(p1, p2, false) == nullptr) cout << "worked" << endl;
    delete p1;
    delete p2;
}

//int main()
//{
//    const int NTRIALS = 10;
//    
//    cout << "Select one of these choices for an example of the game:" << endl;
//    cout << "  1.  A mini-game between two mediocre players" << endl;
//    cout << "  2.  A mediocre player against a human player" << endl;
//    cout << "  3.  A " << NTRIALS
//    << "-game match between a mediocre and an awful player, with no pauses"
//    << endl;
//    cout << "Enter your choice: ";
//    string line;
//    getline(cin,line);
//    if (line.empty())
//    {
//        cout << "You did not enter a choice" << endl;
//    }
//    else if (line[0] == '1')
//    {
//        Game g(2, 3);
//        g.addShip(2, 'R', "rowboat");
//        Player* p1 = createPlayer("mediocre", "Popeye", g);
//        Player* p2 = createPlayer("mediocre", "Bluto", g);
//        cout << "This mini-game has one ship, a 2-segment rowboat." << endl;
//        g.play(p1, p2);
//        delete p1;
//        delete p2;
//    }
//    else if (line[0] == '2')
//    {
//        Game g(10, 10);
//        addStandardShips(g);
//        Player* p1 = createPlayer("mediocre", "Mediocre Midori", g);
//        Player* p2 = createPlayer("human", "Shuman the Human", g);
//        g.play(p1, p2);
//        delete p1;
//        delete p2;
//    }
//    else if (line[0] == '3')
//    {
//        int nMediocreWins = 0;
//        
//        for (int k = 1; k <= NTRIALS; k++)
//        {
//            cout << "============================= Game " << k
//            << " =============================" << endl;
//            Game g(10, 10);
//            addStandardShips(g);
//            Player* p1 = createPlayer("awful", "Awful Audrey", g);
//            Player* p2 = createPlayer("mediocre", "Mediocre Mimi", g);
//            Player* winner = (k % 2 == 1 ?
//                              g.play(p1, p2, false) : g.play(p2, p1, false));
//            if (winner == p2)
//                nMediocreWins++;
//            delete p1;
//            delete p2;
//        }
//        cout << "The mediocre player won " << nMediocreWins << " out of "
//        << NTRIALS << " games." << endl;
//        // We'd expect a mediocre player to win most of the games against
//        // an awful player.  Similarly, a good player should outperform
//        // a mediocre player.
//    }
//    else
//    {
//        cout << "That's not one of the choices." << endl;
//    }
//}

//int main()
//{
//    Point p(0, 1);
//    Game g(10, 10);
//    addStandardShips(g);
//    Board b(g);
//    bool shot, shipD;
//    int shipid;
//
//    b.placeShip(Point(0,0), 0, HORIZONTAL);
////    b.placeShip(Point(1,0), 1, VERTICAL);
//    b.placeShip(Point(6,4), 2, HORIZONTAL);
////    b.placeShip(Point(9,0), 3, HORIZONTAL);
////    b.placeShip(Point(1,9), 4, VERTICAL);
//
//    b.attack(Point(0,0), shot, shipD, shipid);
//    b.attack(Point(0,1), shot, shipD, shipid);
//    b.attack(Point(1,1), shot, shipD, shipid);
//    b.attack(Point(1,0), shot, shipD, shipid);
//    b.attack(Point(2,0), shot, shipD, shipid);
//    b.attack(Point(3,0), shot, shipD, shipid);
//    b.attack(Point(4,0), shot, shipD, shipid);
//    b.attack(Point(0,2), shot, shipD, shipid);
//    b.attack(Point(0,4), shot, shipD, shipid);
//    b.attack(Point(0,3), shot, shipD, shipid);
//
//    b.attack(Point(6,4), shot, shipD, shipid);
//    b.attack(Point(6,5), shot, shipD, shipid);
////    b.attack(Point(6,6), shot, shipD, shipid);
//
////    b.attack(Point(4,0), shot, shipD, shipid);
////    b.attack(Point(1,0), shot, shipD, shipid);
////    b.attack(Point(1,0), shot, shipD, shipid);
//
//
//    b.display(true); cout << endl;
//    b.display(false);
//
//    if (b.allShipsDestroyed())
//        cout << "Game over" << endl;
//}


































