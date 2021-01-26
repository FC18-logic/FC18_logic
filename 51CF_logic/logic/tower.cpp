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
	ID++;
	m_level = 1;//初始等级为1  
	set_all(m_level);
	m_experpoint = 0;//初始经验值为0
	m_productconsume = INF;
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
		Crops temp(m_data, Battle, Warrior, Builder, m_PlayerID, m_position);
	if (protype == PArcher)
		Crops temp(m_data, Battle, Archer, Builder, m_PlayerID, m_position);
	if (protype == PCavalry)
		Crops temp(m_data, Battle, Cavalry, Builder, m_PlayerID, m_position);
	if (protype == PBuilder)
		Crops temp(m_data, Construct, Warrior, Builder, m_PlayerID, m_position);
	if (protype == PExtender)
		Crops temp(m_data, Construct, Warrior, Extender, m_PlayerID, m_position);
}
/*
名称：set_producttype
功能：结算上一回合生产任务完成情况/设置新的生产任务
参数：待设置生产任务 
返回值：上一回合生产任务是否完成
by lxj
*/
bool Tower::set_producttype(productType m_protype)
{
	set_level();
	bool finish = false;
	if (m_productconsume <= 0) //上一回合生产任务完成
	{
		finish = true;
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
			return finish;
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
	return finish; 
}



