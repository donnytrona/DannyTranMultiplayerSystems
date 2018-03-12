#include "Player.h"

Player::Player()
{
	isBlocking = false;
}

void Player::StartTurn()
{
	isBlocking = false;
}

int Player::Attack(Player* other)
{
	int damage;
	damage = (float)attack * TypeMultiplier(other);
	if (other->isBlocking)
	{
		damage = damage - other->defense;
	}
	else
	{
		damage = (damage * 3 - other->defense) / 3;
	}
	if (damage <= 0)
	{
		damage = 1;
	}
	other->hp -= damage;
	return damage;
}

void Player::Block()
{
	isBlocking = true;
}

float Player::TypeMultiplier(Player* other)
{
	if (type == other->type)
	{
		return 1.0f;
	}
	else if (type == fire && other->type == grass)
	{
		return 2.0f;
	}
	else if (type == water && other->type == fire)
	{
		return 2.0f;
	}
	else if (type == grass && other->type == water)
	{
		return 2.0f;
	}
	else
	{
		return 0.5f;
	}
}

bool Player::IsAlive() const
{
	return hp > 0;
}

void Player::SetName(std::string& nm)
{
	name = nm;
}

std::string Player::GetName() const
{
	return name;
}

int Player::GetHP() const
{
	return hp;
}

int Player::GetAtk() const
{
	return attack;
}

int Player::GetDef() const
{
	return defense;
}

std::string Player::GetType() const
{
	switch (type)
	{
	case fire:
		return "Fire";
		break;
	case water:
		return "Water";
		break;
	case grass:
		return "Grass";
		break;
	default:
		return "";
		break;
	}
}

void Player::Die()
{
	hp = 0;
}