//#2021-1-19 <JYP> 熟悉代码，添加注释
#include"map.h"
#include<fstream>
#include<iostream>
#include "data.h"
#include "cell.h"
#include "Crops.h"
#include "player.h"
#include "tentacle.h"
#include <utility>
#include <vector>
#include <map>
#include <algorithm>
using std::pair;
using std::vector;


bool Map::init(ifstream& inMap, TResourceI _MAX_RESOURCE_, bool enableOutput)  //通过文件流初始化信息
{

	//初始化地图高度、宽度
	inMap >> m_height;
	inMap >> m_width;
	//初始化地图上的障碍，FC18初始化地图上的塔和属于每个势力的地块
	int barrierNum;
	TPoint beginP, endP;
	TBarrier _barrier;
	inMap >> barrierNum;

	if (enableOutput)
		cout << "init map......" << endl;
	

	//rankInfo
	//更新当前回合的排名信息JSON
	Json::Value rankInfoJson;
	for (int i = 1; i < 5; i++) rankInfoJson["rank"].append(i);
	rankInfoJson["rank"].append(0);
	for (int i = 0; i < 4; i++) rankInfoJson["resources"].append(40);	
	rankInfoJson["resources"].append(50);
	data->currentRoundJson["rankInfo"] = rankInfoJson;


	//初始化地图上障碍物的信息JSON
	Json::Value barrierAdditionJson;
	for (int i = 0; i < barrierNum; i++)
	{
		inMap >> beginP.m_x;
		inMap >> beginP.m_y;
		//beginP.m_state = Barrier;
		inMap >> endP.m_x;
		inMap >> endP.m_y;
		//endP.m_state = Barrier;

		_barrier.m_beginPoint = beginP;
		_barrier.m_endPoint = endP;
		m_barrier.push_back(_barrier);

		//#json		
		barrierAdditionJson["type"] = Json::Value(uint8_t(1));
		barrierAdditionJson["id"] = Json::Value(i);
		Json::Value startPointJson;
		startPointJson["x"] = Json::Value(beginP.m_x);
		startPointJson["y"] = Json::Value(beginP.m_y);
		barrierAdditionJson["startPoint"] = Json::Value(startPointJson);
		Json::Value endPointJson;
		endPointJson["x"] = Json::Value(endP.m_x);
		endPointJson["y"] = Json::Value(endP.m_y);
		barrierAdditionJson["endPoint"] = Json::Value(endPointJson);
	}
	data->currentRoundJson["barrierActions"].append(barrierAdditionJson);


	//初始化阵营
	if (enableOutput)
		cout << "init team......" << endl;
	inMap >> data->PlayerNum;
	data->root["head"]["totalPlayers"] = data->PlayerNum; //#json
	data->players = new Player[data->PlayerNum];

	//#json add
	//初始化玩家信息JSON
	//初始化玩家操作JSON
	Json::Value playerInfoJson;  //#json
	Json::Value playerActionJson; //
	for (int i = 0; i != data->PlayerNum; ++i)
	{
		data->players[i].setdata(data);
		Json::Value pIJ;
		pIJ["id"] = Json::Value(i + 1);
		pIJ["team"] = Json::Value(i + 1);
		playerInfoJson.append(pIJ);

		Json::Value paj;
		paj["id"] = i + 1;
		paj["type"] = 1;
		paj["rSS"] = 0;
		paj["sS"] = 0;
		paj["eCS"] = 0;
		paj["dS"] = 0;
		data->currentRoundJson["playerAction"].append(paj);
	}
	data->root["head"]["playerInfo"] = playerInfoJson;


	//初始化塔
	if (enableOutput)
		cout << "init towers......" << endl;
	inMap >> data->CellNum;
	data->cells = new Cell[data->CellNum];
	//防御塔的属性都列在下面
	TCellID _id;
	TPoint _point;   //位置
	TPlayerID _camp; //阵营
	TResourceD _resource; //资源值
	TPower _techPower; //科技值系数
	TResourceD _maxResource; //最大资源
	for (int i = 0; i < data->CellNum; i++)
	{
		inMap >> _point.m_x;
		inMap >> _point.m_y;
		//设置塔的位置
		m_studentPos.push_back(_point);

		//从配置中读入某一个塔的每个属性值
		inMap >> _id >> _camp >> _resource >> _techPower >> _maxResource;
		//非过渡状态的塔
		if (_camp != Neutral)
		{
			data->cells[i].init(_id, data, _point, _camp, _resource, _maxResource, _techPower);
			data->players[_camp].cells().insert(i); //势力防御塔集合
		}
		else
		{
			data->cells[i].init(_id, data, _point, _camp, _resource, _maxResource, _techPower);
		}

		//#json
		Json::Value cellAdditionJson;
		cellAdditionJson["type"] = 1;
		cellAdditionJson["id"] = Json::Value(_id);
		cellAdditionJson["team"] = Json::Value(_camp + 1);
		cellAdditionJson["size"] = Json::Value(float(float(sqrt(_resource)*4 + 10)));
		cellAdditionJson["level"] = Json::Value(data->cells[_id].getCellType());
		Json::Value birthPositionJson;
		birthPositionJson["x"] = _point.m_x;
		birthPositionJson["y"] = _point.m_y;
		cellAdditionJson["resources"] = _resource;  //#jsonChange_3_9
		cellAdditionJson["birthPosition"] = birthPositionJson;
		cellAdditionJson["techVal"] = int(data->cells[i].techRegenerateSpeed());
		cellAdditionJson["strategy"] = Json::Value(Normal);
		data->currentRoundJson["cellActions"].append(cellAdditionJson);
	}
	return true;
}

