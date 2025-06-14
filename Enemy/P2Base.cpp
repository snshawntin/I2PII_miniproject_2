#include <string>

#include "P2Base.hpp"

P2Base::P2Base(int x, int y) : Enemy("play/p2base.png", x, y, 10, 0, 100, 0) {
    is_enemybase = 1;
}

float P2Base::gethp(){ return hp; }
void P2Base::addhp(float f) { hp += f; }

Enemy *P2Base::Clone() const
{
    auto *e = new P2Base(Position.x, Position.y);
    e->hp = this->hp;
    e->path = this->path;
    e->Velocity = this->Velocity;
    e->reachEndTime = this->reachEndTime;
    return e;
}