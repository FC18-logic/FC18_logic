#include "game.h"
#include <set>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <cmath> 
#include <math.h>
#include <stdio.h>
#include <Windows.h>
#include <iomanip>

struct configCorpsTool
{
	int player = -1;
	int type = -1;
	int builder_point = -1;
	int movePoint = -1;
	int battlePoint = -1;
	int healthPoint = -1;
	int newC = -1;
};

struct configTowerTool
{
	int player = -1;
	int buildPoint = -1;
	int type = -1;
	int buildCost = -1;
	int healthPoint = -1;
	int battlePoint = -1;
	int starLevel = -1;
	int newT = -1;
};

bool Game::init(string file, string json_file, vector<string> players_name, std::ofstream& cmdF, std::ofstream& infoF)   //[【FC18】接着用，改好了
{
	//【FC18】TXT文件名
	//#json
	roundTime.push_back(clock());
	/*Json::Value Head;     //【FC18】
	Json::Value Body;     //【FC18】
	Json::Value List1;    //【FC18】
	Json::Value List2;    //【FC18】*/
	//data.commandJsonRoot["head"] = Head;   //【FC18】
	//data.commandJsonRoot["body"] = Body;   //【FC18】
	//data.infoJsonRoot["body"]    = Body;   //【FC18】
	/*data.mapInfoJsonRoot["head"] = Head;   //【FC18】
	data.mapInfoJsonRoot["map_logList"] = Body;   //【FC18】
	data.commandJsonRoot["cmd_logList"] = List1;   //【FC18】
	data.infoJsonRoot["info_logList"] = List2;    //【FC18】
	data.currentRoundCommandJson["round"] = Json::Value(std::to_string(0));   //【FC18】*/
	//data.currentRoundCorpsJson["round"]   = Json::Value(std::to_string(0));   //【FC18】
	//data.currentRoundTowerJson["round"]   = Json::Value(std::to_string(0));   //【FC18】
	//data.currentRoundPlayerJson["round"]  = Json::Value(std::to_string(0));   //【FC18】
	//data.currentRoundMapJson["round"]     = Json::Value(std::to_string(0));   //【FC18】



	/****************************FC15的旧代码****************************
	Json::Value head;
	Json::Value body;
	data.root["body"] = body;
	data.root["head"] = head;
	data.currentRoundJson["currentRound"] = Json::Value(0);
	data.currentRoundJson["runDuration"] = Json::Value(20);
	data.currentRoundJson["cellActions"];
	data.currentRoundJson["tentacleActions"];
	data.currentRoundJson["cutTentacleActions"];
	data.currentRoundJson["barrierActions"];
	*********************************************************************/

	data.gameMap.setData(&data);
	ifstream in(file);
	if (!in)
	{
		cerr << "can't open the map file" << endl;
		return false;
	}

	//地图文件读入最大资源数和最大回合数
	//【FC18】读入地图文件并初始化
	//读入地图宽、高，游戏中玩家数量
	//写入第一回合玩家命令Json、塔和地图Json，以及玩家Json
	
	//旧代码//  in >> _MAX_RESOURCE_ >> _MAX_ROUND_;

	if (!data.gameMap.readMap(in, cmdF,infoF,true, players_name))                       //#Json 读入地图，写地图Json文件头，写0回合玩家指令Json
	{
		cerr << "Something wrong when reading the map file." << endl;
		return false;
	}
	if (!data.gameMap.randomInitMap(cmdF,infoF))                         //#Json 保存塔和地图的Json
	{
		cerr << "Something wrong when randomizing the game map." << endl;
		return false;
	}

	printGameMap();

	in.close();
	totalRounds = 0;
	totalPlayers = playerAlive = data.totalPlayers;

	//初始化排名
	for (int i = 0; i < totalPlayers; i++) {
		Rank.push_back(i + 1);  //只push这一次，后面再push就多了
	}


	//data.commandJsonRoot["head"]["totalRounds"] = Json::Value(std::to_string(totalRounds));  //初始化为0回合
	//data.commandJsonRoot["body"].append(data.currentRoundCommandJson);
	/*data.commandJsonRoot["cmd_logList"].append(data.currentRoundCommandJson);
	data.currentRoundCommandJson.clear();*/


	data.gameMap.saveMapJson();
	

	/*Json::Value roundInfo;
	roundInfo["round"] = Json::Value(std::to_string(data.getRound()));
	//data.infoJsonRoot["body"]["corpsInfo"].append(data.currentRoundCorpsJson);
	roundInfo["corps"] = data.currentRoundCorpsJson;
	data.currentRoundCorpsJson.clear();
	//data.infoJsonRoot["body"]["towerInfo"].append(data.currentRoundTowerJson);
	roundInfo["tower"] = data.currentRoundTowerJson;
	data.currentRoundTowerJson.clear();
	//data.infoJsonRoot["body"]["playerInfo"].append(data.currentRoundPlayerJson);
	roundInfo["player"] = data.currentRoundPlayerJson;
	data.currentRoundPlayerJson.clear();
	data.infoJsonRoot["info_logList"].append(roundInfo);*/

	/*//输出到文件 #json 
	Json::FastWriter sw_cmd,sw_info,sw_map;
	ofstream json_cmd,json_info,json_map;
	//写入指令Json
	json_cmd.open(cmd_json_filename);
	//旧代码//json_os << sw.write(data.root);
	json_cmd << sw_cmd.write(data.commandJsonRoot);
	json_cmd.close();
	//写入信息Json
	json_info.open(info_json_filename);
	json_info << sw_info.write(data.infoJsonRoot);
	json_info.close();
	//写入地图Json
	json_map.open(mapinfo_json_filename);
	json_map << sw_map.write(data.mapInfoJsonRoot);
	json_map.close();*/
	return true;
}

