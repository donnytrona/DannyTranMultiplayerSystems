#include "Water.h"

Water::Water() : Player()
{
	maxHp = 25;
	hp = maxHp;
	attack = 4;
	defense = 2;
	type = water;
}

void Water::Special()
{
	hp += 3;
	if (hp > maxHp)
	{
		hp = maxHp;
	}
}