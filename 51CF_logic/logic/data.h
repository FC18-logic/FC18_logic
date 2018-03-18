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
		Cell* cells;
		int CellNum;

		Tentacle*** tentacles;
		int TentacleNum;

		Player* players;
		int PlayerNum;

		Json::Value root;
		Json::Value currentRoundJson;
		vector<vector<CutTentacleInfoJson>>  cutTentacleInfoJson;
		vector<vector<bool>> cutTentacleJson;  //触手切断标志
		vector<vector<bool>> cutTentacleBornJson;  // 断触手生成标志

		Map gameMap;
	};
}

#endif // !_DATA_H