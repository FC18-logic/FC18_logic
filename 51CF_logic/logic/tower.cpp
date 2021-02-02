#include "tower.h"

TTowerID Tower::ID = 0;

Tower::Tower(void) :m_data(nullptr)
{
}
Tower::~Tower(void)
{
}



//�����ƹ��캯��
/*Tower::Tower(const Tower& tower):m_data(tower.m_data) {
	m_PlayerID = tower.m_PlayerID;
	m_position = tower.m_position;
	m_id = tower.m_id;
	m_exsit = tower.m_exsit;
	m_level = tower.m_level;//��ʼ�ȼ�Ϊ1  
	set_all(m_level);
	m_experpoint = tower.m_experpoint;//��ʼ����ֵΪ0
	m_productconsume = tower.m_productconsume;
	for (int i = 0; i < 6; i++)
		task_cache[i] = 0;
	//����data
	m_data->totalTowers++;
	m_data->players[m_PlayerID - 1].getTower().insert(m_id);
	m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex = m_id;
	//by jyp:��������֮���޸ķ������Ϊ��
	m_data->gameMap.map[m_position.m_y][m_position.m_x].type = TRTower;
	//by jyp : ��¼�½�����ID
	m_data->newTower.insert(m_id);
	//����occupypoint/owner
	m_data->gameMap.modifyOccupyPoint(NOTOWER, m_PlayerID, m_position);
}*/


//���캯���������Գ�ʼ��
Tower::Tower(DATA::Data* _data, TPlayerID m_playid, TPoint pos) :m_data(_data)
{
	m_PlayerID = m_playid;
	m_position = pos;
	m_id = ID;
	ID++;//�����ٸ���tower���ID
	m_exsit = true;
	m_level = 1;//��ʼ�ȼ�Ϊ1  
	set_all(m_level);
	m_experpoint = 0;//��ʼ����ֵΪ0
	m_producttype = NOTASK;
	m_productconsume = 0;
	for (int i = 0; i < 6; i++)
		task_cache[i] = 0;
	//����data
	m_data->totalTowers++;
	m_data->players[m_PlayerID - 1].getTower().insert(m_id);
	m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex = m_id;
	//by jyp:��������֮���޸ķ������Ϊ��
	m_data->gameMap.map[m_position.m_y][m_position.m_x].type = TRTower;
	//by jyp : ��¼�½�����ID
	m_data->newTower.insert(m_id);
	//����occupypoint/owner
	m_data->gameMap.modifyOccupyPoint(NOTOWER, m_PlayerID, m_position);
}



/*
���ƣ�set_all
���ܣ����ݵȼ�����������������ֵ��ս�������������辭��ֵ��������Χ
��������ǰ�ȼ�
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
���ƣ�upgrade
���ܣ������������������ʱִ�еĲ���
by lxj
*/
void Tower::upgrade()
{
	m_level++;
	if (m_level > MAX_TOWER_LEVEL) //���ó������ȼ�
		m_level = MAX_TOWER_LEVEL;
	set_all(m_level);
}




/*
���ƣ�level_upgrade
���ܣ����ݵ�ǰ����ֵ���µȼ�
����ֵ���Ƿ�����
by lxj
*/
bool Tower::set_level() 
{
	bool upgrade;
	upgrade = false;
	//�غϿ�ʼ�ۼ���һ�غ����þ���ֵ
	int bonus = 0;
	int m_round = m_data->getRound() - 1;
	if (m_round >= 0 && m_round < 100)
		bonus = 5;
	if (m_round >= 100 && m_round < 200)
		bonus = 10;
	if (m_round >= 200 && m_round <= 300)
		bonus = 15;
	m_experpoint += bonus;
	if (m_level >= MAX_TOWER_LEVEL)//����
		return false;
	//δ����
	if (m_experpoint >= m_upgradexper)//����ֵ������������
	{
		m_level += 1;
		m_experpoint -= m_upgradexper;//���µ�ǰ����ֵ
		set_all(m_level);//�������µ�ǰ�ȼ��������辭��
		upgrade = true;		
	}
	return upgrade;
}



