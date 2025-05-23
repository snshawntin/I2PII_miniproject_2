#ifndef SHIELDENEMY_HPP
#define SHIELDENEMY_HPP
#include "Enemy.hpp"
#include "Engine/Sprite.hpp"

class ShieldEnemy : public Enemy {
private:
    Sprite head;
    float targetRotation;

public:
    ShieldEnemy(int x, int y);
    void Draw() const override;
    void Update(float deltaTime) override;
};
#endif   // SHIELDENEMY_HPP
