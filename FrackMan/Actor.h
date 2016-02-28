#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameController.h"
#include <vector>
using namespace std;


class StudentWorld;

class Actor: public GraphObject
{
public:
    Actor(int imageID, int startX, int startY, StudentWorld* StudentWorld, Direction dir = right, double size = 1.0, unsigned int depth = 0)
    :GraphObject(imageID, startX, startY, dir, size, depth), m_StudentWorld(StudentWorld), m_dead(false)
    {
        setVisible(true);
    }
    virtual ~Actor(){};
    StudentWorld* getStudentWorld() {return m_StudentWorld;}
    virtual void doSomething() = 0;
    virtual bool isBoulder() {return false;}
    virtual bool isBuried() {return false;}
    virtual bool isActorsCanAttack() {return false;}
    bool isDead() {return m_dead;}
    void setDead() {m_dead = true;}
    virtual void Bouldered() {return;}
    virtual void attacked() {return;}
    virtual void activateLeaveField() {return;}
    virtual void setPickUpableByFrackman() {return;}
    virtual bool isLeaveFieldState() {return false;}
    virtual bool isOil() {return false;}
    virtual void reactionToGold() {return;}
    virtual bool isStunned() {return false;}
private:
    StudentWorld* m_StudentWorld;
    bool m_dead;
};


class Dirt: public Actor
{
public:
    Dirt(int startX, int startY, StudentWorld* sw)
    : Actor(IID_DIRT, startX, startY, sw , right , 0.25, 3)
    {}
    ~Dirt() {}
    virtual void doSomething() {return;}
    
};


/* ------------- ActorsCanAttack -------------- */

class ActorsCanAttack: public Actor
{
public:
    ActorsCanAttack(int imageID, int startX, int startY, StudentWorld* sw, Direction dir)
    :Actor(imageID, startX, startY, sw, dir), m_hitPoints(0)
    {}
    virtual ~ActorsCanAttack() {}
    virtual void Bouldered() {m_hitPoints = 0;}
    void setHitPoints(int hitPoints) {m_hitPoints = hitPoints;}
    int getHitPoints() {return m_hitPoints;}
    virtual void attacked() {m_hitPoints -= 2;}
    virtual bool isActorsCanAttack() {return true;}
    
private:
    int m_hitPoints;
};


class FrackMan: public ActorsCanAttack
{
public:
    FrackMan(StudentWorld* sw)
    :ActorsCanAttack(IID_PLAYER, 30, 60, sw, right), m_gold(0), m_squirt(5), m_sonar(1)
    {
        setHitPoints(10);
    }
    virtual ~FrackMan() {}
    virtual void doSomething();
    void addGold() {m_gold ++;}
    void useGold() {m_gold --;}
    int getGold() {return m_gold;}
    void addWater() {m_squirt += 5;}
    void useWater() {m_squirt --;}
    int getWater() {return m_squirt;}
    void addSonar() {m_sonar ++ ;}
    void useSonar() {m_sonar --;}
    int getSonar() {return m_sonar;}
private:
    int m_gold;
    int m_squirt;
    int m_sonar;
};


class Protesters: public ActorsCanAttack
{
public:
    Protesters(StudentWorld* sw, int imageID = IID_PROTESTER);
    virtual ~Protesters() {}
    virtual void doSomething();
    bool canMoveOneStep(int x, int y, Direction dir);
    bool closeEnoughToFrackman();
    bool canMoveALLTheWay();
    void moveTo(Direction dir);
    Direction getFrackmanDirectionRelativeToMe();
    void setNumToZero() {m_numSquaresInCurrentDirection = 0;}
    void decNumSquares() {m_numSquaresInCurrentDirection--;}
    int getNumSquaresLeft() {return m_numSquaresInCurrentDirection;}
    Direction setNewDirection();
    void setNewNumSquares();
    Direction newDirectionAfterPerpendicularTurn();
    virtual void enableMazeSearch() {canUseMaze = true;}
    bool canUseMazeSearch() {return canUseMaze;}
    bool isFacingFrackman (int x, int y);
    virtual void activateLeaveField() {m_leaveFieldState = true;}
    virtual bool isLeaveFieldState() {return m_leaveFieldState;}
    void walkToExit();
    int shortestPath(int sx, int sy, int ex, int ey);
    virtual void attacked();
    void getStunned() {m_isStunned = true;}
    void stopStun() {m_isStunned = false;}
    virtual bool isStunned() {return m_isStunned;}
    virtual void addSquirtScore();
    virtual void reactionToGold();
    
private:
    int m_tick = 0;
    int m_numSquaresInCurrentDirection;
    bool m_leaveFieldState = false;
    int m_turnTicks = 0;
    bool canUseMaze = false;
    int m_yellTicks = 0;
    string maze[64][64];
    bool m_isStunned = false;
    int m_stunTick = 0;
};


