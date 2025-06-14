#include <allegro5/base.h>
#include <cmath>
#include <string>
#include <random>

#include "Bullet/LaserBullet.hpp"
#include "Bullet/StrongBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "LaserTurret.hpp"
#include "Scene/PlayScene.hpp"

const int LaserTurret::Price = 200;
LaserTurret::LaserTurret(float x, float y)
    : Turret(2, "play/tower-base.png", "play/turret-2.png", x, y, 300, Price, 0.5) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}
void LaserTurret::CreateBullet() {
    coolDown = 0.5 * (1 - on_playing_level * 0.1);
    CollisionRadius = 300 * (1 + on_playing_level * 0.2);

    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
    // Change bullet position to the front of the gun barrel.

    // have a small chance of shooting strong bullet.
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 100);
    if(dist(rng) < on_playing_level){
        getPlayScene()->BulletGroup->AddNewObject(new StrongBullet(Position + normalized * 36, diff, 1000, rotation, this));
    }
    else{
        getPlayScene()->BulletGroup->AddNewObject(new LaserBullet(Position + normalized * 36 - normal * 6, diff, 2 + 2 * on_playing_level, rotation, this));
        getPlayScene()->BulletGroup->AddNewObject(new LaserBullet(Position + normalized * 36 + normal * 6, diff, 2 + 2 * on_playing_level, rotation, this));
    }

    AudioHelper::PlayAudio("laser.wav");
}
Bullet *LaserTurret::CreateBulletForSimulate() const
{
    Engine::Point dir(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    Engine::Point normal(-dir.y, dir.x);
    float rot = atan2(dir.y, dir.x);
    return new LaserBullet(Position + dir * 36 + normal * 6, dir, 2 + 2 * on_playing_level, rot, const_cast<LaserTurret *>(this));
}
