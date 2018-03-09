#pragma once
/*
** Class Cell
** 细胞类
** 保存每个细胞的信息
** 最后修改:2017/12/26
** By Chiba
*/
#ifndef  STUDENT_H_
#define  STUDENT_H_

#include<vector>
#include"tentacle.h"
#include"definition.h"
#include <iostream>
#include <set>
#include "data.h"

using namespace std;

struct CellProperty
{
	//TLA m_minLA;
	TResourceD m_maxResource;
	int m_maxTentacleNum;  //最大触手数量
	TPower m_techSpeed;    //科创点数是资源再生速率的几倍
};


class Cell
{
	friend class Tentacle;
public:
	Cell() :data(nullptr) {}   //默认构造函数，构造错误信息
	//根据输入信息进行初始化
	Cell(DATA::Data* _data, TPoint pos, TPlayerID campid, TResourceD resource, TResourceD maxResource, TPower techPower);
	~Cell(){}
	//Cell(const Cell &) = default;
	Cell(Cell& _cell); //#json
	
	void init(TCellID _id, DATA::Data* _data, TPoint pos, TPlayerID campid, TResourceD resource, TResourceD maxResource, TPower techPower);
	
	inline  TResourceD              getResource()           const { return m_resource; }
	inline  TCellID          getID() { return m_ID; }  //#json
	inline  TPlayerID        getPlayerID()const { return m_PlayerID; }
	inline  TPoint           getPos()              const { return m_position; }
	inline  TResourceD              getOccupyPoint()         const { return m_occupyPoint; }
	inline  CellType     getCellType()      const { return m_cellType; }
	inline  CellProperty& getCellProperty() { return m_property; }
	
	inline  CellStrategy getStg() { return m_strategy; }
	bool changeStg(CellStrategy stg);
	inline  void             setOccupyPoint(TResourceD val) { m_occupyPoint = val; }
	inline  TPlayerID getOccupyOwner() { return m_occupyOwner; }

	inline  void             setResource(TResourceD _la) { m_resource = _la; }
	inline  void             setPlayerID(TPlayerID _camp) { m_PlayerID = _camp; }
	inline  void             setCellType(const CellType & _type) { m_cellType = _type; }
	inline  void             setPos(const TPoint& _point) { m_position = _point; }
	TResourceD       totalResource();    //返回该细胞资源值加其触手资源值

	inline  int   TentacleNumber()const { return m_currTentacleNumber; } //返回触手数量
	TSpeed  baseRegenerateSpeed()const;
	TSpeed  baseTransSpeed()const;
	bool    resourceEmpty() { return m_resource <= 0.001; }                  //资源值是否减为0
	TSpeed Cell::techRegenerateSpeed()const;
	void    regenerate();                    //每一回合后的再生资源

	void addResource(TResourceD _r) { m_resource += _r; if (m_resource > m_property.m_maxResource)m_resource = m_property.m_maxResource; }

	bool    addTentacle(const TCellID&);  //增加触手,不成功返回false
	bool    cutTentacle(const TCellID&, TPosition pos);       //在某一个点砍断触手，点无效则砍断操作无效,返回是否成功
	void    updateProperty();            //根据当前资源更新属性
	void    changeOwnerTo(TPlayerID newOwner); //改变主人，资源结算需在外部
	void    N_addOcuppyPoint(TPlayerID owner, TResourceD point);  //N_前缀/只有中立细胞能够调用/加相应点数
	void    subTentacleNum() { m_currTentacleNumber--; }
	std::set<TTentacleID>& getTentacles() { return m_currTentacle; }

private:
	TCellID m_ID; //我的ID
	CellType      m_cellType;      //细胞种类
	CellProperty  m_property;         //特点
	CellStrategy m_strategy;  //策略

	TResourceD       m_resource;           //剩余资源值

	DATA::Data*            data;             //共享数据
	TPlayerID            m_PlayerID;           //所属阵营  int
	TPoint           m_position;         //所在点的位置等信息
	TResourceD              m_occupyPoint;         //中立同学的占有值
	TPlayerID               m_occupyOwner;       //中立点所属者
	TTentacleNum m_currTentacleNumber;
	std::set<TTentacleID> m_currTentacle;
};

#endif // STUDENT_H_