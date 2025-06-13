#ifndef SNAILBOSS_HPP
#define SNAILBOSS_HPP

#include "Enemy.hpp"

class SnailBoss : public Enemy
{
public:
    SnailBoss(int x, int y);
    void Update(float deltaTime) override;
    void Initialize(float interval); // 覆寫初始化
    Enemy *Clone() const override;

private:
    float spawnInterval = 5.0f; // initialize
    float spawnTimer = 0.0f;
};

#endif // SNAILBOSS_HPP
