#include "tentacle.h"
#define min(a,b) (a < b? a:b)
int Tentacle::ID = 0; //#json
int Tentacle::cut_ID = 0;
Tentacle::Tentacle(TCellID source, TCellID target, DATA::Data* _data):
	m_source(source),m_target(target),data(_data),
	m_length(getDistance(_data->cells[source].getPos(),_data->cells[target].getPos())),
	m_maxResource(Density*m_length)
{
	data->tentacles[m_source][m_target] = this;
	data->TentacleNum++;
	m_resource = m_frontResource = m_backResource = 0;
	setID(); //#json
	ID++; //#json
}

Tentacle::Tentacle(const Tentacle & _tentacle) :
	data(_tentacle.data),
	m_source(_tentacle.getSourceCell()), m_target(_tentacle.getTargetCell()), m_length(_tentacle.getLength()),
	m_maxResource(Density*m_length)
{
	m_state = _tentacle.getstate();
	m_resource = _tentacle.getResource();
	m_frontResource = _tentacle.getFrontResource();
	m_backResource = _tentacle.getBackResource();
	m_ID = _tentacle.getID();
	m_cutID = _tentacle.m_cutID;
}


bool Tentacle::cut(TResourceD position /*= 0*/)
{
	//��һ������
	if (position <= 0)
		position = 0;
	if (position >= m_resource)
		position = m_resource;
	bool flag = false; //#json
	double _resource;//#json
					 //���ݴ���״̬��ͬ���д���
	switch (m_state)
	{
	case Extending:
	{
		m_frontResource = 0;
		m_backResource = m_resource;
		_resource = m_resource; //#json

		m_resource = 0;
		m_state = AfterCut;
		flag = true;
		break;
	}
	case Attacking:
	case Confrontation:
	case Backing:
		m_frontResource = 0;
		m_backResource = m_resource;
		_resource = m_resource; //#json
		m_resource = 0;
		m_state = AfterCut;
		getEnemyTentacle()->setstate(Extending);
		flag = true;
		break;
	case Arrived:
	{
		m_frontResource = m_resource - position;
		m_backResource = position;
		_resource = position; //#json
		m_resource = 0;
		//#json
		Json::Value cutTentacleAddtionJson;
		cutTentacleAddtionJson["type"] = 1;
		cutTentacleAddtionJson["id"] = cut_ID;
		m_cutID = cut_ID;
		cut_ID++;
		double tem = position / m_maxResource;
		double dis_x = data->cells[m_target].getPos().m_x - data->cells[m_source].getPos().m_x;
		double dis_y = data->cells[m_target].getPos().m_y - data->cells[m_source].getPos().m_y;
		cutTentacleAddtionJson["birthPosition"]["x"] = dis_x*tem + data->cells[m_source].getPos().m_x;
		cutTentacleAddtionJson["birthPosition"]["y"] = dis_y*tem + data->cells[m_source].getPos().m_y;
		cutTentacleAddtionJson["dstCell"] = m_target;
		cutTentacleAddtionJson["transRate"] = getFrontSpeed();
		cutTentacleAddtionJson["team"] = data->cells[getSourceCell()].getPlayerID() + 1;
		data->currentRoundJson["cutTentacleActions"].append(cutTentacleAddtionJson);
		data->cutTentacleBornJson[m_source][m_target] = true; //#----#
		data->cutTentacleInfoJson[m_source][m_target].cutID = m_cutID;
		m_state = AfterCut;
		flag = true;
		break;
	}
	case AfterCut:
		flag = false;
		break;
	default:
		flag = false;
	}

	if (flag)
	{
		data->cutTentacleInfoJson[m_source][m_target].frontResource = m_frontResource;
		data->cutTentacleJson[m_source][m_target] = true;
		Json::Value tentacleCutJson;
		tentacleCutJson["type"] = 5;
		tentacleCutJson["id"] = m_ID;
		double tem = _resource / m_maxResource;
		double dis_x = data->cells[m_target].getPos().m_x - data->cells[m_source].getPos().m_x;
		double dis_y = data->cells[m_target].getPos().m_y - data->cells[m_source].getPos().m_y;
		double x = dis_x*tem + data->cells[m_source].getPos().m_x;
		double y = dis_y*tem + data->cells[m_source].getPos().m_y;
		tentacleCutJson["cutPosition"]["x"] = x;
		tentacleCutJson["cutPosition"]["y"] = y;
		data->currentRoundJson["tentacleActions"].append(tentacleCutJson);
	}
	return flag;
}


