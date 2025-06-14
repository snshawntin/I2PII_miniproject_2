#ifndef SNAILENEMY_HPP
#define SNAILENEMY_HPP
#include "Enemy.hpp"

class SnailEnemy : public Enemy
{
public:
    SnailEnemy(int x, int y);
    Enemy *Clone() const override;
};
#endif // SNAILENEMY_HPP