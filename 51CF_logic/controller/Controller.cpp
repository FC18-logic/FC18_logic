#include "Controller.h"
#include <ctime>
#include <math.h>
#ifdef FC15_DEBUG
#define _COMMAND_OUTPUT_ENABLED_
#endif // FC15_DEBUG
namespace DAGAN
{
	using namespace std;

	TPoint moveDir[4] =    //移动方向，与enum corpsMoveDir枚举类型对应
	{// dx, dy
		{0,-1},      //上，-y
		{0, 1},      //下，+y
		{-1,0},      //左，-x
		{1, 0}       //右，+x
	};

	void Controller::run(char* json_filename)
	{
	
		//#json getInitdata 
		data->currentRoundJson.clear();
		data->cutTentacleInfoJson.clear();
		data->cutTentacleBornJson.clear();
		data->cutTentacleJson.clear();
		data->cutTentacleBornJson.assign(data->CellNum, vector<bool>(data->CellNum, false));
		data->cutTentacleJson.assign(data->CellNum, vector<bool>(data->CellNum, false));
		data->cutTentacleInfoJson.resize(data->CellNum, vector<CutTentacleInfoJson>(data->CellNum));

		DATA::Data dataCopyLastRound = *data;
		DataSupplement dataSuppleMent;
		{
			dataCopyLastRound.PlayerNum = data->PlayerNum;
			dataCopyLastRound.TentacleNum = data->TentacleNum;
			dataCopyLastRound.CellNum = data->CellNum;

			dataCopyLastRound.players = new Player[data->PlayerNum];
			for (int i = 0; i != data->PlayerNum; i++)
			{
				dataCopyLastRound.players[i] = Player(data->players[i]);
			}

			dataCopyLastRound.cells = new Cell[data->CellNum];
			dataSuppleMent.cellTechPoint.resize(data->CellNum);
			for (int i = 0; i != data->CellNum; i++)
			{
				dataCopyLastRound.cells[i] = Cell(data->cells[i]);
				dataSuppleMent.cellTechPoint[i] = data->cells[i].techRegenerateSpeed(); //科技点数
			}

			dataCopyLastRound.tentacles = new Tentacle**[data->CellNum];
			for (int i = 0; i != data->CellNum; i++)
			{
				dataCopyLastRound.tentacles[i] = new Tentacle*[data->CellNum];
				for (int j = 0; j != data->CellNum; j++)
				{
					if (data->tentacles[i][j])
					{
						Tentacle* tem = new Tentacle(*data->tentacles[i][j]);
						dataCopyLastRound.tentacles[i][j] = tem;
					}
					else
						dataCopyLastRound.tentacles[i][j] = nullptr;
				}
			}
		}
		data->currentRoundJson["currentRound"] = Json::Value(game_.getRound() + 1);


		int playerSize = game_.getPlayerSize();
		volatile TRound round = game_.getRound();

		if (file_output_enabled_ && !ofs.is_open())
		{
			//char buffer[1024];
			//time_t t = time(0);
			//strftime(buffer, sizeof(buffer), "log_%Y%m%d_%H%M%S.txt", localtime(&t)); // #?json
			//ofs.open(buffer);
		}

		if (!silent_mode_) cout << "-=-=-=-=-=-=-=-=-=-=-= Controller: Round[" << round << "] =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
		// 每个玩家开始运行

		if (debug_mode)
			game_.DebugPhase();
		game_.beginPhase();  //空的函数
		game_.regeneratePhase();  //每个塔/兵团按照规则来恢复属性

		//把玩家、塔和兵团的信息都集中收集起来
		vector<Info> info_list = game_.generateInfo();
		vector<CommandList> commands; //选手命令
		for (TPlayerID id = 0; id < playerSize; ++id)
		{
			Player_Code& player = players_[id];
			if (player.isValid() && game_.isAlive(id))
			{
				// 单个玩家执行，运行玩家ai获取指令
				if (!silent_mode_) cout << "Calling Player " << (int)id << "'s Run() method" << endl;
				//run运行dll，然后把对应的myCommandList(由dll修改)回传到这里
				players_[id].run(info_list[id]);
				commands.push_back(info_list[id].myCommandList);
			}
			else
			{
				players_[id].kill();
				commands.push_back(CommandList());
			}

		}
		// 直接输出此玩家的操作
#ifdef _COMMAND_OUTPUT_ENABLED_
		if (file_output_enabled_)
		{
			vector<string> stg2str{ "Normal","Attack","Defence","Grow" };
			vector<string> up2str{ "Regeneration","ExtendingSpeed","ExtraControl","Wall" };
			for (TPlayerID id = 0; id != playerSize; ++id)
			{
				if (!game_.isAlive(id))
					continue;
				cout << "Player " << id << "'s commands:" << endl;
				for (Command& c : commands[id])
				{
					switch (c.type)
					{
					case addTentacle:
						cout << "Add a line from " << c.parameters[0] << " to " << c.parameters[1] << endl;
						break;
					case cutTentacle:
						cout << "Cut the line from " << c.parameters[0] << " to " << c.parameters[1] << " at the position of " << c.parameters[2] << endl;
						break;
					case changeStrategy:
						cout << "Change the strategy of tower " << c.parameters[0] << " to " << stg2str[c.parameters[1]] << endl;
						break;
					case upgrade:
						cout << "Upgrade " << up2str[c.parameters[0]] << endl;
						break;
					default:
						break;
					}
				}
			}
		}
		if (file_output_enabled_) cout << endl;
#endif
		isValid_ = game_.isValid();
		if (!isValid())
		{
			if (!silent_mode_)
			{
				cout << "-=-=-=-=-=-=-=-=-=-=-= GAME OVER ! =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
				cout << "Rank:" << endl;
				for (TPlayerID id : game_.getRank())
				{
					cout << "Player " << id << " : " << players_[id].getName() << endl;
				}
			}
		}
		//执行移动、势力转换和攻防结算的部分
		else //isValid
		{
			//各种塔的操作在这里
			game_.commandPhase(commands);//读取玩家命令，然后有效命令写进JSON里面去，然后执行一些加兵线之类的操作
			game_.movePhase();//兵线的推移和结算
			game_.transPhase();//兵线的传输和结算
			game_.endPhase();//兵线的切断和结算
		}
		// check if killed
		//检查并判断玩家是否出局
		for (TCellID i = 0; i < playerSize; ++i)
			if (!game_.isAlive(i))
				players_[i].kill();
		//回合数增加1
		game_.addRound();

		//#json save
		{
			game_.saveJson(dataCopyLastRound, dataSuppleMent);
			game_.roundTime.push_back(clock());
			data->currentRoundJson["runDuration"] =
				int(game_.roundTime[game_.roundTime.size() - 1] - game_.roundTime[game_.roundTime.size() - 2]);

			data->root["head"]["totalRounds"] = round + 1;
			data->root["body"].append(data->currentRoundJson);

			//输出到文件 #json 
			Json::FastWriter fw;
			ofstream json_os;
			json_os.open(json_filename);
			json_os << fw.write(data->root);
			json_os.close();
		}
	}
	/***********************************************************************************************
	*函数名 :【FC18】run单玩家回合运行函数
	*函数功能描述 : 生成当前游戏信息，向玩家ai发出，接受命令表之后依次执行，执行每个有效命令后修改
					Data修改DATA::Data中所有变化的数据，然后调用函数jsonChange修改命令Json的数据，
					执行完后要判断玩家是否出局以及游戏是否继续进行。这位玩家的命令全部执行完后，再
					修改场上信息的Json数据。
	*函数参数 : id<TPlayerID>--当前玩家id，json_filename<char*>--Json文件名前缀
	*函数返回值 : 无
	*作者 : 姜永鹏
	***********************************************************************************************/
	void Controller::run(TPlayerID id, char* json_filename) {  //@@@【FC18】每个玩家依次执行
		//清空上一回合记录的Json数据
		data->currentRoundCommandJson.clear();
		data->currentRoundPlayerJson.clear();
		data->currentRoundTowerJson.clear();
		data->currentRoundCorpsJson.clear();
		data->currentRoundMapJson.clear();

		//data内的回合数据更新+1
		volatile TRound dataRound = data->addRound();
		//Json更新data中的回合数据，此时game中回合数据还保持在上一回合
		data->currentRoundCommandJson["round"] = dataRound;
		data->currentRoundPlayerJson["round"] = dataRound;
		data->currentRoundTowerJson["round"] = dataRound;
		data->currentRoundCorpsJson["round"] = dataRound;
		data->currentRoundMapJson["round"] = dataRound;
		int playerSize = game_.getTotalPlayerNum();
		volatile TRound round = game_.getCurrentRound() + 1;

		if (!silent_mode_) cout << "-=-=-=-=-=-=-=-=-=-=-= Controller: Round[" << round << "] =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
		if (debug_mode) {
			game_.DebugPhase();      //调试阶段：输出调试信息
		}
		game_.beginPhase();          //启动阶段：玩家/塔/兵团/地图每回合开始前的准备工作放在这里
		game_.regeneratePhase();     //恢复阶段：玩家/塔/兵团/地图属性要进行的恢复放在这里

		//为玩家的AI代码生成数据
		Info info2Player = game_.generatePlayerInfo(id);
		vector<CommandList> commands; //选手命令
		Player_Code& player = players_[id - 1];  //取出玩家对应的ai代码类
		if (player.isValid() && game_.isAlive(id))  //绝不运行出错的ai代码，绝不运行出局玩家的ai代码
		{
			// 单个玩家执行，运行玩家ai获取指令
			if (!silent_mode_) cout << "Calling Player " << (int)id << "'s Run() method" << endl;
			//run运行dll，然后把对应的myCommandList(由dll修改)回传到这里
			players_[id].run(info2Player);
			commands.push_back(info2Player.myCommandList);
		}
		else
		{
			players_[id].kill();
			commands.push_back(CommandList());
		}


		// 直接输出此玩家的操作
#ifdef _COMMAND_OUTPUT_ENABLED_
		if (file_output_enabled_ && game_.isAlive(id))
		{
			if (game_.isAlive(id)) {
				cout << "Player " << id << "'s commands:" << endl;
				for (Command& c : commands[id])
				{
					switch (c.type)
					{
					default:
						break;
					}
				}
			}
		}
		if (file_output_enabled_) cout << endl;
#endif
		isValid_ = game_.isValid();
		if (!isValid())
		{
			if (!silent_mode_)
			{
				cout << "-=-=-=-=-=-=-=-=-=-=-= GAME OVER ! =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
				cout << "Rank:" << endl;
				for (TPlayerID id : game_.getRank())
				{
					cout << "Player " << id << " : " << players_[id].getName() << endl;
				}
			}
		}
		//执行移动、势力转换和攻防结算的部分
		else //isValid
		{
			//各种塔的操作在这里
			game_.commandPhase(commands);//读取玩家命令，然后有效命令写进JSON里面去，然后执行一些加兵线之类的操作
			game_.movePhase();//兵线的推移和结算
			game_.transPhase();//兵线的传输和结算
			game_.endPhase();//兵线的切断和结算
		}
		// check if killed
		//检查并判断玩家是否出局
		for (TCellID i = 0; i < playerSize; ++i)
			if (!game_.isAlive(i))
				players_[i].kill();
		//回合数增加1
		game_.addRound();

		//#json save
		{
			game_.roundTime.push_back(clock());
			data->currentRoundCommandJson["runDuration"] =
				int(game_.roundTime[game_.roundTime.size() - 1] - game_.roundTime[game_.roundTime.size() - 2]);
			game_.saveJson();

			//输出到文件 #json 
			Json::FastWriter fw;
			ofstream json_os;
			json_os.open(json_filename);
			json_os << fw.write(data->root);
			json_os.close();
		}
	}

