#ifndef SHIELDENEMY_HPP
#define SHIELDENEMY_HPP
#include "Enemy.hpp"
#include "Engine/Sprite.hpp"

class ShieldEnemy : public Enemy {
private:
    Sprite shield;
    int shield_countdown;

public:
    ShieldEnemy(int x, int y);
    void Draw() const override;
    void Update(float deltaTime) override;
    virtual void Hit(float damage) override;
};
#endif   // SHIELDENEMY_HPP
