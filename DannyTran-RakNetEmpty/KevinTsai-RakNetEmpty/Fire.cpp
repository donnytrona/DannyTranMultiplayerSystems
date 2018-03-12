#include "Fire.h"

Fire::Fire() : Player()
{
	hp = 20;
	attack = 6;
	defense = 1;
	type = fire;
}

void Fire::Special()
{
	attack += 2;
}