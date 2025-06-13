#include <allegro5/base.h>
#include <random>
#include <string>

#include "Enemy/Enemy.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "LaserBullet.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Turret/Turret.hpp"
#include <iostream>;

class Turret;

LaserBullet::LaserBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent) : Bullet("play/bullet-2.png", 800, 2, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent)
{
}
void LaserBullet::OnExplode(Enemy *enemy)
{
    std::cout << "Turret::simulateMode" << Turret::simulateMode;
    if (Turret::simulateMode)
        return; // 防止模擬時崩潰

    PlayScene *scene = getPlayScene();
    if (!scene)
        return;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 10);
    scene->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-2.png", dist(rng), enemy->Position.x, enemy->Position.y));
}

Bullet *LaserBullet::CreateBulletForSimulate() const
{
    float dirX = cos(Rotation);
    float dirY = sin(Rotation);
    Engine::Point forwardDirection(dirX, dirY);
    return new LaserBullet(Position, forwardDirection, Rotation, parent);
}