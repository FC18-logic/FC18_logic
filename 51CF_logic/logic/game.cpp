#include "game.h"
#include <set>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <cmath> 

bool Game::init(string file, char* json_file, vector<string> players_name)   //[【FC18】接着用，改好了
{
	//【FC18】Json文件名
	cmd_json_filename = file.append("cmd.json");
	info_json_filename = file.append("info.json");
	mapinfo_json_filename = file.append("map.info");
	//#json
	roundTime.push_back(clock());
	Json::Value Head;     //【FC18】
	Json::Value Body;     //【FC18】
	data.commandJsonRoot["head"] = Head;   //【FC18】
	data.commandJsonRoot["body"] = Body;   //【FC18】
	data.infoJsonRoot["body"]    = Body;   //【FC18】
	data.mapInfoJsonRoot["head"] = Head;   //【FC18】
	data.mapInfoJsonRoot["body"] = Body;   //【FC18】
	data.currentRoundCommandJson["round"] = Json::Value(0);   //【FC18】
	data.currentRoundCorpsJson["round"]   = Json::Value(0);   //【FC18】
	data.currentRoundTowerJson["round"]   = Json::Value(0);   //【FC18】
	data.currentRoundPlayerJson["round"]  = Json::Value(0);   //【FC18】
	data.currentRoundMapJson["round"]     = Json::Value(0);   //【FC18】



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

	if (!data.gameMap.readMap(in, true, players_name))                       //#Json 读入地图，写地图Json文件头，写0回合玩家指令Json
	{
		cerr << "Something wrong when reading the map file." << endl;
		return false;
	}
	if (!data.gameMap.randomInitMap())                         //#Json 保存塔和地图的Json
	{
		cerr << "Something wrong when randomizing the game map." << endl;
		return false;
	}


	/****************************FC15的旧代码****************************
	if (!data.gameMap.init(in, _MAX_RESOURCE_, true))
	{
		cerr << "Something wrong when init the map infomation."<<endl;
		return false;
	}
	*********************************************************************/
	in.close();
	//旧代码//currentRound = 0;
	totalRounds = 0;
	//旧代码//playerSize = playerAlive = data.PlayerNum;
	totalPlayers = playerAlive = data.totalPlayers;

	//旧代码//data.tentacles = new Tentacle**[data.CellNum];
	//Tentacle二维数组，[i][j]就对应着i号兵团与j号兵团的作战关系
	//旧代码//for (int i = 0; i != data.CellNum; ++i)
	//旧代码//{
		//旧代码//data.tentacles[i] = new Tentacle*[data.CellNum];
		//旧代码//for (int j = 0; j != data.CellNum; ++j)
			//旧代码//data.tentacles[i][j] = nullptr;
	//旧代码//}
	//初始化排名
	//旧代码//for (int i = 0; i != playerSize; ++i)
		//旧代码//Rank.push_back(i);
	for (int i = 0; i < totalPlayers; i++) {
		Rank.push_back(i);
	}

	//初始化计数
	//旧代码//for (int i = 0; i != playerSize; ++i)
		//旧代码//controlCount.push_back(0);
	for (int i = 0; i < totalPlayers; i++) {
		controlCount.push_back(0);
	}

	//旧代码//data.root["head"]["totalRounds"] = currentRound + 1;
	//旧代码//data.root["body"].append(data.currentRoundJson);
	//旧代码//data.currentRoundJson.clear();
	data.commandJsonRoot["head"]["totalRounds"] = totalRounds + 1;
	data.commandJsonRoot["body"].append(data.currentRoundCommandJson);
	data.currentRoundCommandJson.clear();


	data.gameMap.saveMapJson();
	data.currentRoundMapJson.clear();
	
	data.infoJsonRoot["body"]["corpsInfo"] = data.currentRoundCorpsJson;
	data.infoJsonRoot["body"]["towerInfo"] = data.currentRoundTowerJson;
	data.mapInfoJsonRoot["body"]["playerInfo"] = data.currentRoundPlayerJson;

	//输出到文件 #json 
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
	json_map.close();
	return true;
}

void Game::DebugPhase()
{
	cout << "/*************** DEBUG 信息 ***************/" << endl;
	cout << "Round " << currentRound << endl;
	cout << "玩家剩余： " << playerAlive << " / " << playerSize << endl;
	for (int i = 0; i != data.PlayerNum; ++i)
	{
		cout << "玩家 " << i << " ： " <<" 总资源数： "<< data.players[i].totalResource() << " 科技点数： " << data.players[i].techPoint() << " 技能等级： "
			<< " 回复 " << data.players[i].getRegenerationLevel() << " 移动 " << data.players[i].getMoveLevel()
			<< " 操作 " << data.players[i].getExtraControlLevel() << " 防御 " << data.players[i].getDefenceLevel() 
			<< " 最大操作：" << data.players[i].maxControlNumber() << endl;
		cout << "当前兵塔： ";
		for (TCellID u : data.players[i].cells())   //这个for循环遍历i号玩家所有塔的信息，塔用u来存储
			cout << u <<" ";  //塔的序号
		cout << endl;
		cout << "排名： " << std::find(Rank.begin(),Rank.end(),i) - Rank.begin() + 1;
		cout << endl;
	}
	cout << endl;
	char stg[4] = { 'N','A','D','G' };
	for (int i = 0; i != data.CellNum; ++i)
	{
		cout << "兵塔 " << i << " ： " << "所属： " << data.cells[i].getPlayerID() << " 位置：(" << data.cells[i].getPos().m_x << ", " << data.cells[i].getPos().m_y << ")"
			<< " 资源： " << data.cells[i].getResource() << " 总资源： " << data.cells[i].totalResource()
			<< " 策略： " << stg[data.cells[i].getStg()] << " 总兵线：" << data.cells[i].TentacleNumber()
			<< " 正在攻击： ";
		for (int j = 0; j != data.CellNum; ++j)
		{
			if (data.tentacles[i][j])
				cout << j << " ";
		}
		cout << endl;
		if (data.cells[i].getPlayerID() == Neutral)
			cout << "中立兵塔属性：" << "占领势力： " << data.cells[i].getOccupyOwner() << " 占领点： " << data.cells[i].getOccupyPoint() <<endl;
	}
	cout << "兵线信息: " <<endl;
	for (int i = 0; i != data.CellNum; ++i)
	{
		for (int j = 0; j != data.CellNum; ++j)
			if (data.tentacles[i][j])  //存在对战关系
				cout << 1;
			else
				cout << 0;
		cout << endl;
	}
	cout << endl;
	vector<string> n2str{ "Extending","Attacking","Backing","Confrontation","Arrived","AfterCut" };
	
	//FC18是兵团作战，没有前方、后方、切断这些概念，这个输出回来可以调整一下
	for (int i = 0; i != data.CellNum; ++i)
		for (int j = 0; j != data.CellNum; ++j)
			if (data.tentacles[i][j])
			{
				cout << "兵线 " << i << " -> " << j << " : "
					<< " 状态： " << n2str[data.tentacles[i][j]->getstate()];
				if (data.tentacles[i][j]->getstate() == AfterCut)
					cout << " 前方资源： " << data.tentacles[i][j]->getFrontResource()
					<< "后方资源： " << data.tentacles[i][j]->getBackResource();
				else
					cout << " 资源： " << data.tentacles[i][j]->getResource();
				cout << endl;
			}
	cout << "\n\n";
}

