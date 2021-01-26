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
	productType			m_producttype;//������������  0-4������	5������
	THealthPoint        m_healthpoint;//����ֵ
	TBattlePoint        m_battlepoint;//ս����
	TExperPoint			m_experpoint;//����ֵ
	TExperPoint			m_upgradexper;//��������һ�����辭��ֵ
	TRound				m_round;//��Ϸ�غ���
	int					m_level;//�ȼ�:��1��ʼ                 *���䶨��
	int					m_attackrange;//������Χ               *���䶨��
	int					m_productconsume;//��ǰ���������Ӧ������������ֵ	*���䶨��

	TPlayerID			m_PlayerID;//������Ӫ

	TPoint				m_position;//λ��

	DATA::Data* const		m_data;//��Ϸ��Ϣ
	//���ݵȼ�������������
	void set_all(int level) 
	{
		m_productpoint = TowerInitConfig[level - 1].initBuildPoint;
		m_healthpoint = TowerInitConfig[level - 1].initHealthPoint;
		m_battlepoint = TowerInitConfig[level - 1].initProductPoint;
		m_upgradexper = TowerInitConfig[level - 1].upgradeExper;
		m_attackrange = TowerInitConfig[level - 1].battleRegion;
	}
	//ÿ�غϳ�ʼ���ݾ���ֵ�ж��Ƿ�����
	bool set_level();
	void product_crops(productType protype);
public:
	Tower(void);
	~Tower(void);
	Tower(DATA::Data* _data, TPlayerID m_playid, TPoint pos);
	//ÿһ�غϿ�ʼʱ���������������������������������µ���������
	bool set_producttype(productType m_protype);
	//��ʾ������ID
	const TPlayerID showPlayerID(){ return m_PlayerID; }
	//��ʾλ��
	const TPoint showPosition(){ return m_position; }
    //�������ŵĽ���
	/*
	�����ղ�����
	��������פ�����������ս�������棩
	�з�����ս���������֣�������������ֵ��ʧ��
	*/

	//��ȡ��ǰ�������ĵȼ�
	int getLevel() { return m_level; }
	//��ȡ��ǰ��������λ��
	TPoint getPosition() { return m_position; }
	//��ȡ��ǰ������������Ӫ��������
	TPlayerID getOwnerID() { return m_PlayerID; }
};
#endif