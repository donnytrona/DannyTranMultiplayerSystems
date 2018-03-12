#include "Grass.h"

Grass::Grass() : Player()
{
	hp = 20;
	attack = 5;
	defense = 2;
	type = grass;
}

void Grass::Special()
{
	type = newType;
}

void Grass::SetNewType(PlayerType type)
{
	newType = type;
}