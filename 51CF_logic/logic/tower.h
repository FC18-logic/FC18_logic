#pragma once

#ifndef TOWER_H
#define TOWER_H

#include "definition.h"
#include "data.h"
#include "Crops.h"
#include "player.h"

class Tower
{
private:
	//塔属性
	static TTowerID		ID;
	TTowerID			m_id;//塔ID
	bool				m_exsit;//塔是否存活：即等级是否大于1
	TProductPoint       m_productpoint;//生产力
	productType			m_producttype;//生产任务类型  0-4：生产	5：升级
	THealthPoint        m_healthpoint;//生命值
	TBattlePoint        m_battlepoint;//战斗力
	TExperPoint			m_experpoint;//经验值
	TExperPoint			m_upgradexper;//升级到下一级所需经验值
	int					m_level;//等级:从1开始                 *补充定义
	int					m_attackrange;//攻击范围               *补充定义
	int					m_productconsume;//当前生产任务对应得生产力消耗值	*补充定义
	vector <Crops*>    m_staycrops;//储存驻扎兵团指针
	TPlayerID			m_PlayerID;//所属阵营

	TPoint				m_position;//位置

	DATA::Data* const		m_data;//游戏信息
	//根据等级更新塔的属性
	void set_all(int level) 
	{
		m_productpoint = TowerInitConfig[level - 1].initBuildPoint;
		m_healthpoint = TowerInitConfig[level - 1].initHealthPoint;
		m_battlepoint = TowerInitConfig[level - 1].initProductPoint;
		m_upgradexper = TowerInitConfig[level - 1].upgradeExper;
		m_attackrange = TowerInitConfig[level - 1].battleRegion;
	}
	//每回合初始根据经验值判断是否升级
	bool set_level();
	void product_crops(productType protype);
public:
	Tower(void);
	~Tower(void);
	Tower(DATA::Data* _data, TPlayerID m_playid, TPoint pos);
	//判断上一轮生产任务是否完成
	bool protask_finish()
	{
		if (m_productconsume <= 0)
			return true;
		return false;
	}
	//每一回合开始时，结算上轮生产任务完成情况，并设置新的生产任务
	void set_producttype(productType m_protype);
	//显示所有者ID
	const TPlayerID showPlayerID() { return m_PlayerID; }
	//显示位置
	const TPoint showPosition() { return m_position; }
	//获取当前防御塔的等级
	int getLevel() { return m_level; }
	//获取当前防御塔的位置
	TPoint getPosition() { return m_position; }
	//获取当前防御塔所属阵营的玩家序号
	TPlayerID getOwnerID() { return m_PlayerID; }
	//获取塔当前是否存活（等级小于1即被摧毁）
	bool getexsit() { return m_exsit; }
	//提供防御塔信息>>>>>>>>>>这是【JYP】加的
	//>>>>>>>>>>m_producttype如果没有生产任务请用宏定义NOTASK=-1
	//>>>>>>>>>>需要补充一下info.exist的定义，也就是这个塔是否还在<bool>，是否被摧毁
	TowerInfo ShowInfo() {
		TowerInfo info;
		info.ID = m_id;
		info.ownerID = m_PlayerID;
		info.position = m_position;
		info.productPoint = m_productpoint;
		info.healthPoint = m_healthpoint;
		info.battlePoint = m_battlepoint;
		info.experPoint = m_experpoint;
		info.level = m_level;
		info.pdtType = m_producttype;
		info.productConsume = m_productconsume;
		return info;
	}
	//获取当前防御塔的生产力
	TProductPoint getProductPoint() { return m_productpoint; }
	//获取当前防御塔的生命值
	THealthPoint getHealthPoint() { return m_healthpoint; }
	//获取当前防御塔的战斗力
	TBattlePoint getBattlePoint() { return m_battlepoint; }
	//获取当前防御塔的经验值
	TExperPoint getExperPoint() { return m_experpoint; }
	//获取当前防御塔的攻击范围
	int getExperPoint() { return m_attackrange; }
	//遭到兵团进攻时，计算考虑增益后塔的战斗力
	TBattlePoint get_towerbp();
	//抵御兵团的进攻，返回塔是否被攻陷
	bool Be_Attacked(TPlayerID enemy_id, THealthPoint hp_decrease);
	//兵团驻扎信息录入 
	void input_staycrops(Crops* newcrop) { m_staycrops.push_back(newcrop); }
	//吕梦欣加 修理塔回复生命值
	void Recover();
};
#endif