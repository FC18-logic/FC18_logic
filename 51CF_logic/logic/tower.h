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
	//游戏信息
	DATA::Data* 		m_data;

	//塔信息
	static TTowerID		ID;
	TTowerID			m_id;//塔ID
	TPlayerID			m_PlayerID;//所属玩家ID
	bool				m_exsit;//塔是否存活：即等级是否大于1
	TPoint				m_position;//位置

	//塔属性
	TProductPoint       m_productpoint;//生产力
	productType			m_producttype;//生产任务类型
	THealthPoint        m_healthpoint;//生命值
	THealthPoint		M_healthpoint;//生命值上限
	TBattlePoint        m_battlepoint;//战斗力
	TExperPoint			m_experpoint;//经验值
	int					m_level;//等级:从1开始               
	int					m_attackrange;//攻击范围     

	//辅助属性
	TExperPoint			m_upgradexper;//升级到下一级所需经验值
	int					m_productconsume;//当前生产任务对应的生产力消耗值	
	int					task_cache[6];//缓存未完成任务进度：储存任务的生产力消耗值（若为0，则为完成状态）
public:
	Tower(void);
	~Tower(void);
	//塔构造函数
	Tower(DATA::Data* _data, TPlayerID m_playid, TPoint pos);
	//塔复制构造函数
	//Tower(const Tower& tower);

	/*塔操作*/
	//根据等级更新塔的属性
	void set_all(int level);
	//每回合初始根据经验值判断是否升级
	bool set_level();
	//设置【无任务】状态
	void set_notask() { task_cache[m_producttype] = 0; m_producttype = NOTASK; }
	//判断上一回合生产任务是否完成
	bool protask_finish()
	{
		if (m_productconsume <= 0)
			return true;
		return false;
	}
	//设置新生产任务
	bool set_producttype(productType m_protype);
	//清空生产任务的缓存进度
	void clear_cache(productType task_clear) { task_cache[task_clear] = 0; }
	//生产兵团
	void product_crops(productType protype);
	//塔升级
	void upgrade();
	//设置攻击目标
	bool set_attacktarget(int crop_id);
	



	/*信息获取*/
	//获取所属玩家ID
	const TPlayerID getPlayerID() { return m_PlayerID; }
	//获取塔位置
	const TPoint getPosition() { return m_position; }
	//获取塔等级
	int getLevel() { return m_level; }
	//获取塔当前是否存活（等级小于1即被摧毁）
	bool getexsit() { return m_exsit; }
	//获取塔生产力
	TProductPoint getProductPoint() { return m_productpoint; }
	//获取塔生产任务
	productType	getprotype() { return m_producttype; }
	//获取塔生命值
	THealthPoint getHealthPoint() { return m_healthpoint; }
	//获取塔战斗力
	TBattlePoint getBattlePoint() { return m_battlepoint; }
	//获取塔经验值
	TExperPoint getExperPoint() { return m_experpoint; }
	//获取塔攻击范围
	int getAttackRange() { return m_attackrange; }
	//获取塔ID
	TTowerID getTowerID() { return m_id; }

	/*提供防御塔信息*/
	TowerInfo ShowInfo() {
		TowerInfo info;
		info.exist = m_exsit;
		info.ID = m_id;
		info.ownerID = m_PlayerID;
		info.position = m_position;
		info.productPoint = m_productpoint;
		info.healthPoint = m_healthpoint;
		info.battlePoint = m_battlepoint;
		info.level = m_level;
		if(m_producttype == NOTASK)//没有生产任务:NOTASK=-1
		{
			info.pdtType = NOTASK;
			info.productConsume = -1;
		}
		else
		{
			info.pdtType = m_producttype;
			info.productConsume = m_productconsume;
		}
		return info;
	}


	/*对外接口*/
	//遭到兵团进攻时，计算考虑增益后塔的战斗力
	TBattlePoint get_towerbp();
	//抵御兵团的进攻，返回塔是否被攻陷
	bool Be_Attacked(TPlayerID enemy_id, THealthPoint hp_decrease, bool attackerAlive);
	//修理塔回复生命值
	void Recover();
};
#endif