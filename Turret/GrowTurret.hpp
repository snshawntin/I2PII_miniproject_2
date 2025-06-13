#ifndef GROWTURRET_HPP
#define GROWTURRET_HPP
#include "Turret.hpp"

class GrowTurret : public Turret
{
public:
    static const int Price;
    GrowTurret(float x, float y);
    void CreateBullet() override;

private:
    int level;
    float attack;
    Bullet *CreateBulletForSimulate() const override;
};

#endif // GROWTURRET_HPP