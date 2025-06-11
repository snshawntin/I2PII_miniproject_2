#include <allegro5/base.h>
#include <cmath>
#include <string>
#include <random>

#include "Bullet/FireBullet.hpp"
#include "Bullet/StrongBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "MachineGunTurret.hpp"
#include "Scene/PlayScene.hpp"

/*
when onplaying_level++;
atk++, cd--, there's chance to shoot a strong bullet.(w/ very high damage)
rad++.
*/

const int MachineGunTurret::Price = 50;
MachineGunTurret::MachineGunTurret(float x, float y)
    : Turret(0, "play/tower-base.png", "play/turret-1.png", x, y, 200, Price, 0.5) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}
void MachineGunTurret::CreateBullet() {
    coolDown = 0.5 * (1 - on_playing_level * 0.1);
    CollisionRadius = 200 * (1 + on_playing_level * 0.2);

    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    // Change bullet position to the front of the gun barrel.

    // have a small chance of shooting strong bullet.
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 100);
    if(dist(rng) < on_playing_level){
        getPlayScene()->BulletGroup->AddNewObject(new StrongBullet(Position + normalized * 36, diff, 1000, rotation, this));
    }
    else{
        getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36, diff, 1 + on_playing_level, rotation, this));
    }

    AudioHelper::PlayAudio("gun.wav");
}
