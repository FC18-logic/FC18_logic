#include "tower.h"

TTowerID Tower::ID = 0;

Tower::Tower(void) :m_data(nullptr)
{
}
Tower::~Tower(void)
{
}



//塔复制构造函数
/*Tower::Tower(const Tower& tower):m_data(tower.m_data) {
	m_PlayerID = tower.m_PlayerID;
	m_position = tower.m_position;
	m_id = tower.m_id;
	m_exsit = tower.m_exsit;
	m_level = tower.m_level;//初始等级为1  
	set_all(m_level);
	m_experpoint = tower.m_experpoint;//初始经验值为0
	m_productconsume = tower.m_productconsume;
	for (int i = 0; i < 6; i++)
		task_cache[i] = 0;
	//更新data
	m_data->totalTowers++;
	m_data->players[m_PlayerID - 1].getTower().insert(m_id);
	m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex = m_id;
	//by jyp:塔被建立之后，修改方格地形为塔
	m_data->gameMap.map[m_position.m_y][m_position.m_x].type = TRTower;
	//by jyp : 记录新建的塔ID
	m_data->newTower.insert(m_id);
	//更新occupypoint/owner
	m_data->gameMap.modifyOccupyPoint(NOTOWER, m_PlayerID, m_position);
}*/


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
	m_producttype = NOTASK;
	m_productconsume = 0;
	for (int i = 0; i < 6; i++)
		task_cache[i] = 0;
	//更新data
	m_data->totalTowers++;
	m_data->players[m_PlayerID - 1].getTower().insert(m_id);
	m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex = m_id;
	//by jyp:塔被建立之后，修改方格地形为塔
	m_data->gameMap.map[m_position.m_y][m_position.m_x].type = TRTower;
	//by jyp : 记录新建的塔ID
	m_data->newTower.insert(m_id);
	//更新occupypoint/owner
	m_data->gameMap.modifyOccupyPoint(NOTOWER, m_PlayerID, m_position);
}



/*
名称：set_all
功能：根据等级更新生产力、生命值、战斗力、升级所需经验值、攻击范围
参数：当前等级
by lxj
*/
void Tower::set_all(int level)
{
	m_productpoint = TowerInitConfig[level - 1].initBuildPoint;
	m_healthpoint = TowerInitConfig[level - 1].initHealthPoint;
	m_battlepoint = TowerInitConfig[level - 1].initProductPoint;
	m_upgradexper = TowerInitConfig[level - 1].upgradeExper;
	m_attackrange = TowerInitConfig[level - 1].battleRegion;
}




/*
名称：upgrade
功能：完成塔升级生产任务时执行的操作
by lxj
*/
void Tower::upgrade()
{
	m_level++;
	if (m_level > MAX_TOWER_LEVEL) //不得超过最大等级
		m_level = MAX_TOWER_LEVEL;
	set_all(m_level);
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
	int m_round = m_data->getRound() - 1;
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
		//by jyp开拓者生产出后所在方格等级减小1
		if (m_level > 1) m_level--;
		set_all(m_level);
	}
		
}



/*
名称：set_producttype
功能：设置新的生产任务
参数：待设置生产任务 
返回值：是否设置成功
by lxj
*/
bool Tower::set_producttype(productType m_protype)
{
	//生产任务类型越界：任务暂停一回合
	if (int(m_protype) < 0 || int(m_protype) > 5)
		return false;
	//若上一回合设置生产任务成功：缓存上一回合任务的进度
	if (m_producttype != NOTASK)
		task_cache[int(m_producttype)] = m_productconsume;
	m_producttype = m_protype;
	//新设置生产任务为完成状态
	if (task_cache[(int)m_producttype] <= 0) 
	{
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
	else
	{
		m_productconsume = task_cache[int(m_producttype)];
		m_productconsume -= m_productpoint;
	}
	return true;
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
			m_data->players[m_PlayerID - 1].getTower().erase(m_id);
			m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex = NOTOWER;
			m_data->gameMap.map[m_position.m_y][m_position.m_x].type = TRPlain;  //by jyp: 塔被摧毁之后统一修改方格地形为平原
			//by jyp : 记录被摧毁的塔的ID
			m_data->dieTower.insert(m_id);
			//更新occupypoint/owner
			m_data->gameMap.modifyOccupyPoint(m_PlayerID, NOTOWER, m_position);//塔被摧毁造成的占有属性值改变
		}
		//塔被攻占
		else
		{
			set_all(m_level);
			m_data->players[m_PlayerID - 1].getTower().erase(m_id);//修改原拥有者的塔列表
			m_data->players[enemy_id - 1].getTower().insert(m_id);//修改新拥有者的塔列表
			m_data->gameMap.modifyOccupyPoint(m_PlayerID, enemy_id, m_position);//修改方格塔易主造成的占有属性值重新分配
			m_PlayerID = enemy_id;
		}
		//俘虏驻扎工程兵并修改data
		for(int i = 0; i<m_staycrops.size(); i++)
		{
			if(m_staycrops[i]->getType() == Construct)
			{
				m_staycrops[i]->ChangeOwner(enemy_id);
				int num = m_data->players[enemy_id - 1].getCqCorpsNum() + 1;
				m_data->players[enemy_id - 1].setCqCorpsNum(num);
			}
			else
			{
				m_staycrops[i]->KillCorps();
				int num = m_data->players[enemy_id - 1].getElCorpsNum() + 1;
				m_data->players[enemy_id - 1].setElCorpsNum(num);
			}
		}
		//从驻扎兵团中移除战斗兵团
		for (vector<Crops*>::iterator it = m_staycrops.begin(); it != m_staycrops.end(); )
		{
			if ((*it)->getType() == Battle)
			{
				it = m_staycrops.erase(it);
			}
			else
			{
				++it;
			}
		}
		return true;
	}
	return false;
}




/*
名称：set_attacktarget
功能：设置攻击目标
参数：目标兵团id
返回值：是否攻击成功
by lxj
*/
bool Tower::set_attacktarget(int crop_id) 
{
	//攻击失败
	if (crop_id < 0 || crop_id >= m_data->myCorps.size())//id越界
		return false;
	Crops enemy = m_data->myCorps[crop_id];
	if (enemy.bAlive() == false)//兵团死亡
		return false;
	if (getDist(enemy.getPos(), m_position) > m_attackrange)//超出攻击范围
		return false;
	//攻击成功
	float deta = 0.04 * ((float)m_battlepoint - enemy.getCE());
	int crop_lost = floor(28 * pow(2.71828, deta));
	enemy.BeAttacked(crop_lost, m_PlayerID, true);//@@@ 兵团受到攻击的函数需要一个攻击者是否存活的参数 lmx
	return true;
}



/*
名称：Recover
功能：修理塔回复生命值
by lmx
*/
void Tower::Recover()
{
	struct TowerConfig levelInfo = TowerInitConfig[m_level-1];
	m_healthpoint += floor(levelInfo.initHealthPoint/3);
	if(m_healthpoint >= levelInfo.initHealthPoint)
	{
		m_healthpoint = levelInfo.initHealthPoint;
	}
}
void Tower::remove_crop(TCorpsID crop_id) 
{
	Crops* temp = &(m_data->myCorps[crop_id]);
	for (vector<Crops*>::iterator it = m_staycrops.begin(); it != m_staycrops.end(); )
	{
		if ((*it) == temp)
		{
			it = m_staycrops.erase(it);
		}
		else
		{
			++it;
		}
	}
}

