#include "tower.h"

TTowerID Tower::ID = 0;

Tower::Tower(void) :m_data(nullptr)
{
}
Tower::~Tower(void)
{
}
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
	m_productconsume = INF;
	//����data
	m_data->totalTowers++;
	//by jyp : m_data->players->getTower().insert(m_PlayerID);
	m_data->players->getTower().insert(m_id); //���±��0��ʼ
	//by jyp : m_data->gameMap.map[m_position.m_x][m_position.m_y].TowerIndex = m_id;
	m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex = m_id;
	//by jyp : ��¼�½�����ID
	m_data->newTower.insert(m_id);
	//�ȴ����ø���occupypoint�ĺ���
	//�ȴ����ø���owner�ĺ���
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
	int m_round = m_data->getRound();
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
	}
		
}

/*
���ƣ�set_producttype
���ܣ�������һ�غ���������������/�����µ���������
�������������������� 
����ֵ����һ�غ����������Ƿ����
by lxj
*/
void Tower::set_producttype(productType m_protype)
{
	set_level();
	if (m_productconsume <= 0) //��һ�غ������������
	{
		if (m_producttype < 5) //��ǰ����Ϊ��������    
			product_crops(m_producttype);
		if (m_producttype == PUpgrade)//��ǰ����Ϊ������Ŀ
		{
			m_level += 1;//��ɺ�������һ��
			if (m_level > MAX_TOWER_LEVEL) //���ó������ȼ�
			{
				m_level = MAX_TOWER_LEVEL;
			}
			set_all(m_level);
		}
	}
	else //δ���
	{
		if (m_producttype == m_protype) //������һ�غϵ��������
		{
			m_productconsume -= m_productpoint;
			return;
		}
	}
	m_producttype = m_protype;
	//������һ�غ��������������������ֵ
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
			m_data->players->getTower().erase(m_PlayerID);
			m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex = NOTOWER;
			//by jyp : ��¼���ݻٵ�����ID
			m_data->dieTower.insert(m_id);
			//�ȴ����ø���occupypoint�ĺ���

		}
		//������ռ
		else
		{
			set_all(m_level);
			m_PlayerID = enemy_id;
		}

		//��²פ�����̱����޸�data
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