//每回合-========
void Game::saveJson(DATA::Data & dataLastRound, DataSupplement & dataSuppleMent)
{
	//playerAction	
	//#json add
	{
		//排名信息
		vector<std::pair<TPlayerID, TResourceD> > playerpair;
		for (int i = 0; i != data.PlayerNum; ++i)
		{
			if (data.players[i].isAlive())
				//玩家资源数应该是所有塔和兵团的资源数按规则描述进行求和
				playerpair.push_back({ i + 1,data.players[i].totalResource() });
			else
				playerpair.push_back({ i + 1,data.players[i].getdeadRound() - 1000 });
		}

		//按最大资源数排序
		std::sort(playerpair.begin(), playerpair.end(),
			[](const std::pair<TPlayerID, TResourceD>& a, std::pair<TPlayerID, TResourceD>& b) {return a.second > b.second; });



		Json::Value RankJson;
		for (int i = 0; i != playerpair.size(); i++)
		{
			RankJson["rank"].append(playerpair[i].first);
			if (playerpair[i].second < 0)
				//已经死亡，资源数清零
				RankJson["resources"].append(0);
			else
				RankJson["resources"].append(playerpair[i].second);
		}
		RankJson["rank"].append(0);
		if (data.cells[12].getPlayerID() == Neutral)
			RankJson["resources"].append(data.cells[12].totalResource());
		else
			RankJson["resources"].append(0);
		data.currentRoundJson["rankInfo"]= RankJson;
	}



	for (int i = 0; i != data.PlayerNum; i++)
	{

		//各种属性改变
		if (
			(data.players[i].getDefenceLevel() != dataLastRound.players[i].getDefenceLevel()) ||
			(data.players[i].getExtraControlLevel() != dataLastRound.players[i].getExtraControlLevel()) ||
			(data.players[i].getMoveLevel() != dataLastRound.players[i].getMoveLevel()) ||
			(data.players[i].getRegenerationLevel() != dataLastRound.players[i].getRegenerationLevel())
			)
		{
			Json::Value paj;
			paj["id"] = i + 1;
			paj["type"] = 1;
			paj["rSS"] = data.players[i].getRegenerationLevel();
			paj["sS"] = data.players[i].getMoveLevel();
			paj["eCS"] = data.players[i].getExtraControlLevel();
			paj["dS"] = data.players[i].getDefenceLevel();
			data.currentRoundJson["playerAction"].append(paj);
		}

	}
	//cellActions
	for (int i = 0; i != data.CellNum; i++)
	{
		//大小/资源值/科技值改变
		if (data.cells[i].getResource() != dataLastRound.cells[i].getResource()
			|| (data.cells[i].techRegenerateSpeed() != dataSuppleMent.cellTechPoint[i]))
		{
			Json::Value resourceChangeJson;
			resourceChangeJson["type"] = 2;
			resourceChangeJson["id"] = i;
			float tem = float(sqrt(data.cells[i].getResource()) * 4 + 10); //#jsonChange_3_9
			resourceChangeJson["newSize"] = int(tem);
			resourceChangeJson["newResource"] = float(data.cells[i].getResource());
			float techTem = float(data.cells[i].techRegenerateSpeed());
			resourceChangeJson["newTechVal"] = techTem;

			Json::Value srcTentatclesJson;
			Json::Value dstTentaclesJson;
			Json::Value dstTentaclesCutJson;
			for (int j = 0; j != data.CellNum; j++)
			{
				if (i != j&&data.tentacles[i][j])
				{
					srcTentatclesJson.append(data.tentacles[i][j]->getID());
				}

				if (i != j && data.tentacles[j][i])
				{
					dstTentaclesCutJson.append(data.tentacles[j][i]->getID());
				}

				if (data.tentacles[j][i] && data.tentacles[j][i]->getFrontResource() > 0.001)
				{
					dstTentaclesCutJson.append(data.tentacles[j][i]->getCutID());
				}
			}
			resourceChangeJson["srcTentatcles"] = srcTentatclesJson;
			resourceChangeJson["dstTentacles"] = dstTentaclesJson;
			resourceChangeJson["dstTentaclesCut"] = dstTentaclesCutJson;
			data.currentRoundJson["cellActions"].append(resourceChangeJson);
		}

		//等级改变 done
		if (data.cells[i].getCellType() != dataLastRound.cells[i].getCellType())
		{
			Json::Value typeChangeJson;
			typeChangeJson["type"] = 3;
			typeChangeJson["id"] = i;
			typeChangeJson["newLevel"] = data.cells[i].getCellType();
			data.currentRoundJson["cellActions"].append(typeChangeJson);
		}

		//策略改变 done
		if (data.cells[i].getStg() != dataLastRound.cells[i].getStg())
		{
			Json::Value stgChangeJson;
			stgChangeJson["type"] = 4;
			stgChangeJson["id"] = i;
			stgChangeJson["newStg"] = data.cells[i].getStg();
			data.currentRoundJson["cellActions"].append(stgChangeJson);
		}

		//派系改变 5 change
		if (data.cells[i].getPlayerID() != dataLastRound.cells[i].getPlayerID())
		{
			Json::Value teamJson;
			teamJson["type"] = 5;
			teamJson["id"] = i;
			teamJson["newTeam"] = data.cells[i].getPlayerID() + 1;
			//#jsonChange_3_9 添加兵线列表
			for (TTentacleID id:data.cells[i].getTentacles())
			{
				teamJson["srcTentatcles"].append(id);
			}
			data.currentRoundJson["cellActions"].append(teamJson);
		}

	}

	//tentacleActions & cutTentacleActions
	for (int i = 0; i != data.CellNum; i++)
	{
		for (int j = 0; j != data.CellNum; j++)
		{
			//tentacleActions			
			{
				/*
				cout << "i " << i << " j " << j << endl;
				if (dataLastRound.tentacles[i][j])  cout << " last has " << endl;
				if (data.tentacles[i][j]) cout << " now has " << endl;
				if (dataLastRound.tentacles[i][j] && data.tentacles[i][j]) cout << "both has" << endl;
				if (dataLastRound.tentacles[i][j] && dataLastRound.tentacles[i][j]->getstate() == AfterCut) cout << "last AfterCut" << endl;
				if (data.tentacles[i][j] && data.tentacles[i][j]->getstate() == AfterCut) cout << "now AfterCut" << endl;
				*/

				//新增 done
				if (!dataLastRound.tentacles[i][j] && data.tentacles[i][j] &&
					data.tentacles[i][j]->getResource()>0.001)
				{
					//先新增
					Json::Value tentacleAddtionJson;
					tentacleAddtionJson["type"] = 1;
					tentacleAddtionJson["id"] = data.tentacles[i][j]->getID();
					tentacleAddtionJson["srcCell"] = i;
					tentacleAddtionJson["dstCell"] = j;
					tentacleAddtionJson["transRate"] = data.tentacles[i][j]->getExtendSpeed();  //#json
					data.currentRoundJson["tentacleActions"].append(tentacleAddtionJson);


					//第一次伸长
					Json::Value tentacleExtendJson;
					tentacleExtendJson["type"] = 2;
					tentacleExtendJson["id"] = data.tentacles[i][j]->getID();
					double extendCoefficient = (data.tentacles[i][j]->getResource() + data.tentacles[i][j]->getBackResource())
						/ (data.tentacles[i][j]->getLength()*Density);
					double xLengh = data.cells[j].getPos().m_x - data.cells[i].getPos().m_x;
					double yLengh = data.cells[j].getPos().m_y - data.cells[i].getPos().m_y;
					tentacleExtendJson["movement"]["dx"] = extendCoefficient*xLengh;
					tentacleExtendJson["movement"]["dy"] = extendCoefficient*yLengh;
					data.currentRoundJson["tentacleActions"].append(tentacleExtendJson);

				}

				//伸长  done
				if (dataLastRound.tentacles[i][j] && data.tentacles[i][j] && data.tentacles[i][j]->getResource() > 0.001
					&& (dataLastRound.tentacles[i][j]->getBackResource() + dataLastRound.tentacles[i][j]->getResource() <
						data.tentacles[i][j]->getResource() + data.tentacles[i][j]->getBackResource()))
				{
					Json::Value tentacleExtendJson; 
					tentacleExtendJson["type"] = 2;
					tentacleExtendJson["id"] = dataLastRound.tentacles[i][j]->getID();
					double extendCoefficient = (data.tentacles[i][j]->getResource()+data.tentacles[i][j]->getBackResource() 
						- dataLastRound.tentacles[i][j]->getResource()-dataLastRound.tentacles[i][j]->getBackResource())  //change
						/ (data.tentacles[i][j]->getLength()*Density);
					double xLengh = data.cells[j].getPos().m_x - data.cells[i].getPos().m_x;
					double yLengh = data.cells[j].getPos().m_y - data.cells[i].getPos().m_y;
					tentacleExtendJson["movement"]["dx"] = extendCoefficient*xLengh;
					tentacleExtendJson["movement"]["dy"] = extendCoefficient*yLengh;
					data.currentRoundJson["tentacleActions"].append(tentacleExtendJson);
				}


				//第一次缩短
				if (dataLastRound.tentacles[i][j] && data.tentacles[i][j]
					&& (data.cutTentacleJson[i][j])
					&& (data.tentacles[i][j]->getBackResource()>0.001)
					&& (dataLastRound.tentacles[i][j]->getResource() > 0.001)
					)
				{
					double backingResource = dataLastRound.tentacles[i][j]->getResource() - data.cutTentacleInfoJson[i][j].frontResource;
					Json::Value tentacleBackJson;
					tentacleBackJson["type"] = 3;
					tentacleBackJson["id"] = data.tentacles[i][j]->getID();
					double extendCoefficient = (data.tentacles[i][j]->getBackResource() - backingResource) / (data.tentacles[i][j]->getLength()*Density);
					double xLengh = data.cells[j].getPos().m_x - data.cells[i].getPos().m_x;
					double yLengh = data.cells[j].getPos().m_y - data.cells[i].getPos().m_y;
					tentacleBackJson["movement"]["dx"] = extendCoefficient*xLengh;
					tentacleBackJson["movement"]["dy"] = extendCoefficient*yLengh;
					data.currentRoundJson["tentacleActions"].append(tentacleBackJson);

				}


				//缩短 普通缩短
				if (dataLastRound.tentacles[i][j] && data.tentacles[i][j] 
					&&(!data.cutTentacleJson[i][j]) //此回合没有切断
					&& (data.tentacles[i][j]->getBackResource() + data.tentacles[i][j]->getResource()>0.001)
					&&((dataLastRound.tentacles[i][j]->getResource() + dataLastRound.tentacles[i][j]->getBackResource())
					   >
					  (data.tentacles[i][j]->getResource() + data.tentacles[i][j]->getBackResource()))
					)
				{
					Json::Value tentacleBackJson;
					tentacleBackJson["type"] = 3;
					tentacleBackJson["id"] = data.tentacles[i][j]->getID();
					double r1 = dataLastRound.tentacles[i][j]->getResource() + dataLastRound.tentacles[i][j]->getBackResource();
					double r2 = data.tentacles[i][j]->getResource() + data.tentacles[i][j]->getBackResource();
					double extendCoefficient = (r2 - r1) / (data.tentacles[i][j]->getLength()*Density);
					double xLengh = data.cells[j].getPos().m_x - data.cells[i].getPos().m_x;
					double yLengh = data.cells[j].getPos().m_y - data.cells[i].getPos().m_y;
					tentacleBackJson["movement"]["dx"] = extendCoefficient*xLengh;
					tentacleBackJson["movement"]["dy"] = extendCoefficient*yLengh;
					data.currentRoundJson["tentacleActions"].append(tentacleBackJson);
				}


				//切断 在tentacle.cpp cut()中

				//消失 

				if (dataLastRound.tentacles[i][j] &&
					(!data.tentacles[i][j] ||
					(data.tentacles[i][j]->getResource() + data.tentacles[i][j]->getBackResource())<0.001)
					)
				{
					//先缩回后消失
					Json::Value tentacleBackJson;
					tentacleBackJson["type"] = 3;
					tentacleBackJson["id"] = dataLastRound.tentacles[i][j]->getID();
					double r1=0;
					if (data.cutTentacleJson[i][j]) //如果本轮切断了
						r1 = dataLastRound.tentacles[i][j]->getResource()
						- data.cutTentacleInfoJson[i][j].frontResource;
					else //已经切断过 
						r1 = dataLastRound.tentacles[i][j]->getBackResource();

					double extendCoefficient = -r1 / (dataLastRound.tentacles[i][j]->getLength()*Density);
					double xLengh = data.cells[j].getPos().m_x - data.cells[i].getPos().m_x;
					double yLengh = data.cells[j].getPos().m_y - data.cells[i].getPos().m_y;
					tentacleBackJson["movement"]["dx"] = extendCoefficient*xLengh;
					tentacleBackJson["movement"]["dy"] = extendCoefficient*yLengh;
					data.currentRoundJson["tentacleActions"].append(tentacleBackJson);


					//消失
					Json::Value tentacleGoneJson;
					tentacleGoneJson["type"] = 6;
					tentacleGoneJson["id"] = dataLastRound.tentacles[i][j]->getID();
					data.currentRoundJson["tentacleActions"].append(tentacleGoneJson);
				}

			}


			//FC18没有切断这部分我们应该是不需要改了
			//cutTentacleActions
			{

				//新建时候的第一次缩短
				if (data.tentacles[i][j]
					&&data.tentacles[i][j]->getFrontResource()>0.001
					&& data.cutTentacleBornJson[i][j]
					)
				{
					int m_cutID = data.cutTentacleInfoJson[i][j].cutID;
					double frontResource = data.cutTentacleInfoJson[i][j].frontResource;

					Json::Value cutTentacleBackJson;
					cutTentacleBackJson["type"] = 2;
					cutTentacleBackJson["id"] = data.tentacles[i][j]->getCutID();
					double backCoefficient = ( frontResource - data.tentacles[i][j]->getFrontResource() )  //#问题
						/ (data.tentacles[i][j]->getLength()*Density);
					double xLengh = data.cells[j].getPos().m_x - data.cells[i].getPos().m_x;
					double yLengh = data.cells[j].getPos().m_y - data.cells[i].getPos().m_y;
					cutTentacleBackJson["movement"]["dx"] = backCoefficient*xLengh;
					cutTentacleBackJson["movement"]["dy"] = backCoefficient*yLengh;
					data.currentRoundJson["cutTentacleActions"].append(cutTentacleBackJson);
				}


				//缩短  普通缩短，
				if (dataLastRound.tentacles[i][j] && data.tentacles[i][j]
					&&( dataLastRound.tentacles[i][j]->getFrontResource()>0.0001&&data.tentacles[i][j]->getFrontResource()>0.0001)
					&&(!data.cutTentacleBornJson[i][j])
					&&(data.tentacles[i][j]->getFrontResource()<dataLastRound.tentacles[i][j]->getFrontResource()))
				{
					Json::Value cutTentacleBackJson;
					cutTentacleBackJson["type"] = 2;
					cutTentacleBackJson["id"] = data.tentacles[i][j]->getCutID();
					double backCoefficient = (dataLastRound.tentacles[i][j]->getFrontResource() 
						- data.tentacles[i][j]->getFrontResource())
						/ (data.tentacles[i][j]->getLength()*Density);
					double xLengh = data.cells[j].getPos().m_x - data.cells[i].getPos().m_x;
					double yLengh = data.cells[j].getPos().m_y - data.cells[i].getPos().m_y;
					cutTentacleBackJson["movement"]["dx"] = backCoefficient*xLengh;
					cutTentacleBackJson["movement"]["dy"] = backCoefficient*yLengh;
					data.currentRoundJson["cutTentacleActions"].append(cutTentacleBackJson);
				}

				//cut 消失
				if (
					(dataLastRound.tentacles[i][j] && (dataLastRound.tentacles[i][j]->getFrontResource() > 0.01))
					&& ((!data.tentacles[i][j]) || (data.tentacles[i][j]->getFrontResource() < 0.0001)))
				{
					//先缩短
					Json::Value cutTentacleBackJson;
					cutTentacleBackJson["type"] = 2;
					cutTentacleBackJson["id"] = dataLastRound.tentacles[i][j]->getCutID();
					if (dataLastRound.tentacles[i][j]->getCutID() < 0)
					{
						system("pause");
					}
					double backCoefficient = (dataLastRound.tentacles[i][j]->getFrontResource())
						/ (dataLastRound.tentacles[i][j]->getLength()*Density);
					double xLengh = data.cells[j].getPos().m_x - data.cells[i].getPos().m_x;
					double yLengh = data.cells[j].getPos().m_y - data.cells[i].getPos().m_y;
					cutTentacleBackJson["movement"]["dx"] = backCoefficient*xLengh;
					cutTentacleBackJson["movement"]["dy"] = backCoefficient*yLengh;
					data.currentRoundJson["cutTentacleActions"].append(cutTentacleBackJson);


					//后消失
					Json::Value cutTentacleGoneJson;
					cutTentacleGoneJson["type"] = 3;
					cutTentacleGoneJson["id"] = dataLastRound.tentacles[i][j]->getCutID();
					data.currentRoundJson["cutTentacleActions"].append(cutTentacleGoneJson);
				}


				//一个回合内完成新建、缩短、消失
				if (    //条件：已经完成了新建，且上次还是有的兵线切，但是这次没有兵线了，或者断兵线没有了
					(data.cutTentacleBornJson[i][j]) && dataLastRound.tentacles[i][j]
					&&(dataLastRound.tentacles[i][j]->getFrontResource()<0.001)
				    &&(!data.tentacles[i][j] || data.tentacles[i][j]->getFrontResource() < 0.001)
					)
				{
					int m_cutID = data.cutTentacleInfoJson[i][j].cutID;
					double frontResource = data.cutTentacleInfoJson[i][j].frontResource;
					//先缩短
					Json::Value cutTentacleBackJson;
					cutTentacleBackJson["type"] = 2;
					cutTentacleBackJson["id"] = m_cutID;
					double backCoefficient = frontResource
						/ (dataLastRound.tentacles[i][j]->getLength()*Density);
					double xLengh = data.cells[j].getPos().m_x - data.cells[i].getPos().m_x;
					double yLengh = data.cells[j].getPos().m_y - data.cells[i].getPos().m_y;
					cutTentacleBackJson["movement"]["dx"] = backCoefficient*xLengh;
					cutTentacleBackJson["movement"]["dy"] = backCoefficient*yLengh;
					data.currentRoundJson["cutTentacleActions"].append(cutTentacleBackJson);


					//后消失
					Json::Value cutTentacleGoneJson;
					cutTentacleGoneJson["type"] = 3;
					cutTentacleGoneJson["id"] = m_cutID;
					data.currentRoundJson["cutTentacleActions"].append(cutTentacleGoneJson);
				}


			}

		}
	}
}

