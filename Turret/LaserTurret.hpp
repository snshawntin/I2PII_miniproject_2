#ifndef LASERTURRET_HPP
#define LASERTURRET_HPP
#include "Turret.hpp"

class LaserTurret : public Turret
{
public:
    static const int Price;
    LaserTurret(float x, float y);
    void CreateBullet() override;
    Bullet *CreateBulletForSimulate() const override;
};
#endif // LASERTURRET_HPP
