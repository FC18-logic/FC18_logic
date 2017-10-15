#ifndef TENTACLE_H
#define TENTACLE_H
#include "definition.h"
#include "object.h"

//用来表示对目标同学结算效果的临时变量
struct TransEffect
{
	TId            m_targetStudent;                   //目标同学
	TId            m_sourceStudent;                   //源同学
	TResourceD     m_resourceToTarget = 0.0;          //给目标的资源数
	TResourceD     m_resourceToSource = 0.0;          //给源的资源数
};

class Tentacle:public Object<Tentacle>
{
private:
	const TId             m_sourceStudent;              //源同学
	const TId             m_targetStudent;              //目标同学
	      TentacleStatus  m_status;                     //触手状态
	const TLength         m_length;                     //触手长度（由源/目标决定）
	const TResourceI      m_maxResource;
	      TResourceD      m_resource;                   //当前资源      （切断前有效）
	      TResourceD      m_frontResource;              //切断后前方资源（切断后有效）
	      TResourceD      m_backResource;               //切断后后方资源（切断后有效）
	      TId             m_enemyTentacle;              //对方触手

public:
	//新建触手
	Tentacle(TId source, TId target);
	//切断触手，position为从源细胞算起的资源数，返回值表示是否成功
	bool cut(int position = 0);
	
	//结算移动
	TransEffect move();
	//结算运输
	TransEffect transport();
	
	bool           isTargetEnemy() const;
	const TSpeed         getExtendSpeed()   const;
	const TSpeed         getFontSpeed()   const;
	const TSpeed         getBackSpeed()   const;
	const TSpeed         getTransportSpeed()   const;
	const TId            getTargetStudent() const { return m_targetStudent; }
	const TId            getSourceStudent() const { return m_sourceStudent; }
	      TentacleStatus getStatus()        const { return m_status; }
	const TLength        getLength()        const { return m_length; }
	      TResourceI     getResource()      const { return m_resource; }
	      TResourceI     getFrontResource() const { return m_frontResource; }
	      TResourceI     getBackResource()  const { return m_backResource; }
		  void           setEnemyTentacle(TId enemy) { m_enemyTentacle = enemy; }
		  void           setStatus(TentacleStatus ts) { m_status = ts; }
		  void           finish();                //结束这条触手的一生
};

#endif // !TENTACLE_H