void Game::DebugPhase()
{
	cout << "*************** DEBUG 信息 ***************" << endl;
	cout << "Round " << data.getRound() << " " << std::ceil(data.getRound() / 4.0) << endl;
	cout << "玩家剩余： " << playerAlive << " / " << totalPlayers << endl;
	cout << "玩家信息：" << endl;
	for (int i = 0; i < data.totalPlayers; ++i)
	{
		cout << "玩家 " << i + 1 << " ：姓名：" << data.players[i].getName() << " 占领塔数： " << data.players[i].getCqTowerNum() << " 杀兵团数： " << data.players[i].getElCorpsNum() << " 俘兵团数： "
			<< data.players[i].getCqCorpsNum() << " 存活：" << ((data.players[i].isAlive())?"活":"死") << " 死亡回合：" << ((data.players[i].isAlive()) ? 0 : data.players[i].getdeadRound()) << endl;
		cout << "当前塔：";
		for (TTowerID u : data.players[i].getTower())   //这个for循环遍历i号玩家所有塔的信息，塔用u来存储
		{
			if (!data.myTowers[u].getexsit()) continue;
			cout << u << " ";  //塔的序号
		}
		cout << endl;
		cout << "当前兵团：";
		for (TCorpsID u : data.players[i].getCrops())   //这个for循环遍历i号玩家所有塔的信息，塔用u来存储
		{
			if (!data.myCorps[u].bAlive()) continue;
			cout << u << " ";  //兵团序号
		}
		cout << "得分：" << data.players[i].getScore() << endl;
		cout << "排名：" << data.players[i].getRank();
		cout << endl;
	}
	cout << "塔信息" << endl;
	for (int i = 0; i < data.myTowers.size(); i++) {
		if (!data.myTowers[i].getexsit()) continue;
		cout << "塔 " << i << " ： " << "主人：" << data.myTowers[i].getPlayerID() << " 位于：(" << data.myTowers[i].getPosition().m_x << "," << data.myTowers[i].getPosition().m_y
			<< ") 等级：" << data.myTowers[i].getLevel() << " 生产力：" << data.myTowers[i].getProductPoint() << " 战斗力：" << data.myTowers[i].getBattlePoint() << " 生命值：" << data.myTowers[i].getHealthPoint()
			<< " 生产：" << ((data.myTowers[i].ShowInfo().pdtType >= 0 && data.myTowers[i].ShowInfo().pdtType <= 5) ? ProductCmd[data.myTowers[i].ShowInfo().pdtType] : "N/A")
			<< " 任务消耗：" << ((data.myTowers[i].ShowInfo().pdtType >= 0 && data.myTowers[i].ShowInfo().pdtType <= 5) ? data.myTowers[i].ShowInfo().productConsume:0) << endl;
		for (TCorpsID u : data.gameMap.
				map[data.myTowers[i].getPosition().m_y][data.myTowers[i].getPosition().m_x].corps) {
			if (!data.myCorps[u].bAlive()) continue;
			cout <<u << " "; //兵团序号
		}
		cout << endl;
	}
	cout << "兵团信息" << endl;
	for (int i = 0; i < data.myCorps.size(); i++) {
		if (!data.myCorps[i].bAlive()) continue;
		CorpsInfo currentCrops = data.myCorps[i].ShowInfo();
		cout << "兵团 " << i << " ： " << "主人：" << currentCrops.owner << " 位于：(" << currentCrops.pos.m_x << "," << currentCrops.pos.m_y << " ) 移动力：" << currentCrops.movePoint;
		if (currentCrops.type == Battle) {
			cout << " 类型：作战兵团 + " << BattleName[currentCrops.m_BattleType] << " 生命值：" << currentCrops.HealthPoint;
		}
		else if (currentCrops.type == Construct) {
			cout << " 类型：工程兵团 + " << ConstructName[currentCrops.m_BuildType] << " 劳动力：" << currentCrops.BuildPoint;
		}
		cout << endl;
	}
	cout << "地图信息" << endl;
	for (int i = 0; i < data.gameMap.getHeigth(); i++) {
		for (int j = 0; j < data.gameMap.getWidth(); j++) {
			mapBlockInfo currentBlock = data.gameMap.ShowInfo(j, i);
			cout << "(" << j << "," << i << ") 主人：" ;
			if (currentBlock.owner == PUBLIC)
				cout << " 公有";
			else if (currentBlock.owner == TRANSITION)
				cout << " 过渡";
			else
				cout << " 玩家" << currentBlock.owner;
			if (currentBlock.towerIndex != NOTOWER)
				cout << " 地形：塔";
			else
				cout << " 地形：" << Terrain[currentBlock.type - 1];
			cout << " 占有属性：";
			for (int u : currentBlock.occupyPoint) {
				cout << u << " ";
			}
			cout << endl;
		}
	}
}


