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
	bool				m_exsit;//���Ƿ�����ȼ��Ƿ����1
	TProductPoint       m_productpoint;//������
	productType			m_producttype;//������������  0-4������	5������
	THealthPoint        m_healthpoint;//����ֵ
	TBattlePoint        m_battlepoint;//ս����
	TExperPoint			m_experpoint;//����ֵ
	TExperPoint			m_upgradexper;//��������һ�����辭��ֵ
	int					m_level;//�ȼ�:��1��ʼ                 *���䶨��
	int					m_attackrange;//������Χ               *���䶨��
	int					m_productconsume;//��ǰ���������Ӧ������������ֵ	*���䶨��
	vector <Crops*>    m_staycrops;//����פ������ָ��
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
	//�ж���һ�����������Ƿ����
	bool protask_finish()
	{
		if (m_productconsume <= 0)
			return true;
		return false;
	}
	//ÿһ�غϿ�ʼʱ���������������������������������µ���������
	void set_producttype(productType m_protype);
	//��ʾ������ID
	const TPlayerID showPlayerID() { return m_PlayerID; }
	//��ʾλ��
	const TPoint showPosition() { return m_position; }
	//��ȡ��ǰ�������ĵȼ�
	int getLevel() { return m_level; }
	//��ȡ��ǰ��������λ��
	TPoint getPosition() { return m_position; }
	//��ȡ��ǰ������������Ӫ��������
	TPlayerID getOwnerID() { return m_PlayerID; }
	//��ȡ����ǰ�Ƿ���ȼ�С��1�����ݻ٣�
	bool getexsit() { return m_exsit; }
	//�ṩ��������Ϣ>>>>>>>>>>���ǡ�JYP���ӵ�
	//>>>>>>>>>>m_producttype���û�������������ú궨��NOTASK=-1
	//>>>>>>>>>>��Ҫ����һ��info.exist�Ķ��壬Ҳ����������Ƿ���<bool>���Ƿ񱻴ݻ�
	TowerInfo ShowInfo() {
		TowerInfo info;
		info.ID = m_id;
		info.ownerID = m_PlayerID;
		info.position = m_position;
		info.productPoint = m_productpoint;
		info.healthPoint = m_healthpoint;
		info.battlePoint = m_battlepoint;
		info.experPoint = m_experpoint;
		info.level = m_level;
		info.pdtType = m_producttype;
		info.productConsume = m_productconsume;
		return info;
	}
	//��ȡ��ǰ��������������
	TProductPoint getProductPoint() { return m_productpoint; }
	//��ȡ��ǰ������������ֵ
	THealthPoint getHealthPoint() { return m_healthpoint; }
	//��ȡ��ǰ��������ս����
	TBattlePoint getBattlePoint() { return m_battlepoint; }
	//��ȡ��ǰ�������ľ���ֵ
	TExperPoint getExperPoint() { return m_experpoint; }
	//��ȡ��ǰ�������Ĺ�����Χ
	int getExperPoint() { return m_attackrange; }
	//�⵽���Ž���ʱ�����㿼�����������ս����
	TBattlePoint get_towerbp();
	//�������ŵĽ������������Ƿ񱻹���
	bool Be_Attacked(TPlayerID enemy_id, THealthPoint hp_decrease);
	//����פ����Ϣ¼�� 
	void input_staycrops(Crops* newcrop) { m_staycrops.push_back(newcrop); }
	//�������� �������ظ�����ֵ
	void Recover();
};
#endif