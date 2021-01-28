#include "Crops.h"
#include "tower.h"

TCorpsID Crops::m_staticID = 0;

Crops::Crops():m_data(nullptr)
{
	
}


Crops::~Crops(void)
{
}

Crops::Crops(DATA::Data* _data, corpsType type, battleCorpsType battletype, constructCorpsType buildtype, TPlayerID PlayerID, TPoint pos)
	:m_data(_data)
{
	m_bAlive = true;
	m_type = type;
	m_BattleType = battletype;
	m_BuildType = buildtype;
	m_PlayerID = PlayerID;
	m_position = pos;
	m_myID = m_staticID;
	m_staticID++;
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
	m_data->corps[m_position.m_y][m_position.m_x].push_back(this);
	m_data->players[m_PlayerID].addCrops(m_myID);
	m_data->totalCorps++;
	m_data->newCorps.insert(m_myID);//��¼�²����ı������
}

/*
Move
�����ڵ�ͼ�ڽ����ƶ����޷��ж�Ŀ��ص��Ƿ���ڱ�ı���
dx��x����仯�� dy��y����仯��
�����Ƿ��ƶ��ɹ����ɹ������λ��
*/
bool Crops::Move(int dir)
{
	if(!m_bAlive)
	{
		return false;
	}
	int curpos_x = m_position.m_x;
	int curpos_y = m_position.m_y;
	int next_x = curpos_x;
	int next_y = curpos_y;
	switch(dir)
	{
	case CUp:
		{
			next_y--;
		}
		break;
	case CDown:
		{
			next_y++;
		}
		break;
	case CLeft:
		{
			next_x--;
		}
		break;
	case CRight:
		{
			next_x++;
		}
		break;
	default:
		{
			return false;
		}
	}

	//�ж�Ŀ��λ���Ƿ���ڼ�����
	bool haveTower = false;
	int index = m_data->gameMap.map[next_y][next_x].TowerIndex;
	if(index != NOTOWER)
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
		CorpsUnit targetPos = m_data->corps[next_y][next_x];
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
	terrainType curtype = m_data->gameMap.map[curpos_y][curpos_x].type;
	//Խ��
	if(!m_data->gameMap.isPosValid(next_pos))
	{
		return false;
	}
	terrainType nexttype = m_data->gameMap.map[next_y][next_x].type;
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
����ֵ int ���ٵ�HP
lmx
�ȴ���һ���޸ģ��͹������ϲ�
*/
int Crops::AttackCrops(Crops* enemy)
{
	//������˲�����̷�Χ��
	//��Ӵ���
	//
	int enemylost = 0;
	int mylost = 0;
	//���������ս����
	if(enemy->m_type == Battle)
	{
		//����ս����
		TBattlePoint myCE = getCE();
		TBattlePoint enemyCE = enemy->getCE();
		//������ʧ
		float deta = 0.04*((float)myCE-enemyCE);
		mylost = floor(28*pow(2.71828,-deta));
		enemylost = floor(30*pow(2.71828,deta));
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
	return mylost;
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
		ChangeOwner(ID);
		int num = m_data->players[ID].getCqCorpsNum() + 1;
		m_data->players[ID].setCqCorpsNum(num);
		return true;
	}
	//ս����
	m_PeaceNum = 0;
	m_HealthPoint -= attack;
	if(m_HealthPoint<=0)
	{
		KillCorps();
		int num = m_data->players[ID].getElCorpsNum() + 1;
		m_data->players[ID].setElCorpsNum(num);
	}

	return m_bAlive;
}

bool Crops::bAlive()
{
	return m_bAlive;
}

/*
getCE
���ظñ��ŵ�ս����
*/
TBattlePoint Crops::getCE()
{
	//ս��������
	terrainType type = m_data->gameMap.map[m_position.m_y][m_position.m_x].type;
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
bool Crops::MergeCrops(TCorpsID targetID)
{
	Crops* targetCrops = NULL;
	targetCrops = &(m_data->myCorps[targetID]);
	//���Ŀ����鲻����
	if(targetCrops == NULL)
		return false;

	//������ж���
	if(m_MovePoint == 0)
		return false;

	//���һ���Ѿ�����
	if(!m_bAlive||!(targetCrops->m_bAlive))
		return false;

	//�����ͬ��Ӫ
	if(m_PlayerID!=targetCrops->m_PlayerID)
		return false;

	//�����ͬ����
	if(m_type!=Battle||targetCrops->m_type!=Battle)
		return false;
	if(m_BattleType!=targetCrops->m_BattleType)
		return false;

	//��������ھ�
	if(!IsNeighbor(targetCrops->m_position))
		return false;

	//����Ǽ�������Ҫ��
	int level1 = m_level+1;
	int level2 = targetCrops->m_level+1;
	if(level2>level1)
		return false;
	if(level1+level2 >3)
		return false;

	//����
	float HPSum = m_HealthPoint+targetCrops->m_HealthPoint;
	float TotalSum = battleHealthPoint[m_BattleType][m_level]
					+battleHealthPoint[m_BattleType][targetCrops->m_level];
	m_level = level1+level2-1;
	m_HealthPoint = battleHealthPoint[m_BattleType][m_level]*HPSum/TotalSum;

	m_MovePoint = 0;
	targetCrops->m_HealthPoint = 0;
	targetCrops->KillCorps();
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
	if(!m_bAlive)
		return false;
	if(m_type != Construct)
		return false;
	if(m_BuildType != Builder)
		return false;
	int curID = m_data->gameMap.map[m_position.m_y][m_position.m_x].owner;
	if(curID != m_PlayerID)
		return false;
	if(m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex == NOTOWER)
	{
		m_data->gameMap.map[m_position.m_y][m_position.m_x].type = target;
		m_BuildPoint--;
		m_bResting = false;
		if(m_BuildPoint == 0)
		{
			KillCorps();
		}
		return true;
	}
	return false;
}

//�»غϿ�ʼ
void Crops::newRound()
{
	if(m_bAlive)
	{
		ResetMP();
		Recover();
		if(!m_bResting)
		{
			m_PeaceNum = 0;//��ʼ����
			m_bResting = true;
		}
	}
}

//����פ������״̬
bool Crops::GoRest()
{
	//�������
	if(m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex!=NOTOWER)
	{
		return false;
	}

	if(!m_bResting)
	{
		m_PeaceNum = 0;//��ʼ����
		m_bResting = true;
	}
	return true;
}

//פ���ڼ�������������
bool Crops::StationInTower()
{
	bool bStation = false;
	int index = m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex;
	//���ѡ��פ����λ������ ��פ��������
	if(index != NOTOWER)
	{
		//��������ڼ�����פ��
		if(m_data->myTowers[index].getOwnerID == m_PlayerID)
		{
			m_data->myTowers[index].input_staycrops(this);
			bStation = true;
		}
	}
	return bStation;
}


/*
ShowInfo
�ṩ������Ϣ
���ر�����Ϣ�ṹ��
*/
struct CorpsInfo Crops::ShowInfo()
{
	struct CorpsInfo info;
	info.ID = m_myID;
	info.level = m_level;
	info.owner = m_PlayerID;
	info.pos = m_position;
	info.type = m_type;
	info.movePoint = m_MovePoint;
	if (info.type == Battle) {
		info.m_BattleType = m_BattleType;
		info.HealthPoint = m_HealthPoint;  //���������ֵ�Լ���ս���������ṩ
	}
	else if (info.type == Construct) {
		info.m_BuildType = m_BuildType;
		info.BuildPoint = m_BuildPoint;
	}
	return info;
}

/*
AttackTower
*/
int Crops::AttackTower(class Tower *enemy)
{
	int TowerCE;//����ս����
	//��ȡ����ս����
	TowerCE = enemy->get_towerbp();
	int myCE = getCE();//����ս����
	//����������������̷�Χ��
	//��Ӵ���
	//
	float deta = 0.04*((float)myCE-TowerCE);
	int mylost = floor(28*pow(2.71828,-deta));
	int enemylost = floor(30*pow(2.71828,deta))*corpsAttackTowerGain[m_BattleType][m_level];

	bool IsTowerDestroy = false;
	//�ж����Ƿ񱻹���
	IsTowerDestroy = enemy->Be_Attacked(m_PlayerID, enemylost);
	if(IsTowerDestroy&&!(enemy->getexsit()))
	{
		int num = m_data->players[m_PlayerID].getCqTowerNum() + 1;
		m_data->players[m_PlayerID].setCqTowerNum(num);
	}

	//
	if(m_BattleType == Archer)
		IsTowerDestroy = false;
	if(IsTowerDestroy)
	{
		UpdatePos(enemy->showPosition());
	}
	return mylost;
}

/*
Attack
���ŷ������� �����Ƿ񹥻��ɹ�
���� type<CorpsCommandEnum> �����з����Ż����� ID �з�ID
*/
bool Crops::Attack(int type, int ID)
{
	//�������ս����
	if(m_type!=Battle)
		return false;

	//����Ѿ�����
	if(!m_bAlive)
		return false;

	m_bResting = false;

	if(m_MovePoint == 0)
		return false;

	m_MovePoint = 0;

	//�ҷ�HP����ֵ
	int mylost = 0;
	if(type == CAttackCorps)
	{
		Crops* enemy = NULL;
		//Ѱ�ҵ���
		enemy = &m_data->myCorps[ID];
		//����������
		if(!enemy->m_bAlive)
			return false;
		//����ͬ��Ӫ
		if(enemy->m_PlayerID == m_PlayerID)
			return false;
		//���˲�����̷�Χ��
		if(!IsInRange(enemy->m_position))
			return false;

		//�����������λ�ô��ڵз������� ������������
		int x = enemy->m_position.m_x;
		int y = enemy->m_position.m_y;
		int index = m_data->gameMap.map[y][x].TowerIndex;
		if(index == NOTOWER)
			mylost = AttackCrops(enemy);
		else if(m_data->myTowers[index].showPlayerID() == enemy->m_PlayerID)
			mylost =  AttackTower(&(m_data->myTowers[index]));
		else
			mylost = AttackCrops(enemy);
	}
	else if(type == CAttackTower)
	{
		class Tower* enemy = NULL;
		//Ѱ�ҵ���
		enemy = &m_data->myTowers[ID];
		//����������
		if(!enemy->getexsit())
			return false;
		//����ͬ��Ӫ
		if(enemy->showPlayerID() == m_PlayerID)
			return false;
		//���˲�����̷�Χ��
		if(!IsInRange(enemy->showPosition()))
			return false;

		mylost = AttackTower(enemy);
	}
	else//ָ������
		return false;
	//����������
	if(m_BattleType!= Archer)
	{
		m_HealthPoint -= mylost;
	}
	//���������
	if(m_HealthPoint <= 0)
	{
		KillCorps();
	}

	return true;
}

/*
UpdatePos ����λ��
������m_data��corps��¼��λ���Լ����ڲ���λ��
���� �µ�λ��
*/
void Crops::UpdatePos(TPoint targetpos)
{
	CorpsUnit* thisplace = &(m_data->corps[m_position.m_y][m_position.m_x]);
	CorpsUnit::iterator it;
	for(it = (*thisplace).begin(); it!=(*thisplace).end(); it++)
	{
		if(*it == this)
		{
			(*thisplace).erase(it);
			break;
		}
	}
	m_data->corps[targetpos.m_y][targetpos.m_x].push_back(this);
	m_position = targetpos;
}

/*
ChangeOwner
�ı������� �޸�data�ж�Ӧ��player�е�m_corps
���� ������ID
*/
void Crops::ChangeOwner(TPlayerID newowner)
{
	m_data->players[m_PlayerID].deleteCrops(m_myID);
	m_PlayerID = newowner;
	m_data->players[m_PlayerID].addCrops(m_myID);
}

void Crops::KillCorps()
{
	m_HealthPoint = 0;
	m_BuildPoint = 0;
	m_data->players[m_PlayerID].deleteCrops(m_myID);
	m_data->totalCorps--;
	CorpsUnit* thisplace = &(m_data->corps[m_position.m_y][m_position.m_x]);
	CorpsUnit::iterator it;
	for(it = (*thisplace).begin(); it!=(*thisplace).end(); it++)
	{
		if(*it == this)
		{
			(*thisplace).erase(it);
			break;
		}
	}
	m_bAlive = false;
	m_data->dieCorps.insert(m_myID);   //��¼��������ID
}

//����������
bool Crops::MendTower()
{
	if(!m_bAlive)
		return false;
	if(m_type != Construct)
		return false;
	if(m_BuildType != Builder)
		return false;

	//��ȡ��λ��������Ϣ
	int index = m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex;
	//û����
	if(index == NOTOWER)
		return false;
	//���Ǽ�����
	if(m_data->myTowers[index].getOwnerID()!=m_PlayerID)
		return false;
	//���ѱ��ݻ�
	if(!m_data->myTowers[index].getexsit())
		return false;

	m_BuildPoint--;
	m_bResting = false;
	if(m_BuildPoint == 0)
	{
		KillCorps();
	}

	m_data->myTowers[index].Recover();

}

//�����߽���
bool Crops::BuildTower()
{
	//��ȡ��λ��������Ϣ
	int index = m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex;
	//�����λ���Ѵ�����
	if(index != NOTOWER)
		return false;

	//��ȡ��λ������������
	TPlayerID OwnerID;
	OwnerID = m_data->gameMap.showOwner(m_position);
	//
	//����õ�Ԫ�����ڼ���
	if(OwnerID == m_PlayerID)
	{
		//�½�һ��������
		Tower newTower(m_data, m_PlayerID, m_position);
		m_data->myTowers.push_back(newTower);
	}
}

bool Crops::IsNeighbor(TPoint point)
{
	if(abs(point.m_x-m_position.m_x) == 1)
		return true;
	else if(abs(point.m_y-m_position.m_y) == 1)
		return true;
	return false;
}

bool Crops::IsInRange(TPoint point)
{
	int range = TBattleRange[m_BattleType];
	if(abs(point.m_x-m_position.m_x) <= range)
		return true;
	else if(abs(point.m_y-m_position.m_y) <= range)
		return true;
	return false;
}
