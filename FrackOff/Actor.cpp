#include "Actor.h"
#include "StudentWorld.h"
#include <queue>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

/* ------------------------------------- FrackMan ---------------------------------- */

void FrackMan::doSomething()
{
    if (isDead())
        return;
    
    //get user's key
    int userKey = 0;
    if (getStudentWorld()->getKey(userKey))
    {
        switch (userKey)
        {
            case KEY_PRESS_ESCAPE:
                setDead();
                break;
            case KEY_PRESS_LEFT:
                if (getStudentWorld()->canMoveOneStep(getX(), getY(), left))
                {
                    moveTo(getX()-1, getY());
                }
                setDirection(left);
                break;
            case KEY_PRESS_RIGHT:
                if (getStudentWorld()->canMoveOneStep(getX(), getY(), right))
                {
                    moveTo(getX()+1, getY());
                }
                setDirection(right);
                break;
            case KEY_PRESS_UP:
                if (getStudentWorld()->canMoveOneStep(getX(), getY(), up))
                {
                    moveTo(getX(), getY()+1);
                }
                setDirection(up);
                break;
            case KEY_PRESS_DOWN:
                if (getStudentWorld()->canMoveOneStep(getX(), getY(), down))
                {
                    moveTo(getX(), getY()-1);
                }
                setDirection(down);
                break;
            case KEY_PRESS_SPACE:
                if (getWater() >= 1)
                {
                    getStudentWorld()->createSquirt(getDirection());
                    getStudentWorld()->playSound(SOUND_PLAYER_SQUIRT);
                    useWater();
                }
                
                break;
            case KEY_PRESS_TAB:
                if (getStudentWorld()->getFrackMan()->getGold() > 0 )
                {
                    getStudentWorld()->addGold();
                    useGold();
                }
                break;
            case 'z':
            case 'Z':
                if (getSonar() >= 1)
                {
                    useSonar();
                    getStudentWorld()->illuminate();
                    getStudentWorld()->playSound(SOUND_SONAR);
                }
                break;
        }
        moveTo(getX(), getY());
        
    }
    //dig dirt
    if (getStudentWorld()->removeDirt(getX(), getY()))
        getStudentWorld()->playSound(SOUND_DIG);
    
    //if Frackman lost all hit points
    if (getHitPoints() <= 0)
    {
        setDead();
        getStudentWorld()->playSound(SOUND_PLAYER_GIVE_UP);
        
    }
    return;
}

/* ------------------------ Protesters--------------------------*/

Protesters::Protesters(StudentWorld* sw, int imageID)
:ActorsCanAttack(imageID, 60, 60, sw, left)
{
    setHitPoints(5);
    m_numSquaresInCurrentDirection = getStudentWorld()->randInt(8, 60);
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0 ; j < 64; j++)
            maze[i][j] = ".";
    }
}