//生成选手信息
vector<Info> Game::generateInfo()
{
	vector<Info> info;
	//PlayerInfo playerInfo;   //势力信息
	//CellInfo cellInfo; //同学信息
	//TentacleInfo tentacleInfo; //兵线信息
	BaseMap* mapInfo;  //地图信息
	//初始化其他信息
	//info里的成员数PlayerNum个，记录所有玩家的属性、塔和兵团，也记录一些公共的信息
	//这个是用来向玩家各自的ai提供信息的吗
	for (int i = 0; i != data.PlayerNum; ++i)
	{
		Info I;
		I.playerSize = data.PlayerNum;
		int alive = 0;//统计存活势力个数
		for (int j = 0; j != data.PlayerNum; ++j)
		{
			if (data.players[j].isAlive())
				alive++;
		}
		I.playerAlive = alive;
		I.myID = i;
		I.cellNum = data.CellNum;
		I.round = currentRound;
		I.myMaxControl = data.players[i].maxControlNumber();
		I.mapInfo = &data.gameMap;
		info.push_back(I);
	}
	//初始化阵营
	for (int i = 0; i != data.PlayerNum; ++i)
	{
		PlayerInfo temp;
		Player* curr = &data.players[i];
		temp.alive = curr->isAlive();
		temp.DefenceLevel = curr->getDefenceLevel();
		temp.ExtendingSpeedLevel = curr->getMoveLevel();
		temp.ExtraControlLevel = curr->getExtraControlLevel();
		temp.id = i;
		temp.maxControlNumber = curr->maxControlNumber();
		temp.rank = std::find(Rank.begin(), Rank.end(), i) - Rank.begin() + 1;
		temp.RegenerationSpeedLevel = curr->getRegenerationLevel();
		temp.technologyPoint = curr->techPoint();
		temp.cells = curr->cells();
		for (int j = 0; j != data.PlayerNum; ++j)
		{
			info[j].playerInfo.push_back(temp);
		}
	}

	//初始化兵塔信息
	for (int i = 0; i != data.CellNum; ++i)
	{
		CellInfo temp;
		Cell* curr = &data.cells[i];
		temp.id = i;
		temp.occupyPoint = curr->getOccupyPoint();
		temp.occupyOwner = curr->getOccupyOwner();
		temp.owner = curr->getPlayerID();
		temp.position = curr->getPos();
		temp.resource = curr->getResource();
		temp.strategy = curr->getStg();
		temp.type = curr->getCellType();
		temp.maxResource = curr->getCellProperty().m_maxResource;
		temp.maxTentacleNum = curr->getCellProperty().m_maxTentacleNum;
		temp.currTentacleNum = curr->TentacleNumber();
		for (int j = 0; j != data.PlayerNum; ++j)
		{
			info[j].cellInfo.push_back(temp);
		}
	}

	//初始化Tentacle
	vector<TentacleInfo> templist;
	for (int k = 0; k != data.PlayerNum; ++k)
		for (int j = 0; j != data.CellNum; ++j)
	{
		info[k].tentacleInfo.push_back(templist);
	}

	for (int i = 0; i != data.CellNum; ++i)
		for(int j = 0;j!=data.CellNum;++j)
	{
		TentacleInfo temp;
		Tentacle* curr = data.tentacles[i][j];
		if (!curr)  //i对j不存在攻击关系
		{
			temp.exist = false;
			for (int k = 0; k != data.PlayerNum; ++k)
			{
				info[k].tentacleInfo[i].push_back(temp);
			}
			continue;
		}
		temp.backResource = curr->getBackResource();
		temp.exist = true;
		temp.frontResource = curr->getFrontResource();
		temp.maxlength = curr->getLength();
		temp.resource = curr->getResource();
		temp.sourceCell = curr->getSourceCell();
		temp.state = curr->getstate();
		temp.targetCell = curr->getTargetCell();
		for (int k = 0; k != data.PlayerNum; ++k)
		{
			info[k].tentacleInfo[i].push_back(temp);
		}
	}

	//初始化地图，基类指针
	mapInfo = &data.gameMap;

	return info;
}

