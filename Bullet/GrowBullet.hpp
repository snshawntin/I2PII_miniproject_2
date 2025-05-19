#ifndef GROWBULLET_HPP
#define GROWBULLET_HPP
#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
    struct Point;
}   // namespace Engine

class GrowBullet : public Bullet {
public:
    explicit GrowBullet(Engine::Point position, Engine::Point forwardDirection, float damage, float rotation, Turret *parent);
    void OnExplode(Enemy *enemy) override;
};
#endif // GROWBULLET_HPP