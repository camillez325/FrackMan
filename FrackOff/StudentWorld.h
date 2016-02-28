#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
using namespace std;


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    virtual ~StudentWorld();
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    int randInt(int min, int max);
    bool removeDirt(int x, int y);
    vector<Actor*>& getVector() {return m_actors;}
    FrackMan* getFrackMan() {return m_frackman;}
    int min(int a, int b);
    int max(int a, int b);
    bool isValidPosition(int x, int y);
    bool isValidDistance(int x1, int y1, int x2, int y2);
    void getValidCoord(int &x, int &y);
    
    void setDisplayText();
    string formatText(int socre, int level, int lives, int health, int squirts, int gold, int sonar, int barrelsLeft);
    
    bool isDirt(int x, int y);
    bool isSquareDirt(int x, int y);
    bool isBoulder(int x, int y);
    bool canMoveOneStep(int x, int y, Actor::Direction dir);
    bool twoSquaresOverlap(int x1, int y1, int x2, int y2);
    double distanceBetweenTwoPoints(int x1, int y1, int x2, int y2);
    int oilLeft();
    void illuminate();
    
    void addGold();
    bool canPlaceWater(int x, int y);
    int numProtesters();
    bool protestersCanMoveOneStep(int x, int y, Actor::Direction dir);
    void BoulderCauseDamage(int x, int y);
    bool SquirtProtesters(int x, int y);
    bool canBribe(int x, int y);
    void createSquirt(Actor::Direction dir);

private:
    vector<Actor*> m_actors;
    FrackMan* m_frackman;
    Dirt* m_dirt[VIEW_WIDTH][VIEW_HEIGHT];
    int m_tick = 0;
    
};


#endif // STUDENTWORLD_H_
