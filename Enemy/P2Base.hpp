#ifndef P2BASE_HPP
#define P2BASE_HPP
#include "Enemy.hpp"

class P2Base : public Enemy {
public:
    P2Base(int x, int y);
    float gethp();
    void addhp(float f);
};
#endif   // P2BASE_HPP