//存活玩家数不足2人或超过最大回合数，游戏结束
//【不冲突】就只有player的操作
bool Game::isValid()
{
	if (playerAlive == 1 || currentRound >= _MAX_ROUND_)
		return false;
	else
		return true;
}

//【冲突】兵团移动或传输资源的结算（既有兵团又有塔）
void Game::takeEffect(TransEffect& te)
{
	Tentacle& t = *data.tentacles[te.m_source][te.m_target];
	t.takeEffect(te);
	data.cells[te.m_source].addResource(te.m_resourceToSource);
	if(abs(te.m_resourceToTarget) > 0.0001)//有效
		if (data.cells[te.m_target].getPlayerID() != Neutral)
			data.cells[te.m_target].addResource(te.m_resourceToTarget);
		else//目标为中立
			data.cells[te.m_target].N_addOcuppyPoint(data.cells[te.m_source].getPlayerID(), -te.m_resourceToTarget);
	te.handle = false;
#ifdef FC15_DEBUG
	cout << te.m_source << " -> " << te.m_target;
	switch (te.m_type)
	{
	case moveE:
		cout << " 移动： ";
		break;
	case transE:
		cout << " 传输： ";
		break;
	default:
		break;
	}
	cout << " 对源兵塔 " << te.m_source << " : " << te.m_resourceToSource
		<< " 对目标兵塔 " << te.m_target << " : " << te.m_resourceToTarget
		<< " 自身改变: " << te.m_resourceChange
		<< "/" << te.m_frontChange << "/" << te.m_backChange << endl;
#endif
}

