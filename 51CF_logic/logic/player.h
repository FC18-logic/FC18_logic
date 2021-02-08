#pragma once

#include "definition.h"
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
	DATA::Data* getdata() { return data; }
	bool isAlive() { return alive; }                                        //【FC18】判断玩家是否活着
	//int maxControlNumber();                                                 //@@@【FC18】（看规则怎么改，不知道用不用）获取最大命令条数
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
	bool towerNumControl();                                      //【FC18】玩家防御塔数目检查
	bool battleNumControl();                                      //【FC18】玩家作战兵团数目检查
	bool constructNumControl();                                      //【FC18】玩家工程兵团数目检查
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
};

