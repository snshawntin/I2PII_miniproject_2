#include "Bullet.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"
#include "Turret/Turret.hpp"
#include <iostream>;

PlayScene *Bullet::getPlayScene()
{
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
void Bullet::OnExplode(Enemy *enemy)
{
}
Bullet::Bullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent) : Sprite(img, position.x, position.y), speed(speed), damage(damage), parent(parent)
{
    Velocity = forwardDirection.Normalize() * speed;
    Rotation = rotation;
    CollisionRadius = 4;
}
void Bullet::Update(float deltaTime)
{
    Position = Position + Velocity * deltaTime;

    if (Turret::simulateMode)
        return; // 模擬時不觸發動畫或刪除

    Sprite::Update(deltaTime);
    PlayScene *scene = getPlayScene();
    if (!scene)
        return;

    for (auto &it : scene->EnemyGroup->GetObjects())
    {
        Enemy *enemy = dynamic_cast<Enemy *>(it);
        if (!enemy || !enemy->Visible)
            continue;
        if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, enemy->Position, enemy->CollisionRadius))
        {
            OnExplode(enemy);
            scene->BulletGroup->RemoveObject(objectIterator);
            return;
        }
    }

    if (!Engine::Collider::IsRectOverlap(Position - Size / 2, Position + Size / 2, Engine::Point(0, 0), PlayScene::GetClientSize()))
        scene->BulletGroup->RemoveObject(objectIterator);
}