//【不冲突】主要是访问塔，但regenerate()有对player属性的访问
void Game::regeneratePhase()
{
	/*//回复科技点数
	for (int i = 0;i!=data.PlayerNum;++i)
	{
		Player& p = data.players[i];
		if (p.isAlive())
			p.regenerateTechPoint();
	}*/
	//每个兵塔回复资源//同时回复势力科技点数
	for (int i = 0;i!=data.CellNum;++i)
	{
		if (data.cells[i].getPlayerID() != Neutral) {
			data.cells[i].regenerate();
		}
	}
}

//【可能冲突】主要是兵线的操作，但takeEffect()涉及塔属性的访问，主要OwnerChange既有兵线有有塔会冲突
void Game::movePhase()
{
	TransEffect** TE = new TransEffect*[data.CellNum];
	//得到TE表，对应的触手先移动，然后对源塔和目标塔的资源数进行结算
	for (int i = 0; i != data.CellNum; ++i)
	{
		TE[i] = new TransEffect[data.CellNum];//这样调用的是默认构造函数，TE.handle=false不用处理（即无i->j的兵线）
		for (int j = 0; j != data.CellNum; ++j)
		{
			if (!data.tentacles[i][j])
				continue;
			TE[i][j] = data.tentacles[i][j]->move();//move只给出兵线、两头塔的资源数要变化多少，但是变化值还没加上去
		}
	}
	//处理TE表
	for (int i = 0; i != data.CellNum; ++i)
	{
		for (int j = 0; j != data.CellNum; ++j)
		{
			if (TE[i][j].handle)
			{
				if (TE[j][i].handle)//对面也有兵线
				{
					if (data.tentacles[i][j]->getstate() == Extending
						&& data.tentacles[j][i]->getstate() == Extending)//都在延伸中
					{
						if (data.tentacles[i][j]->getResource() + data.tentacles[j][i]->getResource()
							+ TE[i][j].m_resourceChange + TE[j][i].m_resourceChange <= data.tentacles[i][j]->getLength() / 10)//尚未触碰
						{
							takeEffect(TE[i][j]); takeEffect(TE[j][i]);
						}
						else//聚在中间，两条兵线这一回合要碰头了
						{
							TResourceD distance = data.tentacles[i][j]->getLength() * Density - (data.tentacles[i][j]->getResource() + data.tentacles[j][i]->getResource());
							TE[i][j].m_resourceChange = TE[j][i].m_resourceChange = distance / 2;
							TE[i][j].m_resourceToSource = TE[j][i].m_resourceToSource = -distance / 2;//在两条兵线的中间碰头
							//判定攻防
							if (data.tentacles[i][j]->getResource() + distance / 2 > data.tentacles[i][j]->getLength() *Density / 2)//越过一半
							{
								data.tentacles[i][j]->setstate(Backing);
								data.tentacles[j][i]->setstate(Attacking);
							}
							else
							{
								data.tentacles[j][i]->setstate(Backing);
								data.tentacles[i][j]->setstate(Attacking);
							}
							takeEffect(TE[i][j]); takeEffect(TE[j][i]);
						}
					}
					else if (data.tentacles[i][j]->getstate() == Extending
						&& data.tentacles[j][i]->getstate() == AfterCut)//我方在延伸，对方被切断
					{
						takeEffect(TE[j][i]);
						handleExtending(TE[i][j]);
					}
					else if (data.tentacles[i][j]->getstate() == Attacking
						&& data.tentacles[j][i]->getstate() == Backing)//我方在进攻，对方在后退
					{
						if (data.tentacles[i][j]->getResource() + TE[i][j].m_resourceChange > data.tentacles[i][j]->getLength()*Density / 2) //即将推到中点
						{
							TResourceD distance = data.tentacles[i][j]->getLength() *Density / 2 - data.tentacles[i][j]->getResource();//离中点的距离
							TE[i][j].m_resourceChange = TE[j][i].m_resourceToSource = distance;
							TE[i][j].m_resourceToSource = TE[j][i].m_resourceChange = -distance;
							data.tentacles[j][i]->setstate(Confrontation);
							data.tentacles[i][j]->setstate(Confrontation);
							takeEffect(TE[i][j]); takeEffect(TE[j][i]);
						}
						else//顺其自然
						{
							takeEffect(TE[i][j]); takeEffect(TE[j][i]);
						}
					}
					else if (data.tentacles[i][j]->getstate() == Backing
						&& data.tentacles[j][i]->getstate() == Attacking)//与上面相反
					{
						if (data.tentacles[j][i]->getResource() + TE[j][i].m_resourceChange > data.tentacles[j][i]->getLength()*Density / 2) //即将推到中点
						{
							TResourceD distance = data.tentacles[j][i]->getLength() *Density / 2 - data.tentacles[j][i]->getResource();//离中点的距离
							TE[j][i].m_resourceChange = TE[i][j].m_resourceToSource = distance;
							TE[j][i].m_resourceToSource = TE[i][j].m_resourceChange = -distance;
							data.tentacles[i][j]->setstate(Confrontation);
							data.tentacles[j][i]->setstate(Confrontation);
							takeEffect(TE[j][i]); takeEffect(TE[i][j]);
						}
						else//顺其自然
						{
							takeEffect(TE[j][i]); takeEffect(TE[i][j]);
						}
					}
					else if (data.tentacles[i][j]->getstate() == Confrontation
						&& data.tentacles[j][i]->getstate() == Confrontation)//双方在对峙
					{
						takeEffect(TE[i][j]); takeEffect(TE[j][i]);
					}
					else if (data.tentacles[i][j]->getstate() == AfterCut
						&& data.tentacles[j][i]->getstate() == Extending)//我方被切断，对方在进攻
					{
						takeEffect(TE[i][j]);
						handleExtending(TE[j][i]);
					}
					else if (data.tentacles[i][j]->getstate() == AfterCut
						&& data.tentacles[j][i]->getstate() == AfterCut)//双方都被切断
					{
						takeEffect(TE[i][j]);
						takeEffect(TE[j][i]);
					}
				}
				else//对方塔没有兵线，扑了个空
				{
					switch (data.tentacles[i][j]->getstate())
					{
					case Extending:
						handleExtending(TE[i][j]);
						break;
					case Arrived:
						takeEffect(TE[i][j]);
						break;
					case AfterCut:
						takeEffect(TE[i][j]);
						break;
					default:
						break;
					}
				}
			}
		}
	}
	//看是否出现势力转移
	OwnerChange(TE);
	for (int i = 0; i != data.CellNum; ++i)
		delete[] TE[i];
	delete[] TE;
}


