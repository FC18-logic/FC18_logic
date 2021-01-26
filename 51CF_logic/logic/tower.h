#pragma once

#ifndef TOWER_H
#define TOWER_H

#include "definition.h"
#include "data.h"
#include "Crops.h"
#include "player.h"

class Tower
{
private:
	//������
	static TTowerID		ID;
	TTowerID			m_id;//��ID
	TProductPoint       m_productpoint;//������
	productType			m_producttype;//������������
	int					m_towercommand;//������������  0������	1������	   *���䶨��
	THealthPoint        m_healthpoint;//����ֵ
	TBattlePoint        m_battlepoint;//ս����
	TExperPoint			m_experpoint;//����ֵ
	TExperPoint			m_upgradexper;//��������һ�����辭��ֵ
	TRound				m_round;//��Ϸ�غ���
	int					m_level;//�ȼ�:��0��ʼ                 *���䶨��
	int					m_attackrange;//������Χ               *���䶨��
	int					m_productconsume;//��ǰ���������Ӧ������������ֵ	*���䶨��

	TPlayerID			m_PlayerID;//������Ӫ

	TPoint				m_position;//λ��
	//���ݵȼ�������������
	void set_all(int level) 
	{
		m_productpoint = TowerInitConfig[level].initBuildPoint;
		m_healthpoint = TowerInitConfig[level].initHealthPoint;
		m_battlepoint = TowerInitConfig[level].initProductPoint;
		m_upgradexper = TowerInitConfig[level].upgradeExper;
		m_attackrange = TowerInitConfig[level].battleRegion;
	}
	//ÿ�غϳ�ʼ���ݾ���ֵ�ж��Ƿ�����
	bool set_level();
public:
	Tower(void);
	~Tower(void);
	Tower(TPlayerID ID, TPoint pos);
	//ÿһ�غϿ�ʼʱ���������������������������������µ���������
	bool set_producttype(productType m_protype);
    //�������ŵĽ���
	/*
	�����ղ�����
	��������פ�����������ս�������棩
	�з�����ս���������֣�������������ֵ��ʧ��
	*/
};
#endif