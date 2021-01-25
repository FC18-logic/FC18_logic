#include "Crops.h"

TCorpsID Crops::ID = 0;

Crops::Crops():m_data(nullptr)
{
	
}


Crops::~Crops(void)
{
}

Crops::Crops(DATA::Data* _data, corpsType type, battleCorpsType battletype, constructCorpsType buildtype, TPlayerID PlayerID, TPoint pos)
	:m_data(_data)
{
	m_type = type;
	m_BattleType = battletype;
	m_BuildType = buildtype;
	m_PlayerID = PlayerID;
	m_position = pos;
	m_myID = ID;
	ID++;
	m_PeaceNum = 0;
	m_level = 0;

	m_BuildPoint = 0;
	m_HealthPoint = 0;

	if(type == Battle)
	{
		m_MovePoint = battleMovePoint[battletype][m_level];
		m_HealthPoint = battleHealthPoint[battletype][m_level];
	}
	else{
		if(buildtype == Construct)
		{
			m_BuildPoint = 3;
		}
	}
	m_bStation = false;//兵团生产出来后默认驻守塔
}

/*
Move
兵团在地图内进行移动，无法判断目标地点是否存在别的兵团
dx：x坐标变化量 dy：y坐标变化量
返回是否移动成功，成功则更新位置
*/
bool Crops::Move(int dx, int dy)
{
	int curpos_x = m_position.m_x;
	int curpos_y = m_position.m_y;
	int next_x = curpos_x+dx;
	int next_y = curpos_y+dy;
	TPoint next_pos;
	next_pos.m_x = next_x;
	next_pos.m_y = next_y;
	terrainType curtype = m_data->gameMap.map[curpos_x][curpos_y].type;
	//越界
	if(!m_data->gameMap.isPosValid(next_pos))
	{
		return false;
	}
	terrainType nexttype = m_data->gameMap.map[next_x][next_y].type;
	float dMP = (CorpsMoveCost[curtype]+CorpsMoveCost[nexttype])/2;
	int temp = m_MovePoint - ceil(dMP);
	//行动力不够
	if(temp<0)
	{
		return false;
	}
	m_MovePoint = temp;
	return true;
}


/*
AttackCrops
该兵团对另一兵团发起攻击，负责计算攻击力、攻击兵团、并移动
参数 enemy 受攻击兵团指针
返回值 bool 攻击成功返回true
lmx
等待进一步修改，和攻击塔合并
*/
bool Crops::AttackCrops(Crops* enemy)
{
	if(m_MovePoint == 0)
	{
		return false;
	}
	m_MovePoint = 0;
	//如果没有该敌人
	if(!enemy)
	{
		return false;
	}
	//敌人已阵亡
	if(!enemy->bAlive())
	{
		return false;
	}
	//如果不是战斗兵
	if(m_type!=Battle)
	{
		return false;
	}
	//如果敌人不在射程范围内
	//添加代码
	//
	//计算战斗力
	TBattlePoint myCE = getCE();
	TBattlePoint enemyCE = enemy->getCE();
	//计算损失
	int deta = myCE-enemyCE;
	int mylost = floor(28*pow(2.71828,-deta));
	int enemylost = floor(30*pow(2.71828,deta));
	//计算生命力
	if(m_BattleType!= Archer)
	{
		m_HealthPoint -= mylost;
	}
	if(m_HealthPoint < 0)
	{
		m_HealthPoint = 0;
	}
	//如果对方死亡 则移动位置
	if(!enemy->BeAttacked(enemylost,this))
	{
		if(m_BattleType!= Archer)
		{
			//移动
			m_position = enemy->m_position;
		}
	}
	return true;
}

/*
BeAttacked
兵团受到攻击时调用，内部调用
参数：attack 受到的伤害值，enemy 发动攻击的兵团指针
返回值：是否存活 存活返回true
*/
bool Crops::BeAttacked(int attack,Crops* enemy)
{
	//俘虏工程兵
	if(m_type == Construct)
	{
		m_PlayerID = enemy->m_PlayerID;
		return true;
	}
	//战斗兵
	m_PeaceNum = 0;
	m_HealthPoint -= attack;
	if(m_HealthPoint <= 0)
	{
		m_HealthPoint = 0;
		return false;
	}
	return true;
}