//【冲突】transport()主要涉及塔的操作，但是这个transPhase()整体是兵线的操作
//【冲突】takeEffect()有对塔属性的访问，应该没关系。但OwnerChange()中同时包含塔和兵线的操作，是冲突的
void Game::transPhase()
{
	TransEffect** TE = new TransEffect*[data.CellNum];
	//得到TE表
	for (int i = 0; i != data.CellNum; ++i)
	{
		TE[i] = new TransEffect[data.CellNum];
		for (int j = 0; j != data.CellNum; ++j)
		{
			if (!data.tentacles[i][j])
				continue;
			TE[i][j] = data.tentacles[i][j]->transport();
		}
	}
	for (int i = 0; i != data.CellNum; ++i)
		for (int j = 0; j != data.CellNum; ++j)
			if (data.tentacles[i][j])
				takeEffect(TE[i][j]);
	OwnerChange(TE);
	for (int i = 0; i != data.CellNum; ++i)
		delete[] TE[i];
	delete[] TE;
}

void Game::beginPhase()
{

}


//【冲突】比较麻烦塔、兵线、player操作都有了
void Game::endPhase()
{
	//前后都传输完的兵线直接从兵线表里去掉
	//【不冲突】只有兵线的操作
	for (int i = 0;i!=data.CellNum;++i)
		for (int j = 0; j != data.CellNum; ++j)
		{
			if (data.tentacles[i][j]
				&& data.tentacles[i][j]->getstate() == AfterCut
				&& data.tentacles[i][j]->getFrontResource() <= 0.0001
				&& data.tentacles[i][j]->getBackResource() <= 0.0001) //实际已经没了
			{
				data.tentacles[i][j]->finish();
				delete data.tentacles[i][j];
				data.tentacles[i][j] = nullptr;
				data.TentacleNum--;
			}
		}
	//【不冲突】只有player操作
	for (int i = 0; i != data.PlayerNum; ++i)
	{
		if (data.players[i].isAlive() && data.players[i].cells().empty())
			killPlayer(i);
	}
	//更新兵塔信息
	//【冲突】这一部分涉及塔，也涉及兵线有关参数
	for (int i = 0; i != data.CellNum; ++i)
	{
		data.cells[i].updateProperty();
	}
	//排名信息
	//【不冲突】只有player操作
	vector<std::pair<TPlayerID, TResourceD> > playerpair;
	for (int i = 0;i!=data.PlayerNum;++i)
	{
		if (data.players[i].isAlive())
			playerpair.push_back({ i,data.players[i].totalResource() });
		else
			playerpair.push_back({ i,data.players[i].getdeadRound() - 1000 });
	}
	std::sort(playerpair.begin(), playerpair.end(),
		[](const std::pair<TPlayerID, TResourceD>& a, std::pair<TPlayerID, TResourceD>& b) {return a.second > b.second; });
	int i = 0;
	//录入排名
	//【不冲突】相当于只有player操作
	for (auto &p : playerpair)
	{
		Rank[i++] = p.first;
	}
}