//存活玩家数不足2人或超过最大回合数，游戏结束

bool Game::isValid()
{
	if (playerAlive <= 1 || totalRounds >= 4 * MAX_ROUND)
		return false;
	else
		return true;
}


//【不冲突】主要是访问塔，但regenerate()有对player属性的访问
void Game::regeneratePhase()
{
	//兵团操作
	//所有属于该玩家的兵团回复HP MP
	for (TCorpsID i : data.players[curPlayer - 1].getCrops())
	{
		data.myCorps[i].newRound();
	}
}



void Game::beginPhase()
{
	//by lxj
	//初始化1：回合开始累加上轮经验值并完成塔的升级
	//初始化2：回合开始时判断上一轮生产任务是否完成并结算生产回报
}


//【冲突】比较麻烦塔、兵线、player操作都有了
void Game::endPhase(vector<vector<Command>>& cmd2operate,TPlayerID id)
{
	//判定兵团修改地形指令
	for (int i = 0; i < cmd2operate[id].size(); i++)
	{
		TCorpsID corpsid = cmd2operate[id][i].parameters[1];
		switch (cmd2operate[id][i].parameters[0])
		{//@@@读取坐标
		case(CChangeTerrain):////0:兵团操作类型 1：兵团序号 2：目标地形 3：位置x 4:位置y
		{
			int x = cmd2operate[id][i].parameters[3];
			int y = cmd2operate[id][i].parameters[4];
			terrainType type = (terrainType)cmd2operate[id][i].parameters[2];
			data.myCorps[corpsid].doChangingTerrain(type, x, y);
			break;
		}
		default:
			break;
		}

	}
	cmd2operate[id].clear();

	//判定塔的生产任务
	for (TTowerID t : data.players[curPlayer - 1].getTower())
	{
		Tower& temp = data.myTowers[t];
		//temp.set_level();升级去掉了
		if (temp.protask_finish() == true)//上一回合生产任务完成
		{
			productType m_producttype = temp.getprotype();
			if (m_producttype < 5) //所完成生产任务为生产兵团    
				temp.product_crops(m_producttype);
			if (m_producttype == PUpgrade)//所完成生产任务为升级项目
				temp.upgrade();
			if (m_producttype < 6)//生产消耗缓存置零
				temp.clear_cache(m_producttype);
			temp.set_notask();//设置为【无任务】状态
			data.changeTowers.insert(t);   //by jyp:记录，塔生产任务属性发生了改变
		}
	}
}


