#include "SnailBoss.hpp"
#include "Scene/PlayScene.hpp"
#include "SnailEnemy.hpp"

SnailBoss::SnailBoss(int x, int y)
    : Enemy("play/snail_boss.png", x, y, 20, 8, 3000, 350)
{
    spawnInterval = 5.0f;
    spawnTimer = 0.0f;
}

void SnailBoss::Initialize(float interval)
{
    spawnInterval = interval;
    spawnTimer = 0.0f;
}

void SnailBoss::Update(float deltaTime)
{
    Enemy::Update(deltaTime);

    spawnTimer += deltaTime;
    if (spawnTimer >= spawnInterval)
    {
        spawnTimer = 0.0f;

        auto *scene = getPlayScene();
        if (scene)
        {
            // 直接在 Boss 的位置生成小蝸牛
            int spawnX = static_cast<int>(Position.x);
            int spawnY = static_cast<int>(Position.y);

            SnailEnemy *snail = new SnailEnemy(spawnX, spawnY);

            scene->EnemyGroup->AddNewObject(snail);
            snail->UpdatePath(scene->mapDistance);

            printf("Boss Position: %f, %f\n", Position.x, Position.y);
            printf("Spawn Position: %d, %d\n", spawnX, spawnY);
        }
    }
}
Enemy *SnailBoss::Clone() const
{
    auto *e = new SnailBoss(Position.x, Position.y);
    e->hp = this->hp;
    e->path = this->path;
    e->Velocity = this->Velocity;
    e->reachEndTime = this->reachEndTime;
    e->spawnInterval = this->spawnInterval;
    e->spawnTimer = this->spawnTimer;
    return e;
}