//ֻ��������ʱ����������������ⲿ����
TransEffect Tentacle::move()
{
	//�ҷ�����
	TransEffect te(m_target, m_source, m_state,data->cells[m_source].getPlayerID());
	switch (m_state)
	{
	case Extending:
	{
		/*
		double distance = BaseFontSpeed * power;
		//�������Ҳ�д���
		if (getEnemyTentacle() && getEnemyTentacle()->getstate() == Extending)
		{
			//ֻ��Ҫ������ʱ��Ž��������ж�
			if (m_resource + getEnemyTentacle()->getResource() + distance > m_maxResource)
			{
				te.m_resourceChange = m_maxResource - m_resource - getEnemyTentacle()->getResource();
				{
					//�жϹ���
					if (getEnemyTentacle()->getResource() > m_maxResource / 2)
						te.m_nextState = Attacking;
					else
						te.m_nextState = Backing;
				}
			}
		}
		else*/
		{
			te.m_resourceChange = getExtendSpeed();
			te.m_resourceToSource = - getExtendSpeed();
			te.m_resourceToTarget = 0.0;
			//te.m_nextState = Extending; 
		}
		break;
	}
	case Attacking:
		//��������ез����֣�����±���Ϊ���ߣ��ҷ���Ȼ���ǣ�//��
		//�°汾��ʹ���ƽ�������
		te.m_resourceChange = getExtendSpeed();
		te.m_resourceToSource = -getExtendSpeed();
		te.m_resourceToTarget = 0.0;
			//te.m_nextState = Extending;
		//����Ƿ񵽴��е�����ж�
		/*
		if (m_resource + BaseExtendSpeed * power > m_maxResource / 2)  //���������е�
		{
			m_resource = m_maxResource / 2;
			te.m_resourceToSource = m_resource;
		}*/
		break;
	case Backing:
		//��������ез����֣�����±���Ϊ���ߣ��ҷ���Ȼ���ǣ�//��
		//�°汾��ʹ���ƽ�������
		te.m_resourceChange = -getEnemyTentacle()->getExtendSpeed();
		te.m_resourceToSource = getEnemyTentacle()->getExtendSpeed();
		te.m_resourceToTarget = 0.0;
		break;
	case Confrontation:
		te.m_resourceChange = 0.0;
		te.m_resourceToSource = 0.0;
		te.m_resourceToTarget = 0.0;
		//te.m_nextState = Extending;
		break;
	case Arrived:
		te.m_resourceChange = 0.0;
		te.m_resourceToSource = 0.0;
		te.m_resourceToTarget = 0.0;
		break;
	case AfterCut:
		te.m_frontChange = - min(m_frontResource, getFrontSpeed());
		te.m_backChange = - min(m_backResource, getBackSpeed());
		if (isTargetEnemy())//�ǵ������ȡѹ���㷨
		{
			te.m_resourceToTarget = te.m_frontChange * (((CellSupressPower[srcStg()][tgtStg()]-1)*0.5)+1);//���ǽ��������˺�
			if (data->cells[m_target].getPlayerID() >= 0 && data->cells[m_target].getPlayerID() < data->PlayerNum)//���Ǳ��������ķ���
			{
				te.m_resourceToTarget =te.m_resourceToTarget*DefenceStage[data->players[data->cells[m_target].getPlayerID()].getDefenceLevel()]/1.5;
			}
		}
		else
		{
			te.m_resourceToTarget = -te.m_frontChange;//Ŀ�������ǵ��ˣ��ж�ǰ��ȫ����Դ�����ڶ�Ŀ��������
		}
		te.m_resourceToSource = -te.m_backChange;//��Դ����Դ�� ����
		break;
	default:
		break;
	}
	te.m_type = moveE;//�޸Ĵ�����һ�β���Ϊ�ƶ�
	return te;
}

