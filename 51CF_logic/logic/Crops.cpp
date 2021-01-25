#include "Crops.h"

TCorpsID ID = 0;

Crops::Crops(void)
{
}


Crops::~Crops(void)
{
}

Crops::Crops(corpsType type, battleCorpsType battletype, constructCorpsType buildtype, TPlayerID PlayerID, TPoint pos)
{
	m_type = type;
	m_BattleType = battletype;
	m_BuildType = buildtype;
	m_PlayerID = PlayerID;
	m_position = pos;
	m_myID = ID;
	ID++;
	m_PeaceNum = 0;
	m_level = 0;
	if(type == Battle)
	{
		m_MovePoint = battleMovePoint[battletype][m_level];
		m_HealthPoint = battleHealthPoint[battletype][m_level];
	}
	else{
		if(buildtype == Construct)
		{
			m_BuildPoint = 3;
		}
	}
}

/*
AttackCrops
�ñ��Ŷ���һ���ŷ��𹥻���������㹥�������������š����ƶ�
���� enemyID �ܹ�������ID
����ֵ bool �����ɹ�����true
lmx
�ȴ���һ���޸ģ��͹������ϲ�
*/
bool Crops::AttackCrops(TCorpsID enemyID)
{
	if(m_MovePoint == 0)
	{
		return false;
	}
	m_MovePoint = 0;
	Crops* enemy = NULL;//����ָ��
	//����IDѰ�ҵ���
	//�ȴ���Ӵ���
	//
	//���û�иõ���
	if(!enemy)
	{
		return false;
	}
	//����������
	if(!enemy->bAlive())
	{
		return false;
	}
	//�������ս����
	if(m_type!=Battle)
	{
		return false;
	}
	//������˲�����̷�Χ��
	//��Ӵ���
	//
	//����ս����
	TBattlePoint myCE = getCE();
	TBattlePoint enemyCE = enemy->getCE();
	//������ʧ
	int deta = myCE-enemyCE;
	int mylost = floor(28*pow(2.71828,-deta));
	int enemylost = floor(30*pow(2.71828,deta));
	//����������
	if(m_BattleType!= Archer)
	{
		m_HealthPoint -= mylost;
	}
	if(m_HealthPoint < 0)
	{
		m_HealthPoint = 0;
	}
	//����Է����� ���ƶ�λ��
	if(!enemy->BeAttacked(enemylost,this))
	{
		if(m_BattleType!= Archer)
		{
			//�ƶ�
			m_position = enemy->m_position;
		}
	}
	return true;
}

/*
BeAttacked
�����ܵ�����ʱ���ã��ڲ�����
������attack �ܵ����˺�ֵ��enemy ���������ı���ָ��
����ֵ���Ƿ��� ����true
*/
bool Crops::BeAttacked(int attack,Crops* enemy)
{
	//��²���̱�
	if(m_type == Construct)
	{
		m_PlayerID = enemy->m_PlayerID;
		return true;
	}
	//ս����
	m_PeaceNum = 0;
	m_HealthPoint -= attack;
	if(m_HealthPoint <= 0)
	{
		m_HealthPoint = 0;
		return false;
	}
	return true;
}

bool Crops::bAlive()
{
	if(m_HealthPoint>0)
	{
		return true;
	}
	return false;
}

int Crops::getCE()
{
	//û��ս��������
	return ceil(
		((float)corpsBattlePoint[m_BattleType][m_level]*m_HealthPoint)
		/(float)battleHealthPoint[m_BattleType][m_level]);
}

TPoint Crops::getPos()
{
	return m_position;
}

/*
Recover
���Żظ���������פ����ÿ�غϵ��ã������ڲ��ж��Ƿ���лָ�
�޲�������ֵ
*/
void Crops::Recover()
{
	if(m_type == Construct)
	{
		return;
	}
	if(m_PeaceNum == 3)
	{
		m_HealthPoint += floor(battleHealthPoint[m_BattleType][m_level]/3);
		if(m_HealthPoint > battleHealthPoint[m_BattleType][m_level])
		{
			m_HealthPoint = battleHealthPoint[m_BattleType][m_level];
		}
		return;
	}
	m_PeaceNum ++;
}

/*
MergeCrops
�������࣬��������ı��ŵ��ã������ڲ�ɾ���Է�����
������cropsID��������ŵ�ID
�����Ƿ�����ɹ�
*/
bool Crops::MergeCrops(TCorpsID cropsID)
{
	//������ж���
	if(m_MovePoint == 0)
	{
		return false;
	}
	Crops* targetCrops = NULL;
	//����IDѰ���������
	//�ȴ���Ӵ���
	//
	//���Ŀ����鲻����
	if(targetCrops == NULL)
	{
		return false;
	}
	//�����ͬ��Ӫ
	if(m_PlayerID!=targetCrops->m_PlayerID)
	{
		return false;
	}
	//�����ͬ����
	if(m_type!=Battle||targetCrops->m_type!=Battle)
	{
		return false;
	}
	if(m_BattleType!=targetCrops->m_BattleType)
	{
		return false;
	}
	//����Ǽ�������Ҫ��
	int level1 = m_level+1;
	int level2 = targetCrops->m_level+1;
	if(level2>level1)
	{
		return false;
	}
	if(level1+level2 >3)
	{
		return false;
	}
	//����
	float HPSum = m_HealthPoint+targetCrops->m_HealthPoint;
	float TotalSum = battleHealthPoint[m_BattleType][m_level]+battleHealthPoint[m_BattleType][targetCrops->m_level];
	m_level = level1+level2-1;
	m_HealthPoint = battleHealthPoint[m_BattleType][m_level]*HPSum/TotalSum;
	m_MovePoint = 0;
	//�Է�����HP���㣬�ȴ�ɾ��
	targetCrops->m_HealthPoint = 0;
	//
}

void Crops::ResetMP()
{
	if(m_type == Battle)
	{
		m_MovePoint = battleMovePoint[m_BattleType][m_level];
	}
}
