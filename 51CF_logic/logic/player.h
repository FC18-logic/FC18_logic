#pragma once

#include "definition.h"
#include "cell.h"
#include "tentacle.h"
#include "Crops.h"
#include "tower.h"
#include <set>
#include "data.h"
class Player                                                                //@@@【FC18】玩家类
{
public:
	Player();                                                               //@@@【FC18】玩家类默认构造函数
	Player(Player&);                                                        //@@@【FC18】玩家类复制构造函数   #json
	~Player();                                                              //【FC18】玩家类析构函数
	//增加科技点数
	void setdata(DATA::Data* d) { data = d; }                               //【FC18】设置更新数据Data
	bool isAlive() { return alive; }                                        //【FC18】判断玩家是否活着
	int maxControlNumber();                                                 //@@@【FC18】（看规则怎么改，不知道用不用）获取最大命令条数
	void Kill();                                                            //【FC18】设置玩家死亡
	int getdeadRound() { return deadRound; }                                //【FC18】读取死亡回合
	void setdeadRound(int d) { deadRound = d; }                             //【FC18】设置死亡回合
	//@@@【FC18】获取玩家当前得分（按照规则来，建议考虑到同名次时按什么指标来比较等）
	int getPlayerScore();
	void setID(TPlayerID id) { m_id = id; }
	TPlayerID getId() { return m_id; }                                      //【FC18】获取玩家ID
	int getCqTowerNum() { return conqueTowerNum; }                          //【FC18】获取玩家占领敌方防御塔数
	int getElCorpsNum() { return eliminateCorpsNum; }                       //【FC18】获取玩家消灭敌方兵团数
	int getCqCorpsNum() { return captureCorpsNum; }                         //【FC18】获取玩家俘虏地方兵团数
	void setCqTowerNum(int num) { conqueTowerNum = num; }                   //【FC18】设置玩家占领敌方防御塔数
	void setElCorpsNum(int num) { eliminateCorpsNum = num; }                //【FC18】设置玩家消灭敌方兵团数
	void setCqCorpsNum(int num) { captureCorpsNum = num; }                  //【FC18】设置玩家俘虏地方兵团数
	//【FC18】获取玩家的所有防御塔set
	std::set<TTowerID>& getTower() { return m_tower; }

	//【FC18】获取玩家的所有兵团的set
	std::set<TCorpsID>& getCrops() { return m_crops; }

	//【FC18】添加兵团
	void addCrops(TCorpsID ID);
	//【FC18】删除兵团
	void deleteCrops(TCorpsID ID);
	//【FC18】获取玩家排名
	int getRank() { return rank; }
	//【FC18】设置玩家排名
	void setRank(int Rank) { rank = Rank; }
	//【FC18】获取玩家姓名
	string getName() { return name; }
	//【FC18】设置玩家姓名
	void setName(string Name) { name = Name; }
	//【FC18】获取玩家积分
	TScore getScore() { return score; }
	//【FC18】设置玩家积分
	void setScore(TScore Score) { score = Score; }

	//FC15的
	TResourceD totalResource();//获取所有塔资源数的总和
	TResourceD techPoint() { return m_techPoint; }//获取玩家的科技点数
	std::set<TCellID>& cells() { return m_cells; }//获取玩家的所有细胞set
	void addTechPoint(TResourceD _techPoint);//增加科技点数
	void regenerateTechPoint();//科技点数恢复
	bool upgrade(TPlayerProperty kind);//玩家升级
	bool subTechPoint(TResourceD _techPoint);//扣除相应的科技点数，失败返回false
	int getDefenceLevel() const { return m_DefenceLevel; }//获取玩家防御等级
	bool upgradeDefence(); //升级防御，返回是否成功
	int getRegenerationLevel() const { return m_RegenerationLevel; }//获取玩家再生等级
	bool upgradeRegeneration();//更新玩家再生等级
	int getExtraControlLevel() const { return m_ExtraControlLevel; }//获取玩家额外控制数等级
	bool upgradeExtraControl();//更新玩家额外控制数等级
	int getMoveLevel() const { return m_MoveLevel; }//获取玩家移动等级
	bool upgradeMove();//更新玩家移动等级

private:
	bool alive = true;                                                      //【FC18】是不是还活着
	int deadRound;                                                          //【FC18】死掉的回合
	DATA::Data* data;                                                       //【FC18】数据的存储与共享
	TPlayerID m_id;                                                         //【FC18】玩家ID
	string name;                                                            //【FC18】玩家的姓名
	int conqueTowerNum;                                                     //【FC18】玩家占领防御塔数
	int eliminateCorpsNum;                                                  //【FC18】玩家消灭敌军兵团数量
	int captureCorpsNum;                                                    //【FC18】被俘虏的兵团数
	//@@@【FC18】玩家俘虏敌军兵团数量
	//@@@【FC18】指向当前玩家所有防御塔的set数据结构，参考原来的std::set<TCellID> m_cells
	std::set<TTowerID> m_tower;
	
	//@@@【FC18】玩家所有兵团的序号，建议也用set这种数据结构，内部按兵团序号升序来排序
	std::set<TCorpsID> m_crops;//由兵团类内部修改
	
	int rank;   //【FC18】玩家排名
	int score;  //【FC18】玩家积分
	//FC15的
	std::set<TCellID> m_cells;//当前细胞
	TResourceD m_techPoint;//科技点数
	int m_MoveLevel = 0;//移动技能等级
	int m_RegenerationLevel = 0;//再生技能等级
	int m_ExtraControlLevel = 0;//额外控制数技能等级
	int m_DefenceLevel = 0;//防御技能等级
};