TransEffect Tentacle::transport()
{
	TransEffect te(m_target, m_source, m_state, data->cells[m_source].getPlayerID());
	switch (m_state)
	{
	case Extending:
		te.m_resourceToSource = te.m_resourceToTarget = te.m_resourceChange = 0;
		break;
	case Attacking:
	case Backing:
	case Confrontation://��������Դ������Դ��
	{
		Cell *s, *t; //Դ��Ŀ��
		s = data->cells + m_source;//������ַѰ�ȷ���
		t = data->cells + m_target;
		TResourceD my = s->baseTransSpeed()*CellConfrontPower[s->getStg()][t->getStg()]; //�ҷ�����˺�
		TResourceD enemy = t->baseTransSpeed()*CellConfrontPower[t->getStg()][s->getStg()]; //�Է�����˺�
		//˭���˺��߾Ͱ�˭��������
		te.m_resourceToSource = -(my > enemy ? my : enemy)/ CellConfrontPower[s->getStg()][t->getStg()];
		te.m_resourceChange = te.m_resourceToTarget = 0;
	}
		break;
	case Arrived:
	{
		Cell *s, *t; //Դ��Ŀ��
		s = data->cells + m_source;
		t = data->cells + m_target;
		TResourceD my = s->baseTransSpeed();
		te.m_resourceToSource = -my;//����Ϊ������������
		if (t->getPlayerID() == Neutral)//����
		{
			te.m_resourceToTarget = -my*CellSupressPower[s->getStg()][t->getStg()];
		}
		else if (s->getPlayerID() != t->getPlayerID())//����
		{
			if (t->getPlayerID() >= 0 && t->getPlayerID() < data->PlayerNum)
				te.m_resourceToTarget = -my*CellSupressPower[s->getStg()][t->getStg()] *
				DefenceStage[data->players[t->getPlayerID()].getDefenceLevel()] / 1.5;//����ϵ��
			else
				te.m_resourceToTarget = -my*CellSupressPower[s->getStg()][Normal];
		}
		else//���Լ��ģ�Դ��������٣�Ŀ�����õ�����
		{
			te.m_resourceToTarget = my;
		}
		te.m_resourceChange = 0;
	}
		break;
	case AfterCut:
		te.m_resourceToSource = te.m_resourceToTarget = te.m_resourceChange = 0;
		break;
	default:
		break;
	}
	te.m_type = transE;
	return te;
}

bool Tentacle::isTargetEnemy() const
{
	return data->cells[m_source].getPlayerID() != data->cells[m_target].getPlayerID();
}

const CellStrategy Tentacle::srcStg()
{
	return data->cells[m_source].getStg();
}

const CellStrategy Tentacle::tgtStg()
{
	return data->cells[m_target].getStg();
}

const TSpeed Tentacle::getExtendSpeed() const
{
	double power = 1.0;
	if(data->cells[m_source].getPlayerID()!=Neutral)
		power = SpeedStage[data->players[data->cells[m_source].getPlayerID()].getMoveLevel()];
	return BaseExtendSpeed * power;
}

const TSpeed Tentacle::getFrontSpeed() const
{
	double power = 1.0;
	if (data->cells[m_source].getPlayerID() != Neutral)
		power = SpeedStage[data->players[data->cells[m_source].getPlayerID()].getMoveLevel()];
	return BaseFrontSpeed * power;
}

const TSpeed Tentacle::getBackSpeed() const
{
	double power = 1.0;
	if (data->cells[m_source].getPlayerID() != Neutral)
		power = SpeedStage[data->players[data->cells[m_source].getPlayerID()].getMoveLevel()];
	return BaseBackSpeed * power;
}

void Tentacle::takeEffect(const TransEffect& te)
{
	m_backResource += te.m_backChange;
	m_frontResource += te.m_frontChange;
	m_resource += te.m_resourceChange;
}

TResourceD Tentacle::totalResource() const
{
	return m_backResource + m_frontResource + m_resource;
}

void Tentacle::finish()
{
	data->cells[m_source].m_currTentacleNumber--;
	data->cells[m_source].getTentacles().erase(m_ID);
}