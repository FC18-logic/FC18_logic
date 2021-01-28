#include "tower.h"

TTowerID Tower::ID = 0;

Tower::Tower(void) :m_data(nullptr)
{
}
Tower::~Tower(void)
{
}
//构造函数：塔属性初始化
Tower::Tower(DATA::Data* _data, TPlayerID m_playid, TPoint pos) :m_data(_data)
{
	m_PlayerID = m_playid;
	m_position = pos;
	m_id = ID;
	ID++;//这里再更新tower类的ID
	m_exsit = true;
	m_level = 1;//初始等级为1  
	set_all(m_level);
	m_experpoint = 0;//初始经验值为0
	m_productconsume = INF;
	//更新data
	m_data->totalTowers++;
	//by jyp : m_data->players->getTower().insert(m_PlayerID);
	m_data->players->getTower().insert(m_id); //塔下标从0开始
	//by jyp : m_data->gameMap.map[m_position.m_x][m_position.m_y].TowerIndex = m_id;
	m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex = m_id;
	//by jyp : 记录新建的塔ID
	m_data->newTower.insert(m_id);
	//等待调用更新occupypoint的函数
	//等待调用更新owner的函数
}
/*
名称：level_upgrade
功能：根据当前经验值更新等级
返回值：是否升级
by lxj
*/
bool Tower::set_level() 
{
	bool upgrade;
	upgrade = false;
	//回合开始累加上一回合所得经验值
	int bonus = 0;
	int m_round = m_data->getRound();
	if (m_round >= 0 && m_round < 100)
		bonus = 5;
	if (m_round >= 100 && m_round < 200)
		bonus = 10;
	if (m_round >= 200 && m_round <= 300)
		bonus = 15;
	m_experpoint += bonus;
	if (m_level >= MAX_TOWER_LEVEL)//满级
		return false;
	//未满级
	if (m_experpoint >= m_upgradexper)//经验值满足升级条件
	{
		m_level += 1;
		m_experpoint -= m_upgradexper;//更新当前经验值
		set_all(m_level);//包含更新当前等级升级所需经验
		upgrade = true;		
	}
	return upgrade;
}
/*
名称：product_crops
功能：生产兵团
参数：待生产兵团种类
by lxj
*/
void Tower::product_crops(productType protype)
{
	if (protype == PWarrior) 
	{
		Crops temp(m_data, Battle, Warrior, Builder, m_PlayerID, m_position);
		m_data->myCorps.push_back(temp);
	}
	if (protype == PArcher) 
	{
		Crops temp(m_data, Battle, Archer, Builder, m_PlayerID, m_position);
		m_data->myCorps.push_back(temp);
	}
	if (protype == PCavalry) 
	{
		Crops temp(m_data, Battle, Cavalry, Builder, m_PlayerID, m_position);
		m_data->myCorps.push_back(temp);
	}
	if (protype == PBuilder) 
	{
		Crops temp(m_data, Construct, Warrior, Builder, m_PlayerID, m_position);
		m_data->myCorps.push_back(temp);
	}
	if (protype == PExtender) 
	{
		Crops temp(m_data, Construct, Warrior, Extender, m_PlayerID, m_position);
		m_data->myCorps.push_back(temp);
	}
		
}

/*
名称：set_producttype
功能：结算上一回合生产任务完成情况/设置新的生产任务
参数：待设置生产任务 
返回值：上一回合生产任务是否完成
by lxj
*/
void Tower::set_producttype(productType m_protype)
{
	set_level();
	if (m_productconsume <= 0) //上一回合生产任务完成
	{
		if (m_producttype < 5) //当前任务为生产兵团    
			product_crops(m_producttype);
		if (m_producttype == PUpgrade)//当前任务为升级项目
		{
			m_level += 1;//完成后塔上升一级
			if (m_level > MAX_TOWER_LEVEL) //不得超过最大等级
			{
				m_level = MAX_TOWER_LEVEL;
			}
			set_all(m_level);
		}
	}
	else //未完成
	{
		if (m_producttype == m_protype) //继续上一回合的完成任务
		{
			m_productconsume -= m_productpoint;
			return;
		}
	}
	m_producttype = m_protype;
	//更新新一回合生产任务的生产力消耗值
	if (m_producttype < 5)
	{
		m_productconsume = TowerProductCost[m_producttype];
		m_productconsume -= m_productpoint;
	}
	if (m_producttype == PUpgrade)
	{
		m_productconsume = 40 * m_level;
		m_productconsume -= m_productpoint;
	}
}
/*
名称：get_towerbp
功能：遭到兵团进攻时，计算考虑增益后塔的战斗力
返回值：塔的战斗力(考虑增益)
by lxj
*/
TBattlePoint Tower::get_towerbp()
{
	int bonus = 0;
	for (int i = 0; i < m_staycrops.size(); i++)
		bonus += corpsBattleGain[m_staycrops[i]->getbattleType()][0] * (m_staycrops[i]->getLevel() + 1);
	return (m_battlepoint + bonus);
}
/*
名称：Be_Attacked
功能：兵团攻击塔时更新塔的生命值
参数：生命值损失
返回值：塔是否被攻陷（即生命值是否小于0，但塔不一定能被攻占，有可能已经被摧毁）
by lxj
*/
bool Tower::Be_Attacked(TPlayerID enemy_id,THealthPoint hp_decrease)
{
	m_healthpoint -= hp_decrease;
	if (m_healthpoint <= 0)
	{
		m_level -= 4;//塔的等级下降4级
		//塔被摧毁
		if (m_level < 1)
		{
			m_exsit = false;
			//更新data
			m_data->totalTowers--;
			m_data->players->getTower().erase(m_PlayerID);
			m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex = NOTOWER;
			//by jyp : 记录被摧毁的塔的ID
			m_data->dieTower.insert(m_id);
			//等待调用更新occupypoint的函数

		}
		//塔被攻占
		else
		{
			set_all(m_level);
			m_PlayerID = enemy_id;
		}

		//俘虏驻扎工程兵并修改data
		for(int i = 0; i<m_staycrops.size(); i++)
		{
			if(m_staycrops[i]->getType() == Construct)
			{
				m_staycrops[i]->ChangeOwner(enemy_id);
				int num = m_data->players[enemy_id].getElCorpsNum() + 1;
				m_data->players[ID].setElCorpsNum(num);
			}
			else
			{
				m_staycrops[i]->KillCorps();
				int num = m_data->players[enemy_id].getCqCorpsNum() + 1;
				m_data->players[enemy_id].setCqCorpsNum(num);
			}
		}
		return true;
	}
	return false;
}


void Tower::Recover()
{
	struct TowerConfig levelInfo = TowerInitConfig[m_level-1];
	m_healthpoint += floor(levelInfo.initHealthPoint/3);
}

