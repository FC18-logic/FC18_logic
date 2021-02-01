#include "Crops.h"
#include "tower.h"

TCorpsID Crops::m_staticID = 0;

Crops::Crops():m_data(nullptr)
{
	
}


Crops::~Crops(void)
{
}

Crops::Crops(DATA::Data* _data, corpsType type, battleCorpsType battletype, constructCorpsType buildtype, TPlayerID PlayerID, TPoint pos)
	:m_data(_data)
{
	m_bAlive = true;
	m_type = type;
	m_BattleType = battletype;
	m_BuildType = buildtype;
	m_PlayerID = PlayerID;
	m_position = pos;
	m_myID = m_staticID;
	m_staticID++;
	m_PeaceNum = 0;
	m_level = 0;
	m_StationTower = nullptr;//兵团生产出来后，需要指令才驻守当地的塔
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
	m_bResting = true;//兵团生产出来后默认休整
	m_data->corps[m_position.m_y][m_position.m_x].push_back(this);
	m_data->players[m_PlayerID - 1].addCrops(m_myID);
	m_data->totalCorps++;
	m_data->newCorps.insert(m_myID);//记录新产生的兵团序号
}


Crops::Crops(const Crops& corps):m_data(corps.m_data)
{
	m_myID = corps.m_myID;
	m_type = corps.m_type;
	m_BattleType = corps.m_BattleType;
	m_BuildType = corps.m_BuildType;
	m_PlayerID = corps.m_PlayerID;
	m_MovePoint = corps.m_MovePoint;
	m_HealthPoint = corps.m_HealthPoint;
	m_level = corps.m_level;
	m_BuildPoint = corps.m_BuildPoint;
	m_bResting = corps.m_bResting;
	m_position = corps.m_position;
	m_PeaceNum = corps.m_PeaceNum;
	m_bAlive = corps.m_bAlive;
}


