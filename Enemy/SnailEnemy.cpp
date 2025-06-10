#include <string>

#include "SnailEnemy.hpp"

SnailEnemy::SnailEnemy(int x, int y) : Enemy("play/snail_enemy.png", x, y, 12, 100, 1, 0)
{
    // 參數說明：
    // x, y：初始位置
    // 12：碰撞半徑
    // 100：移動速度
    // 1：血量
    // 0：死亡不掉金幣
}