void Protesters::doSomething()
{
    //record resting ticks
    int T = getStudentWorld()->max(0, 3-(getStudentWorld()->getLevel())/4);
    int N = getStudentWorld()->max(50, 100-(getStudentWorld()->getLevel())*10);
    
    if (isDead())
        return;
    
    if (getHitPoints()<=0)
        activateLeaveField();
    
    if (isLeaveFieldState())
    {
        if ( getX() == 60 && getY() == 60)
            setDead();
        else
        {
            walkToExit();
            return;
        }
    }
    
    if (isStunned())
    {
        while (m_stunTick < N)
        {
            m_stunTick ++;
            return;
        }
        m_stunTick = 0;
    }
    stopStun();
    
    while (m_tick < T)
    {
        m_tick++;
        return;
    }
    m_tick = 0;
    m_turnTicks ++;
    m_yellTicks ++;
    
    //can do something
    
    //if it's time to yell
    if (closeEnoughToFrackman() && isFacingFrackman(getStudentWorld()->getFrackMan()->getX(), getStudentWorld()->getFrackMan()->getY()) && m_yellTicks > 15)
    {
        cout<<"I'm yelling now" <<endl;
        getStudentWorld()->playSound(SOUND_PROTESTER_YELL);
        getStudentWorld()->getFrackMan()->attacked();
        m_yellTicks = 0;
        return;
    }
    //if it's within line of sight.
    if (((getX() == getStudentWorld()->getFrackMan()->getX()) || (getY() == getStudentWorld()->getFrackMan()->getY())) && canMoveALLTheWay() && (!closeEnoughToFrackman()))
    {
        setDirection(getFrackmanDirectionRelativeToMe());
        moveTo(getDirection());
        setNumToZero();
        return;
    }
    //compute shortest distance
    int fx = getStudentWorld()->getFrackMan()->getX();
    int fy = getStudentWorld()->getFrackMan()->getY();
    
    int M = 16 + (getStudentWorld()->getLevel())*2;
    
    int leftInt = 0;
    if (canMoveOneStep(getX(), getY(), left))
        leftInt = shortestPath(getX()-1, getY(), fx, fy);
    else
        leftInt = 1000;
    
    int rightInt = 0;
    if (canMoveOneStep(getX(), getY(), right))
        rightInt = shortestPath(getX()+1, getY(), fx, fy);
    else
        rightInt = 1000;
    
    int upInt = 0;
    if (canMoveOneStep(getX(), getY(), up))
        upInt = shortestPath(getX(), getY()+1, fx, fy);
    else
        upInt = 1000;
    
    int downInt = 0;
    if (canMoveOneStep(getX(), getY(), down))
        downInt = shortestPath(getX(), getY()-1, fx, fy);
    else
        downInt = 1000;
    
    int minDir = min(min(leftInt,rightInt), min(upInt, downInt));
    
    if (minDir <= M && canUseMazeSearch() && !closeEnoughToFrackman())
    {
        cout <<"in the loop!" << endl;
        if (minDir == leftInt && canMoveOneStep(getX(), getY(), left))
        {
            cout <<"left" << leftInt<<endl;
            setDirection(left);
            moveTo(left);
            return;
        }
        if (minDir == rightInt && canMoveOneStep(getX(), getY(), right))
        {
            cout << "right" << rightInt<<endl;
            setDirection(right);
            moveTo(right);
            return;
        }
        if (minDir == upInt && canMoveOneStep(getX(), getY(), up))
        {
            cout << "up" << upInt << endl;
            setDirection(up);
            moveTo(up);
            return;
        }
        if (minDir == downInt && canMoveOneStep(getX(), getY(), down))
        {
            cout << "down" << downInt << endl;
            setDirection(down);
            moveTo(down);
            return;
        }
    }
    
    else
    {
        decNumSquares();
        if (getNumSquaresLeft() <= 0)
        {
            Direction dir = setNewDirection();
            setDirection(dir);
            setNewNumSquares();
        }
        if ((newDirectionAfterPerpendicularTurn() != none && m_turnTicks > 200))
        {
            setDirection(newDirectionAfterPerpendicularTurn());
            setNewNumSquares();
            m_turnTicks = 0;
        }
    }
    if (canMoveOneStep(getX(), getY(), getDirection()))
        moveTo(getDirection());
    else
    {
        setNumToZero();
        return;
    }
    
}

void Protesters::addSquirtScore()
{
    getStudentWorld()->increaseScore(100);
}

void HardcoreProtesters::addSquirtScore()
{
    getStudentWorld()->increaseScore(250);
}

void Protesters::reactionToGold()
{
    getStudentWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    getStudentWorld()->increaseScore(25);
    activateLeaveField();
    //    return;
}

void HardcoreProtesters::reactionToGold()
{
    getStudentWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    getStudentWorld()->increaseScore(50);
    getStunned();
    //    return;
}
/*--------------------------Protesters Helper-----------------------------*/

