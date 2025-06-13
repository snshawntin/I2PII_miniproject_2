#ifndef SOLDIERENEMY_HPP
#define SOLDIERENEMY_HPP
#include "Enemy.hpp"

class SoldierEnemy : public Enemy
{
public:
    SoldierEnemy(int x, int y);
    Enemy *Clone() const override;
};
#endif // SOLDIERENEMY_HPP
