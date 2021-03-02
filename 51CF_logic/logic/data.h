//#pragma once
#ifndef _DATA_H
#define _DATA_H
/*
**  Data结构体
**  用于数据的存储和共享
**  在外部存储相关类只需存储 ID 在 Data 中调用
*/
//以下来自FC15，我们应该用不到了
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Cell;//原来的细胞类
class Tentacle;//原来的触手类
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//@@@【FC18】防御塔类，这里是声明，定义在类的头文件里，参考原来的class Cell

//【FC18】兵团类，这里是声明，定义在类的头文件里，参考原来的class Tentacle
class Crops;//【FC18】兵团类
class Tower;//【FC18】防御塔类
class Player;//【FC18】玩家类


#include "map.h"
#include "../json/json.h"

struct CutTentacleInfoJson
{
	int cutID = -1;
	double frontResource = -1;
};

namespace DATA                                         //@@@【FC18】名空间
{
	struct Data                                        //@@@【FC18】Data结构体，用于在内存中进行类与类的数据共享、存储
	{
		TTower totalTowers;                            //【FC18】总的防御塔数
		//【FC18】指向所有防御塔（数组/vector均可）的指针，可以参考原来的Cell* cells
		vector<Tower> myTowers;                  //【FC18】所有防御塔的向量
		
		
		TCorps totalCorps;                             //【FC18】总的兵团数
		vector<Crops> myCorps;                         //【FC18】所有兵团按序号排布
		
		
		TPlayer totalPlayers;                          //【FC18】玩家数
		//@@@【FC18】指向所有玩家（数组/vector均可）的指针，可以参考原来的Player* players
		Player* players;//所有玩家的指针

		Map gameMap;                                   //@@@【FC18】当前的地图
		TRound totalRounds;                            //【FC18】当前的回合数


		set<TTowerID> newTower;                //【FC18】每回合的新塔
		set<TTowerID> dieTower;                //【FC18】每回合被消灭的塔
		set<TCorpsID> newCorps;                //【FC18】每回合的新兵团
		set<TCorpsID> dieCorps;                //【FC18】每回合被消灭的兵团
		set<TTowerID> changeTowers;            //【FC18】属性值改变需要记录的塔编号
		set<TCorpsID> changeCorps;             //【FC18】属性值改变需要记录的兵团编号


		//关于写JSON文档
		//Json::Value commandJsonRoot;                   //【FC18】所有回合命令的Json根节点
		//Json::Value infoJsonRoot;                      //【FC18】所有回合玩家、防御塔和兵团Json的根节点
		//Json::Value mapInfoJsonRoot;                   //【FC18】所有回合地图信息的Json根节点
		//Json::Value currentRoundCommandJson;           //【FC18】当前回合命令的Json根节点
		//Json::Value currentRoundPlayerJson;            //【FC18】当前回合玩家的Json根节点
		//Json::Value currentRoundTowerJson;             //【FC18】当前回合防御塔的Json根节点
		//Json::Value currentRoundCorpsJson;             //【FC18】当前回合兵团的Json根节点
		Json::Value currentRoundMapJson;               //【FC18】当前回合地图的Json根节点
		Json::Value lastRoundMapJson;                  //【FC18】记录上一回合地图的Json根节点

		void resetAllCnt() { players = nullptr, totalRounds = 0; totalTowers = 0; totalCorps = 0; totalPlayers = 0; }//[【FC18】data中几个关键的数据全部清零
		TRound getRound() { return totalRounds; }      //【FC18】获取当前data正在记录第几回合的数据（小回合）
		TRound addRound() { totalRounds++; return totalRounds; }             //【FC18】data回合数增加，在上一回合Json数据清空，每个玩家ai代码开始跑之前（小回合）
		TTower getTotalTower() { return totalTowers; } //【FC18】获取当前data记录的总防御塔数
		TCorps getTotalCorp() { return totalCorps; }   //【FC18】获取当前data记录的总兵团数
		vector<CommandList> memoCommand;               //【FC18]从指令存档恢复数据时的指令记录           
		int gameState;                                 //【FC18】当前游戏模式


		//FC15的
		int CellNum;//塔数
		int TentacleNum;//兵线数
		TPlayer PlayerNum;//玩家数
		Cell* cells;//所有塔的指针
		vector<vector<CutTentacleInfoJson>>  cutTentacleInfoJson;//二维数组存储每条兵线的切断信息
		vector<vector<bool>> cutTentacleJson;  //二维数组每条兵线切断标志
		vector<vector<bool>> cutTentacleBornJson;  // 二维数组每条兵线生成标志
		Tentacle*** tentacles;//【FC15】所有兵线的指针（兵线是由一个二维数组存的，每个兵线就是i->j）
		Json::Value root;                              //JSON根节点
		Json::Value currentRoundJson;                  //当前回合JSON的根节点，saveJson()的时候插到总Json的根节点下
	};
}

#endif // !_DATA_H