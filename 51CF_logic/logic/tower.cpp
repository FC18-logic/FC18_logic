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
	m_healthpoint = M_healthpoint;
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
	//m_data->gameMap.map[m_position.m_y][m_position.m_x].type = TRTower;
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
	M_healthpoint = TowerInitConfig[level - 1].initHealthPoint;
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
	THealthPoint old_Mhp = M_healthpoint;
	set_all(m_level);
	//实际生命值与生命值上限成比例改变
	m_healthpoint = (M_healthpoint * m_healthpoint) / old_Mhp;
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
	int m_round = floor(m_data->getRound() - 1 / 4.0F);
	if (m_round >= 0 && m_round < 100)
		bonus = 5;
	if (m_round >= 100 && m_round < 200)
		bonus = 10;
	if (m_round >= 200 && m_round <= 300)
		bonus = 15;
	m_experpoint += bonus;
	//满级
	if (m_level >= MAX_TOWER_LEVEL)
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
	if (m_data->players[m_PlayerID - 1].battleNumControl() == false) {
		if (protype == PWarrior)
		{
			Crops temp(m_data, Battle, Warrior, Builder, m_PlayerID, m_position);
			m_data->myCorps.push_back(temp);
			m_data->changeCorps.insert(temp.getID());
		}
		else if (protype == PArcher)
		{
			Crops temp(m_data, Battle, Archer, Builder, m_PlayerID, m_position);
			m_data->myCorps.push_back(temp);
			m_data->changeCorps.insert(temp.getID());
		}
		else if (protype == PCavalry)
		{
			Crops temp(m_data, Battle, Cavalry, Builder, m_PlayerID, m_position);
			m_data->myCorps.push_back(temp);
			m_data->changeCorps.insert(temp.getID());
		}
	}
	if (m_data->players[m_PlayerID - 1].constructNumControl() == false) {
		if (protype == PBuilder)
		{
			Crops temp(m_data, Construct, Warrior, Builder, m_PlayerID, m_position);
			m_data->myCorps.push_back(temp);
			m_data->changeCorps.insert(temp.getID());
		}
		else if (protype == PExtender)
		{
			Crops temp(m_data, Construct, Warrior, Extender, m_PlayerID, m_position);
			m_data->myCorps.push_back(temp);
			m_data->changeCorps.insert(temp.getID());
			//by jyp：开拓者生产出后所在方格塔等级减小1
			if (m_level > 1)
			{
				m_level--;
				THealthPoint old_Mhp = M_healthpoint;
				set_all(m_level);
				//实际生命值与生命值上限成比例改变
				m_healthpoint = (M_healthpoint * m_healthpoint) / old_Mhp;
			}
		}
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
	m_data->changeTowers.insert(m_id);//by jyp:记录塔的生产任务改变
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
	vector<TCorpsID> Unit = m_data->gameMap.map[m_position.m_y][m_position.m_x].corps;
	TCorpsID index;
	int baseBattlePoint = std::ceil((float)m_battlepoint * (float)m_healthpoint / (float)TowerInitConfig[m_level - 1].initHealthPoint);
	for (int i = 0; i < Unit.size(); i++)
	{
		index = Unit[i];
		if (m_data->myCorps[index].getPlayerID() != m_PlayerID)//非己方兵团
			continue;
		bonus += corpsBattleGain[m_data->myCorps[index].getbattleType()][0] * (m_data->myCorps[index].getLevel() + 1);
	}
	return (baseBattlePoint + bonus);
}




