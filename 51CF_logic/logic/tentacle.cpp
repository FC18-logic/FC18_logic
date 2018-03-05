#include "tentacle.h"
#define min(a,b) (a < b? a:b)
Tentacle::Tentacle(TCellID source, TCellID target, DATA::Data* _data):
	m_source(source),m_target(target),data(_data),
	m_length(getDistance(_data->cells[source].getPos(),_data->cells[target].getPos())),
	m_maxResource(Density*m_length)
{
	data->tentacles[m_source][m_target] = this;
	data->TentacleNum++;
	m_resource = m_frontResource = m_backResource = 0;
	m_id = data->TentacleCount++;
}

bool Tentacle::cut(TResourceD position /*= 0*/)
{
	//归一化操作
	if (position <= 0)
		position = 0;
	if (position >= m_resource)
		position = m_resource;
	//根据触手状态不同进行处理
	switch (m_state)
	{
	case Extending:
		m_frontResource = 0;
		m_backResource = m_resource;
		m_resource = 0;
		m_state = AfterCut;
		return true;
		break;
	case Attacking:
	case Confrontation:
	case Backing:
		m_frontResource = 0;
		m_backResource = m_resource;
		m_resource = 0;
		m_state = AfterCut;
		getEnemyTentacle()->setstate(Extending);
		return true;
		break;
	case Arrived:
		m_frontResource = m_resource - position;
		m_backResource = position;
		m_resource = 0;
		m_state = AfterCut;
		return true;
		break;
	case AfterCut:
		return false;
		break;
	default:
		return false;
	}
}

//只考虑理想时的情况，不够的在外部讨论
TransEffect Tentacle::move()
{
	//我方倍率
	TransEffect te(m_target, m_source, m_state,data->cells[m_source].getPlayerID());
	switch (m_state)
	{
	case Extending:
	{
		/*
		double distance = BaseFontSpeed * power;
		//如果对面也有触手
		if (getEnemyTentacle() && getEnemyTentacle()->getstate() == Extending)
		{
			//只有要相遇的时候才进行特殊判定
			if (m_resource + getEnemyTentacle()->getResource() + distance > m_maxResource)
			{
				te.m_resourceChange = m_maxResource - m_resource - getEnemyTentacle()->getResource();
				{
					//判断攻守
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
		//如果对面有敌方触手，则更新倍率为大者（我方自然考虑）//旧
		//新版本：使用推进方速率
		te.m_resourceChange = getExtendSpeed();
		te.m_resourceToSource = -getExtendSpeed();
		te.m_resourceToTarget = 0.0;
			//te.m_nextState = Extending;
		//针对是否到达中点进行判定
		/*
		if (m_resource + BaseExtendSpeed * power > m_maxResource / 2)  //即将超过中点
		{
			m_resource = m_maxResource / 2;
			te.m_resourceToSource = m_resource;
		}*/
		break;
	case Backing:
		//如果对面有敌方触手，则更新倍率为大者（我方自然考虑）//旧
		//新版本：使用推进方速率
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
		if (isTargetEnemy())//是敌人则采取压制算法
		{
			te.m_resourceToTarget = te.m_frontChange * CellConfrontPower[srcStg()][tgtStg()];
		}
		else
		{
			te.m_resourceToTarget = -te.m_frontChange;
		}
		te.m_resourceToSource = -te.m_backChange;
		break;
	default:
		break;
	}
	te.m_type = moveE;
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
	case Confrontation:
	{
		Cell *s, *t; //源、目标
		s = data->cells + m_source;
		t = data->cells + m_target;
		TResourceD my = s->baseTransSpeed()*CellConfrontPower[s->getStg()][t->getStg()]; //我方造成伤害
		TResourceD enemy = t->baseTransSpeed()*CellConfrontPower[t->getStg()][s->getStg()]; //对方造成伤害
		//谁的伤害高就按谁的来结算
		te.m_resourceToSource = -(my > enemy ? s->baseTransSpeed() : enemy);
		te.m_resourceChange = te.m_resourceToTarget = 0;
	}
		break;
	case Arrived:
	{
		Cell *s, *t; //源、目标
		s = data->cells + m_source;
		t = data->cells + m_target;
		TResourceD my = s->baseTransSpeed();
		te.m_resourceToSource = -my;
		if (t->getPlayerID() == Neutral)//中立
			te.m_resourceToTarget = -my*CellSupressPower[s->getStg()][t->getStg()];
		else if (s->getPlayerID() != t->getPlayerID())//敌人
			te.m_resourceToTarget = -my*CellSupressPower[s->getStg()][t->getStg()]*
			DefenceStage[data->players[t->getPlayerID()].getDefenceLevel()]/1.5;//防御系数
		else
			te.m_resourceToTarget = my;
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
	double power = SpeedStage[data->players[data->cells[m_source].getPlayerID()].getMoveLevel()];
	return BaseExtendSpeed * power;
}

const TSpeed Tentacle::getFrontSpeed() const
{
	double power = SpeedStage[data->players[data->cells[m_source].getPlayerID()].getMoveLevel()];
	return BaseFrontSpeed * power;
}

const TSpeed Tentacle::getBackSpeed() const
{
	double power = SpeedStage[data->players[data->cells[m_source].getPlayerID()].getMoveLevel()];
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
}