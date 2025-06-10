#include <string>

#include "P2Base.hpp"

P2Base::P2Base(int x, int y) : Enemy("play/p2base.png", x, y, 10, 0, 100, 0) {}

float P2Base::gethp(){ return hp; }
void P2Base::addhp(float f) { hp += f; }