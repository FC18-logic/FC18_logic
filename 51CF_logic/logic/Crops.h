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
	bool					m_bResting;//�Ƿ�������
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
	bool Attack(CorpsCommandEnum type, int ID);
	//�Ƿ���
	bool bAlive();
	//��ȡս����
	TBattlePoint getCE();
	//������� �����Ƿ�����ɹ�
	bool MergeCrops(TCorpsID ID);
	//���Ž���פ��״̬
	void GoRest();
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
	//��ȡ��ǰ���ŵ�λ��
	TPoint getPos() { return m_position; }
	//��ȡ��ǰս�����ŵ�����
	battleCorpsType getbattleType() { return m_BattleType; }
	//�޸ĵ�ǰ�����������
	void set_playid(TPlayerID id) { m_PlayerID = id; }
	//�޸ĵ�ǰ���ŵ�����ֵ
	void set_hp(THealthPoint hp) { m_HealthPoint = hp; }
protected:
	//��ս�����ܵ����� �����Ƿ��� �������
	bool BeAttacked(int attack, TPlayerID ID);
	//�����ж���
	void ResetMP();
	//�ظ�HP
	void Recover();
	//��ս���Ź���
	int AttackCrops(Crops* enemy);
	//��ս���Ź�����
	int AttackTower(class Tower *enemy);
	//����λ��
	void UpdatePos(TPoint targetpos);
};

#endif