void Protesters::attacked()
{
    ActorsCanAttack::attacked();
    if (getHitPoints() > 0)
    {
        getStudentWorld()->playSound(SOUND_PROTESTER_ANNOYED);
        getStunned(); // implement stun
    }
    if (getHitPoints() <= 0)
    {
        activateLeaveField();
        getStudentWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        addSquirtScore();
    }
    return;
}
bool Protesters::closeEnoughToFrackman()
{
    if (getStudentWorld()->distanceBetweenTwoPoints(getX(), getY(), getStudentWorld()->getFrackMan()->getX(), getStudentWorld()->getFrackMan()->getY()) <= 4 )
        return true;
    return false;
}

bool Protesters::isFacingFrackman(int x, int y)
{
    if ( getX() >= x - 4 && getX() <= x + 4 && getY() >= y - 4 && getDirection() == up)
        return true;
    if ( getX() >= x - 4 && getY() <= x + 4 && getY() <= y + 4 && getDirection() == down)
        return true;
    if ( getX() >= x - 4 && getY() >= y - 4 && getY() <= y + 4 && getDirection() == right)
        return true;
    if ( getX() <= x + 4 && getY() >= y - 4 && getY() <= y + 4 && getDirection() == left)
        return true;
    else
        return false;
}

bool Protesters::canMoveOneStep(int x, int y, Direction dir)
{
    switch (dir)
    {
        case GraphObject::left:
            if (getStudentWorld()->canMoveOneStep(x, y, left) && !(getStudentWorld()->isSquareDirt(x-1, y)))
                return true;
            break;
        case GraphObject::right:
            if (getStudentWorld()->canMoveOneStep(x, y, right) && !(getStudentWorld()->isSquareDirt(x+1, y)))
                return true;
            break;
        case GraphObject::up:
            if (getStudentWorld()->canMoveOneStep(x, y, up) && !(getStudentWorld()->isSquareDirt(x, y+1)))
                return true;
            break;
        case GraphObject::down:
            if (getStudentWorld()->canMoveOneStep(x, y, down) && !(getStudentWorld()->isSquareDirt(x, y-1)))
                return true;
            break;
    }
    return false;
}

Actor::Direction Protesters::getFrackmanDirectionRelativeToMe()
{
    int fx = getStudentWorld()->getFrackMan()->getX();
    int fy = getStudentWorld()->getFrackMan()->getY();
    if (fx < getX())
        return left;
    if (fx > getX())
        return right;
    if (fy < getY())
        return down;
    if (fy > getY())
        return up;
    return none;
}

bool Protesters::canMoveALLTheWay()
{
    int fx = getStudentWorld()->getFrackMan()->getX();
    int fy = getStudentWorld()->getFrackMan()->getY();
    
    switch (getFrackmanDirectionRelativeToMe())
    {
        case GraphObject::left:
            for (int i = 0; i < getX()-fx; i += 1)
            {
                
                if (!(getStudentWorld()->canMoveOneStep(getX()-i, getY(), left) && !(getStudentWorld()->isSquareDirt(getX()-i, getY()))))
                {
                    return false;
                }
            }
            break;
        case GraphObject::right:
            for (int j = 0; j < fx-getX(); j += 1)
            {
                
                if (!(getStudentWorld()->canMoveOneStep(getX()+j, getY(), right) && !(getStudentWorld()->isSquareDirt(getX()+j, getY()))))
                {
                    return false;
                }
            }
            break;
        case GraphObject::down:
            for (int m = 0 ; m < getY()-fy; m += 1)
            {
                if (!(getStudentWorld()->canMoveOneStep(getX(), getY()-m, down) && !(getStudentWorld()->isSquareDirt(getX(), getY()-m))))
                {
                    return false;
                }
            }
            break;
        case GraphObject::up:
            for (int n = 0 ; n < fy-getY(); n += 1)
            {
                if (!(getStudentWorld()->canMoveOneStep(getX(), getY()+n, up) && !(getStudentWorld()->isSquareDirt(getX(), getY()+n))))
                {
                    return false;
                }
            }
            break;
    }
    return true;
}