/*
���ƣ�product_crops
���ܣ���������
��������������������
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
		//by jyp�����������������ڷ���ȼ���С1
		if (m_level > 1) m_level--;
		set_all(m_level);
	}
		
}



/*
���ƣ�set_producttype
���ܣ������µ���������
�������������������� 
����ֵ���Ƿ����óɹ�
by lxj
*/
bool Tower::set_producttype(productType m_protype)
{
	//������������Խ�磺������ͣһ�غ�
	if (int(m_protype) < 0 || int(m_protype) > 5)
		return false;
	//����һ�غ�������������ɹ���������һ�غ�����Ľ���
	if (m_producttype != NOTASK)
		task_cache[int(m_producttype)] = m_productconsume;
	m_producttype = m_protype;
	//��������������Ϊ���״̬
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
���ƣ�get_towerbp
���ܣ��⵽���Ž���ʱ�����㿼�����������ս����
����ֵ������ս����(��������)
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
���ƣ�Be_Attacked
���ܣ����Ź�����ʱ������������ֵ
����������ֵ��ʧ
����ֵ�����Ƿ񱻹��ݣ�������ֵ�Ƿ�С��0��������һ���ܱ���ռ���п����Ѿ����ݻ٣�
by lxj
*/
bool Tower::Be_Attacked(TPlayerID enemy_id,THealthPoint hp_decrease)
{
	m_healthpoint -= hp_decrease;
	if (m_healthpoint <= 0)
	{
		m_level -= 4;//���ĵȼ��½�4��
		//�����ݻ�
		if (m_level < 1)
		{
			m_exsit = false;
			//����data
			m_data->totalTowers--;
			m_data->players[m_PlayerID - 1].getTower().erase(m_id);
			m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex = NOTOWER;
			m_data->gameMap.map[m_position.m_y][m_position.m_x].type = TRPlain;  //by jyp: �����ݻ�֮��ͳһ�޸ķ������Ϊƽԭ
			//by jyp : ��¼���ݻٵ�����ID
			m_data->dieTower.insert(m_id);
			//����occupypoint/owner
			m_data->gameMap.modifyOccupyPoint(m_PlayerID, NOTOWER, m_position);//�����ݻ���ɵ�ռ������ֵ�ı�
		}
		//������ռ
		else
		{
			set_all(m_level);
			m_data->players[m_PlayerID - 1].getTower().erase(m_id);//�޸�ԭӵ���ߵ����б�
			m_data->players[enemy_id - 1].getTower().insert(m_id);//�޸���ӵ���ߵ����б�
			m_data->gameMap.modifyOccupyPoint(m_PlayerID, enemy_id, m_position);//�޸ķ�����������ɵ�ռ������ֵ���·���
			m_PlayerID = enemy_id;
		}
		//��²פ�����̱����޸�data
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
		//��פ���������Ƴ�ս������
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
���ƣ�set_attacktarget
���ܣ����ù���Ŀ��
������Ŀ�����id
����ֵ���Ƿ񹥻��ɹ�
by lxj
*/
bool Tower::set_attacktarget(int crop_id) 
{
	//����ʧ��
	if (crop_id < 0 || crop_id >= m_data->myCorps.size())//idԽ��
		return false;
	Crops enemy = m_data->myCorps[crop_id];
	if (enemy.bAlive() == false)//��������
		return false;
	if (getDist(enemy.getPos(), m_position) > m_attackrange)//����������Χ
		return false;
	//�����ɹ�
	float deta = 0.04 * ((float)m_battlepoint - enemy.getCE());
	int crop_lost = floor(28 * pow(2.71828, deta));
	enemy.BeAttacked(crop_lost, m_PlayerID, true);//@@@ �����ܵ������ĺ�����Ҫһ���������Ƿ���Ĳ��� lmx
	return true;
}



/*
���ƣ�Recover
���ܣ��������ظ�����ֵ
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