bool Crops::bAlive()
{
	if(m_type == Battle)
	{
		if(m_HealthPoint>0)
		{
			return true;
		}
	}
	else if(m_BuildType == Builder)
	{
		if(m_BuildPoint>0)
		{
			return true;
		}
	}
	else
	{
		return true;
	}
	return false;
}

/*
getCE
返回该兵团的战斗力
*/
TBattlePoint Crops::getCE()
{
	//战斗力增益
	terrainType type = m_data->gameMap.map[m_position.m_x][m_position.m_y].type;
	TBattlePoint Attack = CorpsBattleGain[type];
	//总战斗力
	Attack += (TBattlePoint)ceil(
		(float)(corpsBattlePoint[m_BattleType][m_level]*m_HealthPoint)
		/(float)battleHealthPoint[m_BattleType][m_level]);
	return Attack;
}


/*
Recover
兵团回复生命力，驻扎后每回合调用，函数内部判断是否进行恢复
无参数返回值
*/
void Crops::Recover()
{
	if(m_type == Construct)
	{
		return;
	}
	if(!m_bStation)
	{
		return;
	}
	if(m_PeaceNum == 3)
	{
		m_HealthPoint += floor(battleHealthPoint[m_BattleType][m_level]/3);
		if(m_HealthPoint > battleHealthPoint[m_BattleType][m_level])
		{
			m_HealthPoint = battleHealthPoint[m_BattleType][m_level];
		}
		return;
	}
	m_PeaceNum ++;
}

/*
MergeCrops
兵团整编，发起整编的兵团调用，函数内部删除对方兵团
参数：cropsID：对面兵团的ID
返回是否整编成功
*/
bool Crops::MergeCrops(Crops* targetCrops)
{
	//如果无行动力
	if(m_MovePoint == 0)
	{
		return false;
	}
	//如果目标队伍不存在
	if(targetCrops == NULL)
	{
		return false;
	}
	//如果不同阵营
	if(m_PlayerID!=targetCrops->m_PlayerID)
	{
		return false;
	}
	//如果不同类型
	if(m_type!=Battle||targetCrops->m_type!=Battle)
	{
		return false;
	}
	if(m_BattleType!=targetCrops->m_BattleType)
	{
		return false;
	}
	//如果星级不符合要求
	int level1 = m_level+1;
	int level2 = targetCrops->m_level+1;
	if(level2>level1)
	{
		return false;
	}
	if(level1+level2 >3)
	{
		return false;
	}
	//整编
	float HPSum = m_HealthPoint+targetCrops->m_HealthPoint;
	float TotalSum = battleHealthPoint[m_BattleType][m_level]+battleHealthPoint[m_BattleType][targetCrops->m_level];
	m_level = level1+level2-1;
	m_HealthPoint = battleHealthPoint[m_BattleType][m_level]*HPSum/TotalSum;
	m_MovePoint = 0;
	//对方兵团HP归零，等待删除
	targetCrops->m_HealthPoint = 0;
	//
}

/*
ResetMP
回合开始时重置行动力
*/
void Crops::ResetMP()
{
	if(m_type == Battle)
	{
		m_MovePoint = battleMovePoint[m_BattleType][m_level];
	}
	else
	{
		m_MovePoint = constructMovePoint[m_BattleType];
	}
}


bool Crops::ChangeTerrain(terrainType target)
{
	if(!bAlive())
	{
		return false;
	}
	if(m_type != Construct)
		return false;
	if(m_BuildType != Builder)
		return false;
	int curID = m_data->gameMap.map[m_position.m_x][m_position.m_y].owner;
	if(curID != m_PlayerID)
	{
		return false;
	}
	m_data->gameMap.map[m_position.m_x][m_position.m_y].type = target;
	m_BuildPoint--;
	return true;
}
