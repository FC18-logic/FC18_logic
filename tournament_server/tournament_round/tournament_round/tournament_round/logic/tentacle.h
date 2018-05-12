#pragma once
/*
**Tencacle类
**
**2017/12/26
**By Chiba
**
*/
#ifndef TENTACLE_H
#define TENTACLE_H
#include "definition.h"
#include "data.h"
#include "cell.h"
#include "player.h"
enum TransType
{
	moveE, transE
};
using namespace DATA;
//用来表示对目标同学结算效果的临时变量
struct TransEffect
{
	TransType      m_type;
	TCellID        m_target;                   //目标细胞
	TCellID        m_source;                   //源细胞
	TPlayerID      m_currOwner; //当前源细胞的主人
	TResourceD     m_resourceToTarget = 0.0;          //给目标的资源数
	TResourceD     m_resourceToSource = 0.0;          //给源的资源数
	TResourceD     m_resourceChange = 0.0;           //本资源改变数
	TentacleState m_currstate;                      //当前状态-用于判定归属时
	TResourceD     m_frontChange = 0.0;        //只用于切断
	TResourceD     m_backChange = 0.0;         //只用于切断
	bool handle; //是否还要处理
	TransEffect(int target, int source,TentacleState ts, TPlayerID p):
		m_target(target), m_source(source), m_currstate(ts),m_currOwner(p) {handle = true;}
	TransEffect() { handle = false; }
};

class Tentacle
{
private:
	const TCellID             m_source;              //源细胞
	const TCellID             m_target;              //目标细胞
	TentacleState  m_state;                     //触手状态

	const TLength         m_length;                     //触手长度（由源/目标决定）
	const TResourceD      m_maxResource;
	TResourceD      m_resource;                   //当前资源      （切断前有效）
	TResourceD      m_frontResource;              //切断后前方资源（切断后有效）
	TResourceD      m_backResource;               //切断后后方资源（切断后有效）
	TTentacleID		m_ID;
	TTentacleID		m_cutID;
	static TTentacleID	ID;  //#json 增加触手id属性
	static TTentacleID  cut_ID; //断触手id
	DATA::Data* const     data;

public:
	//#json
	int getID() const { return m_ID; }
	int getCutID() const { return m_cutID; }
	void setID() { m_ID = ID; }

	//新建触手
	Tentacle(const Tentacle& _tentacle); //#json
	Tentacle() :m_source(NO_DATA),
		m_target(NO_DATA), m_length(0), m_maxResource(0), data(nullptr) { setID(); ID++;}
	Tentacle(TCellID source, TCellID target, DATA::Data* _data);


	//切断触手，position为从源细胞算起的资源数，返回值表示是否成功
	bool cut(TResourceD position = 0);
	//结算移动，只对Extending/Attacking/Backing有效
	TransEffect move();
	//结算运输
	TransEffect transport();

	bool           isTargetEnemy() const;
	const CellStrategy   srcStg(); //源细胞策略
	const CellStrategy   tgtStg(); //目标细胞策略
	
	const TSpeed         getExtendSpeed()   const;
	const TSpeed         getFrontSpeed()   const;
	const TSpeed         getBackSpeed()   const;
	const TCellID            getTargetCell() const { return m_target; }
	const TCellID            getSourceCell() const { return m_source; }
	TentacleState getstate()        const { return m_state; }
	const TLength        getLength()        const { return m_length; }
	TResourceD     getResource()      const { return m_resource; }
	TResourceD     getFrontResource() const { return m_frontResource; }
	TResourceD     getBackResource()  const { return m_backResource; }
	Tentacle*      getEnemyTentacle() const { return data->tentacles[m_target][m_source]; }
	void takeEffect(const TransEffect& te);
	void           setstate(TentacleState ts) { m_state = ts; }
	TResourceD     totalResource()const;
	void           finish();                //结束这条触手的一生
};

#endif // !TENTACLE_H