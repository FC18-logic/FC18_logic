#include "tentacle.h"
#include "data.h"
#include<algorithm>
using namespace DATA;
Tentacle::Tentacle(TId source, TId target) :
	Object(), m_sourceStudent(source), m_targetStudent(target),
	m_length(getDistance(Data::students[source].getPos(), Data::students[target].getPos())),
	m_maxResource(m_length*Density)
{
	m_resource = 0;
	m_backResource = 0;
	m_frontResource = 0;
	m_status = Extending;

	//设置敌方触手
	m_enemyTentacle = NO_DATA;
	for (TId tentacleId : Data::students[target].getTentacles())
	{
		if (Data::tentacles[tentacleId].getTargetStudent() == ID())
		{
			m_enemyTentacle = tentacleId;
			Data::tentacles[tentacleId].setEnemyTentacle(ID());
		}
	}
}

bool Tentacle::cut(int position /*= 0*/)
{
	switch (m_status)
	{
	case Extending:
	case Backing:
	case Attacking:
	case Confrontation:
		m_status = AfterCut;
		m_backResource = m_resource;
		m_frontResource = 0;
		m_resource = 0;
		return true;
	case Arrived:
		//如果位置不合法则返回False
		if (m_resource < position)return false;
		m_backResource = position;
		m_frontResource = m_resource - position;
		m_resource = 0;
		return true;
	case AfterCut:
	case Finished:
		std::cerr << "Cut at invalid tentacle!" << std::endl;
		return false;
	}
}

const TSpeed Tentacle::getExtendSpeed() const
{
	TPower extraPower = 1.0;

	//院系技能特判
	if (Data::players[Data::students[m_sourceStudent].getCampID()].getDepartment() == Medical
		&& Data::players[Data::students[m_sourceStudent].getCampID()].skillLeftRound() > 0)
		extraPower = 2.0;

	//院系自身判断
	extraPower *= SpeedStage[Data::players[Data::students[m_sourceStudent].getCampID()].extendingSpeedLevel()];
	
	return BaseExtendSpeed*extraPower;
}

const TSpeed Tentacle::getFontSpeed() const
{
	TPower extraPower = 1.0;

	//院系技能特判
	if (Data::players[Data::students[m_sourceStudent].getCampID()].getDepartment() == Medical
		&& Data::players[Data::students[m_sourceStudent].getCampID()].skillLeftRound() > 0)
		extraPower = 2.0;

	//院系自身判断
	extraPower *= SpeedStage[Data::players[Data::students[m_sourceStudent].getCampID()].extendingSpeedLevel()];

	return BaseFontSpeed*extraPower;
}

const TSpeed Tentacle::getBackSpeed() const
{
	TPower extraPower = 1.0;

	//院系技能特判
	if (Data::players[Data::students[m_sourceStudent].getCampID()].getDepartment() == Medical
		&& Data::players[Data::students[m_sourceStudent].getCampID()].skillLeftRound() > 0)
		extraPower = 2.0;

	//院系自身判断
	extraPower *= SpeedStage[Data::players[Data::students[m_sourceStudent].getCampID()].extendingSpeedLevel()];

	return BaseBackSpeed*extraPower;
}

const TSpeed Tentacle::getTransportSpeed() const
{
	TPower extraPower = 1.0;

	//院系技能特判
	if (Data::players[Data::students[m_sourceStudent].getCampID()].getDepartment() == Medical
		&& Data::players[Data::students[m_sourceStudent].getCampID()].skillLeftRound() > 0)
		extraPower = 2.0;

	//院系统一判定
	extraPower*=RegenerationSpeedStage[Data::players[Data::students[m_sourceStudent].getCampID()].regenerationSpeedLevel()];
	
	//根据细胞信息判断传输速率
	Student* p_source = &Data::students[m_sourceStudent];
	int tentacleNum = p_source->getTentacles().size();
	//总分配资源
	TResourceD TotalResource(p_source->getStudentProperty().m_regenarationSpeed);
	for (TId _tentacleId : p_source->attackedBy())
	{
		Student* p_sourcesource = &Data::students[Data::tentacles[_tentacleId].getSourceStudent()];
		TotalResource += p_sourcesource->getStudentProperty().m_regenarationSpeed
			/ p_sourcesource->getTentacles().size();
	}
	return TotalResource*extraPower / p_source->getTentacles().size();
}

void Tentacle::finish()
{
	m_status = Finished;

	//源细胞中移除该触手
	vector<TId>& sourceTeneacles = Data::students[m_sourceStudent].getTentacles();
	for (int i = 0; i != sourceTeneacles.size(); ++i)
		if (sourceTeneacles[i] == ID())
			sourceTeneacles.erase(sourceTeneacles.begin() + i);

	//设置对峙触手属性
	if (m_enemyTentacle != NO_DATA)
		Data::tentacles[m_enemyTentacle].setEnemyTentacle(NO_DATA);
}