/*
Move
兵团在地图内进行移动，无法判断目标地点是否存在别的兵团
dx：x坐标变化量 dy：y坐标变化量
返回是否移动成功，成功则更新位置
*/
bool Crops::Move(int dir)
{
	if(!m_bAlive)
	{
		return false;
	}
	int curpos_x = m_position.m_x;
	int curpos_y = m_position.m_y;
	int next_x = curpos_x;
	int next_y = curpos_y;
	switch(dir)
	{
	case CUp:
		{
			next_y--;
		}
		break;
	case CDown:
		{
			next_y++;
		}
		break;
	case CLeft:
		{
			next_x--;
		}
		break;
	case CRight:
		{
			next_x++;
		}
		break;
	default:
		{
			return false;
		}
	}
	TPoint next_pos;
	next_pos.m_x = next_x;
	next_pos.m_y = next_y;
	if (m_data->gameMap.withinMap(next_pos) == false) return false;  //by jyp要前往的位置不在地图内，判断失败

	//判断目标位置是否存在己方塔
	bool haveTower = false;
	int index = m_data->gameMap.map[next_y][next_x].TowerIndex;
	if(index != NOTOWER)
	{
		class Tower targettower = m_data->myTowers[index];
		if(targettower.getPlayerID() != m_PlayerID)
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
		CorpsUnit targetPos = m_data->corps[next_y][next_x];
		
		//工程兵移动
		if(m_type == Construct)
		{
			for(int i = 0; i<targetPos.size(); i++)
			{
				if(targetPos[i]->m_type == Battle && targetPos[i]->m_PlayerID == m_PlayerID)
				{
					continue;
				}
				return false;
			}
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
	terrainType curtype = m_data->gameMap.map[curpos_y][curpos_x].type;
	terrainType nexttype = m_data->gameMap.map[next_y][next_x].type;
	float dMP = (float(CorpsMoveCost[curtype])+float(CorpsMoveCost[nexttype]))/2.0;
	int temp = m_MovePoint - ceil(dMP);
	//行动力不够
	if(temp<0)
	{
		return false;
	}
	m_MovePoint = temp;

	//修改data中的位置
	UpdatePos(next_pos);
	return true;
}


/*
AttackCrops
该兵团对另一兵团发起攻击，负责计算攻击力、攻击兵团、并移动
参数 enemy 受攻击兵团指针
返回值 int 减少的HP
lmx
*/
int Crops::AttackCrops(Crops* enemy)
{
	int enemylost = 0;
	int mylost = 0;
	//如果敌方是工程兵
	if(enemy->m_type == Construct)
	{
		Crops* colleage = NULL;
		//是否存在护卫
		for(int i = 0; i<m_data->corps[m_position.m_y][m_position.m_x].size(); i++)
		{
			colleage = m_data->corps[m_position.m_y][m_position.m_x][i];
			if(colleage->m_type == Battle&&colleage->m_PlayerID == enemy->m_PlayerID)
				enemy = colleage;
		}
	}

	//如果敌人是战斗兵
	if(enemy->m_type == Battle)
	{
		//计算战斗力
		TBattlePoint myCE = getCE();
		TBattlePoint enemyCE = enemy->getCE();
		//计算损失
		float deta = 0.04*((float)myCE-enemyCE);
		mylost = floor(28*pow(2.71828,-deta));
		enemylost = floor(30*pow(2.71828,deta));
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
	return mylost;
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
		ChangeOwner(ID);
		int num = m_data->players[ID - 1].getCqCorpsNum() + 1;
		m_data->players[ID - 1].setCqCorpsNum(num);
		return true;
	}
	//战斗兵
	m_PeaceNum = 0;
	m_HealthPoint -= attack;
	if(m_HealthPoint<=0)
	{
		KillCorps();
		int num = m_data->players[ID - 1].getElCorpsNum() + 1;
		m_data->players[ID - 1].setElCorpsNum(num);
		//同一位置的工程兵被俘虏
		Crops* colleage = NULL;
		for(int i = 0; i<m_data->corps[m_position.m_y][m_position.m_x].size(); i++)
		{
			colleage = m_data->corps[m_position.m_y][m_position.m_x][i];
			//如果工程兵和正在被攻击的战斗兵是同一玩家的兵团
			if(colleage->m_type == Construct&&colleage->m_PlayerID == m_PlayerID)
				colleage->BeAttacked(0,ID);
		}
	}
	m_bResting = false;
	m_PeaceNum = 0;
	return m_bAlive;
}

bool Crops::bAlive()
{
	return m_bAlive;
}

/*
getCE
返回该兵团的战斗力
*/
TBattlePoint Crops::getCE()
{
	//战斗力增益
	terrainType type = m_data->gameMap.map[m_position.m_y][m_position.m_x].type;
	TBattlePoint Attack = CorpsBattleGain[type];
	//总战斗力
	Attack += (TBattlePoint)ceil(
		(float)(corpsBattlePoint[m_BattleType][m_level]*m_HealthPoint)
		/(float)battleHealthPoint[m_BattleType][m_level]);
	return Attack;
}


/*
Recover
根据上一回合状态兵团回复生命力，函数内部判断是否进行恢复
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
bool Crops::MergeCrops(TCorpsID targetID)
{
	Crops* targetCrops = NULL;
	targetCrops = &(m_data->myCorps[targetID]);
	//如果目标队伍不存在
	if(targetCrops == NULL)
		return false;

	//如果无行动力
	if(m_MovePoint == 0)
		return false;

	//如果一方已经阵亡
	if(!m_bAlive||!(targetCrops->m_bAlive))
		return false;

	//如果不同阵营
	if(m_PlayerID!=targetCrops->m_PlayerID)
		return false;

	//如果至少其中一方不是战斗兵团，不同类型
	if(m_type!=Battle||targetCrops->m_type!=Battle)
		return false;
	if(m_BattleType!=targetCrops->m_BattleType)
		return false;

	//如果不是邻居，可以在当前格或者周围的8个格里
	if(!IsNeighbor(targetCrops->m_position))
		return false;

	//如果星级不符合要求
	int level1 = m_level+1;
	int level2 = targetCrops->m_level+1;
	if(level2>level1)
		return false;
	if(level1+level2 >3)
		return false;

	//整编
	float HPSum = m_HealthPoint+targetCrops->m_HealthPoint;
	float TotalSum = battleHealthPoint[m_BattleType][m_level]
					+battleHealthPoint[m_BattleType][targetCrops->m_level];
	m_level = level1+level2-1;
	m_HealthPoint = battleHealthPoint[m_BattleType][m_level]*HPSum/TotalSum;

	m_MovePoint = 0;
	targetCrops->KillCorps();
	return true;
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
	if(!m_bAlive)
		return false;
	if(m_type != Construct)
		return false;
	if(m_BuildType != Builder)
		return false;
	int curID = m_data->gameMap.map[m_position.m_y][m_position.m_x].owner;
	if(curID != m_PlayerID)
		return false;
	if(m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex == NOTOWER)
	{
		m_data->gameMap.map[m_position.m_y][m_position.m_x].type = target;
		m_BuildPoint--;
		if(m_BuildPoint == 0)
		{
			KillCorps();
		}
		return true;
	}
	return false;
}

/*
newRound
新回合开始，回复HP和MP
*/
void Crops::newRound()
{
	if(m_bAlive)
	{
		ResetMP();
		Recover();
		if(!m_bResting)
		{
			m_PeaceNum = 0;//开始计数
			m_bResting = true;
		}
	}
}

/*
GoRest
进入驻扎休整状态
如果该位置没有塔则返回true，有塔则返回false
*/
bool Crops::GoRest()
{
	//如果有塔
	if(m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex!=NOTOWER)
	{
		return false;
	}

	//如果之前没有处在驻扎状态中，则先将变量清零并进入驻扎状态
	if(!m_bResting)
	{
		m_PeaceNum = 0;//开始计数
		m_bResting = true;
	}
	return true;
}

/*
StationInTower
如果该位置存在己方势力的塔，则返回true
*/
bool Crops::StationInTower()
{
	bool bStation = false;
	int index = m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex;
	//如果选择驻扎的位置有塔 则驻扎在塔中
	if(index != NOTOWER)
	{
		//如果塔属于己方则驻扎
		if(m_data->myTowers[index].getPlayerID() == m_PlayerID)
		{
			m_StationTower = &(m_data->myTowers[index]);
			m_StationTower->input_staycrops(this);
			bStation = true;
		}
	}
	return bStation;
}


/*
ShowInfo
提供兵团信息
返回兵团信息结构体
*/
struct CorpsInfo Crops::ShowInfo()
{
	struct CorpsInfo info;
	info.ID = m_myID;
	info.level = m_level;
	info.owner = m_PlayerID;
	info.pos = m_position;
	info.type = m_type;
	info.movePoint = m_MovePoint;
	m_HealthPoint = m_BuildPoint = 0;
	m_BattleType = Warrior;
	m_BuildType = Builder;
	if (info.type == Battle) {
		info.m_BattleType = m_BattleType;
		info.HealthPoint = m_HealthPoint;  //玩家用生命值自己算战斗力，不提供
	}
	else if (info.type == Construct) {
		info.m_BuildType = m_BuildType;
		info.BuildPoint = m_BuildPoint;
	}
	return info;
}

/*
AttackTower
*/
int Crops::AttackTower(class Tower *enemy)
{
	int TowerCE;//塔的战斗力
	//获取塔的战斗力
	TowerCE = enemy->get_towerbp();
	int myCE = getCE();//兵团战斗力
	
	float deta = 0.04*((float)myCE-TowerCE);
	int mylost = floor(28*pow(2.71828,-deta));
	int enemylost = floor(30*pow(2.71828,deta))*corpsAttackTowerGain[m_BattleType][m_level];

	bool IsTowerDestroy = false;
	//判断塔是否被攻陷(占领、摧毁都算)
	IsTowerDestroy = enemy->Be_Attacked(m_PlayerID, enemylost);
	if(IsTowerDestroy/*&&!(enemy->getexsit())*/)
	{
		int num = m_data->players[m_PlayerID - 1].getCqTowerNum() + 1;
		m_data->players[m_PlayerID - 1].setCqTowerNum(num);
	}

	//
	if(m_BattleType == Archer)
		IsTowerDestroy = false;
	if(IsTowerDestroy)
	{
		UpdatePos(enemy->getPosition());
	}
	return mylost;
}

/*
Attack
兵团发动攻击 返回是否攻击成功
参数 type<CorpsCommandEnum> 攻击敌方兵团或塔， ID 敌方ID
*/
bool Crops::Attack(int type, TCorpsID ID)
{
	//如果不是战斗兵
	if(m_type!=Battle)
		return false;

	//如果已经死亡，已经在controller里判断过了
	//if(!m_bAlive)
		//return false;

	//行动力不足
	if(m_MovePoint == 0)
		return false;

	m_MovePoint = 0;

	//我方HP减少值
	int mylost = 0;
	if(type == CAttackCorps)
	{
		Crops* enemy = NULL;
		//寻找敌人
		enemy = &m_data->myCorps[ID];
		//敌人已阵亡
		if(!enemy->m_bAlive)
			return false;
		//敌人同阵营
		if(enemy->m_PlayerID == m_PlayerID)
			return false;
		//敌人不在射程范围内
		if(!IsInRange(enemy->m_position))
			return false;

		//如果敌人所在位置存在敌方势力塔 优先与塔结算
		int x = enemy->m_position.m_x;
		int y = enemy->m_position.m_y;
		int index = m_data->gameMap.map[y][x].TowerIndex;
		if(index == NOTOWER)
			mylost = AttackCrops(enemy);
		else if(m_data->myTowers[index].getPlayerID() == enemy->m_PlayerID)
			mylost =  AttackTower(&(m_data->myTowers[index]));
		else
			mylost = AttackCrops(enemy);
	}
	else if(type == CAttackTower)
	{
		class Tower* enemy = NULL;
		//寻找敌人
		enemy = &m_data->myTowers[ID];
		//敌人已阵亡
		if(!enemy->getexsit())
			return false;
		//敌人同阵营
		if(enemy->getPlayerID() == m_PlayerID)
			return false;
		//敌人不在射程范围内
		if(!IsInRange(enemy->getPosition()))
			return false;

		mylost = AttackTower(enemy);
	}
	else//指令有误
		return false;
	//计算生命力
	if(m_BattleType!= Archer)
	{
		m_HealthPoint -= mylost;
	}
	//如果已阵亡
	if(m_HealthPoint <= 0)
	{
		KillCorps();
	}

	return true;
}

/*
UpdatePos 更新位置
更新在m_data中corps记录的位置以及类内部的位置
参数 新的位置
*/
void Crops::UpdatePos(TPoint targetpos)
{
	CorpsUnit* thisplace = &(m_data->corps[m_position.m_y][m_position.m_x]);
	CorpsUnit::iterator it;
	for(it = (*thisplace).begin(); it!=(*thisplace).end(); it++)
	{
		if(*it == this)
		{
			(*thisplace).erase(it);
			break;
		}
	}
	m_data->corps[targetpos.m_y][targetpos.m_x].push_back(this);
	m_position = targetpos;
	//如果驻守在塔中 则删除
	if(m_StationTower)
	{
		m_StationTower->remove_crop(m_myID);
		m_StationTower = NULL;
	}

}

/*
ChangeOwner
改变所有者 修改data中对应的player中的m_corps
参数 新主人ID
*/
void Crops::ChangeOwner(TPlayerID newowner)
{
	m_data->players[m_PlayerID - 1].deleteCrops(m_myID);
	m_PlayerID = newowner;
	m_data->players[m_PlayerID - 1].addCrops(m_myID);
}

/*
KillCorps
兵团死亡 修改player、data以及map中的数据 将兵团状态设置为死亡
*/
void Crops::KillCorps()
{
	m_HealthPoint = 0;
	m_BuildPoint = 0;
	m_data->players[m_PlayerID - 1].deleteCrops(m_myID);
	m_data->totalCorps--;
	CorpsUnit* thisplace = &(m_data->corps[m_position.m_y][m_position.m_x]);
	CorpsUnit::iterator it;
	for(it = (*thisplace).begin(); it!=(*thisplace).end(); it++)
	{
		if(*it == this)
		{
			(*thisplace).erase(it);
			break;
		}
	}
	m_bAlive = false;
	m_data->dieCorps.insert(m_myID);   //记录死亡兵团ID
}

//建筑兵修塔
bool Crops::MendTower()
{
	if(!m_bAlive)
		return false;
	if(m_type != Construct)
		return false;
	if(m_BuildType != Builder)
		return false;

	//获取该位置塔的信息
	int index = m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex;
	//没有塔
	if(index == NOTOWER)
		return false;
	//不是己方塔
	if(m_data->myTowers[index].getPlayerID()!=m_PlayerID)
		return false;
	//塔已被摧毁
	if(!m_data->myTowers[index].getexsit())
		return false;

	m_BuildPoint--;
	if(m_BuildPoint <= 0)
	{
		KillCorps();
	}

	m_data->myTowers[index].Recover();
	return true;
}

//开拓者建塔
bool Crops::BuildTower()
{
	if (!m_bAlive)//by jyp 当前兵团死亡
		return false;
	if (m_type != Construct)//by jyp当前兵团不是工程兵团
		return false;
	if (m_BuildType != Extender)//by jyp当前兵团不是开拓者
		return false;

	//获取该位置塔的信息
	int index = m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex;
	//如果该位置已存在塔
	if(index != NOTOWER)
		return false;

	//获取该位置所有者属性
	TPlayerID OwnerID;
	OwnerID = m_data->gameMap.showOwner(m_position);
	//
	//如果该单元格属于己方
	if(OwnerID != OUTOFRANGE && OwnerID == m_PlayerID)
	{
		//新建一个防御塔
		Tower newTower(m_data, m_PlayerID, m_position);
		m_data->myTowers.push_back(newTower);
		//开拓者没有劳动力属性 by lmx
		//m_BuildPoint--;  //by jyp 工程兵团劳动力-1
		return true;
	}
	return false;
}

bool Crops::IsNeighbor(TPoint point)
{
	if(abs(point.m_x-m_position.m_x) <= 1&&abs(point.m_y-m_position.m_y) <= 1)
		return true;
	return false;
}

bool Crops::IsInRange(TPoint point)
{
	int range = TBattleRange[m_BattleType];
	if(abs(point.m_x-m_position.m_x) <= range&&abs(point.m_y-m_position.m_y) <= range)
		return true;
	return false;
}


void Crops::haveCmd()
{
	m_bResting = false;
	m_PeaceNum = 0;
}
