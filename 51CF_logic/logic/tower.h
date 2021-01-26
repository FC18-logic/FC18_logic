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
	TProductPoint       m_productpoint;//生产力
	productType			m_producttype;//生产任务类型
	int					m_towercommand;//操作任务类型  0：生产	1：升级	   *补充定义
	THealthPoint        m_healthpoint;//生命值
	TBattlePoint        m_battlepoint;//战斗力
	TExperPoint			m_experpoint;//经验值
	TExperPoint			m_upgradexper;//升级到下一级所需经验值
	TRound				m_round;//游戏回合数
	int					m_level;//等级:从0开始                 *补充定义
	int					m_attackrange;//攻击范围               *补充定义
	int					m_productconsume;//当前生产任务对应得生产力消耗值	*补充定义

	TPlayerID			m_PlayerID;//所属阵营

	TPoint				m_position;//位置
	//根据等级更新塔的属性
	void set_all(int level) 
	{
		m_productpoint = TowerInitConfig[level].initBuildPoint;
		m_healthpoint = TowerInitConfig[level].initHealthPoint;
		m_battlepoint = TowerInitConfig[level].initProductPoint;
		m_upgradexper = TowerInitConfig[level].upgradeExper;
		m_attackrange = TowerInitConfig[level].battleRegion;
	}
	//每回合初始根据经验值判断是否升级
	bool set_level();
public:
	Tower(void);
	~Tower(void);
	Tower(TPlayerID ID, TPoint pos);
	//每一回合开始时，结算上轮生产任务完成情况，并设置新的生产任务
	bool set_producttype(productType m_protype);
    //抵御兵团的进攻
	/*
	待接收参数：
	己方兵团驻扎情况（计算战斗力增益）
	敌方兵团战斗力、兵种（计算两方生命值损失）
	*/
};
#endif