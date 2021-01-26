#include "tower.h"

TTowerID ID = 0;

Tower::Tower(void)
{
}
Tower::~Tower(void)
{
}
//���캯���������Գ�ʼ��

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
���ƣ�set_producttype
���ܣ�������һ�غ���������������/�����µ���������
��������������������    *�����䣨��Ϊ�������������贫��������ࣩ
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
		if (m_producttype == 0) //��ǰ����Ϊ��������      *������
		{
		}
		if (m_producttype == 1)//��ǰ����Ϊ������Ŀ
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
	if (m_producttype == 0)
	{
		//m_productconsume = TowerProductCost[��������];
		m_productconsume -= m_productpoint;
	}
	if (m_producttype == 1)
	{
		m_productconsume = 40 * m_level;
		m_productconsume -= m_productpoint;
	}
	return finish;
}



