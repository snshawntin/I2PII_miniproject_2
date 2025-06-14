#ifndef TURRET_HPP
#define TURRET_HPP
#include <allegro5/base.h>
#include <list>
#include <string>

#include "Engine/Sprite.hpp"

class Enemy;
class PlayScene;

class Turret : public Engine::Sprite
{
protected:
    int price;
    float coolDown;
    float reload = 0;
    float rotateRadian = 2 * ALLEGRO_PI;
    Sprite imgBase;
    std::list<Turret *>::iterator lockedTurretIterator;
    PlayScene *getPlayScene();
    // Reference: Design Patterns - Factory Method.
    virtual void CreateBullet() = 0;

public:
    int type; //1:machinegun, 2:grow, 3:laser

    bool Enabled = true;
    bool Preview = false;
    static bool simulateMode;
    Enemy *Target = nullptr;
    int on_playing_level = 0;

    Turret(int type, std::string imgBase, std::string imgTurret, float x, float y, float radius, int price, float coolDown);
    void Update(float deltaTime) override;
    void Draw() const override;
    int GetPrice() const;
    virtual Bullet *CreateBulletForSimulate() const { return nullptr; }
};
#endif // TURRET_HPP