/*
名称：Be_Attacked
功能：兵团攻击塔时更新塔的生命值
参数：生命值损失
返回值：塔的生命值是否降为0（攻占/摧毁：m_exsit = false）
by lxj
*/
bool Tower::Be_Attacked(TPlayerID enemy_id,THealthPoint hp_decrease, bool attackerAlive)
{
	m_healthpoint -= hp_decrease;
	m_data->changeTowers.insert(m_id);//by jyp:记录塔的生命值等改变
	if (m_healthpoint <= 0)
	{
		m_level -= 4;//塔的等级下降4级
		TPlayerID oldOwner = m_PlayerID;//缓存旧主信息
		//塔被摧毁：等级小于1/攻击者防御塔数目超过限制
		if (m_level < 1 || m_data->players[enemy_id - 1].towerNumControl() == true)  
		{
			m_exsit = false;
			//更新data
			m_data->totalTowers--;
			m_data->players[m_PlayerID - 1].getTower().erase(m_id);
			m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex = NOTOWER;
			//m_data->gameMap.map[m_position.m_y][m_position.m_x].type = TRPlain;//by jyp: 塔被摧毁之后统一修改方格地形为平原
			//by jyp : 记录被摧毁的塔的ID
			m_data->dieTower.insert(m_id);
			//更新occupypoint/owner
			m_data->gameMap.modifyOccupyPoint(m_PlayerID, NOTOWER, m_position);
			//塔被摧毁更新攻击者占塔数量
			int currentCqTowerNum = m_data->players[enemy_id - 1].getCqTowerNum();
			m_data->players[enemy_id - 1].setCqTowerNum(currentCqTowerNum + 1);
		}
		//如果攻方还活着（攻方是塔的话判定攻方死亡，即不能占领）：塔被攻占
		else if (attackerAlive == true)
		{
			set_all(m_level);
			//生命值回满
			m_healthpoint = M_healthpoint;
			m_data->players[m_PlayerID - 1].getTower().erase(m_id);//修改原拥有者的塔列表
			m_data->players[enemy_id - 1].getTower().insert(m_id);//修改新拥有者的塔列表
			//更新occupypoint/owner
			m_data->gameMap.modifyOccupyPoint(m_PlayerID, enemy_id, m_position);
			m_PlayerID = enemy_id;
			//塔被攻占更新攻击者占塔数量
			int currentCqTowerNum = m_data->players[enemy_id - 1].getCqTowerNum();
			m_data->players[enemy_id - 1].setCqTowerNum(currentCqTowerNum + 1);
		}
		else//攻方占领失败，塔的所有者不变，按当前等级重置各项属性（满级）
		{
			set_all(m_level);
			//生命值回满
			m_healthpoint = M_healthpoint;
		}
		//塔被摧毁或攻占后所在方格己方兵团消灭
		int newDieCorps = m_data->players[enemy_id - 1].getElCorpsNum();//记录新增死亡兵团数
		TPoint towerPos = m_position;
		for (TCorpsID c : m_data->gameMap.map[towerPos.m_y][towerPos.m_x].corps)
		{
			if (m_data->myCorps[c].getPlayerID() == oldOwner)
			{
				m_data->myCorps[c].KillCorps();
				newDieCorps++;
			}
		}
		m_data->players[enemy_id - 1].setElCorpsNum(newDieCorps);
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
	if (m_data->myCorps[crop_id].isStation() == true)//兵团驻扎到塔
		return false;   
	if (m_data->myCorps[crop_id].getPlayerID() == m_PlayerID)//攻击目标为己方兵团
		return false;
	Crops* enemy = &(m_data->myCorps[crop_id]);
	if (enemy->bAlive() == false)//兵团死亡
		return false;
	if (getDist(enemy->getPos(), m_position) > 2)//超出攻击范围
		return false;
	//如果是工程兵，判断是否存在护卫
	if (enemy->getType() == Construct)
	{
		Crops* colleage;
		TPoint pos = enemy->getPos();
		//是否存在护卫
		for (int i = 0; i < m_data->gameMap.map[pos.m_y][pos.m_x].corps.size(); i++)
		{
			int index = m_data->gameMap.map[pos.m_y][pos.m_x].corps[i];
			colleage = &(m_data->myCorps[index]);
			if (colleage->getType() == Battle && colleage->getPlayerID() == enemy->getPlayerID())
				enemy = colleage;
		}
	}

	//攻击成功
	float deta = 0.04 * ((float)get_towerbp() - (float)enemy->getCE());
	int crop_lost = floor(30 * pow(2.71828, deta));
	enemy->BeAttacked(crop_lost, m_PlayerID, m_exsit);
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
	if (m_healthpoint != levelInfo.initHealthPoint) m_data->changeTowers.insert(m_id);   //by jyp:记录塔的生命值改变
	m_healthpoint += floor(float(levelInfo.initHealthPoint)/3.0F);
	if(m_healthpoint >= levelInfo.initHealthPoint)
	{
		m_healthpoint = levelInfo.initHealthPoint;
	}
}

