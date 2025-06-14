#include <allegro5/base.h>
#include <random>
#include <string>

#include "Enemy/Enemy.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "FireBullet.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Turret/Turret.hpp"
#include <iostream>;

class Turret;

FireBullet::FireBullet(Engine::Point position, Engine::Point forwardDirection, float damage, float rotation, Turret *parent) : Bullet("play/bullet-1.png", 500, damage, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent) {
}
void FireBullet::OnExplode(Enemy *enemy)
{
    std::cout << "Turret::simulateMode" << Turret::simulateMode;
    if (Turret::simulateMode)
        return; // 模擬模式不用產生效果

    PlayScene *scene = getPlayScene();
    if (!scene)
        return;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    scene->GroundEffectGroup->AddNewObject(
        new DirtyEffect("play/dirty-1.png", dist(rng), enemy->Position.x, enemy->Position.y));
}

Bullet *FireBullet::CreateBulletForSimulate() const
{
    float dirX = cos(Rotation);
    float dirY = sin(Rotation);
    Engine::Point forwardDirection(dirX, dirY);
    return new FireBullet(Position, forwardDirection, damage, Rotation, parent);
}