TransEffect Tentacle::move()
{
	TransEffect te;
	te.m_resourceToSource = te.m_resourceToTarget = 0;
	te.m_sourceStudent = m_sourceStudent;
	te.m_targetStudent = m_targetStudent;
	TResourceD newResource = m_resource;
	switch (m_status)
	{
	case Extending:
	{
		//到达目标判定
		if (m_enemyTentacle == NO_DATA && m_resource + getExtendSpeed() > m_maxResource)
		{
			newResource = m_maxResource;
			m_status = Arrived;
		}
		//遇到对方触手判定
		else if (m_enemyTentacle != NO_DATA
			&&m_resource + getExtendSpeed() + Data::tentacles[m_enemyTentacle].getResource() > m_maxResource)
		{
			if (m_resource > m_maxResource / 2)
			{
				m_status = Backing;
				Data::tentacles[m_enemyTentacle].setStatus(Attacking);
			}
			else
			{
				m_status = Attacking;
				Data::tentacles[m_enemyTentacle].setStatus(Backing);
			}
			newResource = m_maxResource - Data::tentacles[m_enemyTentacle].getResource();
		}
		//一般情况
		else
			newResource = m_resource + getExtendSpeed();
	}
		break;
	case Backing:
	{
		//在对面触手之后——由对面触手控制
		if (m_enemyTentacle < ID())
		{
			newResource = m_maxResource - Data::tentacles[m_enemyTentacle].getResource();
			if (Data::tentacles[m_enemyTentacle].getStatus() == Confrontation)
				m_status = Confrontation;
		}
		else
		{
			//推到了中点之后
			if (m_resource - Data::tentacles[m_enemyTentacle].getExtendSpeed() < m_maxResource / 2.0)
			{
				m_status = Confrontation;
				newResource = m_maxResource / 2.0;
			}
			else
				newResource = m_resource - Data::tentacles[m_enemyTentacle].getExtendSpeed();
		}
	}
		break;
	case Attacking:
	{
		//在对面触手之后——由对面触手控制
		if (m_enemyTentacle < ID())
		{
			newResource = m_maxResource - Data::tentacles[m_enemyTentacle].getResource();
			if (Data::tentacles[m_enemyTentacle].getStatus() == Confrontation)
				m_status = Confrontation;
		}
		else
		{
			//推到了中点之后
			if (m_resource - getExtendSpeed() > m_maxResource / 2.0)
			{
				m_status = Confrontation;
				newResource = m_maxResource / 2.0;
			}
			else
				newResource = m_resource + getExtendSpeed();
		}
	}
	case Confrontation:
		break;
	case Arrived:
		break;
	case AfterCut:
		break;
	case Finished:
		break;
	default:
		break;
	}
	te.m_resourceToSource = m_resource - newResource;
	m_resource = newResource;
	return te;
}

TransEffect Tentacle::transport()
{
	TransEffect te;
	te.m_resourceToSource = te.m_resourceToTarget = 0;
	te.m_sourceStudent = m_sourceStudent;
	te.m_targetStudent = m_targetStudent;

	switch (m_status)
	{
	case Extending:
		break;
	case Attacking:
		break;
	case Backing:
		break;
	case Confrontation:
	{
		te.m_resourceToSource = -getTransportSpeed();
	}
		break;
	case Arrived:
	{
		te.m_resourceToSource = -getTransportSpeed();
		//如果打的是自己人
		if (!isTargetEnemy())
			te.m_resourceToTarget = getTransportSpeed();
		else
			te.m_resourceToTarget = -1.5 * getTransportSpeed();
	}
		break;
	case AfterCut:
	{
		bool over[2] = { false,false };   //判断是否传输完毕
		if (getFontSpeed() < m_frontResource)
		{
			m_frontResource = 0;
			over[0] = true;
			if (!isTargetEnemy())
				te.m_resourceToTarget = m_frontResource;
			else
				te.m_resourceToTarget = -m_frontResource;
		}
		else
		{
			m_frontResource -= getFontSpeed();
			if (!isTargetEnemy())
				te.m_resourceToTarget = getFontSpeed();
			else
				te.m_resourceToTarget = -getFontSpeed();
		}
		if (getFontSpeed() < m_frontResource)
		{
			over[1] = true;
			m_frontResource = 0;
			if (!isTargetEnemy())
				te.m_resourceToTarget = m_frontResource;
			else
				te.m_resourceToTarget = -m_frontResource;
		}
		else
		{
			m_frontResource -= getFontSpeed();
			if (!isTargetEnemy())
				te.m_resourceToTarget = getFontSpeed();
			else
				te.m_resourceToTarget = -getFontSpeed();
		}
		if (over[0] && over[1])
			finish();
	}
		break;
	case Finished:
		break;
	default:
		break;
	}
	return te;
}

bool Tentacle::isTargetEnemy() const
{
	return Data::players[Data::students[getSourceStudent()].getCampID()].ID()
		!= Data::players[Data::students[getSourceStudent()].getCampID()].ID();
}
