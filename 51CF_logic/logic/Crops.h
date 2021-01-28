#pragma once

#ifndef CROPS_H
#define CROPS_H

#include "definition.h"
#include "data.h"
#include "cell.h"
#include "player.h"



class Crops
{
private:

	static TCorpsID		m_staticID;
	TCorpsID			m_myID;			//兵团ID
	corpsType			m_type;			//兵团属性
	battleCorpsType		m_BattleType;	//战斗兵用
	constructCorpsType	m_BuildType;	//建造兵用
	TPlayerID			m_PlayerID;		//所属阵营

	TMovePoint				m_MovePoint;	//行动力
	THealthPoint			m_HealthPoint;	//生命值 归0即死亡
	int						m_level;		//等级 从0开始
	TBuildPoint				m_BuildPoint;	//劳动力
	bool					m_bResting;		//是否在休整
	TPoint					m_position;		//位置
	int						m_PeaceNum;		//驻扎后多少个回合没有受到攻击

	DATA::Data* const		m_data;	//游戏信息

	bool					m_bAlive;	//是否存活

public:

	Crops(void);
	~Crops(void);
	Crops(DATA::Data* _data, corpsType type, battleCorpsType battletype, constructCorpsType buildtype, TPlayerID ID, TPoint pos);
	
	//兵团操作

	bool Move(int dir);					//作战兵团移动 返回是否成功移动
	bool Attack(int type, int ID);	//兵团攻击
	bool MergeCrops(TCorpsID ID);				//整编兵团 返回是否整编成功
	bool GoRest();								//兵团进入驻扎休整状态
	bool StationInTower();						//玩家驻扎所在位置的己方塔
	bool MendTower();							//建造兵修塔
	bool ChangeTerrain(terrainType target);		//建造兵更改地形
	bool BuildTower();							//开拓者建塔
	void ChangeOwner(TPlayerID newowner);		//兵团改变所属
	void KillCorps();							//兵团死亡
	void newRound();							//新回合开始

	//获取兵团属性

	bool bAlive();					//是否存活
	TBattlePoint getCE();			//获取战斗力
	struct CorpsInfo ShowInfo();	//提供兵团信息
	TCorpsID getID() { return m_myID; }		//获取当前兵团的id
	corpsType getType() { return m_type; }	//获取当前兵团的类型
	int getLevel() { return m_level; }		//获取当前兵团的星级（从0开始）
	TPoint getPos() { return m_position; }	//获取当前兵团的位置
	battleCorpsType getbattleType() { return m_BattleType; }	//获取当前战斗兵团的类型
	
protected:

	bool BeAttacked(int attack, TPlayerID ID);	//作战兵团受到攻击 返回是否存活 如果死亡
	void ResetMP();	//重置行动力
	void Recover();	//回复HP
	int AttackCrops(Crops* enemy);			//作战兵团攻击
	int AttackTower(class Tower *enemy);	//作战兵团攻击塔
	void UpdatePos(TPoint targetpos);		//更新位置
	bool IsNeighbor(TPoint point);			//判断是否相邻
	bool IsInRange(TPoint point);			//判断是否处在攻击范围内
};

#endif