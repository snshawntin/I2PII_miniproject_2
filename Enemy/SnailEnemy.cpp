#include <string>

#include "SnailEnemy.hpp"

SnailEnemy::SnailEnemy(int x, int y) : Enemy("play/snail_enemy.png", x, y, 12, 100, 1, 0)
{
}
Enemy *SnailEnemy::Clone() const
{
    auto *e = new SnailEnemy(Position.x, Position.y);
    e->hp = this->hp;
    e->path = this->path;
    e->Velocity = this->Velocity;
    e->reachEndTime = this->reachEndTime;
    return e;
}