void Protesters::moveTo(GraphObject::Direction dir)
{
    switch (dir)
    {
        case GraphObject::left:
            GraphObject::moveTo(getX()-1, getY());
            break;
        case GraphObject::right:
            GraphObject::moveTo(getX()+1, getY());
            break;
        case GraphObject::up:
            GraphObject::moveTo(getX(), getY()+1);
            break;
        case GraphObject::down:
            GraphObject::moveTo(getX(), getY()-1);
            break;
    }
    return;
}

Actor::Direction Protesters::setNewDirection()
{
    Direction dir = none;
    int d = getStudentWorld()->randInt(1, 4);
    if (d == 1)
        dir = left;
    if (d == 2)
        dir = right;
    if (d == 3)
        dir = up;
    if (d == 4)
        dir = down;
    while (!canMoveOneStep(getX(), getY(), dir))
    {
        int d = getStudentWorld()->randInt(1, 4);
        if (d == 1)
            dir = left;
        if (d == 2)
            dir = right;
        if (d == 3)
            dir = up;
        if (d == 4)
            dir = down;
    }
    return dir;
}

void Protesters::setNewNumSquares()
{
    int newNum = getStudentWorld()->randInt(8, 60);
    m_numSquaresInCurrentDirection = newNum;
}

Actor::Direction Protesters::newDirectionAfterPerpendicularTurn()
{
    switch (getDirection())
    {
        case GraphObject::left:
        case GraphObject::right:
        {
            bool canMoveUp = false;
            bool canMoveDown = false;
            if (canMoveOneStep(getX(), getY(),up))
                canMoveUp = true;
            if (canMoveOneStep(getX(), getY(), down))
                canMoveDown = true;
            if (canMoveUp && canMoveDown)
            {
                int getNum = getStudentWorld()->randInt(1, 2);
                if (getNum == 1)
                    return up;
                else
                    return down;
            }
            if (canMoveUp)
                return up;
            if (canMoveDown)
                return down;
        }
            break;
            
        case GraphObject::up:
        case GraphObject::down:
        {
            bool canMoveLeft = false;
            bool canMoveRight = false;
            if (canMoveOneStep(getX(), getY(), left))
                canMoveLeft = true;
            if (canMoveOneStep(getX(), getY(), right))
                canMoveRight = true;
            if (canMoveLeft && canMoveRight)
            {
                int getNum = getStudentWorld()->randInt(1, 2);
                if (getNum == 1)
                    return left;
                else
                    return right;
            }
            if (canMoveLeft)
                return left;
            if (canMoveRight)
                return right;
        }
            break;
    }
    return none;
}

int Protesters::shortestPath(int sx, int sy, int ex, int ey) //CHECK THIS!
{
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0 ; j < 64; j++)
            maze[i][j] = ".";
    }
    
    queue<Coord> coordQueue;
    queue<int> lengths;
    
    coordQueue.push(Coord(sx,sy));
    maze[sx][sy] = "#";
    lengths.push(0);
    
    
    while (!coordQueue.empty())
    {
        Coord temp = coordQueue.front();
        int x = temp.x();
        int y = temp.y();
        coordQueue.pop();
        
        int tempInt = lengths.front();
        lengths.pop();
        
        if ( x == ex && y == ey)
        {
            cout << tempInt << endl;
            return tempInt;
        }
        
        if (canMoveOneStep(x,y,left) && maze[x-1][y]== ".")
        {
            coordQueue.push(Coord(x-1, y));
            lengths.push(tempInt+1);
            maze[x-1][y] = "#";
        }
        if (canMoveOneStep(x, y, right) && maze[x+1][y] == ".")
        {
            coordQueue.push(Coord(x+1, y));
            lengths.push(tempInt+1);
            maze[x+1][y] = "#";
        }
        if (canMoveOneStep(x, y, up) && maze[x][y+1] == ".")
        {
            coordQueue.push(Coord(x, y+1));
            lengths.push(tempInt+1);
            maze[x][y+1] = "#";
        }
        if (canMoveOneStep(x, y, down) && maze[x][y-1] == ".")
        {
            coordQueue.push(Coord(x, y-1));
            lengths.push(tempInt+1);
            maze[x][y-1] = "#";
        }
    }
    cout <<"shit im here"<<endl;
    return 0;
}

