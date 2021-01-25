#include "Crops.h"

TCorpsID Crops::ID = 0;

Crops::Crops():m_data(nullptr)
{
	
}


Crops::~Crops(void)
{
}

Crops::Crops(DATA::Data* _data, corpsType type, battleCorpsType battletype, constructCorpsType buildtype, TPlayerID PlayerID, TPoint pos)
	:m_data(_data)
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

	m_BuildPoint = 0;
	m_HealthPoint = 0;

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
	m_bStation = false;//��������������Ĭ��פ����
}

/*
Move
�����ڵ�ͼ�ڽ����ƶ����޷��ж�Ŀ��ص��Ƿ���ڱ�ı���
dx��x����仯�� dy��y����仯��
�����Ƿ��ƶ��ɹ����ɹ������λ��
*/
bool Crops::Move(int dx, int dy)
{
	int curpos_x = m_position.m_x;
	int curpos_y = m_position.m_y;
	int next_x = curpos_x+dx;
	int next_y = curpos_y+dy;
	TPoint next_pos;
	next_pos.m_x = next_x;
	next_pos.m_y = next_y;
	terrainType curtype = m_data->gameMap.map[curpos_x][curpos_y].type;
	//Խ��
	if(!m_data->gameMap.isPosValid(next_pos))
	{
		return false;
	}
	terrainType nexttype = m_data->gameMap.map[next_x][next_y].type;
	float dMP = (CorpsMoveCost[curtype]+CorpsMoveCost[nexttype])/2;
	int temp = m_MovePoint - ceil(dMP);
	//�ж�������
	if(temp<0)
	{
		return false;
	}
	m_MovePoint = temp;
	return true;
}


/*
AttackCrops
�ñ��Ŷ���һ���ŷ��𹥻���������㹥�������������š����ƶ�
���� enemy �ܹ�������ָ��
����ֵ bool �����ɹ�����true
lmx
�ȴ���һ���޸ģ��͹������ϲ�
*/
bool Crops::AttackCrops(Crops* enemy)
{
	if(m_MovePoint == 0)
	{
		return false;
	}
	m_MovePoint = 0;
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
	if(m_type == Battle)
	{
		if(m_HealthPoint>0)
		{
			return true;
		}
	}
	else if(m_BuildType == Builder)
	{
		if(m_BuildPoint>0)
		{
			return true;
		}
	}
	else
	{
		return true;
	}
	return false;
}

/*
getCE
���ظñ��ŵ�ս����
*/
TBattlePoint Crops::getCE()
{
	//ս��������
	terrainType type = m_data->gameMap.map[m_position.m_x][m_position.m_y].type;
	TBattlePoint Attack = CorpsBattleGain[type];
	//��ս����
	Attack += (TBattlePoint)ceil(
		(float)(corpsBattlePoint[m_BattleType][m_level]*m_HealthPoint)
		/(float)battleHealthPoint[m_BattleType][m_level]);
	return Attack;
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
	if(!m_bStation)
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
bool Crops::MergeCrops(Crops* targetCrops)
{
	//������ж���
	if(m_MovePoint == 0)
	{
		return false;
	}
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

/*
ResetMP
�غϿ�ʼʱ�����ж���
*/
void Crops::ResetMP()
{
	if(m_type == Battle)
	{
		m_MovePoint = battleMovePoint[m_BattleType][m_level];
	}
	else
	{
		m_MovePoint = constructMovePoint[m_BattleType];
	}
}


bool Crops::ChangeTerrain(terrainType target)
{
	if(!bAlive())
	{
		return false;
	}
	if(m_type != Construct)
		return false;
	if(m_BuildType != Builder)
		return false;
	int curID = m_data->gameMap.map[m_position.m_x][m_position.m_y].owner;
	if(curID != m_PlayerID)
	{
		return false;
	}
	m_data->gameMap.map[m_position.m_x][m_position.m_y].type = target;
	m_BuildPoint--;
	return true;
}