//【冲突】既有player的操作也有cell的操作
//【冲突】因为添加兵线则还有兵线操作
void Game::commandPhase(vector<CommandList>& command_list)
{
}




/***********************************************************************************************
*函数名 :【FC18】generatePlayerInfo生成投放给选手ai代码的信息info
*函数功能描述 : 将场上信息通过info传给选手的ai代码
*函数参数 : 无
*函数返回值 : <Info>信息
*作者 : 姜永鹏
***********************************************************************************************/


Info Game::generatePlayerInfo(TPlayerID id) {
	Info info;
	info.myID = id;
	info.totalPlayers = getTotalPlayerNum();
	info.playerAlive = getTotalPlayerAlive();
	info.totalRounds = ceil(data.getRound() / 4.0F);     //比game的round计数要提前1回合，把小回合数除以每轮4个小回合，得到大回合数传给选手
	info.totalTowers = data.getTotalTower();
	info.totalCorps = data.getTotalCorp();

	//info中的玩家数据
	for (int i = 0; i < info.totalPlayers; i++) {
		PlayerInfo newPlayer;
		Player currentPlayer = data.players[i];
		newPlayer.id = currentPlayer.getId();
		newPlayer.alive = currentPlayer.isAlive();
		newPlayer.tower = currentPlayer.getTower();
		newPlayer.corps = currentPlayer.getCrops();
		newPlayer.rank = currentPlayer.getRank();//可见Rank中存的是玩家id-1
		info.playerInfo.push_back(newPlayer);
	}

	//info中的防御塔
	for (int i = 0; i < data.myTowers.size(); i++) {
		//if (data.myTowers[i].getexsit() == false) continue;    //塔没了，也记录信息
		TowerInfo newTower = data.myTowers[i].ShowInfo();
		info.towerInfo.push_back(newTower);
	}


	//info中的兵团信息
	for (int i = 0; i < data.myCorps.size(); i++) {
		//if (data.myCorps[i].bAlive() == false) continue;    //兵团没了，也记录信息
		CorpsInfo newCorps = data.myCorps[i].ShowInfo();
		info.corpsInfo.push_back(newCorps);
	}

	/*for (int i = 0; i < data.gameMap.getHeigth(); i++) {
		vector<mapBlockInfo> newMapRow;
		info.mapInfo.push_back(newMapRow);
		for (int j = 0; j < data.gameMap.getWidth(); j++) {
			mapBlockInfo newBlock = data.gameMap.ShowInfo(j,i);
			info.mapInfo[i].push_back(newBlock);
		}
	}*///直接传地图指针给玩家，常量指针，玩家不可修改！
	info.gameMapInfo = &data.gameMap.map;

	return info;
}


