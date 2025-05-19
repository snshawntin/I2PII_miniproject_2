#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Bullet/FireBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "GrowTurret.hpp"
#include "Scene/PlayScene.hpp"

//TODO PROJECT-3 (1/3): Add new turret: can be placed and will automatically attack enemies.
//have chance to update every shot.

const int GrowTurret::Price = 100;
GrowTurret::GrowTurret(float x, float y)
    : Turret("play/tower-base.png", "play/turret-6.png", x, y, 100, Price, 0.7) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
    level = 0;
}

void GrowTurret::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36, diff, rotation, this));
    AudioHelper::PlayAudio("gun.wav");

    // if(level < 9){
    //     //& have chance to update every shot.
    //     //& level++, cooldown--, radius++, attack++.
    //     level++;
    //     coolDown *= 0.8;
    //     CollisionRadius += 10;
        
    // }
}