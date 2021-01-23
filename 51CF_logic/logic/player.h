#pragma once

#include "definition.h"
#include "cell.h"
#include "tentacle.h"
#include <set>
#include "data.h"
class Player
{
public:
	Player();
	Player(Player&); //#json
	~Player();
	//增加科技点数
	void setdata(DATA::Data* d) { data = d; }//设置数据Data
	void addTechPoint(TResourceD _techPoint);//增加科技点数
	void regenerateTechPoint();//科技点数恢复
	bool upgrade(TPlayerProperty kind);//玩家升级
	bool subTechPoint(TResourceD _techPoint);//扣除相应的科技点数，失败返回false
	int getDefenceLevel() const { return m_DefenceLevel; }
	bool upgradeDefence(); //升级防御，返回是否成功
	int getRegenerationLevel() const { return m_RegenerationLevel; }//获取玩家再生等级
	bool upgradeRegeneration();//更新玩家再生等级
	int getExtraControlLevel() const { return m_ExtraControlLevel; }//获取玩家额外控制数等级
	bool upgradeExtraControl();//更新玩家额外控制数等级
	int getMoveLevel() const { return m_MoveLevel; }//获取玩家移动等级
	bool upgradeMove();//更新玩家移动等级
	bool isAlive() { return alive; }//判断玩家是否活着
	int maxControlNumber();//获取最大命令条数
	TResourceD totalResource();//获取所有塔资源数的总和
	TResourceD techPoint() { return m_techPoint; }//获取玩家的科技点数
	std::set<TCellID>& cells() { return m_cells; }//获取玩家的所有塔set
	void Kill();//设置玩家死亡
	int getdeadRound() { return deadRound; }//读取死亡回合
	void setdeadRound(int d) { deadRound = d; }//设置死亡回合
private:
	bool alive = true;//是不是还活着
	int deadRound;//死掉的回合
	std::set<TCellID> m_cells;//当前细胞
	TResourceD m_techPoint;//科技点数
	int m_MoveLevel = 0;//移动技能等级
	int m_RegenerationLevel = 0;//再生技能等级
	int m_ExtraControlLevel = 0;//额外控制数技能等级
	int m_DefenceLevel = 0;//防御技能等级
	DATA::Data* data;//数据的存储与共享
};

