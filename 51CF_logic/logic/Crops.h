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
	//新增兵团属性
	static TCorpsID		ID;
	TCorpsID			m_myID;//兵团ID
	corpsType			m_type;//兵团属性
	battleCorpsType		m_BattleType;//战斗兵用
	constructCorpsType	m_BuildType;//建造兵用
	TPlayerID				m_PlayerID;//所属阵营

	TMovePoint				m_MovePoint;//行动力
	THealthPoint			m_HealthPoint;//生命值 归0即死亡
	int						m_level;//等级 从0开始
	TBuildPoint				m_BuildPoint;//劳动力
	bool					m_bResting;//是否在休整
	TPoint					m_position;//位置
	int						m_PeaceNum;//驻扎后多少个回合没有受到攻击
	//
	DATA::Data* const		m_data;//游戏信息
public:
	Crops(void);
	~Crops(void);
	Crops(DATA::Data* _data, corpsType type, battleCorpsType battletype, constructCorpsType buildtype, TPlayerID ID, TPoint pos);
	//作战兵团移动 返回是否成功移动
	bool Move(int dx, int dy);
	bool Attack(CorpsCommandEnum type, int ID);
	//是否存活
	bool bAlive();
	//获取战斗力
	TBattlePoint getCE();
	//整编兵团 返回是否整编成功
	bool MergeCrops(TCorpsID ID);
	//兵团进入驻扎状态
	void GoRest();
	//建造兵修塔
	bool DoConstruct();
	//建造兵更改地形
	bool ChangeTerrain(terrainType target);
	//新回合开始
	void newRound();
	//提供兵团信息
	struct CorpsInfo ShowInfo();
	//获取当前兵团的id
	TCorpsID getID() { return m_myID; }
	//获取当前兵团的类型
	corpsType getType() { return m_type; }
	//获取当前兵团的星级（从0开始）
	int getLevel() { return m_level; }
	//获取当前兵团的位置
	TPoint getPos() { return m_position; }
	//获取当前战斗兵团的类型
	battleCorpsType getbattleType() { return m_BattleType; }
	//修改当前兵团所属玩家
	void set_playid(TPlayerID id) { m_PlayerID = id; }
	//修改当前兵团的生命值
	void set_hp(THealthPoint hp) { m_HealthPoint = hp; }
protected:
	//作战兵团受到攻击 返回是否存活 如果死亡
	bool BeAttacked(int attack, TPlayerID ID);
	//重置行动力
	void ResetMP();
	//回复HP
	void Recover();
	//作战兵团攻击
	int AttackCrops(Crops* enemy);
	//作战兵团攻击塔
	int AttackTower(class Tower *enemy);
	//更新位置
	void UpdatePos(TPoint targetpos);
};

#endif