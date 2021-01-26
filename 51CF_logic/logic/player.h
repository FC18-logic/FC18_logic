#pragma once

#include "definition.h"
#include "cell.h"
#include "tentacle.h"
#include "Crops.h"
#include "tower.h"
#include <set>
#include "data.h"
class Player                                                                //@@@��FC18�������
{
public:
	Player();                                                               //@@@��FC18�������Ĭ�Ϲ��캯��
	Player(Player&);                                                        //@@@��FC18������ิ�ƹ��캯��   #json
	~Player();                                                              //��FC18���������������
	//���ӿƼ�����
	void setdata(DATA::Data* d) { data = d; }                               //��FC18�����ø�������Data
	bool isAlive() { return alive; }                                        //��FC18���ж�����Ƿ����
	int maxControlNumber();                                                 //@@@��FC18������������ô�ģ���֪���ò��ã���ȡ�����������
	void Kill();                                                            //��FC18�������������
	int getdeadRound() { return deadRound; }                                //��FC18����ȡ�����غ�
	void setdeadRound(int d) { deadRound = d; }                             //��FC18�����������غ�
	//@@@��FC18����ȡ��ҵ�ǰ�÷֣����չ����������鿼�ǵ�ͬ����ʱ��ʲôָ�����Ƚϵȣ�
	int getPlayerScore();
	void setID(TPlayerID id) { m_id = id; }
	TPlayerID getId() { return m_id; }                                      //��FC18����ȡ���ID

	//��FC18����ȡ��ҵ����з�����set
	std::set<TTowerID>& getTower() { return m_tower; }

	//��FC18����ȡ��ҵ����б��ŵ�set
	std::set<TCorpsID>& getCrops() { return m_crops; }


	//FC15��
	TResourceD totalResource();//��ȡ��������Դ�����ܺ�
	TResourceD techPoint() { return m_techPoint; }//��ȡ��ҵĿƼ�����
	std::set<TCellID>& cells() { return m_cells; }//��ȡ��ҵ�����ϸ��set
	void addTechPoint(TResourceD _techPoint);//���ӿƼ�����
	void regenerateTechPoint();//�Ƽ������ָ�
	bool upgrade(TPlayerProperty kind);//�������
	bool subTechPoint(TResourceD _techPoint);//�۳���Ӧ�ĿƼ�������ʧ�ܷ���false
	int getDefenceLevel() const { return m_DefenceLevel; }//��ȡ��ҷ����ȼ�
	bool upgradeDefence(); //���������������Ƿ�ɹ�
	int getRegenerationLevel() const { return m_RegenerationLevel; }//��ȡ��������ȼ�
	bool upgradeRegeneration();//������������ȼ�
	int getExtraControlLevel() const { return m_ExtraControlLevel; }//��ȡ��Ҷ���������ȼ�
	bool upgradeExtraControl();//������Ҷ���������ȼ�
	int getMoveLevel() const { return m_MoveLevel; }//��ȡ����ƶ��ȼ�
	bool upgradeMove();//��������ƶ��ȼ�

private:
	bool alive = true;                                                      //��FC18���ǲ��ǻ�����
	int deadRound;                                                          //��FC18�������Ļغ�
	DATA::Data* data;                                                       //��FC18�����ݵĴ洢�빲��
	TPlayerID m_id;                                                         //��FC18�����ID
	//@@@��FC18��ָ��ǰ������з�������set���ݽṹ���ο�ԭ����std::set<TCellID> m_cells
	std::set<TTowerID> m_tower;
	
	//@@@��FC18��������б��ŵ���ţ�����Ҳ��set�������ݽṹ���ڲ��������������������
	std::set<TCorpsID> m_crops;
	

	//FC15��
	std::set<TCellID> m_cells;//��ǰϸ��
	TResourceD m_techPoint;//�Ƽ�����
	int m_MoveLevel = 0;//�ƶ����ܵȼ�
	int m_RegenerationLevel = 0;//�������ܵȼ�
	int m_ExtraControlLevel = 0;//������������ܵȼ�
	int m_DefenceLevel = 0;//�������ܵȼ�
};