//这个函数看起来没什么用
bool Map::init(const TMapID& filename,TResourceI _MAX_RESOURCE_)
{
	bool ret;
	setID(filename);
	ifstream inMap(filename, ios_base::binary);
	if (!inMap)
	{
		cerr << "can't open the map file" << endl;
		return false;
	}
	ret = init(filename,_MAX_RESOURCE_);
	inMap.close();
	return ret;
}

/***********************************************************************************************
*函数名 :【FC18】randomInitMap随机地图产生器
*函数功能描述 : 初始化确定地图长、宽之后，为每个地图方格分配地形，划分各势力的初始领土，分配塔
                的初始位置
*函数参数 : 无
*函数返回值 : false--随机地图初始化失败，true--随机地图初始化成功
*作者 : 姜永鹏
***********************************************************************************************/
bool Map::randomInitMap() {
	//【FC18】利用柏林噪声方法产生随机地图
	Perlin perlinNoiseGen;
	int terrainArea[TERRAIN_TYPE_NUM] = { 0 };      //【FC18】统计每种地形生成了多少格
	double** perlinNoise = new double* [m_height];  //【FC18】柏林噪声表
	for (int i = 0; i < m_height; i++) {
		perlinNoise[i] = new double[m_width];
	}
	int** typeOfTerrain = new int* [m_height];      //【FC18】地形预分配表
	for (int i = 0; i < m_height; i++) {
		typeOfTerrain[i] = new int[m_width];
	}

	double minNoise = 1, maxNoise = -1;
	for (int i = 0; i < m_height; i++) {             //生成柏林噪声表，并记录其中的最大、最小噪声值
		for (int j = 0; j < m_width; j++) {
			typeOfTerrain[i][j] = UNALLOCATED;       //初始化所有格地形均为未分配
			perlinNoise[i][j] = perlinNoiseGen.PerlinNoise(i, j);            //生成初始的柏林噪声
			if (perlinNoise[i][j] > maxNoise) maxNoise = perlinNoise[i][j];
			if (perlinNoise[i][j] < minNoise) minNoise = perlinNoise[i][j];
		}
	}
	double interval = (maxNoise - minNoise) / TERRAIN_TYPE_NUM;
	for (int i = 0; i < m_height; i++) {
		for (int j = 0; j < m_width; j++) {
			for (int k = 0; k < TERRAIN_TYPE_NUM; k++) {
				if (((perlinNoise[i][j] - minNoise) >= k * interval) && ((perlinNoise[i][j] - minNoise) < (k + 1) * interval)) {
					typeOfTerrain[i][j] = k;
					break;
				}
			}
			if (typeOfTerrain[i][j] == UNALLOCATED) typeOfTerrain[i][j] = TERRAIN_TYPE_NUM - 1;
		}
	}
	for (int itercnt = 0; itercnt < perlinNoiseGen.iterRound; itercnt++) {
		for (int i = 1; i < m_height - 1; i++) {
			for (int j = 1; j < m_width - 1; j++) {
				int nums[TERRAIN_TYPE_NUM] = { 0 };
				for (int k = 0; k < 8; k++) {
					int newPosX = j + paraOffset[k].m_x;
					int newPosY = i + paraOffset[k].m_y;
					int scaleValue = typeOfTerrain[newPosY][newPosX];
					nums[scaleValue]++;
				}
				for (int cnt = 0; cnt < TERRAIN_TYPE_NUM; cnt++) {
					if (nums[cnt] >= perlinNoiseGen.connectStandard) typeOfTerrain[i][j] = cnt;
				}
				if (itercnt == perlinNoiseGen.iterRound - 1) {
					int currentType = typeOfTerrain[i][j];
					terrainArea[currentType]++;
				}
			}
		}
	}
	vector<pair<int, int>> areaRankPair;
	for (int i = 0; i < TERRAIN_TYPE_NUM; i++) {
		areaRankPair.push_back({i,terrainArea[i]});
	}
	std::sort(areaRankPair.begin(),areaRankPair.end(),
		[](const pair<int, int>& a, pair<int, int>& b) {return a.second > b.second; });
	std::map<int, int> areaRankMap;
	for (int i = 0; i < TERRAIN_TYPE_NUM; i++) {
		areaRankMap.insert(pair<int, int>(areaRankPair[i].first, i));
	}
	for (int i = 0; i < m_height; i++) {
		vector<mapBlock> newVectorMapBlock;
		map.push_back(newVectorMapBlock);
		for (int j = 0; j < m_width; j++) {
			mapBlock newBlock;
			map[i].push_back(newBlock);
		}
	}
	for (int i = 0; i < m_height; i++) {
		for (int j = 0; j < m_width; j++) {
			int type = typeOfTerrain[i][j];
			int rank = areaRankMap[type];
			map[i][j].type = terrain[rank];
		}
	}
	for (int i = 0; i < m_height; i++) {
		delete[] perlinNoise[i];
		delete[] typeOfTerrain[i];
	}
	delete[] perlinNoise;
	delete[] typeOfTerrain;

	////////////////////////////////////////////////////////
	//【FC18】分配各势力的初始领土，占据方形地图的四个角落//
	//地图的样子                                          //
	//      X(i=0)--------m_width---------                //
	//Y(j=0)         0     PUB      1                     //
	//  |           PUB    PUB     PUB                    //
	//  |            3     PUB      2                     //
	////////////////////////////////////////////////////////
	int Xinterval, Yinterval;         //X方向和Y方向地图的公共区域条带宽度
	Xinterval = (m_width % 2 == 0) ? 2 : 3;
	Yinterval = (m_height % 2 == 0) ? 2 : 3;
	for (int j = 0; j < m_height; j++) {
		for (int i = 0; i < m_width; i++) {
			if ((i < ((m_width - Xinterval) / 2)) && (j < ((m_height - Yinterval) / 2))) map[j][i].owner = 0;
			else if ((i > ((m_width + Xinterval) / 2) - 1) && (j < ((m_height - Yinterval) / 2))) map[j][i].owner = 1;
			else if ((i > ((m_width + Xinterval) / 2) - 1) && (j > ((m_height + Yinterval) / 2) - 1)) map[j][i].owner = 2;
			else if ((i < ((m_width - Xinterval) / 2)) && (j > ((m_height + Yinterval) / 2 - 1))) map[j][i].owner = 3;
			else map[j][i].owner = PUBLIC;
		}
	}

	//【FC18】为每个势力生成防御塔
	if (((m_width - Xinterval) < 3 * 2) || ((m_height - Yinterval) < 3 * 2)) {      //判断是否有空间生成防御塔
		cout << "map size: (" << m_width << "*" << m_height << ") too small!\n";
		return false;
	}
	
	TPoint towerPoint;         //随机防御塔坐标
	towerPoint.m_x = generateRanInt(1,(m_width - Xinterval) / 2 -2);
	towerPoint.m_y = generateRanInt(1, (m_height - Yinterval) / 2 - 2);
	map[towerPoint.m_y][towerPoint.m_x].type = Tower;
	for (int i = 0; i < 8; i++) {
		TPoint p;
		p.m_x = towerPoint.m_x + paraOffset[i].m_x;
		p.m_y = towerPoint.m_y + paraOffset[i].m_y;
		map[p.m_y][p.m_x].type = Road;
	}

	towerPoint.m_x = generateRanInt((m_width + Xinterval) / 2 + 1, m_width - 2);
	towerPoint.m_y = generateRanInt(1, (m_height - Yinterval) / 2 - 2);
	map[towerPoint.m_y][towerPoint.m_x].type = Tower;
	for (int i = 0; i < 8; i++) {
		TPoint p;
		p.m_x = towerPoint.m_x + paraOffset[i].m_x;
		p.m_y = towerPoint.m_y + paraOffset[i].m_y;
		map[p.m_y][p.m_x].type = Road;
	}

	towerPoint.m_x = generateRanInt((m_width + Xinterval) / 2 + 1, m_width - 2);
	towerPoint.m_y = generateRanInt((m_height + Yinterval) / 2 + 1, m_height - 2);
	map[towerPoint.m_y][towerPoint.m_x].type = Tower;
	for (int i = 0; i < 8; i++) {
		TPoint p;
		p.m_x = towerPoint.m_x + paraOffset[i].m_x;
		p.m_y = towerPoint.m_y + paraOffset[i].m_y;
		map[p.m_y][p.m_x].type = Road;
	}

	towerPoint.m_x = generateRanInt(1, (m_width - Xinterval) / 2 - 2);
	towerPoint.m_y = generateRanInt((m_height + Yinterval) / 2 + 1, m_height - 2);
	map[towerPoint.m_y][towerPoint.m_x].type = Tower;
	for (int i = 0; i < 8; i++) {
		TPoint p;
		p.m_x = towerPoint.m_x + paraOffset[i].m_x;
		p.m_y = towerPoint.m_y + paraOffset[i].m_y;
		map[p.m_y][p.m_x].type = Road;
	}

	return true;
}
