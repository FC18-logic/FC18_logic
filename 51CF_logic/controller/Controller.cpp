#include "Controller.h"
#include <ctime>
#include <math.h>
#include <cmath>
#include <algorithm>
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

	struct rankCmp {  //用于给玩家排名的结构体
		TPlayerID ID;
		TScore score;
		int CQTowerNum;//攻占塔数
		int ELCorpsNum;//消灭兵团数
		int CPCorpsNum;//俘虏兵团数
		int randomCode;//随机码
		bool operator < (rankCmp& b) {  //重载的比较运算符，用于排名
			if (score > b.score) return true;
			else if (score == b.score) {
				if (CQTowerNum > b.CQTowerNum) return true;
				else if (CQTowerNum == b.CQTowerNum) {
					if (ELCorpsNum > b.ELCorpsNum) return true;
					else if (ELCorpsNum == b.ELCorpsNum) {
						if (CPCorpsNum > b.CPCorpsNum) return true;
						else {
							return (randomCode > b.randomCode);   //在0,1间返回随机数来随机给出排名
						}
					}
					else return false;
				}
				else return false;
			}
			else return false;
		}
	};


	struct configCmdTool { //写Json文档的Cmd部分统一接口需要
		int from_id = -1;
		int cm_type = -1;
		int getcm_id = -1;
		int aim_x = -1;
		int aim_z = -1;
		int result = 0;
		int dT = -1;
		int pT = -1;
		int another_id = -1;
	};

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
	void Controller::run(TPlayerID id, ofstream& cmdFile,ofstream& infoFile) {  //@@@【FC18】每个玩家依次执行
		//清空上一回合记录的Json数据
		/*data->currentRoundCommandJson.clear();
		data->currentRoundPlayerJson.clear();
		data->currentRoundTowerJson.clear();
		data->currentRoundCorpsJson.clear();
		data->currentRoundMapJson.clear();*/

		//清空上一回合塔增加、塔消失、兵团增加、兵团消失的数据
		data->newTower.clear();
		data->dieTower.clear();
		data->newCorps.clear();
		data->dieCorps.clear();
		data->changeTowers.clear();
		data->changeCorps.clear();

		//data内的回合数据更新+1
		volatile TRound dataRound = data->addRound();
		//Json更新data中的回合数据，此时game中回合数据还保持在上一回合
		//data->currentRoundCommandJson["round"] = Json::Value(std::to_string(dataRound));
		//data->currentRoundPlayerJson["round"] = Json::Value(std::to_string(dataRound));
		//data->currentRoundTowerJson["round"] = Json::Value(std::to_string(dataRound));
		//data->currentRoundCorpsJson["round"] = Json::Value(std::to_string(dataRound));

		//data->currentRoundMapJson["round"] = Json::Value(std::to_string(dataRound));
		char buffer[64];
		sprintf(buffer, "\nround %d\n", dataRound);
		cmdFile << buffer;
		infoFile << buffer;
		char bufferCmd[64];
		sprintf(bufferCmd, "#command\n");
		cmdFile << bufferCmd;

		int playerSize = game_.getTotalPlayerNum();
		volatile TRound round = game_.getCurrentRound() + 1;

		if (!silent_mode_) cout << "-=-=-=-=-=-=-=-=-=-=-= Controller: Round[" << round << "] =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
		/*if (debug_mode) {
			game_.DebugPhase();      //@@@调试阶段：输出调试信息
		}*/
		game_.setPlayerID(id);		//@@@设置当前玩家编号
		game_.beginPhase();          //@@@启动阶段：玩家/塔/兵团/地图每回合开始前的准备工作放在这里,现已对FC15代码注释
		game_.regeneratePhase();     //@@@恢复阶段：玩家/塔/兵团/地图属性要进行的恢复放在这里,现已对FC15代码注释

		//为玩家的AI代码生成数据
		Info info2Player = game_.generatePlayerInfo(id);
		CommandList commands; //选手命令
		Player_Code& player = players_[id - 1];  //取出玩家对应的ai代码类
		if (player.isValid() && game_.isAlive(id))  //绝不运行出错的ai代码，绝不运行出局玩家的ai代码
		{
			// 单个玩家执行，运行玩家ai获取指令
			if (!silent_mode_) cout << "Calling Player " << (int)id << "'s run() method" << endl;
			//run运行dll，然后把对应的myCommandList(由dll修改)回传到这里
			//player.run(info2Player);//【FC18】补充超时的判定，命令数过多的判定
			testPlayerCommand(info2Player);
			commands = info2Player.myCommandList;
		}
		else
		{
			player.kill();
			commands = CommandList();
		}
		commandRead = 0;
		//输出玩家命令的标题
#ifdef _COMMAND_OUTPUT_ENABLED_
		if (file_output_enabled_)
			cout << "Player " << id << "'s commands:" << endl;
#endif
		//循环执行玩家命令
		set<TTowerID> towerBanned;//不能再执行操作的塔
		set<TCorpsID> corpsBanned;//不能再执行操作的兵团
		if (commands.size() > MAX_CMD_NUM) {    //命令数超过限制是非法的，说明选手改动了不改改的东西，直接出局
			data->players[id - 1].Kill();
			cout << "Player " << id << " break the rules! Out!" << endl;
		}
	
		//回合数增加1
		game_.addRound();
		//执行本回合命令
		for (Command c : commands.getCommand()) {
			commandRead++;  //更新读取指令数，有效、无效指令都要读取
			if (c.cmdType == corpsCommand) {
				if (c.parameters.size() != CorpsOperaNumNeed[c.parameters[0]]) continue;   //判断操作数合法性
				if (data->newCorps.find(c.parameters[1]) != data->newCorps.end() || corpsBanned.find(c.parameters[1]) != corpsBanned.end()) continue;     //这个兵团本回合不能(再)接受操作，请求驳回
				if (handleCorpsCommand(id, c) == true) {   //记录不能再进行其他操作的兵团序号
					jsonChange(id, c,cmdFile);   //更新有效的指令Json
//					outPutCommand(id, c);  //复读被执行的命令，未执行的不复读
					switch (c.parameters[0]) {
					//case(CStation):
					case(CStationTower):
					case(CBuild):
					case(CRepair):
					case(CChangeTerrain):
						corpsBanned.insert(c.parameters[1]);//记录不能继续操作的兵团ID
						break;
					default:;
					}
				}
				else continue; //指令执行失败，丢弃，读取下一条
			}
			else if (c.cmdType == towerCommand) {
				if (c.parameters.size() != towerOperaNumNeed[c.parameters[0]]) continue;   //判断操作数的数量是否合法
				if (data->newTower.find(c.parameters[1]) != data->newTower.end() || towerBanned.find(c.parameters[1]) != towerBanned.end()) continue; //这个塔当前回合不能(再)操作，请求驳回
				if (handleTowerCommand(id, c) == true) {   //记录不能再进行其他操作的塔序号
					jsonChange(id, c,cmdFile);   //更新有效的指令Json
				    //outPutCommand(id, c);  //复读被执行的命令，未执行的不复读
					switch (c.parameters[0]) {
					case(TProduct):
					case(TAttackCorps):
						towerBanned.insert(c.parameters[1]);//记录不能继续操作的塔ID
						break;
					default:;
					}
				}
				else continue; //指令执行失败，丢弃，读取下一条
			}
			else continue;   //指令有误，直接丢弃
			//指令正常执行后才会到这里
			killPlayers();  //判断设置玩家出局
			if (game_.goNext() == false) {   //设置控制器无效，游戏结束，退出读取命令的循环
				setValid(false);  //关闭Controller，中断游戏，直接game_over
				break;
			}
			if (moreCommand(id, towerBanned, corpsBanned) == false) break;  //接收不了更多命令了，直接跳出
		}
		game_.endPhase(Corpslastcmd,id);
		getGameRank();   //获取游戏中玩家排名

		isValid_ = game_.isValid();
		if (!isValid())
		{
			if (!silent_mode_)
			{
				cout << "-=-=-=-=-=-=-=-=-=-=-= GAME OVER ! =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
				cout << "Rank:" << endl;
				int rank = 0;
				for (TPlayerID r : game_.getRank())
				{
					//
					cout << "Rank " << ++rank << " : player " << players_[r - 1].getName() << endl;
				}
			}
		}


		//#json save，保存Json存档数据
		{
			game_.roundTime.push_back(clock());
			//data->currentRoundCommandJson["runDuration"] =
				//Json::Value(std::to_string(int(game_.roundTime[game_.roundTime.size() - 1] - game_.roundTime[game_.roundTime.size() - 2])));
			game_.saveJson(infoFile);//保存及写入Json文档
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
	
	
	void Controller::jsonChange(TPlayerID id, Command& c, ofstream& cmdFile) {
		configCmdTool newCmd;
		newCmd.from_id = id;
		newCmd.getcm_id = c.parameters[1];
		if (c.cmdType == corpsCommand) {
			//Json::Value newCmd;
			//Json::Value pos;
			TPoint point = data->myCorps[c.parameters[1]].getPos();
			TPoint point2;
			//pos["x"] = Json::Value(std::to_string(point.m_x));
			//pos["z"] = Json::Value(std::to_string(point.m_y));
			//newCmd["spot"] = pos;
			//newCmd["oId"] = Json::Value(std::to_string(id));
			//newCmd["cT"] = Json::Value(std::to_string(int(corpsCommand)));
			//newCmd["tp"] = Json::Value(std::to_string(int(c.parameters[0])));
			//newCmd["id"] = Json::Value(std::to_string(int(c.parameters[1])));
			switch (c.parameters[0])  //第0个参数
			{
			case(CMove):
				newCmd.cm_type = JMove;
				newCmd.aim_x = point.m_x /*+ DAGAN::moveDir[c.parameters[2]].m_x*/;
				newCmd.aim_z = point.m_y /*+ DAGAN::moveDir[c.parameters[2]].m_y*/;
				//newCmd["mv"] = Json::Value(std::to_string(int(c.parameters[2])));
				//newCmd["dir"] = Json::Value(std::to_string(std::atan2(DAGAN::moveDir[c.parameters[2]].m_y, DAGAN::moveDir[c.parameters[2]].m_x)));
				break;
			/*case(CStation):
				newCmd.cm_type = JStation;
				break;*/
			case(CStationTower):
				newCmd.cm_type = JStationTower;
				break;
			case(CAttackCorps):
				newCmd.cm_type = JAttackCorps;
				//point = data->myCorps[c.parameters[1]].getPos();
				point2 = data->myCorps[c.parameters[2]].getPos();
				newCmd.aim_x = point2.m_x;
				newCmd.aim_z = point2.m_y;
				newCmd.another_id = c.parameters[2];
				//pos["x"] = Json::Value(std::to_string(point2.m_x));
				//pos["z"] = Json::Value(std::to_string(point2.m_y));
				//newCmd["pos"] = pos;
				//newCmd["dEC"] = Json::Value(std::to_string(int(c.parameters[2])));
				//TPoint dirTPoint = point2 - point;
				//newCmd["dir"] = Json::Value(std::to_string(std::atan2(dirTPoint.m_y, dirTPoint.m_x)));
				if (data->dieCorps.find(c.parameters[1]) == data->dieCorps.end() && data->dieCorps.find(c.parameters[2]) == data->dieCorps.end()) newCmd.result = 0;     //都还在
				else if (data->dieCorps.find(c.parameters[1]) == data->dieCorps.end() && data->dieCorps.find(c.parameters[2]) != data->dieCorps.end()) newCmd.result = 1;    //自己还在，对方没了
				else if (data->dieCorps.find(c.parameters[1]) != data->dieCorps.end() && data->dieCorps.find(c.parameters[2]) == data->dieCorps.end()) newCmd.result = 2;    //自己没了，对方还在
				else  newCmd.result = 3;    //都没了
				break;
			case(CAttackTower):
				newCmd.cm_type = JAttackTower;
				point2 = data->myTowers[c.parameters[2]].getPosition();
				newCmd.aim_x = point2.m_x;
				newCmd.aim_z = point2.m_y;
				newCmd.another_id = c.parameters[2];
				//pos["x"] = Json::Value(std::to_string(point2.m_x));
				//pos["z"] = Json::Value(std::to_string(point2.m_y));
				//newCmd["pos"] = pos;
				//newCmd["dET"] = Json::Value(std::to_string(int(c.parameters[2])));
				//TPoint dirTPoint = point2 - point;
				//newCmd["dir"] = Json::Value(std::to_string(std::atan2(dirTPoint.m_y, dirTPoint.m_x)));
				if (data->dieCorps.find(c.parameters[1]) == data->dieCorps.end() && data->dieTower.find(c.parameters[2]) == data->dieTower.end()) newCmd.result = 0;    //都还在
				else if (data->dieCorps.find(c.parameters[1]) == data->dieCorps.end() && data->dieTower.find(c.parameters[2]) != data->dieTower.end()) newCmd.result = 1;     //自己还在，对方没了
				else if (data->dieCorps.find(c.parameters[1]) != data->dieCorps.end() && data->dieTower.find(c.parameters[2]) == data->dieTower.end()) newCmd.result = 2;     //自己没了，对方还在
				else  newCmd.result = 3;    //都没了
				break;
			/*case(CRegroup):
				newCmd.cm_type = JRegroup;
				//TPoint point2 = data->myCorps[c.parameters[2]].getPos();
				newCmd.another_id = c.parameters[2];
				//pos["x"] = Json::Value(std::to_string(point2.m_x));
				//pos["z"] = Json::Value(std::to_string(point2.m_y));
				//newCmd["pos"] = pos;
				//newCmd["dFC"] = Json::Value(std::to_string(int(c.parameters[2])));
				//if (data->dieCorps.find(c.parameters[2]) != data->dieCorps.end()) newCmd.result = -1;
				break;*/
			case(CBuild):
				newCmd.cm_type = JBuild;
				newCmd.another_id = data->myTowers.size() - 1;
				break;
			case(CRepair):
				newCmd.cm_type = JRepair;
				break;
			case(CChangeTerrain):
				newCmd.cm_type = JChangeTerrain;
				//newCmd["dT"] = Json::Value(std::to_string(int(c.parameters[2])));
				newCmd.dT = c.parameters[2];
				break;
			default:return;
			}
			//data->currentRoundCommandJson["command"].append(newCmd);
		}
		else if (c.cmdType == towerCommand) {
			TPoint point3;
			//Json::Value newCmd;
			//Json::Value pos;
			//newCmd["oId"] = Json::Value(std::to_string(id));
			//newCmd["cT"] = Json::Value(std::to_string(int(towerCommand)));
			//newCmd["tp"] = Json::Value(std::to_string(int(c.parameters[0])));
			//newCmd["id"] = Json::Value(std::to_string(int(c.parameters[1])));
			switch (c.parameters[0]) 
			{
			case(TProduct):
				newCmd.cm_type = JProduct;
				newCmd.pT = c.parameters[2];
				//newCmd["pT"] = Json::Value(std::to_string(int(c.parameters[2])));
				break;
			case(TAttackCorps):
				newCmd.cm_type = JTowerAttackCorps;
				point3 = data->myCorps[c.parameters[2]].getPos();
				newCmd.aim_x = point3.m_x;
				newCmd.aim_z = point3.m_y;
				newCmd.another_id = c.parameters[2];
				if (data->dieCorps.find(c.parameters[2]) != data->dieCorps.end()) newCmd.result = 1;   //对方没了
				//pos["x"] = Json::Value(std::to_string(point.m_x));
				//pos["z"] = Json::Value(std::to_string(point.m_y));
				//newCmd["pos"] = pos;
				//newCmd["dEC"] = Json::Value(std::to_string(int(c.parameters[2])));
				//TPoint dirTPoint = point2 - data->myTowers[c.parameters[1]].getPosition();
				//newCmd["dir"] = Json::Value(std::to_string(std::atan2(dirTPoint.m_y, dirTPoint.m_x)));
				break;
			//case(TAttackTower):
				//TPoint point = data->myTowers[c.parameters[2]].getPosition();
				//pos["x"] = Json::Value(std::to_string(point.m_x));
				//pos["z"] = Json::Value(std::to_string(point.m_y));
				//newCmd["pos"] = pos;
				//newCmd["dET"] = Json::Value(std::to_string(int(c.parameters[2])));
				//TPoint dirTPoint = point - data->myTowers[c.parameters[1]].getPosition();
				//newCmd["dir"] = Json::Value(std::to_string(std::atan2(dirTPoint.m_y, dirTPoint.m_x)));
				//break;
			default:return;
			}
			//data->currentRoundCommandJson["command"].append(newCmd);
		}
		/*Json::Value newCommand;
		newCommand["oId"] = newCmd.from_id;
		newCommand["tp"] = newCmd.cm_type;
		newCommand["id"] = newCmd.getcm_id;
		newCommand["x"] = newCmd.aim_x;
		newCommand["z"] = newCmd.aim_z;
		newCommand["rst"] = newCmd.result;
		newCommand["dT"] = newCmd.dT;
		newCommand["pT"] = newCmd.pT;
		newCommand["aid"] = newCmd.another_id;
		data->currentRoundCommandJson["command"].append(newCommand);*/
		char bufferCmd[512];
		sprintf(bufferCmd, "%d %d %d %d %d %d %d %d %d\n", newCmd.from_id,
			                                               newCmd.cm_type, 
			                                               newCmd.getcm_id, 
			                                               newCmd.aim_x, 
			                                               newCmd.aim_z,
			                                               newCmd.result,
			                                               newCmd.dT,
			                                               newCmd.pT,
			                                               newCmd.another_id);
		cmdFile << bufferCmd;
	}
	/***********************************************************************************************
	*函数名 :【FC18】moreCommand判断玩家还能否继续下一条指令
	*函数功能描述 : 塔不能再操作条件：这个塔攻击过一次|这个塔正在执行生产任务
	                工程兵团不能再操作条件：工程兵团已经下达工作命令|在驻扎
					战斗兵团不能再操作条件：战斗兵团在驻扎
					这三个条件满足其一，或者：
					已经执行的操作数达到最大操作数，就不再接受命令
	*函数参数 : id<TPlayerID>---玩家id，tBanned<set<TTowerID>&>---不能执行操作的塔ID，cBanned
	            <set<TCorpsID>&>---不能执行操作的兵团ID
	*函数返回值 : <bool>---能否读下一条指令，false---不能，true---能
	*作者 : 姜永鹏
	***********************************************************************************************/
	bool Controller::moreCommand(TPlayerID id, set<TTowerID>& tBanned, set<TCorpsID>& cBanned) {
		bool towerFree = false;  //能进行塔操作
		bool corpsFree = false;  //能进行兵团操作
		for (TTowerID i : data->players[id - 1].getTower()) {
			if (tBanned.find(i) == tBanned.end())//用户现存的塔还有能进行操作的
			{
				towerFree = true;
				break;
			}
		}
		if (!towerFree)   //若塔不能操作了
		{
			for (TCorpsID i : data->players[id - 1].getCrops()) {
				if (cBanned.find(i) == cBanned.end())//用户现存的兵团还有能进行操作的
				{
					corpsFree = true;
					break;
				}
			}
		}
		if (commandRead >= MAX_CMD_NUM || (!towerFree && !corpsFree))//超过最大命令条数，或者没有可操作性的塔或兵团了
			return false;
		else return true;
	}


	/***********************************************************************************************
	*函数名 :【FC18】killPlayers判断玩家出局函数
	*函数功能描述 : 看看哪个玩家防御塔数减到0，让他出局
	*函数参数 : 无
	*函数返回值 : 无
	*作者 : 姜永鹏
	***********************************************************************************************/
	void Controller::killPlayers() {
		for (int i = 0; i < 4; i++) {
			if (data->players[i].getTower().size() <= 0)  //没有防御塔的玩家直接出局，打出局回合的标签
			{
				data->players[i].Kill();
			}
		}
	}


	/***********************************************************************************************
	*函数名 :【FC18】outPutCommand指令输出函数
	*函数功能描述 : 输出当前玩家实际执行的指令
	*函数参数 : 无
	*函数返回值 : 无
	*作者 : 姜永鹏
	***********************************************************************************************/
	void Controller::outPutCommand(TPlayerID id, Command& c) {
		// 直接输出此玩家的操作
#ifdef _COMMAND_OUTPUT_ENABLED_
		if (file_output_enabled_ && game_.isAlive(id))
		{
			if (c.cmdType == corpsCommand) {
				cout << "corps " << c.parameters[1] << " " << CorpsCmd[c.parameters[0]];
				switch (c.parameters[0]) {
				case(CMove):
					cout << " " << Direction[c.parameters[2]];
					break;
				//case(CStation):
				case(CBuild):
				case(CRepair):
				case(CStationTower):
					cout << " at (" << data->myCorps[c.parameters[1]].getPos().m_x << "," << data->myCorps[c.parameters[1]].getPos().m_y << ")";
					break;
				case(CAttackCorps):
				case(CAttackTower):
					cout << " " << c.parameters[2];
					break;
				/*case(CRegroup):
					cout << " with corps " << c.parameters[2];
					break;*/
				case(CChangeTerrain):
					cout << " of (" << data->myCorps[c.parameters[1]].getPos().m_x << "," << data->myCorps[c.parameters[1]].getPos().m_y << ")" << " to " << Terrain[c.parameters[2] - 1];
					break;
				default:;
				}
			}
			else if (c.cmdType == towerCommand) {
				cout << "tower " << c.parameters[1] << " " << TowerCmd[c.parameters[0]];
				switch (c.parameters[0]) {
				case(TProduct):
					cout << " " << ProductCmd[c.parameters[2]];
					break;
				case(TAttackCorps):
				case(TAttackTower):
					cout << " " << c.parameters[2];
					break;
				default:;
				}
			}
			cout << ".";
		}
		if (file_output_enabled_) cout  << endl;
#endif
	}

	/***********************************************************************************************
	*函数名 :【FC18】handleCorpsCommand兵团指令执行函数
	*函数功能描述 : 执行当前兵团指令，并返回是否执行成功
	*函数参数 : 令id<TPlayerID>---当前玩家ID,c<Command&>---当前指
	*函数返回值 : <bool>指令执行情况false---执行成功，true---没有执行成功
	*作者 : 姜永鹏
	***********************************************************************************************/
	bool Controller::handleCorpsCommand(TPlayerID ID, Command& c) {
		//需要return返回命令执行是否成功<bool>
		bool bCmdSucs = false;
		TCorpsID id = c.parameters[1];
		//如果兵团id越界
		if(id < 0 || id >= data->myCorps.size())
			return false;

		//如果兵团id不属于该玩家
		if(data->myCorps[id].getPlayerID() != ID)
		{
			return false;
		}

		//兵团已死亡，判定命令失败
		if (data->myCorps[id].bAlive() == false)
		{
			return false;
		}

		//data->myCorps[id].haveCmd();
		switch (c.parameters[0]) {
		case(CMove):
			//兵团移动的操作
			{
				int dir = c.parameters[2];
				bCmdSucs = data->myCorps[id].Move(dir);
			}
			break;
			
		/*case(CStation)://兵团没有属性值改变
			//兵团原地驻扎的操作
			{
				bCmdSucs = data->myCorps[id].GoRest();
			}
			break;
			*/
		case(CStationTower)://兵团没有属性值改变
			//兵团驻扎当前格防御塔的操作
			{
				bCmdSucs = data->myCorps[id].StationInTower();
			}
			break;
		case(CAttackCorps):
		case(CAttackTower):
			//兵团攻击的操作
			{
				int type = c.parameters[0];
				TCorpsID enemyid = c.parameters[2];
				if (type == CAttackCorps && (enemyid < 0 || enemyid >= data->myCorps.size())) return false;   //越界：操作数非法
				if (type == CAttackTower && (enemyid < 0 || enemyid >= data->myTowers.size())) return false;   //越界：操作数非法
				bCmdSucs = data->myCorps[id].Attack(type,enemyid);
			}
			break;
		/*case(CRegroup)://这个操作就不要了
			//兵团整编的操作
			{
				TCorpsID target = c.parameters[2];
				if (target < 0 || target >= data->myCorps.size()) return false;     //越界：操作数非法
				bCmdSucs = data->myCorps[id].MergeCrops(target);
			}
			break;*/
		case(CBuild):
			//兵团修建新塔的操作
			{
				if (data->players[ID - 1].towerNumControl() == true) 
					return false;
				bCmdSucs = data->myCorps[id].BuildTower();
			}
			break;
		case(CRepair):
			//兵团修理防御塔的操作
			{
				bCmdSucs = data->myCorps[id].MendTower();
			}
			break;
		case(CChangeTerrain):
			//兵团改变方格地形的操作
			{
				//如果地形参数不符合要求
				if(c.parameters[2] < 1 || c.parameters[2] > 5)
				{
					return false;
				}
				//terrainType type = (terrainType)(c.parameters[2]);
				//bCmdSucs = data->myCorps[id].ChangeTerrain(type);
				terrainType type = (terrainType)(c.parameters[2]);
				bCmdSucs = data->myCorps[id].JudgeChangeTerrain(c);
				if (bCmdSucs)
				{
					Corpslastcmd[ID].push_back(c);
				}
			}
			break;
		default:
			return false;
		}
		return bCmdSucs;
	}

	/***********************************************************************************************
	*函数名 :【FC18】handleTowerCommand防御塔指令执行函数
	*函数功能描述 : 执行当前防御塔指令，并返回是否执行成功
	*函数参数 : id<TPlayerID>---当前玩家ID,c<Command&>---当前指令
	*函数返回值 : <bool>指令执行情况false---执行成功，true---没有执行成功
	*作者 : 姜永鹏
	***********************************************************************************************/
	bool Controller::handleTowerCommand(TPlayerID ID, Command& c) {
		//需要return返回命令执行是否成功<bool>
		bool bCmdSucs = false;
		TCorpsID id = c.parameters[1];
		int pdtType = c.parameters[2];
		//塔id越界
		if (id < 0 || id >= data->myTowers.size())
			return false;  
		if (data->myTowers[id].getexsit() == false)
			return false;
		switch (c.parameters[0]) {
		case(TProduct):
			if ((pdtType == PWarrior || pdtType == PArcher || pdtType == PCavalry) && data->players[ID - 1].battleNumControl() == true) return false;
			else if ((pdtType == PBuilder || pdtType == PExtender) && data->players[ID - 1].constructNumControl() == true) return false;
			//设置生产任务，任务种类不越界就判定成功
			bCmdSucs = data->myTowers[id].set_producttype(enum productType(pdtType));
			return bCmdSucs;
			break;
		case(TAttackCorps):
			//by lxj：函数内包含此判断
			//设置攻击目标
			//越界：操作数非法
			//if (c.parameters[2] < 0 || c.parameters[2] >= data->myCorps.size()) 
			//	return false;   
			bCmdSucs = data->myTowers[id].set_attacktarget(c.parameters[2]);
			return bCmdSucs;
			break;
		default:
			return false;
		}
		return bCmdSucs;
	}
	/***********************************************************************************************
	*函数名 :【FC18】getGameRank获取更新游戏排名函数函数
	*函数功能描述 : 计算玩家游戏排名，得到降序排序放到Game::Rank里面
	                第一关键字---玩家得分，第二关键字---玩家占领防御塔个数
					第三关键字---玩家消灭敌方兵团个数，第四关键字---玩家俘虏敌方兵团个数
					之后将随机排序
	*函数参数 : 无
	*函数返回值 : 无
	*作者 : 姜永鹏
	***********************************************************************************************/
	void Controller::getGameRank() {
		vector<rankCmp> Ranker;
		for (int i = 0; i < 4; i++) {
			rankCmp playerRanker;
			Player newPlayer = data->players[i];
			playerRanker.ID = newPlayer.getId();
			playerRanker.score = newPlayer.getPlayerScore();
			playerRanker.CQTowerNum = newPlayer.getCqTowerNum();
			playerRanker.ELCorpsNum = newPlayer.getElCorpsNum();
			playerRanker.CPCorpsNum = newPlayer.getCqCorpsNum();
			bool ok = false;
			do {
				ok = false;
				playerRanker.randomCode = generateRanInt(0, 1000);
				for (int cnt = 0; cnt < i; cnt++) {
					if (playerRanker.randomCode == Ranker[cnt].randomCode) {
						ok = true;
						break;
					}
				}
			} while (ok);
			Ranker.push_back(playerRanker);
			data->players[i].setScore(playerRanker.score);
		}
		std::sort(Ranker.begin(), Ranker.end());  //对玩家排序，按名次升序排序
		for (int i = 1; i <= 4; i++) {
			int Rank = 0;
			for (int j = 0; j < 4; j++) {
				if (Ranker[j].ID == i) {
					Rank = j + 1;
					break;
				}
			}
			data->players[i - 1].setRank(Rank);
			game_.getRank()[Rank - 1] = Ranker[Rank - 1].ID;
		}
	}
	/***********************************************************************************************
	*函数名 :【FC18】testPlayerCommand模拟AI给出玩家命令函数
	*函数功能描述 : 模拟玩家AI，给出游戏指令，方便调试
	*函数参数 : Info& info
	*函数返回值 : 无
	*作者 : 姜永鹏
	***********************************************************************************************/
	void Controller::testPlayerCommand(Info& info) {
		TPlayerID m_ID = info.myID;
		for (TTowerID t : info.playerInfo[m_ID - 1].tower)
		{
			for (int i = 0; i < info.corpsInfo.size(); i++) {
				if (info.corpsInfo[i].owner != m_ID && getDist(info.towerInfo[t].position, info.corpsInfo[i].pos) <= 2) {
					info.myCommandList.addCommand(towerCommand, { TAttackCorps,t,i });
					break;
				}
			}
			if(info.totalRounds >= 6 && (m_ID == 1 || m_ID == 2)) info.myCommandList.addCommand(towerCommand, { TProduct,t,PBuilder});   //让玩家的所有塔生产建造者
			if (info.totalRounds >= 6 && (m_ID == 3 || m_ID == 4)) info.myCommandList.addCommand(towerCommand, { TProduct,t,PExtender });   //让玩家的所有塔生产开拓者
		}
		if (info.totalRounds < 11) {
			for (TCorpsID c : info.playerInfo[m_ID - 1].corps)
			{
				info.myCommandList.addCommand(corpsCommand, { CMove,c,CUp });
				info.myCommandList.addCommand(corpsCommand, { CMove,c,CUp });         //让玩家的所有兵团向上移动
				info.myCommandList.addCommand(corpsCommand, { CMove,c,CRight });
			}
		}
		if (info.totalRounds >= 11)
		{	
			if (m_ID == 3) {
				for (TCorpsID t : info.playerInfo[m_ID - 1].corps)
				{
					info.myCommandList.addCommand(corpsCommand, { CBuild,t });
					info.myCommandList.addCommand(corpsCommand, { CMove,t,CLeft });
				}
			}
			if (m_ID == 1) {
				for (TCorpsID t : info.playerInfo[m_ID - 1].corps)
				{
					info.myCommandList.addCommand(corpsCommand, { CMove,t,CUp });
					if(info.totalRounds % 2 == 0)
						info.myCommandList.addCommand(corpsCommand, { CChangeTerrain,t,TRForest });
					else
						info.myCommandList.addCommand(corpsCommand, { CChangeTerrain,t,TRPlain });
				}
			}
		}
		cout << (*info.gameMapInfo)[4][5].type << "\n";
	}
}



