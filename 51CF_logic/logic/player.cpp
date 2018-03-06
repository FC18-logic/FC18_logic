#include "player.h"



Player::Player()
{
	m_techPoint = 0.0;
}

//#json
Player::Player(Player& _player)
{
	alive = _player.isAlive();
	m_cells = _player.cells();
	m_techPoint = _player.techPoint();
	m_RegenerationLevel = _player.getRegenerationLevel();
	m_ExtraControlLevel = _player.getExtraControlLevel();
	m_DefenceLevel = _player.getDefenceLevel();
	data = nullptr;
}


Player::~Player()
{
}

void Player::addTechPoint(TResourceD _techPoint)
{
	m_techPoint += _techPoint;
}

void Player::regenerateTechPoint()
{
	TResourceD r = 0;
	for (TCellID c : m_cells)
	{
		r += data->cells[c].techRegenerateSpeed();
	}
	addTechPoint(r);
}

bool Player::upgrade(TPlayerProperty kind)
{
	switch (kind)
	{
	case RegenerationSpeed:
		return upgradeRegeneration();
		break;
	case ExtendingSpeed:
		return upgradeMove();
		break;
	case ExtraControl:
		return upgradeExtraControl();
		break;
	case CellWall:
		return upgradeDefence();
		break;
	default:
		return false;
		break;
	}
}

bool Player::subTechPoint(TResourceD _techPoint)
{
	if (_techPoint <= m_techPoint)
	{
		m_techPoint -= _techPoint;
		return true;
	}
	else
		return false;
}

bool Player::upgradeDefence()
{
	if (m_DefenceLevel < MAX_DEFENCE_LEVEL &&
		m_techPoint > DefenceStageUpdateCost[m_DefenceLevel])
	{
		subTechPoint(DefenceStageUpdateCost[m_DefenceLevel]);
		m_DefenceLevel++;
		return true;
	}
	else
		return false;
}

bool Player::upgradeRegeneration()
{
	if (m_RegenerationLevel < MAX_REGENERATION_SPEED_LEVEL &&
		m_techPoint > RegenerationSpeedUpdateCost[m_RegenerationLevel])
	{
		subTechPoint(RegenerationSpeedUpdateCost[m_RegenerationLevel]);
		m_RegenerationLevel++;
		return true;
	}
	else
		return false;
}

bool Player::upgradeExtraControl()
{
	if (m_ExtraControlLevel < MAX_EXTRA_CONTROL_LEVEL &&
		m_techPoint > ExtraControlStageUpdateCost[m_ExtraControlLevel])
	{		
		subTechPoint(ExtraControlStageUpdateCost[m_ExtraControlLevel]);
		m_ExtraControlLevel++;
		return true;
	}
	else
		return false;
}

bool Player::upgradeMove()
{
	if (m_MoveLevel < MAX_EXTENDING_SPEED_LEVEL &&
		m_techPoint > ExtendingSpeedUpdateCost[m_MoveLevel])
	{
		subTechPoint(ExtendingSpeedUpdateCost[m_MoveLevel]);
		m_MoveLevel++;
		return true;
	}
	else
		return false;
}

int Player::maxControlNumber()
{
	return (m_cells.size() + m_ExtraControlLevel) / 2 + 1;
}

TResourceD Player::totalResource()
{
	TResourceD total = 0.0;
	for (TCellID i : m_cells)
	{
		total += data->cells[i].totalResource();
	}
	return total;
}

void Player::Kill()
{
	alive = false;
}

