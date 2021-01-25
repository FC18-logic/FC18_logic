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

	TPoint		m_position;//λ��

	int			m_PeaceNum;//���ٸ��غ�û���ܵ�����
	//
public:
	Crops(void);
	~Crops(void);
	Crops(corpsType type, battleCorpsType battletype, constructCorpsType buildtype, TPlayerID ID, TPoint pos);
	//��ս�����ƶ� �д���ͼ�ӿ��жϵ���

	//��ս���Ź��� �����Ƿ�ɹ����� ����ȥ����ֵ ����������ڱ���������ɾ��
	bool AttackCrops(TCorpsID enemyID);
	//�Ƿ���
	bool bAlive();
	//��ȡս����
	TBattlePoint getCE();
	//����λ��
	TPoint getPos();
	//
	TPlayerID getPlayerID();
	//�ظ�HP ÿ�غϵ������б��Ÿú����ظ�������
	void Recover();
	//������� �����Ƿ�����ɹ�
	bool MergeCrops(TCorpsID cropsID);
	//�ñ������������ñ��Ŵӱ��ż�����ɾ��
	void Delete();
	//�»غϿ�ʼ�������ж���
	void ResetMP();
protected:
	//��ս�����ܵ����� �����Ƿ��� ������������Զ��ڱ���������ɾ��
	bool BeAttacked(int attack, Crops* enemy);
};

#endif