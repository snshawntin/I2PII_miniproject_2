#ifndef STRONGBULLET_HPP
#define STRONGBULLET_HPP
#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
    struct Point;
}   // namespace Engine

class StrongBullet : public Bullet {
public:
    explicit StrongBullet(Engine::Point position, Engine::Point forwardDirection, float damage, float rotation, Turret *parent);
    void OnExplode(Enemy *enemy) override;
};
#endif   // STRONGBULLET_HPP