	/***********************************************************************************************
	*函数名 :【FC18】jsonChange命令Json修改函数
	*函数功能描述 : 将ID为id的玩家下达的命令c录入当前回合命令Json中
	                注意，该玩家的命令是按它命令表中顺序从先至后进行
					的。并且调用本函数前请确保命令执行过，也就是要确
					保每个参数都合法，并且与场上情况对应。
	*函数参数 : id<TPlayerID>--当前玩家的ID，c<Command&>--当前指令
	*函数返回值 : 无
	*作者 : 姜永鹏
	***********************************************************************************************/
	
	
	void Controller::jsonChange(TPlayerID id, Command& c) {
		if (c.cmdType == corpsCommand) {
			Json::Value newCmd;
			newCmd["cT"] = Json::Value(int(corpsCommand));
			newCmd["tp"] = Json::Value(int(c.parameters[0]));
			newCmd["id"] = Json::Value(int(c.parameters[1]));
			switch (c.parameters[0])  //第0个参数
			{
			case(CMove):
				newCmd["mv"] = Json::Value(int(c.parameters[2]));
				newCmd["dir"] = Json::Value(std::atan2(DAGAN::moveDir[c.parameters[2]].m_y, DAGAN::moveDir[c.parameters[2]].m_x));
				break;
			case(CStation):
				break;
			case(CStationTower):
				newCmd["dFT"] = Json::Value(int(c.parameters[2]));
				break;
			case(CAttackCorps):
				newCmd["dEC"] = Json::Value(int(c.parameters[2]));
				TPoint dirTPoint = data->myCorps[c.parameters[2]].getPos() - data->myCorps[c.parameters[1]].getPos();
				newCmd["dir"] = Json::Value(std::atan2(dirTPoint.m_y, dirTPoint.m_x));
				break;
			case(CAttackTower):
				newCmd["dET"] = Json::Value(int(c.parameters[2]));
				TPoint dirTPoint = data->myTowers[c.parameters[2]].getPosition() - data->myCorps[c.parameters[1]].getPos();
				newCmd["dir"] = Json::Value(std::atan2(dirTPoint.m_y, dirTPoint.m_x));
				break;
			case(CRegroup):
				newCmd["dFC"] = Json::Value(int(c.parameters[2]));
				break;
			case(CBuild):
				break;
			case(CRepair):
				newCmd["dFT"] = Json::Value(int(c.parameters[2]));
				break;
			case(CChangeTerrain):
				newCmd["dT"] = Json::Value(int(c.parameters[2]));
				break;
			default:;
			}
			data->currentRoundCommandJson["command"].append(newCmd);
		}
		else if (c.type == towerCommand) {
			Json::Value newCmd;
			newCmd["cT"] = Json::Value(int(towerCommand));
			newCmd["tp"] = Json::Value(int(c.parameters[0]));
			newCmd["id"] = Json::Value(int(c.parameters[1]));
			switch (c.parameters[0]) 
			{
			case(TProduct):
				newCmd["pT"] = Json::Value(int(c.parameters[2]));
				break;
			case(TAttackCorps):
				newCmd["dEC"] = Json::Value(int(c.parameters[2]));
				TPoint dirTPoint = data->myCorps[c.parameters[2]].getPos() - data->myTowers[c.parameters[1]].getPosition();
				newCmd["dir"] = Json::Value(std::atan2(dirTPoint.m_y, dirTPoint.m_x));
				break;
			case(TAttackTower):
				newCmd["dET"] = Json::Value(int(c.parameters[2]));
				TPoint dirTPoint = data->myTowers[c.parameters[2]].getPosition() - data->myTowers[c.parameters[1]].getPosition();
				newCmd["dir"] = Json::Value(std::atan2(dirTPoint.m_y, dirTPoint.m_x));
				break;
			default:;
			}
			data->currentRoundCommandJson["command"].append(newCmd);
		}
	}
}

