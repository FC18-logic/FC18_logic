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
	DATA::Data* const		m_data;

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
	TBattlePoint        m_battlepoint;//战斗力
	TExperPoint			m_experpoint;//经验值
	int					m_level;//等级:从1开始               
	int					m_attackrange;//攻击范围     

	//辅助属性
	TExperPoint			m_upgradexper;//升级到下一级所需经验值
	int					m_productconsume;//当前生产任务对应得生产力消耗值	
	vector <Crops*>    m_staycrops;//储存驻扎兵团指针
	
	/*辅助操作*/
	//根据等级更新塔的属性
	void set_all(int level);
	//每回合初始根据经验值判断是否升级
	bool set_level();
	//生产兵团
	void product_crops(productType protype);
public:
	Tower(void);
	~Tower(void);
	//塔构造函数
	Tower(DATA::Data* _data, TPlayerID m_playid, TPoint pos);

	/*塔操作*/
	//判断上一轮生产任务是否完成
	bool protask_finish()
	{
		if (m_productconsume <= 0)
			return true;
		return false;
	}
	//设置新生产任务
	void set_producttype(productType m_protype);
	//设置攻击目标
	bool set_attacktarget(int crop_id);


	/*信息获取*/
	//获取所属玩家ID
	const TPlayerID showPlayerID() { return m_PlayerID; }
	//获取塔位置
	const TPoint showPosition() { return m_position; }
	//获取塔等级
	int getLevel() { return m_level; }
	//获取塔当前是否存活（等级小于1即被摧毁）
	bool getexsit() { return m_exsit; }
	//获取驻扎兵团
	vector<Crops*> getCrops() { return m_staycrops; }
	//获取塔生产力
	TProductPoint getProductPoint() { return m_productpoint; }
	//获取塔生命值
	THealthPoint getHealthPoint() { return m_healthpoint; }
	//获取塔战斗力
	TBattlePoint getBattlePoint() { return m_battlepoint; }
	//获取塔经验值
	TExperPoint getExperPoint() { return m_experpoint; }
	//获取塔攻击范围
	int getExperPoint() { return m_attackrange; }


	/*提供防御塔信息*/
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
		if(protask_finish() == true)//没有生产任务:NOTASK=-1
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
	bool Be_Attacked(TPlayerID enemy_id, THealthPoint hp_decrease);
	//兵团驻扎信息录入 
	void input_staycrops(Crops* newcrop) { m_staycrops.push_back(newcrop); }
	//修理塔回复生命值
	void Recover();
};
#endif