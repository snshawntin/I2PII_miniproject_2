#include <allegro5/base.h>
#include <random>
#include <string>
#include <iostream>

#include "Engine/Point.hpp"
#include "ShieldEnemy.hpp"

#include "Enemy.hpp"
#include "Bullet/Bullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"
#include "Turret/Turret.hpp"

//(END) TODO PROJECT-3 (2/3): Add new enemy: can follow the path and be damaged by turrets.
// sheld enemy: have chance to open shield every time getting hitted.

ShieldEnemy::ShieldEnemy(int x, int y) 
    : Enemy("play/enemy-4.png", x, y, 20, 20, 50, 100), 
    shield("play/target-invalid.png", x, y),
    shield_countdown(0){}

void ShieldEnemy::Draw() const {
    Enemy::Draw();

    if (shield_countdown > 0){
        shield.Draw();
    }
}

void ShieldEnemy::Update(float deltaTime) {
    Enemy::Update(deltaTime);
    shield.Position = Position;

    if(shield_countdown > 0){
        shield_countdown--;
    }
}

void ShieldEnemy::Hit(float damage){
    if(shield_countdown == 0){
        hp -= damage;
    }

    if (hp <= 0) {
        OnExplode();
        // Remove all turret's reference to target.
        for (auto &it : lockedTurrets)
            it->Target = nullptr;
        for (auto &it : lockedBullets)
            it->Target = nullptr;
        getPlayScene()->EarnMoney(money);
        getPlayScene()->EnemyGroup->RemoveObject(objectIterator);
        AudioHelper::PlayAudio("explosion.wav");
    }
    else{
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, 100);
        std::cout << dist(rng) << std::endl;
        if(dist(rng) < 10){
            shield_countdown = 120;
        }
    }

    return;
}
