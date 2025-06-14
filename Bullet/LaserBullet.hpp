#ifndef LASERBULLET_HPP
#define LASERBULLET_HPP
#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine
{
    struct Point;
} // namespace Engine

class LaserBullet : public Bullet
{
public:
    explicit LaserBullet(Engine::Point position, Engine::Point forwardDirection, float damage, float rotation, Turret *parent);
    void OnExplode(Enemy *enemy) override;
    Bullet *OnExplode(Enemy *enemy) const;
    Bullet *CreateBulletForSimulate() const override;
};
#endif // LASERBULLET_HPP
