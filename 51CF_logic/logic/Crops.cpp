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
//	m_PeaceNum = 0;
	m_level = 0;
	m_BuildPoint = 0;
	m_HealthPoint = 0;
	m_MovePoint = 0;

	if (type == Battle)
	{
		m_MovePoint = battleMovePoint[battletype][m_level];
		m_HealthPoint = battleHealthPoint[battletype][m_level];
	}
	else if (type == Construct)
	{
		m_MovePoint = constructMovePoint[buildtype];
		if(buildtype == Builder )m_BuildPoint = 3;
	}
	m_data->gameMap.map[m_position.m_y][m_position.m_x].corps.push_back(m_myID);
	m_data->players[m_PlayerID - 1].addCrops(m_myID);
	m_data->totalCorps++;
	m_data->newCorps.insert(m_myID);//记录新产生的兵团序号
}




/*
Move
兵团在地图内进行移动
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

	//判断目标位置是否存在塔
	bool haveTower = false;
	int index = m_data->gameMap.map[next_y][next_x].TowerIndex;
	if(index != NOTOWER)
	{
		haveTower = true;
	}
	if(!haveTower)
	{
		//目标位置是否存在别的兵团
		vector<TCorpsID> targetPos = m_data->gameMap.map[next_y][next_x].corps;
		if(m_type == Construct)
		{
			for(int i = 0; i<targetPos.size(); i++)
			{
				int index = targetPos[i];
				if(m_data->myCorps[index].m_type == Battle && m_data->myCorps[index].m_PlayerID == m_PlayerID)
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
				int index = targetPos[i];
				if(m_data->myCorps[index].m_type == Construct && m_data->myCorps[index].m_PlayerID == m_PlayerID)
				{
					continue;
				}
				return false;
			}
		}
	}
	terrainType curtype = m_data->gameMap.map[curpos_y][curpos_x].type;
	terrainType nexttype = m_data->gameMap.map[next_y][next_x].type;
	float dMP = (float(CorpsMoveCost[curtype])+float(CorpsMoveCost[nexttype]))/2.0f;
	int temp = m_MovePoint - ceil(dMP);
	//行动力不够
	if (temp < 0)
	{
		return false;
	}
	m_MovePoint = temp;

	//修改data中的位置
	UpdatePos(next_pos);
	m_data->changeCorps.insert(m_myID);//by jyp:记录兵团的移动力改变
	return true;
}


/*
AttackCrops
该兵团对另一兵团发起攻击，负责计算攻击力、攻击兵团、并移动
参数 enemy 受攻击兵团指针
lmx
*/
void Crops::AttackCrops(Crops* enemy)
{
	int enemylost = 0;
	int mylost = 0;
	//如果敌方是工程兵
	if(enemy->m_type == Construct)
	{
		Crops* colleage = NULL;
		//是否存在护卫
		for(int i = 0; i< m_data->gameMap.map[enemy->m_position.m_y][enemy->m_position.m_x].corps.size(); i++)
		{
			int index = m_data->gameMap.map[enemy->m_position.m_y][enemy->m_position.m_x].corps[i];
			colleage = &(m_data->myCorps[index]);
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
		double deta = 0.04 * ((double)myCE - enemyCE);
		mylost = floor(28 * pow(2.71828, -deta));
		enemylost = floor(30 * pow(2.71828, deta));
	}
	//计算生命力
	if (m_BattleType != Archer)
	{
		m_HealthPoint -= mylost;
	}
	//如果已阵亡
	if (m_HealthPoint <= 0)
	{
		KillCorps();
		int num = m_data->players[enemy->getPlayerID() - 1].getElCorpsNum();
		m_data->players[enemy->getPlayerID() - 1].setElCorpsNum(num + 1);
	}
	//如果对方死亡 则移动位置
	if (!enemy->BeAttacked(enemylost, m_PlayerID, m_bAlive))
	{
		if (m_BattleType != Archer && m_bAlive)
		{
			//移动
			UpdatePos(enemy->m_position);
		}
	}
}

/*
BeAttacked
兵团受到攻击时调用，内部调用
参数：attack 受到的伤害值，enemy 发动攻击的兵团指针, bAlive 攻击方是否存活
返回值：是否存活 存活返回true
*/
bool Crops::BeAttacked(int attack, TPlayerID ID, bool bAlive)
{
	//俘虏工程兵
	if(m_type == Construct)
	{
		if(bAlive){   
			if (m_data->players[ID - 1].constructNumControl() == true)//如果攻击方的工程兵团数目满了，那么不俘虏，直接消灭
			{
				KillCorps();
				int num = m_data->players[ID - 1].getElCorpsNum() + 1;
				m_data->players[ID - 1].setElCorpsNum(num);
				return false;
			}
			ChangeOwner(ID);
			int num = m_data->players[ID - 1].getCqCorpsNum() + 1;
			m_data->players[ID - 1].setCqCorpsNum(num);
			m_data->changeCorps.insert(m_myID);//by jyp:记录工程兵团所有者改变
		}
		return true;//不移动 如果移动则return false
	}
	//战斗兵
	//m_PeaceNum = 0;
	m_HealthPoint -= attack;
	m_data->changeCorps.insert(m_myID);//by jyp:记录作战兵团生命值等改变
	if(m_HealthPoint<=0)
	{
		KillCorps();
		int num = m_data->players[ID - 1].getElCorpsNum() + 1;
		m_data->players[ID - 1].setElCorpsNum(num);
		//同一位置的工程兵被俘虏
		Crops* colleage = NULL;
		for (int i = 0; i < m_data->gameMap.map[m_position.m_y][m_position.m_x].corps.size(); i++)
		{
			int index = m_data->gameMap.map[m_position.m_y][m_position.m_x].corps[i];
			colleage = &(m_data->myCorps[index]);
			//如果工程兵和正在被攻击的战斗兵是同一玩家的兵团
			if(colleage->m_type == Construct&&colleage->m_PlayerID == m_PlayerID)
				colleage->BeAttacked(0,ID,bAlive);
		}
	}
	//m_bResting = false;
	//m_PeaceNum = 0;
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
/*
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
		m_HealthPoint += (int)floor((float)battleHealthPoint[m_BattleType][m_level]/3.0F);
		if(m_HealthPoint > battleHealthPoint[m_BattleType][m_level])
		{
			m_HealthPoint = battleHealthPoint[m_BattleType][m_level];
		}
		m_data->changeCorps.insert(m_myID);
		return;
	}
	m_PeaceNum ++;
}
*/

/*
ResetMP
回合开始时重置行动力
*/
void Crops::ResetMP()
{
	if(m_type == Battle)
	{
		if (m_MovePoint != battleMovePoint[m_BattleType][m_level])
		{
			m_MovePoint = battleMovePoint[m_BattleType][m_level];
			m_data->changeCorps.insert(m_myID);
		}
	}
	else
	{
		if (m_MovePoint != constructMovePoint[m_BattleType])
		{
			m_MovePoint = constructMovePoint[m_BattleType];
			m_data->changeCorps.insert(m_myID);
		}
	}
}

/*
ChangeTerrain
建筑兵改造所在单元地形
参数：target 目标地形
返回是否改造成功
*/

bool Crops::JudgeChangeTerrain(Command& c)
{
	terrainType target = (terrainType)c.parameters[2];
	terrainType current = (terrainType)m_data->gameMap.map[m_position.m_y][m_position.m_x].type;
	c.parameters.clear();
	c.parameters.push_back(CChangeTerrain);
	c.parameters.push_back(m_myID);
	c.parameters.push_back(target);
	c.parameters.push_back(m_position.m_x);
	c.parameters.push_back(m_position.m_y);
	if (!m_bAlive)
		return false;
	if (m_type != Construct)
		return false;
	if (m_BuildType != Builder)
		return false;
	int curID = m_data->gameMap.map[m_position.m_y][m_position.m_x].owner;
	if (curID != m_PlayerID)
		return false;
	if (target == current)
		return false;
	if (!((current == TRPlain && target == TRForest) || (current == TRForest && target == TRPlain)))
		return false;
	if (m_MovePoint <= 0)
		return false;
	if (m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex == NOTOWER)
	{
		m_MovePoint = 0;//建造后行动力为零
		return true;
	}
	return false;
}
/*bool Crops::ChangeTerrain(terrainType target)
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
	terrainType curType = m_data->gameMap.map[m_position.m_y][m_position.m_x].type;
	if (!((curType == TRPlain && target == TRForest) || (curType == TRForest && target == TRPlain))) return false;    //by jyp 只能平原改成森林，或者森林改成平原
	if(m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex == NOTOWER)
	{
		m_data->gameMap.map[m_position.m_y][m_position.m_x].type = target;
		m_BuildPoint--;
		m_MovePoint = 0;
		if (m_BuildPoint == 0)
		{
			KillCorps();
		}
		m_data->changeCorps.insert(m_myID);  //by jyp:记录工程兵团劳动力改变
		return true;
	}
	return false;
}*/

/*
newRound
新回合开始，回复HP和MP
*/
void Crops::newRound()
{
	if(m_bAlive)
	{
		ResetMP();
		//Recover();
		/*
		if(!m_bResting)
		{
			m_PeaceNum = 0;//开始计数
			m_bResting = true;
		}
		*/
	}
}

/*
GoRest
进入驻扎休整状态
如果该位置没有塔则返回true，有塔则返回false
*/
/*
bool Crops::GoRest()
{
	//如果有塔
	if(m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex!=NOTOWER)
	{
		return false;
	}
	//如果之前没有处在驻扎状态中，则先将变量清零并进入驻扎状态
	if (!m_bResting)
	{
		m_PeaceNum = 0;//开始计数
		m_bResting = true;
	}
	return true;
}
*/



/*
ShowInfo
提供兵团信息
返回兵团信息结构体
*/
struct CorpsInfo Crops::ShowInfo()
{
	struct CorpsInfo info;
	info.exist = m_bAlive;
	info.ID = m_myID;
	//info.level = m_level + 1;//兵团的等级m_level从0开始，展示的兵团信息等级从1开始
	info.owner = m_PlayerID;
	info.pos = m_position;
	info.type = m_type;
	info.movePoint = m_MovePoint;
	info.HealthPoint = info.BuildPoint = 0;
	info.m_BattleType = Warrior;
	info.m_BuildType = Builder;
	if (info.type == Battle) {
		info.m_BattleType = m_BattleType;
		info.HealthPoint = m_HealthPoint; //玩家用生命值自己算战斗力，不提供
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
void Crops::AttackTower(class Tower *enemy)
{
	int TowerCE;//塔的战斗力
	//获取塔的战斗力
	TowerCE = enemy->get_towerbp();
	int myCE = getCE();//兵团战斗力
	
	double deta = 0.04*((double)myCE-TowerCE);
	int mylost = floor(28*pow(2.71828,-deta));
	int enemylost = floor(30*pow(2.71828,deta))*corpsAttackTowerGain[m_BattleType][m_level];

	//计算生命力
	if (m_BattleType != Archer)
	{
		m_HealthPoint -= mylost;
	}
	//如果已阵亡
	if (m_HealthPoint <= 0)
	{
		KillCorps();
		int num = m_data->players[enemy->getPlayerID() - 1].getElCorpsNum();
		m_data->players[enemy->getPlayerID() - 1].setElCorpsNum(num + 1);
	}

	bool IsTowerDestroy = false;
	//判断塔是否被攻陷(占领、摧毁都算)
	IsTowerDestroy = !enemy->Be_Attacked(m_PlayerID, enemylost,m_bAlive);

	if(IsTowerDestroy/*&&!(enemy->getexsit())*/)
	{
		//int num = m_data->players[m_PlayerID - 1].getCqTowerNum() + 1;
		//m_data->players[m_PlayerID - 1].setCqTowerNum(num);
	}

	if(m_bAlive)
	{
		if(m_BattleType == Archer)
			IsTowerDestroy = false;
		if(IsTowerDestroy)
		{
			UpdatePos(enemy->getPosition());
		}
	}
}

/*
Attack
兵团发动攻击 返回是否攻击成功
参数 type<CorpsCommandEnum> 攻击敌方兵团或塔， ID 敌方ID
*/
bool Crops::Attack(int type, TCorpsID ID, Command& c)
{
	//如果不是战斗兵
	if(m_type!=Battle)
		return false;

	//行动力不足
	if(m_MovePoint == 0)
		return false;


	if(type == CAttackCorps)
	{
		Crops* enemy = NULL;
		//寻找敌人
		enemy = &m_data->myCorps[ID];
		//敌人已阵亡
		if (!enemy->m_bAlive)
			return false;
		//敌人同阵营
		if (enemy->m_PlayerID == m_PlayerID)
			return false;
		//敌人不在射程范围内
		if (!IsInRange(enemy->m_position))
			return false;

		//如果敌人驻扎到了所在位置存在敌方势力塔 优先与塔结算
		int index = m_data->gameMap.map[enemy->m_position.m_y][enemy->m_position.m_x].TowerIndex;
		if (index != NOTOWER && m_data->myTowers[index].getPlayerID() == enemy->m_PlayerID) {
			c.parameters[0] = CAttackTower;
			c.parameters[2] = index;
			AttackTower(&(m_data->myTowers[index]));
		}
		else
			AttackCrops(enemy);
	}
	else if(type == CAttackTower)
	{
		class Tower* enemy = NULL;
		//寻找敌人
		enemy = &m_data->myTowers[ID];
		//敌人已阵亡
		if (!enemy->getexsit())
			return false;
		//敌人同阵营
		if (enemy->getPlayerID() == m_PlayerID)
			return false;
		//敌人不在射程范围内
		if (!IsInRange(enemy->getPosition()))
			return false;

		AttackTower(enemy);
	}
	else//指令有误
		return false;

	m_MovePoint = 0;
	m_data->changeCorps.insert(m_myID);//by jyp:记录兵团行动力等的改变
	return true;
}

/*
UpdatePos 更新位置
更新在m_data中corps记录的位置以及类内部的位置
参数 新的位置
*/
void Crops::UpdatePos(TPoint targetpos)
{
	vector<TCorpsID>::iterator it;
	for(it = m_data->gameMap.map[m_position.m_y][m_position.m_x].corps.begin(); 
		it!= m_data->gameMap.map[m_position.m_y][m_position.m_x].corps.end(); it++)
	{
		if(*it == m_myID)
		{
			m_data->gameMap.map[m_position.m_y][m_position.m_x].corps.erase(it);
			break;
		}
	}
	//在data中更新
	m_position = targetpos;
	m_data->gameMap.map[m_position.m_y][m_position.m_x].corps.push_back(m_myID);
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


	vector<TCorpsID>::iterator it;
	for (it = m_data->gameMap.map[m_position.m_y][m_position.m_x].corps.begin();
		it != m_data->gameMap.map[m_position.m_y][m_position.m_x].corps.end(); it++)
	{
		if (*it == m_myID)
		{
			m_data->gameMap.map[m_position.m_y][m_position.m_x].corps.erase(it);
			break;
		}
	}
	m_bAlive = false;
	m_data->dieCorps.insert(m_myID);    //记录死亡兵团ID
}


bool Crops::MendTower()
{
	if(!m_bAlive)
		return false;
	if(m_type != Construct)
		return false;
	if(m_BuildType != Builder)
		return false;
	if (m_MovePoint <= 0)
		return false;
	//获取该位置塔的信息
	int index = m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex;
	//如果没有塔
	if(index == NOTOWER)
		return false;
	//如果不是己方塔
	if(m_data->myTowers[index].getPlayerID()!=m_PlayerID)
		return false;
	//塔已摧毁
	if(!m_data->myTowers[index].getexsit())
		return false;

	m_BuildPoint--;
	m_MovePoint = 0;
	if (m_BuildPoint <= 0)
	{
		KillCorps();
	}

	m_data->myTowers[index].Recover();
	m_data->changeCorps.insert(m_myID);  //by jyp:记录工程兵团劳动力改变
	return true;
}

//开拓者建塔
bool Crops::BuildTower()
{
	if (!m_bAlive)
		return false;
	if (m_type != Construct)
		return false;
	if (m_BuildType != Extender)
		return false;
	if (m_MovePoint <= 0)
		return false;

	//获取该位置塔的信息
	int index = m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex;
	//如果该位置已存在塔
	if (index != NOTOWER)
		return false;

	//获取该位置所有者属性
	TPlayerID OwnerID;
	OwnerID = m_data->gameMap.showOwner(m_position);
	//
	//如果该单元格属于己方
	if (OwnerID != OUTOFRANGE && OwnerID == m_PlayerID)
	{
		//新建一个防御塔
		Tower newTower(m_data, m_PlayerID, m_position);
		m_data->myTowers.push_back(newTower);
		m_data->changeTowers.insert(m_data->myTowers.size() - 1);  //by jyp:记录新塔各项属性变化，因为工程兵直接没了，所以不记录变化了
		KillCorps();
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

/*
void Crops::haveCmd()
{
	m_bResting = false;
	m_PeaceNum = 0;
}
*/

void Crops::doChangingTerrain(terrainType target, int x, int y)
{
	if (m_bAlive == false) return;
	m_data->gameMap.map[y][x].type = target;
	m_BuildPoint--;
	if (m_BuildPoint == 0)
	{
		KillCorps();
	}
	m_data->changeCorps.insert(m_myID);
}

bool Crops::isStation()
{
	int index = m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex;
	if (index != NOTOWER&&m_data->myTowers[index].getPlayerID() == m_PlayerID)
		return true;
	return false;
}