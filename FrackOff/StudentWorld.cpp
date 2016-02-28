#include "StudentWorld.h"
#include <string>
#include <random>
#include <vector>
#include <cmath>
using namespace std;

int StudentWorld::randInt(int min, int max)
{
    return min + (rand() % (int)(max-min+1));
}

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(std::string assetDir)
: GameWorld(assetDir)
{
    m_frackman = NULL;
    for (int i = 0; i < VIEW_WIDTH; i ++)
    {
        for (int j = 0; j < 60; j ++)
        {
            m_dirt[i][j] = NULL;
        }
    }
}

StudentWorld::~StudentWorld()
{
    delete m_frackman;
    for (int i = 0; i < VIEW_WIDTH; i ++)
    {
        for (int j = 0; j < 60; j ++)
        {
            if (m_dirt[i][j] != NULL)
            {
                delete m_dirt[i][j];
                m_dirt[i][j] = NULL;
            }
            
        }
    }
    while (m_actors.size()>0)
    {
        Actor* ap = m_actors.back();
        delete ap;
        m_actors.pop_back();
    }
}



// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
/*--------------------------------INIT-------------------------------*/

int StudentWorld::init()
{
    m_frackman = new FrackMan(this);
    
    int numBoulders = min(getLevel()/2+2, 6);
    int numNuggets = max(5-getLevel()/2, 2);
    int numBarrelsOfOil = min(2+getLevel(), 20);
    
    for (int i = 0; i < VIEW_WIDTH; i++)
    {
        for (int j = 0; j < 60; j++)
        {
            if ( (i >= 30 && i <= 33) && (j>= 4 && j <= 59))
                continue;
            m_dirt[i][j] = new Dirt(i,j,this);
        }
    }
    
    for ( int i = 0; i < numBoulders; i ++)
    {
        int x=0;
        int y=0;
        getValidCoord(x, y);
        m_actors.push_back(new Boulders(x, y, this));
        removeDirt(x, y);
    }
    for ( int j = 0 ; j < numNuggets; j++)
    {
        int x=0;
        int y=0;
        getValidCoord(x, y);
        m_actors.push_back(new GoldNugget(x, y, this, false));
    }
    for (int k = 0; k < numBarrelsOfOil; k++)
    {
        int x=0;
        int y=0;
        getValidCoord(x, y);
        m_actors.push_back(new BarrelsOfOil(x, y, this));
    }
    return 0;
}


bool StudentWorld::removeDirt(int x, int y)
{
    bool dirtToRemove = false;
    for (int i = x; i < x+4 ; i ++)
    {
        for (int j = y; j < y+4; j++)
        {
            if (m_dirt[i][j] != NULL)
            {
                delete m_dirt[i][j];
                m_dirt[i][j] = NULL;
                dirtToRemove = true;
            }
        }
    }
    if (dirtToRemove)
        return true;
    return false;
}

int StudentWorld::min(int a, int b)
{
    if (a<b)
        return a;
    else
        return b;
}

int StudentWorld::max(int a, int b)
{
    if ( a<b )
        return b;
    else
        return a;
}

bool StudentWorld::isValidPosition(int x, int y)
{
    if (( x >= 30-4 && x <= 33) && ( y >= 4-4 && y <= 59-4 ))
        return false;
    if (m_actors.size() == 0)
        return true;
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (!isValidDistance(x, y, m_actors[i]->getX(), m_actors[i]->getY()))
            return false;
    }
    return true;
}

bool StudentWorld::isValidDistance(int x1, int y1, int x2, int y2)
{
    if ((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) >= 36)
        return true;
    return false;
}

void StudentWorld::getValidCoord(int &x, int &y)
{
    x = randInt(0, 60-4);
    y = randInt(20, 56-4);
    while (!isValidPosition(x, y))
    {
        x = randInt(0, 60-4);
        y = randInt(20, 56-4);
    }
    return;
}

/*---------------------------------------MOVE-----------------------------------*/

