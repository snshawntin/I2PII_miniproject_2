#include <allegro5/base.h>
#include <random>
#include <string>

#include "Enemy/Enemy.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "GrowBullet.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Turret/Turret.hpp"
#include <iostream>;

class Turret;

GrowBullet::GrowBullet(Engine::Point position, Engine::Point forwardDirection, float damage, float rotation, Turret *parent)
    : Bullet("play/bullet-6.png", 500, 1, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent) {}

void GrowBullet::OnExplode(Enemy *enemy)
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
    scene->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-1.png", dist(rng), enemy->Position.x, enemy->Position.y));
}

Bullet *GrowBullet::CreateBulletForSimulate() const
{
    float dirX = cos(Rotation);
    float dirY = sin(Rotation);
    Engine::Point forwardDirection(dirX, dirY);
    return new GrowBullet(Position, forwardDirection, damage, Rotation, parent);
}