void Protesters::walkToExit()
{
    int leftInt = 0;
    if (canMoveOneStep(getX(), getY(), left))
        leftInt = shortestPath(getX()-1, getY(), 60, 60);
    else
        leftInt = 1000;
    
    int rightInt = 0;
    if (canMoveOneStep(getX(), getY(), right))
        rightInt = shortestPath(getX()+1, getY(), 60, 60);
    else
        rightInt = 1000;
    
    int upInt = 0;
    if (canMoveOneStep(getX(), getY(), up))
        upInt = shortestPath(getX(), getY()+1, 60, 60);
    else
        upInt = 1000;
    
    int downInt = 0;
    if (canMoveOneStep(getX(), getY(), down))
        downInt = shortestPath(getX(), getY()-1, 60, 60);
    else
        downInt = 1000;
    
    int minDir = min(min(leftInt,rightInt), min(upInt, downInt));
    if (minDir == leftInt && canMoveOneStep(getX(), getY(), left))
    {
        cout <<"left" << leftInt<<endl;
        setDirection(left);
        moveTo(left);
        return;
    }
    if (minDir == rightInt && canMoveOneStep(getX(), getY(), right))
    {
        cout << "right" << rightInt<<endl;
        setDirection(right);
        moveTo(right);
        return;
    }
    if (minDir == upInt && canMoveOneStep(getX(), getY(), up))
    {
        cout << "up" << upInt << endl;
        setDirection(up);
        moveTo(up);
        return;
    }
    if (minDir == downInt && canMoveOneStep(getX(), getY(), down))
    {
        cout << "down" << downInt << endl;
        setDirection(down);
        moveTo(down);
        return;
    }
    return;
    
}
/* ------------------------------ SQUIRTS------------------------------ */

void Squirts::doSomething()
{
    if (m_pauseTick == 1)
    {
        setDead();
        m_pauseTick = 0;
        return;
    }
    while (m_travelSquares > 0)
    {
        if (getStudentWorld()->SquirtProtesters(getX(), getY()))
        {
            m_pauseTick ++;
            return;
        }
        move(getDirection());
        m_travelSquares--;
        return;
    }
    setDead();
}

void Squirts::move(Direction dir)
{
    switch (dir)
    {
        case GraphObject::left:
            if (getStudentWorld()->canMoveOneStep(getX(), getY(), left) && !(getStudentWorld()->isSquareDirt(getX()-1, getY())))
                moveTo(getX()-1, getY());
            break;
        case GraphObject::right:
            if (getStudentWorld()->canMoveOneStep(getX(), getY(), right) && !(getStudentWorld()->isSquareDirt(getX()+1, getY())))
                moveTo(getX()+1, getY());
            break;
        case GraphObject::up:
            if (getStudentWorld()->canMoveOneStep(getX(), getY(), up) && !(getStudentWorld()->isSquareDirt(getX(), getY()+1)))
                moveTo(getX(), getY()+1);
            break;
        case GraphObject::down:
            if (getStudentWorld()->canMoveOneStep(getX(), getY(), down) && !(getStudentWorld()->isSquareDirt(getX(), getY()-1)))
                moveTo(getX(), getY()-1);
            break;
    }
}

