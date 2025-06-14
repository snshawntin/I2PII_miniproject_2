#include <string>

#include "SoldierEnemy.hpp"

//(END) TODO HACKATHON-3 (1/3): You can imitate the 2 files: 'SoldierEnemy.hpp', 'SoldierEnemy.cpp' to create a new enemy.
SoldierEnemy::SoldierEnemy(int x, int y) : Enemy("play/enemy-1.png", x, y, 10, 50, 5, 5)
{
}
Enemy *SoldierEnemy::Clone() const
{
    auto *e = new SoldierEnemy(Position.x, Position.y);
    e->hp = this->hp;
    e->path = this->path;
    e->Velocity = this->Velocity;
    e->reachEndTime = this->reachEndTime;
    return e;
}
