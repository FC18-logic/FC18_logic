#include "Crops.h"
#include "tower.h"

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
	m_bResting = true;//��������������Ĭ��פ����
	m_data->corps[m_position.m_x][m_position.m_y].push_back(this);
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
	//�ж�Ŀ��λ���Ƿ�Ѱ�Ҽ�����
	bool haveTower = false;
	int index = m_data->gameMap.map[next_x][next_y].TowerIndex;
	if(index != HAVE_NO_TOWER)
	{
		class Tower targettower = m_data->myTowers[index];
		if(targettower.showPlayerID() != m_PlayerID)
		{
			return false;
		}
		else
		{
			haveTower = true;
		}
	}
	if(!haveTower)
	{
		//Ŀ��λ���Ƿ���ڱ�ı���
		CorpsUnit targetPos = m_data->corps[next_x][next_y];
		//���̱�
		if(m_type == Construct)
		{
			if(targetPos.size()!=0)
				return false;
		}
		//ս����
		else
		{
			for(int i = 0; i<targetPos.size(); i++)
			{
				if(targetPos[i]->m_type == Construct && targetPos[i]->m_PlayerID == m_PlayerID)
				{
					continue;
				}
				return false;
			}
		}
	}
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
	m_bResting = false;
	//�޸�data�е�λ��
	UpdatePos(next_pos);
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
	//������˲�����̷�Χ��
	//��Ӵ���
	//
	int enemylost = 0;
	//���������ս����
	if(enemy->m_type == Battle)
	{
		//����ս����
		TBattlePoint myCE = getCE();
		TBattlePoint enemyCE = enemy->getCE();
		//������ʧ
		float deta = 0.04*((float)myCE-enemyCE);
		int mylost = floor(28*pow(2.71828,-deta));
		enemylost = floor(30*pow(2.71828,deta));
		//����������
		if(m_BattleType!= Archer)
		{
			m_HealthPoint -= mylost;
		}
		if(m_HealthPoint < 0)
		{
			m_HealthPoint = 0;
		}
	}

	//����Է����� ���ƶ�λ��
	if(!enemy->BeAttacked(enemylost,m_PlayerID))
	{
		if(m_BattleType!= Archer)
		{
			//�ƶ�
			UpdatePos(enemy->m_position);
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
bool Crops::BeAttacked(int attack,TPlayerID ID)
{
	//��²���̱�
	if(m_type == Construct)
	{
		m_PlayerID = ID;
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
���Żظ��������������ڲ��ж��Ƿ���лָ�
�޲�������ֵ
*/
void Crops::Recover()
{
	if(m_type == Construct)
	{
		return;
	}
	if(!m_bResting)
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
�������Է�����ָ�룬���� Ҳ������λ�û���ID
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
	float TotalSum = battleHealthPoint[m_BattleType][m_level]
					+battleHealthPoint[m_BattleType][targetCrops->m_level];
	m_level = level1+level2-1;
	m_HealthPoint = battleHealthPoint[m_BattleType][m_level]*HPSum/TotalSum;
	m_MovePoint = 0;
	//�Է�����HP���㣬�ȴ�ɾ��
	targetCrops->m_HealthPoint = 0;
	m_bResting = false;
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

/*
ChangeTerrain
�������������ڵ�Ԫ����
������target Ŀ�����
�����Ƿ����ɹ�
*/
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
	if(m_data->gameMap.map[m_position.m_x][m_position.m_y].TowerIndex == HAVE_NO_TOWER)
	{
		m_data->gameMap.map[m_position.m_x][m_position.m_y].type = target;
		m_BuildPoint--;
		m_bResting = false;
		return true;
	}
	return false;
}

//�»غϿ�ʼ
void Crops::newRound()
{
	ResetMP();
	Recover();
	GoRest();
}

//����פ������״̬
void Crops::GoRest()
{
	if(!m_bResting)
	{
		m_PeaceNum = 0;//��ʼ����
		m_bResting = true;
	}
}

/*
ShowInfo
�ṩ������Ϣ
���ر�����Ϣ�ṹ��
*/
struct CorpsInfo Crops::ShowInfo()
{
	struct CorpsInfo info;
	info.BuildPoint = m_BuildPoint;
	info.HealthPoint = m_HealthPoint;
	info.exist = 1;
	info.ID = m_myID;
	info.level = m_level;
	info.owner = m_PlayerID;
	info.pos = m_position;
	info.type = m_type;
	info.movePoint = m_MovePoint;
	return info;
}

/*
AttackTower
*/
bool Crops::AttackTower(class Tower *enemy)
{
	int TowerCE;//����ս����
	int myCE = getCE();//����ս����
	//����������������̷�Χ��
	//��Ӵ���
	//
	float deta = 0.04*((float)myCE-TowerCE);
	int mylost = floor(28*pow(2.71828,-deta));
	int enemylost = floor(30*pow(2.71828,deta))*corpsAttackTowerGain[m_BattleType][m_level];
	if(m_BattleType == Archer)
		mylost = 0;
	m_HealthPoint -= mylost;
	if(m_HealthPoint < 0)
		m_HealthPoint = 0;
	bool canmove = false;
	//�ж����Ƿ񱻴ݻ�
	//��Ӵ���
	//
	if(m_BattleType == Archer)
		canmove = false;
	if(canmove)
	{
		UpdatePos(enemy->showPosition());
	}
	return true;
}

/*
Attack
���ŷ������� �����Ƿ񹥻��ɹ�
���� type<CorpsCommandEnum> �����з����Ż����� ID �з�ID
*/
bool Crops::Attack(CorpsCommandEnum type, int ID)
{
	//�������ս����
	if(m_type!=Battle)
	{
		return false;
	}

	m_bResting = false;
	if(m_MovePoint == 0)
	{
		return false;
	}
	m_MovePoint = 0;
	if(type == CAttackCorps)
	{
		Crops* enemy = NULL;
		//Ѱ�ҵ���
		enemy = &m_data->myCorps[ID];
		//����������
		if(!enemy->bAlive())
		{
			return false;
		}
		//����ͬ��Ӫ
		if(enemy->m_PlayerID == m_PlayerID)
			return false;
		//�����������λ�ô��ڵз������� ������������
		int x = enemy->m_position.m_x;
		int y = enemy->m_position.m_y;
		int index = m_data->gameMap.map[x][y].TowerIndex;
		if(index == NOTOWER)
			return AttackCrops(enemy);
		if(m_data->myTowers[index].showPlayerID() == enemy->m_PlayerID)
			return AttackTower(&(m_data->myTowers[index]));
		else
			return AttackCrops(enemy);
	}
	else if(type == CAttackTower)
	{
		class Tower* enemy = NULL;
		//Ѱ�ҵ���
		enemy = &m_data->myTowers[ID];
		//����������
		//��Ӵ���
		//
		//����ͬ��Ӫ
		if(enemy->showPlayerID() == m_PlayerID)
			return false;
		return AttackTower(enemy);
	}
	return false;
}


void Crops::UpdatePos(TPoint targetpos)
{
	CorpsUnit thisplace = m_data->corps[m_position.m_x][m_position.m_y];
	CorpsUnit::iterator it;
	for(it = thisplace.begin(); it!=thisplace.end(); it++)
	{
		if(*it == this)
		{
			thisplace.erase(it);
			break;
		}
	}
	m_data->corps[targetpos.m_x][targetpos.m_y].push_back(this);
	m_position = targetpos;
}