/***********************************************************************************************
*函数名 :【FC18】saveJson保存这一轮的Json数据
*函数功能描述 : 将当前回合的Json数据保存到总的Json数据中
*函数参数 : 无
*函数返回值 : 无
*作者 : 姜永鹏
***********************************************************************************************/
void Game::saveJson(ofstream& infoFile) {
	cout << "begin saving data" << endl;
	//保存这一轮的用户命令数据
	/*Json::Value roundInfo;  //当前回合数据
	roundInfo["round"] = Json::Value(std::to_string(data.getRound()));   //在这里增加回合数
	data.commandJsonRoot["cmd_logList"].append(data.currentRoundCommandJson);
	data.currentRoundCommandJson.clear();*/
	
	//保存这一轮结束的玩家数据
	/*for (int i = 0; i < 4; i++) {
		//Json::Value playerJson;
		Player currentPlayer = data.players[i];
		playerJson["id"] = Json::Value(std::to_string(currentPlayer.getId()));
		playerJson["nm"] = Json::Value(currentPlayer.getName());
		playerJson["cpN"] = Json::Value(std::to_string(currentPlayer.getCrops().size()));
		playerJson["twN"] = Json::Value(std::to_string(currentPlayer.getTower().size()));
		playerJson["scr"] = Json::Value(std::to_string(currentPlayer.getScore()));
		playerJson["rk"] = Json::Value(std::to_string(currentPlayer.getRank()));
		data.currentRoundPlayerJson.append(playerJson);
	}
	roundInfo["player"] = data.currentRoundPlayerJson;
	data.currentRoundPlayerJson.clear();*/

	//保存这一轮结束的兵团数据
	char bufferCorpsHead[64];
	sprintf(bufferCorpsHead, "#corps\n");
	infoFile << bufferCorpsHead;
	//保存所有兵团的ID和位置信息
	for (int i = 0; i < data.myCorps.size(); i++) {
		if (data.myCorps[i].bAlive() == false) {
			/*char bufferCorpsDead[32];
			sprintf(bufferCorpsDead,"%d \n", i);
			infoFile << bufferCorpsDead;*/
			continue;    //兵团没了，就不记录信息
		}
		char bufferCorpsBasic[64];
		sprintf(bufferCorpsBasic, "%d %d %d ", i, data.myCorps[i].getPos().m_x, data.myCorps[i].getPos().m_y);
		infoFile << bufferCorpsBasic;
		if (data.changeCorps.find(i) != data.changeCorps.end())  //若该兵团包含信息变更，则更新信息
		{
			char bufferCorps[512];
			configCorpsTool corps;
			CorpsInfo currentCorpsInfo = data.myCorps[i].ShowInfo();
			corps.player = currentCorpsInfo.owner;
			if (currentCorpsInfo.type == Battle) {
				corps.type = currentCorpsInfo.m_BattleType;
				corps.builder_point = -1;
				corps.movePoint = currentCorpsInfo.movePoint;
				corps.battlePoint = data.myCorps[i].getCE();
				corps.healthPoint = currentCorpsInfo.HealthPoint;
			}
			else if (currentCorpsInfo.type == Construct) {
				corps.type = currentCorpsInfo.m_BuildType + 3;
				corps.builder_point = currentCorpsInfo.BuildPoint;
				corps.movePoint = currentCorpsInfo.movePoint;
				corps.battlePoint = -1;
				corps.healthPoint = -1;
			}
			//corps.starLevel = currentCorpsInfo.level;
			if (data.newCorps.find(currentCorpsInfo.ID) != data.newCorps.end()) {
				corps.newC = 1;
			}
			else
				corps.newC = 0;
			//data.currentRoundCorpsJson.append(corpsJson);
			sprintf(bufferCorps,"%d %d %d %d %d %d %d \n"        //, data.myCorps[i].getPos().m_x
				                                                 //, data.myCorps[i].getPos().m_y
				                                                 , corps.player
																 , corps.type
																 , corps.builder_point
																 , corps.movePoint
																 , corps.battlePoint
																 , corps.healthPoint
																 , corps.newC);
			infoFile << bufferCorps;
		}
		else  //若兵团信息未变更，则不发送新信息
			infoFile << "\n";
	}
	//roundInfo["corps"] = data.currentRoundCorpsJson;
	//data.currentRoundCorpsJson.clear();
	//data.infoJsonRoot["info_logList"].append(roundInfo);


	//保存这一轮结束的防御塔数据
	char bufferTowerHead[64];
	sprintf(bufferTowerHead, "#tower\n");
	infoFile << bufferTowerHead;
	for (int i = 0; i < data.myTowers.size(); i++) {
		if (data.myTowers[i].getexsit() == false) {
			/*char bufferTowerDead[32];
			sprintf(bufferTowerDead, "%d \n", i);
			infoFile << bufferTowerDead;*/
			continue;   //塔没了，就不记录信息
		}
		char bufferTowerBasic[64];
		sprintf(bufferTowerBasic, "%d %d %d ", i, data.myTowers[i].getPosition().m_x, data.myTowers[i].getPosition().m_y);
		infoFile << bufferTowerBasic;
		if (data.changeTowers.find(i) != data.changeTowers.end())   //只记录变更了的防御塔信息
		{
			char bufferTower[512];
			configTowerTool tower;
			TowerInfo currentTowerInfo = data.myTowers[i].ShowInfo();
			tower.player = currentTowerInfo.ownerID;
			tower.buildPoint = currentTowerInfo.productPoint;
			if (data.myTowers[i].protask_finish() == true)  //生产任务完成
			{
				tower.type = NOTASK;
				tower.buildCost = -1;
			}
			else
			{
				tower.type = currentTowerInfo.pdtType;
				tower.buildCost = currentTowerInfo.productConsume;
			}
			tower.healthPoint = currentTowerInfo.healthPoint;
			tower.battlePoint = currentTowerInfo.battlePoint;
			tower.starLevel = currentTowerInfo.level;
			if (data.newTower.find(currentTowerInfo.ID) != data.newTower.end())
				tower.newT = 1;
			else
				tower.newT = 0;
			sprintf(bufferTower, "%d %d %d %d %d %d %d %d\n"         //, data.myTowers[i].getPosition().m_x
				                                                     //, data.myTowers[i].getPosition().m_y
				                                                     , tower.player
																	 , tower.buildPoint
																	 , tower.type
																	 , tower.buildCost
																	 , tower.healthPoint
																	 , tower.battlePoint
																	 , tower.starLevel
																	 , tower.newT);
			infoFile << bufferTower;
		}
		else  //防御塔信息未变更，则不做记录
			infoFile << "\n";
		
	}
	//roundInfo["tower"] = data.currentRoundTowerJson;
	//data.currentRoundTowerJson.clear();

	

	//保存这一轮的地图数据
	for (int i = 0; i < data.gameMap.getHeigth(); i++) {
		for (int j = 0; j < data.gameMap.getWidth(); j++) {
			Json::Value blockJson;
			mapBlockInfo currentBlockInfo = data.gameMap.ShowInfo(j, i);
			//Json::Value blockPos;
			blockJson["x"] = Json::Value(j);
			blockJson["z"] = Json::Value(i);
			//blockJson["pos"] = blockPos;
			if (currentBlockInfo.towerIndex != NOTOWER)
				blockJson["tp"] = Json::Value(TRTower);
			else
				blockJson["tp"] = Json::Value(currentBlockInfo.type);
			blockJson["oId"] = Json::Value(currentBlockInfo.owner);
			data.currentRoundMapJson.append(blockJson);
		}
	}
	//Json::Value outputJson;
	//outputJson["round"] = Json::Value(std::to_string(data.getRound()));
	//地图信息存档
	char bufferMapHead[64];
	sprintf(bufferMapHead, "#map\n");
	infoFile << bufferMapHead;
	for (int i = 0; i < data.currentRoundMapJson.size(); i++) {
		//Json::Value mapChange;
		if (data.currentRoundMapJson[i]["tp"] != data.lastRoundMapJson[i]["tp"] || data.currentRoundMapJson[i]["oId"] != data.lastRoundMapJson[i]["oId"]) {
			char bufferBlock[64];
			sprintf(bufferBlock, "%d %d %d %d\n", data.currentRoundMapJson[i]["x"].asInt(), data.currentRoundMapJson[i]["z"].asInt(), data.currentRoundMapJson[i]["tp"].asInt(), data.currentRoundMapJson[i]["oId"].asInt());
			infoFile << bufferBlock;
			/*mapChange["x"] = data.currentRoundMapJson["map"][i]["x"];
			mapChange["z"] = data.currentRoundMapJson["map"][i]["z"];
			mapChange["tp"] = data.currentRoundMapJson["map"][i]["tp"];
			mapChange["oId"] = data.currentRoundMapJson["map"][i]["oId"];
			outputJson["map"].append(mapChange);*/
		}
	}
	//data.mapInfoJsonRoot["map_logList"].append(outputJson);
	data.lastRoundMapJson.clear();
	data.lastRoundMapJson = data.currentRoundMapJson;
	data.currentRoundMapJson.clear();

	//玩家信息存档
	char bufferInfo[64];
	sprintf(bufferInfo, "#players\n");
	infoFile << bufferInfo;
	for (int i = 0; i < data.totalPlayers; i++)
	{
		char bufferPlayer[256];
		sprintf(bufferPlayer, "%d %s %lld %lld %d %d\n", data.players[i].getId()
													   , data.players[i].getName().c_str()
													   , data.players[i].getCrops().size()
													   , data.players[i].getTower().size()
													   , data.players[i].getScore()
													   , data.players[i].getRank());
		infoFile << bufferPlayer;
	}

	return;
}