int StudentWorld::move()
{
    setDisplayText();
    //add sonar or water
    if (randInt(1, getLevel()*25+300)==1)
    {
        if (randInt(1, 5)==1)
        {
            m_actors.push_back(new Sonarkit(0,60,this));
        }
        else
        {
            int x = randInt(0, VIEW_WIDTH);
            int y = randInt(0, VIEW_HEIGHT);
            while (!canPlaceWater(x, y))
            {
                x = randInt(0, VIEW_WIDTH);
                y = randInt(0, VIEW_HEIGHT);
            }
            m_actors.push_back(new WaterPool(x,y,this));
        }
    }
    
    //add protesters
    int T = max(25, 200 - getLevel());
    int P = min(15, 2 + getLevel()*15);
    int probabilityOfHardcore = min(90, getLevel()*10 + 30);
    
    if (m_tick == 0 || (m_tick > T && numProtesters() < P))
    {
        if (randInt(1, 100) <= probabilityOfHardcore)
            m_actors.push_back(new HardcoreProtesters(this));
        else
            m_actors.push_back(new Protesters(this));
        m_tick = 0;
    }
    m_tick++;
    
    //ask actors to do something
    if (!(getFrackMan()->isDead()))
        getFrackMan()->doSomething();
    
    for (int i = 0 ; i < m_actors.size(); i ++)
    {
        if (!(m_actors[i]->isDead()))
        {
            m_actors[i]->doSomething();
            if (getFrackMan()->isDead())
            {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (oilLeft() == 0)
            {
                playSound(SOUND_FINISHED_LEVEL);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }
    for (int k = 0; k < m_actors.size(); k ++ )
    {
        if (m_actors[k]->isDead())
        {
            Actor* deadActor = m_actors[k];
            vector<Actor*>::iterator p = find(m_actors.begin(), m_actors.end(), deadActor);
            delete *p;
            m_actors.erase(p);
        }
    }
    if (getFrackMan()->isDead())
    {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    if (oilLeft() == 0)
    {
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    return GWSTATUS_CONTINUE_GAME;
}

/* ====================Display=========================*/

void StudentWorld::setDisplayText()
{
    int score = getScore();
    int level = getLevel();
    int lives = getLives();
    int health = m_frackman->getHitPoints();
    int squirts = m_frackman->getWater();
    int gold = m_frackman->getGold();
    int sonar = m_frackman->getSonar();
    int barrelsLeft = oilLeft();
    string s = formatText(score, level, lives, health, squirts, gold, sonar, barrelsLeft);
    setGameStatText(s);
}

string StudentWorld::formatText(int score, int level, int lives, int health, int squirts, int gold, int sonar, int barrelsLeft)
{
    string ScrString1 = "00000000";
    string ScrString2 = to_string(score);
    
    int i = ScrString1.size()-1;
    for (int j = ScrString2.size()-1; j >= 0; j--)
    {
        ScrString1[i] = ScrString2[j];
        i--;
    }
    
    string s = "Scr: " + ScrString1 + "  " + "Lvl: " + to_string(level) + "  " + "Lives: " + to_string(lives) + "  " + "Hlth: " + to_string(health) + "0%" + "  " + "Water: " + to_string(squirts) + "  " + "Gld: " + to_string(gold) + "  " + "Sonar: " + to_string(sonar) + "  " + " Oil Left: " + to_string(barrelsLeft);
    return s;
}

//check if one coordinate is dirt
bool StudentWorld::isDirt(int x, int y)
{
    if (m_dirt[x][y] != NULL)
        return true;
    return false;
}
//check if one coordinate is boulder
bool StudentWorld::isBoulder(int x, int y)
{
    for (int i = 0 ; i < m_actors.size(); i++)
    {
        if (m_actors[i]->isBoulder())
        {
            if (distanceBetweenTwoPoints(x, y, m_actors[i]->getX(), m_actors[i]->getY())<= 3.00)
                return true;
        }
    }
    return false;

    
}
//check if one can go one step without going out of screen AND hitting a boulder
bool StudentWorld::canMoveOneStep(int x, int y, Actor::Direction dir)
{
    switch (dir)
    {
        case GraphObject::left:
            if (x-1 >= 0 &&!( isBoulder(x-1, y)))
                return true;
            break;
        case GraphObject::right:
            if (x+1 <= VIEW_WIDTH-4 &&!( isBoulder(x+1, y)))
                return true;
            break;
        case GraphObject::up:
            if (y+1 <= VIEW_HEIGHT-4 &&!( isBoulder(x, y+1)))
                return true;
            break;
        case GraphObject::down:
            if (y-1 >= 0 &&!( isBoulder(x, y-1)))
                return true;
            break;
    }
    return false;
}

bool StudentWorld::twoSquaresOverlap(int x1, int y1, int x2, int y2)
{
    if ( (x1-x2)*(x1-x2)  < 16 && (y1-y2)*(y1-y2) < 16 )
        return true;
    return false;
}

//check if the entire square contains any dirt, from 4 directions.
bool StudentWorld::isSquareDirt(int x, int y)
{
    for (int i = x; i < x+4; i ++)
    {
        for (int j = y; j <y +4; j++)
        {
            if (isDirt(i, j))
            {
                    return true;
            }
        }
    }
    return false;
}

double StudentWorld::distanceBetweenTwoPoints(int x1, int y1, int x2, int y2)
{
    double d = (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
    return sqrt(d);
}

void StudentWorld::addGold()
{
    m_actors.push_back(new GoldNugget(getFrackMan()->getX(), getFrackMan()->getY(), this, true));
}

int StudentWorld::oilLeft()
{
    int countOil = 0;
    for (int i = 0 ; i < m_actors.size(); i ++)
    {
        if (m_actors[i] -> isOil())
            countOil++;
    }
    return countOil;
}

bool StudentWorld::canPlaceWater(int x, int y)
{
    for (int i = x; i <= x + 3; i ++)
    {
        for (int j = y; j <= y + 3; j++)
        {
            if (isDirt(i, j))
            {
                return false;
            }
        }
    }
    return true;
}

int StudentWorld::numProtesters()
{
    int protestersCount = 0;
    for (int k = 0; k < m_actors.size(); k ++ )
    {
        if (m_actors[k]->isActorsCanAttack())
            protestersCount ++;
    }
    return protestersCount;
}

void StudentWorld::illuminate()
{
    for (int k = 0; k < m_actors.size(); k ++)
    {
        if (m_actors[k]->isBuried())
        {
            if (distanceBetweenTwoPoints(getFrackMan()->getX(), getFrackMan()->getY(), m_actors[k]->getX(), m_actors[k]->getY()) < 12)
            {
                m_actors[k]->setVisible(true);
                m_actors[k]->setPickUpableByFrackman();
            }
        }
    }
}

void StudentWorld::BoulderCauseDamage(int x, int y)
{
    for (int i = 0 ; i < m_actors.size(); i++)
    {
        if (m_actors[i]->isActorsCanAttack())
            if (distanceBetweenTwoPoints(x, y, m_actors[i]->getX(), m_actors[i]->getY()) < 3)
            {
                m_actors[i]->Bouldered();
                playSound(SOUND_PROTESTER_GIVE_UP);
                increaseScore(500);
            }
    }
    if (distanceBetweenTwoPoints(x, y, getFrackMan()->getX(), getFrackMan()->getY()) < 3)
        getFrackMan()->Bouldered();
}

//can squirt multiple protesters
bool StudentWorld::SquirtProtesters(int x, int y)
{
    bool getAttacked = false;
    for (int i = 0 ; i < m_actors.size(); i++)
    {
        if (m_actors[i]->isActorsCanAttack())
            if (distanceBetweenTwoPoints(x, y, m_actors[i]->getX(), m_actors[i]->getY()) <= 3)
            {
                if (!(m_actors[i]->isLeaveFieldState()))
                {
                    m_actors[i]-> attacked();
                    getAttacked = true;
                }
            }
    }
    if (getAttacked)
        return true;
    return false;
}

//can only bribe one protester
bool StudentWorld::canBribe(int x, int y)
{
    for (int i = 0 ; i < m_actors.size(); i ++)
    {
        if (m_actors[i] -> isActorsCanAttack())
            if (distanceBetweenTwoPoints(x, y, m_actors[i]->getX(), m_actors[i]->getY()) <= 3)
            {
                if (!(m_actors[i]->isLeaveFieldState()) && !(m_actors[i]->isStunned()))
                {
                    m_actors[i]->reactionToGold();
                    return true;
                }
            }
    }
    return false;
}

void StudentWorld::createSquirt(Actor::Direction dir)
{
    int fx = getFrackMan()->getX();
    int fy = getFrackMan()->getY();
    switch(dir)
    {
        case GraphObject::left:
            if (canMoveOneStep(fx-3 , fy, GraphObject::left) && !(isSquareDirt(fx-4, fy)))
                m_actors.push_back (new Squirts(fx-4, fy, this, GraphObject::left));
            break;
        case GraphObject::right:
            if (canMoveOneStep(fx+3 , fy, GraphObject::right) && !(isSquareDirt(fx+4, fy)))
                m_actors.push_back (new Squirts(fx+4, fy, this, GraphObject::right));
            break;
        case GraphObject::up:
            if (canMoveOneStep(fx , fy+3, GraphObject::up) && !(isSquareDirt(fx, fy+4)))
                m_actors.push_back (new Squirts(fx, fy+4, this, GraphObject::up));
            break;
        case GraphObject::down:
            if (canMoveOneStep(fx , fy-3, GraphObject::down) && !(isSquareDirt(fx, fy-4)))
                m_actors.push_back (new Squirts(fx, fy-4, this, GraphObject::down));
            break;
    }
}
/*-------------------------------------CLEANUP-------------------------------*/

void StudentWorld::cleanUp()
{
    delete m_frackman;
    for (int i = 0; i < VIEW_WIDTH; i ++)
    {
        for (int j = 0; j < 60; j ++)
        {
            if (m_dirt[i][j] != NULL)
            {
                delete m_dirt[i][j];
                m_dirt[i][j] = NULL;
            }
            
        }
    }
    while (m_actors.size()>0)
    {
        Actor* ap = m_actors.back();
        delete ap;
        m_actors.pop_back();
    }
    m_tick = 0;
}