//【冲突】既有player的操作也有cell的操作
//【冲突】因为添加兵线则还有兵线操作
void Game::commandPhase(vector<CommandList>& command_list)
{
	for (int i = 0; i != playerSize; ++i)
	{
		controlCount[i] = 0;  //已经执行的操作个数
		for (Command& c : command_list[i])
		{
			//操作数个数溢出
			if (data.players[i].maxControlNumber() <= controlCount[i])
				break;
			switch (c.type)
			{
			case addTentacle: 
			{
				if (c.parameters.size() < 2)break;//操作数过少
				TCellID source = c.parameters[0];
				TCellID target = c.parameters[1];
				if (data.cells[source].getPlayerID() == i)//是己方细胞
					data.cells[source].addTentacle(target);
				//写JSON
			}
			break;
			case cutTentacle:
			{
				if (c.parameters.size() < 3)break;//操作数过少
				TCellID source = c.parameters[0];
				TCellID target = c.parameters[1];
				TPosition pos = c.parameters[2];
				if (data.cells[source].getPlayerID() == i)//是己方细胞
					data.cells[source].cutTentacle(target, pos);
				//写JSON
			}
			break;
			case changeStrategy:
			{
				if (c.parameters.size() < 1)break;//操作数过少
				TCellID cell = c.parameters[0];
				CellStrategy nextStg = static_cast<CellStrategy>(c.parameters[1]);
				if (data.cells[cell].getPlayerID() == i)//是己方细胞
					data.cells[cell].changeStg(nextStg);
				//写JSON
			}
			break;
			case upgrade:
			{
				if (c.parameters.size() < 1)break;//操作数过少
				TPlayerProperty upgradeType = static_cast<TPlayerProperty>(c.parameters[0]);
				//直接由player访问修改塔的等级，不需要判断是否己方细胞
				data.players[i].upgrade(upgradeType);
				//写JSON
			}
			break;
			default:
				break;
			}
			controlCount[i]++;
		}
		
	}
}