/***********************************************************************************************
*函数名 :【FC18】goNext判断新回合进行函数
*函数功能描述 : 更新玩家出局信息，更新存活玩家数，判断游戏能否继续进行
*函数参数 : 无
*函数返回值 : <bool>是否能继续进行false---不能，true---能
*作者 : 姜永鹏
***********************************************************************************************/
bool Game::goNext() {
	if (getCurrentRound() >= 4 * MAX_ROUND) return false;//超过最大回合数，利用的是Game中的回合数，超过4倍（4人）最大回合数，直接game_over
	int aliveNum = 0;
	for (int i = 0; i < 4; i++) {
		if (data.players[i].isAlive() == true)
			aliveNum++;
	}
	playerAlive = aliveNum;//更新游戏存活人数
	if (playerAlive <= 1)  return false;
	else return true;
}


/***********************************************************************************************
*函数名 :【FC18】printGameMap游戏地图输出函数
*函数功能描述 : 将不同地形以不同颜色的代号输出于控制台，方便获取地图概况
*函数参数 : 无
*函数返回值 : 无
*作者 : 姜永鹏
***********************************************************************************************/
void Game::printGameMap() {
	for (int i = 0; i < data.gameMap.getHeigth(); i++) {
		for (int j = 0; j < data.gameMap.getWidth(); j++) {
			std::cout.setf(std::ios::fixed, std::ios::floatfield);
			switch (data.gameMap.map[i][j].type) {
			case(0):SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED); break;
			case(1):SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN); break;
			case(2):SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE); break;
			case(3):SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN); break;
			case(4):SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE); break;
			case(5):SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE); break;
			}
			std::cout << setprecision(5) << int(data.gameMap.map[i][j].type) << " ";
		}
		std::cout << "\n";
	}
	std::cout << "\n\n";
}



void Game::printJson() {
	/*//输出到文件 #json 
	Json::FastWriter sw_cmd, sw_info, sw_map;
	ofstream json_cmd, json_info, json_map;
	//写入指令Json
	json_cmd.open(cmd_json_filename);
	//旧代码//json_os << sw.write(data.root);
	json_cmd << sw_cmd.write(data.commandJsonRoot);
	json_cmd.close();
	//写入信息Json
	json_info.open(info_json_filename);
	json_info << sw_info.write(data.infoJsonRoot);
	json_info.close();
	//写入地图Json
	json_map.open(mapinfo_json_filename);
	json_map << sw_map.write(data.mapInfoJsonRoot);
	json_map.close();*/
}