class HardcoreProtesters: public Protesters
{
public:
    HardcoreProtesters(StudentWorld* sw)
    :Protesters(sw, IID_HARD_CORE_PROTESTER)
    {
        setHitPoints(20);
        enableMazeSearch();
    }
    virtual ~HardcoreProtesters() {}
    virtual void addSquirtScore();
    virtual void reactionToGold();
private:
    
};

class Coord
{
public:
    Coord(int x, int y): m_x(x), m_y(y) {}
    int x() const {return m_x;}
    int y() const {return m_y;}
private:
    int m_x;
    int m_y;
};

/* ------------- Boulder ---------------- */

class Boulders: public Actor
{
public:
    Boulders(int startX, int startY, StudentWorld* sw)
    :Actor(IID_BOULDER, startX, startY, sw, down)
    {}
    virtual ~Boulders() {}
    virtual void doSomething()
    {
        if (isDead())
            return;
        move();
    }
    void move();
    bool canMove();
    virtual bool isBoulder() {return true;}
private:
    int m_tickCount = 30;
    bool movingState = false;
};

/*---------------------Squirts--------------------*/


class Squirts: public Actor
{
public:
    Squirts(int startX, int startY, StudentWorld* sw, Direction dir)
    :Actor(IID_WATER_SPURT, startX, startY, sw, dir)
    {}
    virtual ~Squirts(){}
    virtual void doSomething();
    void move(Direction dir);
private:
    int m_travelSquares = 4;
    int m_pauseTick = 0;
};

/*---------------Goodies-------------------*/


class Goodies: public Actor
{
public:
    Goodies(int imageID, int startX, int startY, StudentWorld* sw)
    :Actor(imageID, startX, startY, sw, right, 1.0, 2)
    {}
    virtual ~Goodies() {}
    virtual void doSomething();
    
    virtual void setPickUpableByFrackman() {m_isPickUpableByFrackman = true;}
    void setPickUpableByProtesters() {m_isPickUpableByProtesters = true;}
    
    virtual void playSound();
    
    virtual bool isPickUpableByFrackMan()
    {return m_isPickUpableByFrackman;}
    bool isPickUpableByProtesters()
    {return m_isPickUpableByProtesters;}
    virtual int getCount() {return 0;}
    virtual void incCount() {return;}
    virtual bool isActive() {return m_isActive;}
    virtual void deactivate() {m_isActive = false;}
private:
    virtual void addScore() = 0;
    virtual void addToInventory() = 0;
    bool m_isPickUpableByFrackman = false;
    bool m_isPickUpableByProtesters = false;
    bool m_isActive = true;
};

class TemporaryGoodies: public Goodies
{
public:
    TemporaryGoodies(int imageID, int startX, int startY, StudentWorld* sw)
    :Goodies(imageID, startX, startY, sw), m_tick(0)
    {}
    virtual bool isPickUpableByFrackMan();
    ~TemporaryGoodies() {}
private:
    int m_tick;
    
};

class BarrelsOfOil: public Goodies
{
public:
    BarrelsOfOil(int startX, int startY, StudentWorld* sw)
    :Goodies(IID_BARREL, startX, startY, sw)
    {
        setVisible(false);
    }
    virtual void playSound();
    virtual bool isBuried() {return true;}
    virtual bool isOil() {return true;}
private:
    virtual void addScore();
    virtual void addToInventory() {return;}
    
};

class GoldNugget: public Goodies
{
public:
    GoldNugget(int startX, int startY, StudentWorld* sw, bool isVisible)
    :Goodies(IID_GOLD, startX, startY, sw)
    {
        setVisible(isVisible);
        if (GraphObject::isVisible() == true)
            setPickUpableByProtesters();
    }
    virtual bool isBuried() {return true;}
    virtual int getCount() {return m_count;}
    virtual void incCount() {m_count ++;}
private:
    virtual void addScore();
    virtual void addToInventory();
    int m_count = 0;
};

class Sonarkit: public TemporaryGoodies
{
public:
    Sonarkit(int startX, int startY, StudentWorld* sw)
    :TemporaryGoodies(IID_SONAR, startX, startY, sw)
    {}
    ~Sonarkit() {}
private:
    virtual void addScore();
    virtual void addToInventory();
};

class WaterPool: public TemporaryGoodies
{
public:
    WaterPool(int startX, int startY, StudentWorld* sw)
    :TemporaryGoodies(IID_WATER_POOL, startX, startY, sw)
    {}
    ~WaterPool() {}
private:
    virtual void addScore();
    virtual void addToInventory();
};

#endif // ACTOR_H_


