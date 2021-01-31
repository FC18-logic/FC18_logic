#include "player.h"
using std::vector;


Player::Player()
{
	alive = true;
	deadRound = -1;
	m_id = -1;
	data = nullptr;
	conqueTowerNum = 0;
	eliminateCorpsNum = 0;
	captureCorpsNum = 0;
}

//#json
Player::Player(Player& _player)// copy构造player
{
	m_crops = _player.getCrops();
	m_tower = _player.getTower();
	alive = _player.isAlive();
	m_id = _player.getId();
	conqueTowerNum = _player.getCqTowerNum();
	eliminateCorpsNum = _player.getElCorpsNum();
	captureCorpsNum = _player.getCqCorpsNum();

	data = nullptr;   //避免浅复制

	//FC15的
	//m_cells = _player.cells();
	//m_techPoint = _player.techPoint();
	//m_RegenerationLevel = _player.getRegenerationLevel();
	//m_ExtraControlLevel = _player.getExtraControlLevel();
	//m_DefenceLevel = _player.getDefenceLevel();
	//m_MoveLevel = _player.getMoveLevel();
}


Player::~Player()
{
}

void Player::addTechPoint(TResourceD _techPoint)//增加相应科技点数
{
	m_techPoint += _techPoint;
}

//【冲突】涉及塔属性的访问
//玩家的科技点数再生，按所拥有的的每个塔贡献的再生速率再生
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

//减去相应资源数，不够减就不减并且返回false
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

//防御能力提升
bool Player::upgradeDefence()
{
	if (m_DefenceLevel < MAX_DEFENCE_LEVEL &&
		m_techPoint > DefenceStageUpdateCost[m_DefenceLevel])//等级没到顶，并且科技点数足够
	{
		subTechPoint(DefenceStageUpdateCost[m_DefenceLevel]);
		m_DefenceLevel++;
		return true;
	}
	else//失败就不扣除科技点
		return false;
}

//再生能力提升
bool Player::upgradeRegeneration()
{
	if (m_RegenerationLevel < MAX_REGENERATION_SPEED_LEVEL &&
		m_techPoint > RegenerationSpeedUpdateCost[m_RegenerationLevel])//等级没到顶，并且科技点数足够
	{
		subTechPoint(RegenerationSpeedUpdateCost[m_RegenerationLevel]);
		m_RegenerationLevel++;
		return true;
	}
	else//失败就不扣除科技点
		return false;
}

//额外控制数能力提升
bool Player::upgradeExtraControl()
{
	if (m_ExtraControlLevel < MAX_EXTRA_CONTROL_LEVEL &&
		m_techPoint > ExtraControlStageUpdateCost[m_ExtraControlLevel])//等级没到顶，并且科技点数足够
	{		
		subTechPoint(ExtraControlStageUpdateCost[m_ExtraControlLevel]);
		m_ExtraControlLevel++;
		return true;
	}
	else//失败就不扣除科技点
		return false;
}

//移动能力提升
bool Player::upgradeMove()
{
	if (m_MoveLevel < MAX_EXTENDING_SPEED_LEVEL &&
		m_techPoint > ExtendingSpeedUpdateCost[m_MoveLevel])//等级没到顶，并且科技点数足够
	{
		subTechPoint(ExtendingSpeedUpdateCost[m_MoveLevel]);
		m_MoveLevel++;
		return true;
	}
	else//失败就不扣除科技点
		return false;
}

//获取允许的最大控制指令条数
int Player::maxControlNumber()
{
	return (m_cells.size() + m_ExtraControlLevel) / 2 + 1;
}

//获取每个塔资源数总和
//【冲突】需要访问塔的属性
TResourceD Player::totalResource()
{
	TResourceD total = 0.0;
	for (TCellID i : m_cells)
	{
		total += data->cells[i].totalResource();
	}
	return total;
}

//杀死玩家
void Player::Kill()
{
	alive = false;
	deadRound = std::ceil(data->getRound() / 4); //返回死亡回合数是大回合数
}

/***********************************************************************************************
*函数名 :【FC18】getPlayScore获取玩家得分
*函数功能描述 : 根据玩家的防御塔和兵团信息，计算玩家当前得分，用于排名
*函数参数 : 无，直接由Player类调用
*函数返回值 : <int>分值
*作者 : 姜永鹏
***********************************************************************************************/
int Player::getPlayerScore() {
	if (isAlive() == false)   //玩家已经出局，根据出局的回合数记录进行分值
		return deadRound - MAX_ROUND;
	TScore corpsScore, towerScore;
	corpsScore = towerScore = 0;
	for (TCorpsID i : m_crops)
	{
		if (data->myCorps[i].getType() == Battle)  //兵团星级从0开始[!!!反复确认]
			corpsScore += BATTLE_CORP_SCORE * (data->myCorps[i].getLevel() + 1);
		else if (data->myCorps[i].getType() == Construct)
			corpsScore += CONSTRUCT_CORP_SCORE * 1;
	}
	for (TTowerID i : m_tower)
	{
		towerScore += TOWER_SCORE * (data->myTowers[i].getLevel());             //防御塔等级从0开始[!!!反复确认]
	}
	return corpsScore + towerScore;
}

/***********************************************************************************************
*函数名 :【FC18】addCrops
*函数功能描述 : 在m_crops中添加ID，在兵团构造时调用
*函数参数 : 添加的兵团ID
*函数返回值 : 无
*作者 : 吕梦欣
***********************************************************************************************/
void Player::addCrops(TCorpsID ID)
{
	m_crops.insert(ID);
}

/***********************************************************************************************
*函数名 :【FC18】deleteCrops
*函数功能描述 : 在m_crops中删除ID，在兵团内部检测到HP为0时调用
*函数参数 : 删除的兵团ID
*函数返回值 : 无
*作者 : 吕梦欣
***********************************************************************************************/
void Player::deleteCrops(TCorpsID ID)
{
	m_crops.erase(ID);
}