//【不冲突】只涉及player
void Game::killPlayer(TPlayerID id)
{
	data.players[id].Kill();
	data.players[id].setdeadRound(currentRound);
	playerAlive--;
}

//【不冲突】应该是只涉及兵团操作，不过takeEffect()里面有对cell属性的访问
void Game::handleExtending(TransEffect& te)
{
	//下面是通常的处理Extend的方法
	int i = te.m_source;
	int j = te.m_target;
	//把兵线当做进度条处理，进度条满了就是抵达目标塔了
	if (data.tentacles[i][j]->getResource() + te.m_resourceChange > data.tentacles[i][j]->getLength()*Density)//即将抵达，即在这一回合内进度条要满了，也就是要抵达了
	{
		TResourceD distance = data.tentacles[i][j]->getLength() *Density - data.tentacles[i][j]->getResource();//离目标的距离
		te.m_resourceChange = distance;//进度条拉满，需要多少资源就给多少，不多也不少
		te.m_resourceToSource = -distance;
		data.tentacles[i][j]->setstate(Arrived);//直接把进度条拉满，让兵线抵达目标塔
		takeEffect(te);//兵线抵达目标塔结算效果
	}
	else
		takeEffect(te);//兵线没抵达目标塔结算效果，也就是兵线增加资源，源塔消耗资源
}

//【冲突】前一部分在访问兵线的属性(通过TE表)，后一部分在访问塔的属性
void Game::OwnerChange(TransEffect** TE)
{
	for (int i = 0; i != data.CellNum; ++i)
	{
		if (data.cells[i].getResource()!=Neutral && data.cells[i].getResource() <= 0 )
		{
			set<TPlayerID> Cuter, Arriver, Confrontationer;//对应切断、压制、对峙三种攻击优先等级
			for (int j = 0; j != data.CellNum; ++j)//考虑所有到这个i号塔的兵线
			{
				if (data.tentacles[j][i]
					&& data.cells[j].getPlayerID() != data.cells[i].getPlayerID())//存在兵线且不是自己到自己塔的兵线
				{
					switch (TE[j][i].m_currstate)
					{
					case AfterCut:
						if(TE[j][i].m_resourceToTarget <= -0.01) //确实造成有效攻击的切断兵线，就是切断后兵线前方能向源塔输送资源数的情况
							Cuter.insert(TE[j][i].m_currOwner);
						break;
					case Arrived:
						Arriver.insert(TE[j][i].m_currOwner);
						break;
					case Attacking:
					case Backing:
					case Confrontation:
						Confrontationer.insert(TE[j][i].m_currOwner);
						break;
					default:
						break;
					}
				}
			}
			//依据攻击等级判定归属，优先级切断>压制>对峙
			if (!Cuter.empty())
				if (Cuter.size() == 1)
					data.cells[i].changeOwnerTo(*Cuter.begin());
				else
					data.cells[i].changeOwnerTo(Neutral);
			else if (!Arriver.empty())
				if (Arriver.size() == 1)
					data.cells[i].changeOwnerTo(*Arriver.begin());
				else
					data.cells[i].changeOwnerTo(Neutral);
			else if(!Confrontationer.empty())
				if (Confrontationer.size() == 1)
					data.cells[i].changeOwnerTo(*Confrontationer.begin());
				else
					data.cells[i].changeOwnerTo(Neutral);
			else//没有人在进攻,直接切断所有了事
				for (int j = 0; j != data.CellNum; ++j)
					data.cells[i].cutTentacle(j, 1000000/*一个很大的数*/);//直接切断都回收了
		}
		else if (data.cells[i].getPlayerID() == Neutral && data.cells[i].getOccupyPoint() > data.cells[i].getResource() / 3)//中立改变
		{
			data.cells[i].changeOwnerTo(data.cells[i].getOccupyOwner());
		}
	}
}
