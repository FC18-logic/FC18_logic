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

//@@@【FC18】兵团类，这里是声明，定义在类的头文件里，参考原来的class Tentacle

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
		//@@@【FC18】指向所有防御塔（数组/vector均可）的指针，可以参考原来的Cell* cells
		Cell* cells;//所有塔的指针
		
		TCorps totalCorps;                             //【FC18】总的兵团数
		//@@@【FC18】指向所有兵团（数组/vector均可）的指针，可以参考原来的Tentacle*** tentacles
		Tentacle*** tentacles;//所有兵线的指针（兵线是由一个二维数组存的，每个兵线就是i->j）
		
		TPlayer totalPlayers;                          //【FC18】玩家数
		//@@@【FC18】指向所有玩家（数组/vector均可）的指针，可以参考原来的Player* players
		Player* players;//所有玩家的指针

		Map gameMap;                                   //@@@【FC18】当前的地图

		//关于写JSON文档
		Json::Value root;                              //【FC18】JSON根节点
		Json::Value currentRoundJson;                  //【FC18】当前回合JSON的根节点，saveJson()的时候插到总Json的根节点下


		//FC15的
		int CellNum;//塔数
		int TentacleNum;//兵线数
		TPlayer PlayerNum;//玩家数
		vector<vector<CutTentacleInfoJson>>  cutTentacleInfoJson;//二维数组存储每条兵线的切断信息
		vector<vector<bool>> cutTentacleJson;  //二维数组每条兵线切断标志
		vector<vector<bool>> cutTentacleBornJson;  // 二维数组每条兵线生成标志
	};
}

#endif // !_DATA_H