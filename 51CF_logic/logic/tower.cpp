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
	ID++;
	m_level = 1;//��ʼ�ȼ�Ϊ1  
	set_all(m_level);
	m_experpoint = 0;//��ʼ����ֵΪ0
	m_productconsume = INF;
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
���ƣ�set_producttype
���ܣ�������һ�غ���������������/�����µ���������
�������������������� 
����ֵ����һ�غ����������Ƿ����
by lxj
*/
bool Tower::set_producttype(productType m_protype)
{
	set_level();
	bool finish = false;
	if (m_productconsume <= 0) //��һ�غ������������
	{
		finish = true;
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
			return finish;
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
	return finish; 
}



