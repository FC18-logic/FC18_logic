//#pragma once
#ifndef _DATA_H
#define _DATA_H
/*
**  Data结构体
**  用于数据的存储和共享
**  在外部存储相关类只需存储 ID 在 Data 中调用
*/
class Cell;
class Tentacle;
class Player;


#include "map.h"
#include "../json/json.h"

struct CutTentacleInfoJson
{
	int cutID = -1;
	double frontResource = -1;
};

namespace DATA//名空间
{
	struct Data
	{
		Cell* cells;//所有塔的指针
		int CellNum;//塔数

		Tentacle*** tentacles;//所有兵线的指针（兵线是由一个二位数组存的，每个兵线就是i->j）
		int TentacleNum;//兵线数

		Player* players;//所有玩家的指针
		int PlayerNum;//玩家数

		Json::Value root;//JSON根节点
		Json::Value currentRoundJson;//当前回合JSON的根节点
		vector<vector<CutTentacleInfoJson>>  cutTentacleInfoJson;//二维数组存储每条兵线的切断信息
		vector<vector<bool>> cutTentacleJson;  //二维数组每条兵线切断标志
		vector<vector<bool>> cutTentacleBornJson;  // 二维数组每条兵线生成标志

		Map gameMap;//当前的地图
	};
}

#endif // !_DATA_H