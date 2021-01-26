#include "Crops.h"
#include "tower.h"

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
	m_bResting = true;//兵团生产出来后默认驻守塔
	m_data->corps[m_position.m_x][m_position.m_y].push_back(this);
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
	//判断目标位置是否寻找己方塔
	bool haveTower = false;
	int index = m_data->gameMap.map[next_x][next_y].TowerIndex;
	if(index != HAVE_NO_TOWER)
	{
		class Tower targettower = m_data->myTowers[index];
		if(targettower.showPlayerID() != m_PlayerID)
		{
			return false;
		}
		else
		{
			haveTower = true;
		}
	}
	if(!haveTower)
	{
		//目标位置是否存在别的兵团
		CorpsUnit targetPos = m_data->corps[next_x][next_y];
		//工程兵
		if(m_type == Construct)
		{
			if(targetPos.size()!=0)
				return false;
		}
		//战斗兵
		else
		{
			for(int i = 0; i<targetPos.size(); i++)
			{
				if(targetPos[i]->m_type == Construct && targetPos[i]->m_PlayerID == m_PlayerID)
				{
					continue;
				}
				return false;
			}
		}
	}
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
	m_bResting = false;
	//修改data中的位置
	UpdatePos(next_pos);
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
	//如果敌人不在射程范围内
	//添加代码
	//
	int enemylost = 0;
	//如果敌人是战斗兵
	if(enemy->m_type == Battle)
	{
		//计算战斗力
		TBattlePoint myCE = getCE();
		TBattlePoint enemyCE = enemy->getCE();
		//计算损失
		float deta = 0.04*((float)myCE-enemyCE);
		int mylost = floor(28*pow(2.71828,-deta));
		enemylost = floor(30*pow(2.71828,deta));
		//计算生命力
		if(m_BattleType!= Archer)
		{
			m_HealthPoint -= mylost;
		}
		if(m_HealthPoint < 0)
		{
			m_HealthPoint = 0;
		}
	}

	//如果对方死亡 则移动位置
	if(!enemy->BeAttacked(enemylost,m_PlayerID))
	{
		if(m_BattleType!= Archer)
		{
			//移动
			UpdatePos(enemy->m_position);
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
bool Crops::BeAttacked(int attack,TPlayerID ID)
{
	//俘虏工程兵
	if(m_type == Construct)
	{
		m_PlayerID = ID;
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
兵团回复生命力，函数内部判断是否进行恢复
无参数返回值
*/
void Crops::Recover()
{
	if(m_type == Construct)
	{
		return;
	}
	if(!m_bResting)
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
参数：对方兵团指针，待定 也可以是位置或者ID
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
	float TotalSum = battleHealthPoint[m_BattleType][m_level]
					+battleHealthPoint[m_BattleType][targetCrops->m_level];
	m_level = level1+level2-1;
	m_HealthPoint = battleHealthPoint[m_BattleType][m_level]*HPSum/TotalSum;
	m_MovePoint = 0;
	//对方兵团HP归零，等待删除
	targetCrops->m_HealthPoint = 0;
	m_bResting = false;
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

/*
ChangeTerrain
建筑兵改造所在单元地形
参数：target 目标地形
返回是否改造成功
*/
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
	if(m_data->gameMap.map[m_position.m_x][m_position.m_y].TowerIndex == HAVE_NO_TOWER)
	{
		m_data->gameMap.map[m_position.m_x][m_position.m_y].type = target;
		m_BuildPoint--;
		m_bResting = false;
		return true;
	}
	return false;
}

//新回合开始
void Crops::newRound()
{
	ResetMP();
	Recover();
	GoRest();
}

//进入驻扎休整状态
void Crops::GoRest()
{
	if(!m_bResting)
	{
		m_PeaceNum = 0;//开始计数
		m_bResting = true;
	}
}

/*
ShowInfo
提供兵团信息
返回兵团信息结构体
*/
struct CorpsInfo Crops::ShowInfo()
{
	struct CorpsInfo info;
	info.BuildPoint = m_BuildPoint;
	info.HealthPoint = m_HealthPoint;
	info.exist = 1;
	info.ID = m_myID;
	info.level = m_level;
	info.owner = m_PlayerID;
	info.pos = m_position;
	info.type = m_type;
	info.movePoint = m_MovePoint;
	return info;
}

/*
AttackTower
*/
bool Crops::AttackTower(class Tower *enemy)
{
	int TowerCE;//塔的战斗力
	int myCE = getCE();//兵团战斗力
	//敌人已阵亡或不在射程范围内
	//添加代码
	//
	float deta = 0.04*((float)myCE-TowerCE);
	int mylost = floor(28*pow(2.71828,-deta));
	int enemylost = floor(30*pow(2.71828,deta))*corpsAttackTowerGain[m_BattleType][m_level];
	if(m_BattleType == Archer)
		mylost = 0;
	m_HealthPoint -= mylost;
	if(m_HealthPoint < 0)
		m_HealthPoint = 0;
	bool canmove = false;
	//判断塔是否被摧毁
	//添加代码
	//
	if(m_BattleType == Archer)
		canmove = false;
	if(canmove)
	{
		UpdatePos(enemy->showPosition());
	}
	return true;
}

/*
Attack
兵团发动攻击 返回是否攻击成功
参数 type<CorpsCommandEnum> 攻击敌方兵团或塔， ID 敌方ID
*/
bool Crops::Attack(CorpsCommandEnum type, int ID)
{
	//如果不是战斗兵
	if(m_type!=Battle)
	{
		return false;
	}

	m_bResting = false;
	if(m_MovePoint == 0)
	{
		return false;
	}
	m_MovePoint = 0;
	if(type == CAttackCorps)
	{
		Crops* enemy = NULL;
		//寻找敌人
		enemy = &m_data->myCorps[ID];
		//敌人已阵亡
		if(!enemy->bAlive())
		{
			return false;
		}
		//敌人同阵营
		if(enemy->m_PlayerID == m_PlayerID)
			return false;
		//如果敌人所在位置存在敌方势力塔 优先与塔结算
		int x = enemy->m_position.m_x;
		int y = enemy->m_position.m_y;
		int index = m_data->gameMap.map[x][y].TowerIndex;
		if(index == NOTOWER)
			return AttackCrops(enemy);
		if(m_data->myTowers[index].showPlayerID() == enemy->m_PlayerID)
			return AttackTower(&(m_data->myTowers[index]));
		else
			return AttackCrops(enemy);
	}
	else if(type == CAttackTower)
	{
		class Tower* enemy = NULL;
		//寻找敌人
		enemy = &m_data->myTowers[ID];
		//敌人已阵亡
		//添加代码
		//
		//敌人同阵营
		if(enemy->showPlayerID() == m_PlayerID)
			return false;
		return AttackTower(enemy);
	}
	return false;
}


void Crops::UpdatePos(TPoint targetpos)
{
	CorpsUnit thisplace = m_data->corps[m_position.m_x][m_position.m_y];
	CorpsUnit::iterator it;
	for(it = thisplace.begin(); it!=thisplace.end(); it++)
	{
		if(*it == this)
		{
			thisplace.erase(it);
			break;
		}
	}
	m_data->corps[targetpos.m_x][targetpos.m_y].push_back(this);
	m_position = targetpos;
}