/*---------------------------------BOULDER----------------------------------*/
//no one layer of dirt underneath OR potentially hitting another boulder, OR going out of screen
bool Boulders::canMove()
{
    if (getY()-1 < 0)
        return false;
    StudentWorld* sw = getStudentWorld();
    for (int i = 0 ; i < 4 ; i++)
    {
        if (sw->isDirt(getX()+i, getY()-1))
        {
            return false;
        }
    }
    
    for (int j = 0 ; j < (getStudentWorld()->getVector()).size(); j ++)
    {
        Actor* ap = getStudentWorld()->getVector()[j];
        if ( ap != this && ap -> isBoulder())
        {
            if (getStudentWorld()->twoSquaresOverlap(getX(), getY(), ap->getX(), ap->getY()))
                return false;
        }
    }
    return true;
}

void Boulders::move()
{
    if (canMove())
    {
        movingState = true;
        while (m_tickCount > 0)
        {
            m_tickCount--;
            return;
        }
        while (m_tickCount >= 0)
        {
            getStudentWorld()->playSound(SOUND_FALLING_ROCK);
            m_tickCount--;
            return;
        }
        moveTo(getX(), getY()-1);
        getStudentWorld()->BoulderCauseDamage(getX(),getY());
        return;
    }
    if (movingState == true)
        setDead();
    return;
}

/*--------------------------------Goodies----------------------*/

void Goodies::doSomething()
{
    if (isDead())
        return;
    if ((getStudentWorld()->distanceBetweenTwoPoints(getX(), getY(), getStudentWorld()->getFrackMan()->getX(), getStudentWorld()->getFrackMan()->getY())<= 4.00 ) && (getStudentWorld()->distanceBetweenTwoPoints(getX(), getY(), getStudentWorld()->getFrackMan()->getX(), getStudentWorld()->getFrackMan()->getY()) > 3.00) && isVisible() == false)
    {
        setVisible(true);
        setPickUpableByFrackman();
        return;
    }
    if (isPickUpableByFrackMan() && getStudentWorld()->distanceBetweenTwoPoints(getX(), getY(), getStudentWorld()->getFrackMan()->getX(), getStudentWorld()->getFrackMan()->getY()) <= 3.00 && isVisible()==true)
    {
        playSound();
        addScore();
        addToInventory();
        setDead();
    }
    if (isPickUpableByProtesters())
    {
        while (getCount() <= 100)
        {
            if (isActive() && getStudentWorld()->canBribe(getX(), getY()))
            {
                deactivate();
            }
            incCount();
            return;
        }
        setDead();
    }
    return;
}

void Goodies::playSound()
{
    getStudentWorld()->playSound(SOUND_GOT_GOODIE);
}
/*==============TemporaryGoodies===================*/

bool TemporaryGoodies::isPickUpableByFrackMan()
{
    while (m_tick <= (getStudentWorld()->max(100, 300-10*(getStudentWorld()->getLevel()))))
    {
        setPickUpableByFrackman();
        m_tick++;
        return true;
    }
    setDead();
    return false;
}
/*=====BarrelsOfOil=========*/

void BarrelsOfOil::playSound()
{
    getStudentWorld()->playSound(SOUND_FOUND_OIL);
}

void BarrelsOfOil::addScore()
{
    getStudentWorld()->increaseScore(1000);
}

/*=======GoldNuggest=========*/

void GoldNugget::addScore()
{
    getStudentWorld()->increaseScore(10);
}

void GoldNugget::addToInventory()
{
    getStudentWorld()->getFrackMan()->addGold();
}
/*========SonarKit========*/

void Sonarkit::addScore()
{
    getStudentWorld()->increaseScore(75);
}

void Sonarkit::addToInventory()
{
    getStudentWorld()->getFrackMan()->addSonar();
}

/*========WaterPool=========*/

void WaterPool::addScore()
{
    getStudentWorld()->increaseScore(100);
}

void WaterPool::addToInventory()
{
    getStudentWorld()->getFrackMan()->addWater();
}

