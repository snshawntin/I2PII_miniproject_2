#include <allegro5/base.h>
#include <cmath>
#include <string>
#include <sstream>
#include <random>
#include <cmath>

#include <iostream>

#include "Bullet/GrowBullet.hpp"
#include "Bullet/StrongBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/Resources.hpp"
#include "GrowTurret.hpp"
#include "Scene/PlayScene.hpp"

//(END) TODO PROJECT-3 (1/3): Add new turret: can be placed and will automatically attack enemies.
// have chance to update every shot.
static int shoot_count = 0;

const int GrowTurret::Price = 100;
GrowTurret::GrowTurret(float x, float y)
    : Turret(1, "play/tower-base.png", "play/turret-6-lv0.png", x, y, 100, Price, 0.7) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
    level = 0;
    attack = 1;
}

void GrowTurret::CreateBullet() {
    coolDown = 0.7 * pow(0.8, level) * (1 - on_playing_level * 0.1);
    CollisionRadius = 100 + (20 * level) * (1 + on_playing_level * 0.2);

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
        getPlayScene()->BulletGroup->AddNewObject(new GrowBullet(Position + normalized * 36, diff, attack + on_playing_level, rotation, this));
    }
    AudioHelper::PlayAudio("gun.wav");

    if (level < 9)
    {
        if (shoot_count == 4 + 2 * (level % 3))
        {
            shoot_count = 0;

            std::random_device dev;
            std::mt19937 rng(dev());
            std::uniform_int_distribution<std::mt19937::result_type> dist(0, 100);

            if (dist(rng) < 50 - 5 * level)
            {
                //& have chance to update every shot.
                //& level++, cooldown--, radius++, attack++.
                level++;
                coolDown *= 0.8;
                CollisionRadius += 20;
                attack += 0.4;

                std::stringstream sst;
                sst << "play/turret-6-lv" << level << ".png";
                std::string new_turretmap;
                sst >> new_turretmap;
                bmp = Engine::Resources::GetInstance().GetBitmap(new_turretmap);
            }
        }
        else
        {
            shoot_count++;
        }
    }
}
Bullet *GrowTurret::CreateBulletForSimulate() const
{
    Engine::Point dir(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rot = atan2(dir.y, dir.x);
    return new GrowBullet(Position + dir * 36, dir, attack, rot, const_cast<GrowTurret *>(this));
}
