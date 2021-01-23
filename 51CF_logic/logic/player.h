#pragma once

#include "definition.h"
#include "cell.h"
#include "tentacle.h"
#include <set>
#include "data.h"
class Player
{
public:
	Player();
	Player(Player&); //#json
	~Player();
	//���ӿƼ�����
	void setdata(DATA::Data* d) { data = d; }//��������Data
	void addTechPoint(TResourceD _techPoint);//���ӿƼ�����
	void regenerateTechPoint();//�Ƽ������ָ�
	bool upgrade(TPlayerProperty kind);//�������
	bool subTechPoint(TResourceD _techPoint);//�۳���Ӧ�ĿƼ�������ʧ�ܷ���false
	int getDefenceLevel() const { return m_DefenceLevel; }
	bool upgradeDefence(); //���������������Ƿ�ɹ�
	int getRegenerationLevel() const { return m_RegenerationLevel; }//��ȡ��������ȼ�
	bool upgradeRegeneration();//������������ȼ�
	int getExtraControlLevel() const { return m_ExtraControlLevel; }//��ȡ��Ҷ���������ȼ�
	bool upgradeExtraControl();//������Ҷ���������ȼ�
	int getMoveLevel() const { return m_MoveLevel; }//��ȡ����ƶ��ȼ�
	bool upgradeMove();//��������ƶ��ȼ�
	bool isAlive() { return alive; }//�ж�����Ƿ����
	int maxControlNumber();//��ȡ�����������
	TResourceD totalResource();//��ȡ��������Դ�����ܺ�
	TResourceD techPoint() { return m_techPoint; }//��ȡ��ҵĿƼ�����
	std::set<TCellID>& cells() { return m_cells; }//��ȡ��ҵ�������set
	void Kill();//�����������
	int getdeadRound() { return deadRound; }//��ȡ�����غ�
	void setdeadRound(int d) { deadRound = d; }//���������غ�
private:
	bool alive = true;//�ǲ��ǻ�����
	int deadRound;//�����Ļغ�
	std::set<TCellID> m_cells;//��ǰϸ��
	TResourceD m_techPoint;//�Ƽ�����
	int m_MoveLevel = 0;//�ƶ����ܵȼ�
	int m_RegenerationLevel = 0;//�������ܵȼ�
	int m_ExtraControlLevel = 0;//������������ܵȼ�
	int m_DefenceLevel = 0;//�������ܵȼ�
	DATA::Data* data;//���ݵĴ洢�빲��
};

