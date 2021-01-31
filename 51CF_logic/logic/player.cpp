#include "player.h"
using std::vector;


Player::Player()
{
	alive = true;
	deadRound = -1;
	m_id = -1;
	data = nullptr;
	conqueTowerNum = 0;
	eliminateCorpsNum = 0;
	captureCorpsNum = 0;
}

//#json
Player::Player(Player& _player)// copy����player
{
	m_crops = _player.getCrops();
	m_tower = _player.getTower();
	alive = _player.isAlive();
	m_id = _player.getId();
	conqueTowerNum = _player.getCqTowerNum();
	eliminateCorpsNum = _player.getElCorpsNum();
	captureCorpsNum = _player.getCqCorpsNum();

	data = nullptr;   //����ǳ����

	//FC15��
	//m_cells = _player.cells();
	//m_techPoint = _player.techPoint();
	//m_RegenerationLevel = _player.getRegenerationLevel();
	//m_ExtraControlLevel = _player.getExtraControlLevel();
	//m_DefenceLevel = _player.getDefenceLevel();
	//m_MoveLevel = _player.getMoveLevel();
}


Player::~Player()
{
}

void Player::addTechPoint(TResourceD _techPoint)//������Ӧ�Ƽ�����
{
	m_techPoint += _techPoint;
}

//����ͻ���漰�����Եķ���
//��ҵĿƼ���������������ӵ�еĵ�ÿ�������׵�������������
void Player::regenerateTechPoint()
{
	TResourceD r = 0;
	for (TCellID c : m_cells)
	{
		r += data->cells[c].techRegenerateSpeed();
	}
	addTechPoint(r);
}

bool Player::upgrade(TPlayerProperty kind)
{
	switch (kind)
	{
	case RegenerationSpeed:
		return upgradeRegeneration();
		break;
	case ExtendingSpeed:
		return upgradeMove();
		break;
	case ExtraControl:
		return upgradeExtraControl();
		break;
	case CellWall:
		return upgradeDefence();
		break;
	default:
		return false;
		break;
	}
}

//��ȥ��Ӧ��Դ�����������Ͳ������ҷ���false
bool Player::subTechPoint(TResourceD _techPoint)
{
	if (_techPoint <= m_techPoint)
	{
		m_techPoint -= _techPoint;
		return true;
	}
	else
		return false;
}

//������������
bool Player::upgradeDefence()
{
	if (m_DefenceLevel < MAX_DEFENCE_LEVEL &&
		m_techPoint > DefenceStageUpdateCost[m_DefenceLevel])//�ȼ�û���������ҿƼ������㹻
	{
		subTechPoint(DefenceStageUpdateCost[m_DefenceLevel]);
		m_DefenceLevel++;
		return true;
	}
	else//ʧ�ܾͲ��۳��Ƽ���
		return false;
}

//������������
bool Player::upgradeRegeneration()
{
	if (m_RegenerationLevel < MAX_REGENERATION_SPEED_LEVEL &&
		m_techPoint > RegenerationSpeedUpdateCost[m_RegenerationLevel])//�ȼ�û���������ҿƼ������㹻
	{
		subTechPoint(RegenerationSpeedUpdateCost[m_RegenerationLevel]);
		m_RegenerationLevel++;
		return true;
	}
	else//ʧ�ܾͲ��۳��Ƽ���
		return false;
}

//�����������������
bool Player::upgradeExtraControl()
{
	if (m_ExtraControlLevel < MAX_EXTRA_CONTROL_LEVEL &&
		m_techPoint > ExtraControlStageUpdateCost[m_ExtraControlLevel])//�ȼ�û���������ҿƼ������㹻
	{		
		subTechPoint(ExtraControlStageUpdateCost[m_ExtraControlLevel]);
		m_ExtraControlLevel++;
		return true;
	}
	else//ʧ�ܾͲ��۳��Ƽ���
		return false;
}

//�ƶ���������
bool Player::upgradeMove()
{
	if (m_MoveLevel < MAX_EXTENDING_SPEED_LEVEL &&
		m_techPoint > ExtendingSpeedUpdateCost[m_MoveLevel])//�ȼ�û���������ҿƼ������㹻
	{
		subTechPoint(ExtendingSpeedUpdateCost[m_MoveLevel]);
		m_MoveLevel++;
		return true;
	}
	else//ʧ�ܾͲ��۳��Ƽ���
		return false;
}

//��ȡ�����������ָ������
int Player::maxControlNumber()
{
	return (m_cells.size() + m_ExtraControlLevel) / 2 + 1;
}

//��ȡÿ������Դ���ܺ�
//����ͻ����Ҫ������������
TResourceD Player::totalResource()
{
	TResourceD total = 0.0;
	for (TCellID i : m_cells)
	{
		total += data->cells[i].totalResource();
	}
	return total;
}

//ɱ�����
void Player::Kill()
{
	alive = false;
	deadRound = std::ceil(data->getRound() / 4); //���������غ����Ǵ�غ���
}

/***********************************************************************************************
*������ :��FC18��getPlayScore��ȡ��ҵ÷�
*������������ : ������ҵķ������ͱ�����Ϣ��������ҵ�ǰ�÷֣���������
*�������� : �ޣ�ֱ����Player�����
*��������ֵ : <int>��ֵ
*���� : ������
***********************************************************************************************/
int Player::getPlayerScore() {
	if (isAlive() == false)   //����Ѿ����֣����ݳ��ֵĻغ�����¼���з�ֵ
		return deadRound - MAX_ROUND;
	TScore corpsScore, towerScore;
	corpsScore = towerScore = 0;
	for (TCorpsID i : m_crops)
	{
		if (data->myCorps[i].getType() == Battle)  //�����Ǽ���0��ʼ[!!!����ȷ��]
			corpsScore += BATTLE_CORP_SCORE * (data->myCorps[i].getLevel() + 1);
		else if (data->myCorps[i].getType() == Construct)
			corpsScore += CONSTRUCT_CORP_SCORE * 1;
	}
	for (TTowerID i : m_tower)
	{
		towerScore += TOWER_SCORE * (data->myTowers[i].getLevel());             //�������ȼ���0��ʼ[!!!����ȷ��]
	}
	return corpsScore + towerScore;
}

/***********************************************************************************************
*������ :��FC18��addCrops
*������������ : ��m_crops�����ID���ڱ��Ź���ʱ����
*�������� : ��ӵı���ID
*��������ֵ : ��
*���� : ������
***********************************************************************************************/
void Player::addCrops(TCorpsID ID)
{
	m_crops.insert(ID);
}

/***********************************************************************************************
*������ :��FC18��deleteCrops
*������������ : ��m_crops��ɾ��ID���ڱ����ڲ���⵽HPΪ0ʱ����
*�������� : ɾ���ı���ID
*��������ֵ : ��
*���� : ������
***********************************************************************************************/
void Player::deleteCrops(TCorpsID ID)
{
	m_crops.erase(ID);
}