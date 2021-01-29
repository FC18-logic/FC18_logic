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
	//��Ϸ��Ϣ
	DATA::Data* const		m_data;

	//����Ϣ
	static TTowerID		ID;
	TTowerID			m_id;//��ID
	TPlayerID			m_PlayerID;//�������ID
	bool				m_exsit;//���Ƿ�����ȼ��Ƿ����1
	TPoint				m_position;//λ��

	//������
	TProductPoint       m_productpoint;//������
	productType			m_producttype;//������������
	THealthPoint        m_healthpoint;//����ֵ
	TBattlePoint        m_battlepoint;//ս����
	TExperPoint			m_experpoint;//����ֵ
	int					m_level;//�ȼ�:��1��ʼ               
	int					m_attackrange;//������Χ     

	//��������
	TExperPoint			m_upgradexper;//��������һ�����辭��ֵ
	int					m_productconsume;//��ǰ���������Ӧ������������ֵ	
	vector <Crops*>    m_staycrops;//����פ������ָ��
	
	/*��������*/
	//���ݵȼ�������������
	void set_all(int level);
	//ÿ�غϳ�ʼ���ݾ���ֵ�ж��Ƿ�����
	bool set_level();
	//��������
	void product_crops(productType protype);
public:
	Tower(void);
	~Tower(void);
	//�����캯��
	Tower(DATA::Data* _data, TPlayerID m_playid, TPoint pos);

	/*������*/
	//�ж���һ�����������Ƿ����
	bool protask_finish()
	{
		if (m_productconsume <= 0)
			return true;
		return false;
	}
	//��������������
	void set_producttype(productType m_protype);
	//���ù���Ŀ��
	bool set_attacktarget(int crop_id);


	/*��Ϣ��ȡ*/
	//��ȡ�������ID
	const TPlayerID showPlayerID() { return m_PlayerID; }
	//��ȡ��λ��
	const TPoint showPosition() { return m_position; }
	//��ȡ���ȼ�
	int getLevel() { return m_level; }
	//��ȡ����ǰ�Ƿ���ȼ�С��1�����ݻ٣�
	bool getexsit() { return m_exsit; }
	//��ȡפ������
	vector<Crops*> getCrops() { return m_staycrops; }
	//��ȡ��������
	TProductPoint getProductPoint() { return m_productpoint; }
	//��ȡ������ֵ
	THealthPoint getHealthPoint() { return m_healthpoint; }
	//��ȡ��ս����
	TBattlePoint getBattlePoint() { return m_battlepoint; }
	//��ȡ������ֵ
	TExperPoint getExperPoint() { return m_experpoint; }
	//��ȡ��������Χ
	int getExperPoint() { return m_attackrange; }


	/*�ṩ��������Ϣ*/
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
		if(protask_finish() == true)//û����������:NOTASK=-1
		{
			info.pdtType = NOTASK;
			info.productConsume = -1;
		}
		else
		{
			info.pdtType = m_producttype;
			info.productConsume = m_productconsume;
		}
		return info;
	}


	/*����ӿ�*/
	//�⵽���Ž���ʱ�����㿼�����������ս����
	TBattlePoint get_towerbp();
	//�������ŵĽ������������Ƿ񱻹���
	bool Be_Attacked(TPlayerID enemy_id, THealthPoint hp_decrease);
	//����פ����Ϣ¼�� 
	void input_staycrops(Crops* newcrop) { m_staycrops.push_back(newcrop); }
	//�������ظ�����ֵ
	void Recover();
};
#endif