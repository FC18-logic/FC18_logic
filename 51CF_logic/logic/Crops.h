#pragma once

#ifndef CROPS_H
#define CROPS_H

#include "definition.h"
#include "data.h"
#include "cell.h"
#include "player.h"

class Crops
{
private:
	//������������
	static TCorpsID		ID;
	TCorpsID			m_myID;//����ID
	corpsType			m_type;//��������
	battleCorpsType		m_BattleType;//ս������
	constructCorpsType	m_BuildType;//�������
	TPlayerID				m_PlayerID;//������Ӫ

	TMovePoint				m_MovePoint;//�ж���
	THealthPoint			m_HealthPoint;//����ֵ ��0������
	int						m_level;//�ȼ� ��0��ʼ
	TBuildPoint				m_BuildPoint;//�Ͷ���
	bool					m_bStation;//����״̬
	TPoint					m_position;//λ��
	int						m_PeaceNum;//פ������ٸ��غ�û���ܵ�����
	//
	DATA::Data* const		m_data;//��Ϸ��Ϣ
public:
	Crops(void);
	~Crops(void);
	Crops(DATA::Data* _data, corpsType type, battleCorpsType battletype, constructCorpsType buildtype, TPlayerID ID, TPoint pos);
	//��ս�����ƶ� �����Ƿ�ɹ��ƶ�
	bool Move(int dx, int dy);
	//��ս���Ź��� �����Ƿ�ɹ����� ����ȥ����ֵ ����������ڱ���������ɾ��
	bool AttackCrops(Crops* enemy);
	//�Ƿ���
	bool bAlive();
	//��ȡս����
	TBattlePoint getCE();
	//������� �����Ƿ�����ɹ�
	bool MergeCrops(Crops* targetCrops);
	//���Ž���פ��״̬
	void GoStation();
	//���������
	bool DoConstruct();
	//��������ĵ���
	bool ChangeTerrain(terrainType target);
	//�»غϿ�ʼ
	void newRound();
	//�ṩ������Ϣ
	struct CorpsInfo ShowInfo();
	//��ȡ��ǰ���ŵ�id
	TCorpsID getID() { return m_myID; }
	//��ȡ��ǰ���ŵ�����
	corpsType getType() { return m_type; }
	//��ȡ��ǰ���ŵ��Ǽ�����0��ʼ��
	int getLevel() { return m_level; }
protected:
	//��ս�����ܵ����� �����Ƿ��� ������������Զ��ڱ���������ɾ��
	bool BeAttacked(int attack, Crops* enemy);
	//�����ж���
	void ResetMP();
	//�ظ�HP
	void Recover();
};

#endif