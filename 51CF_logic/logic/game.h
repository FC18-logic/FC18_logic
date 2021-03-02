//Git bashed file here
#ifndef GAME_H
#define GAME_H

#include "definition.h"
#include "data.h"
#include "player.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
using std::vector;
using std::string;
using std::size_t;
using std::cout;
using std::cin;
using std::endl;

class Game                                                            //【FC18】当前游戏进程类
{
public:
	Game() {}														  //【FC18】当前游戏进程类构造函数
	vector<time_t> roundTime;                                         //【FC18】游戏每回合时间   #json
	bool init(string filename, string json_filename,vector<string> players_name, std::ofstream& cmdF,std::ofstream& infoF);  //@@@【FC18】从文件读入信息，初始化游戏，并写第0轮的JSON
	DATA::Data& getData() { return data; }                            //【FC18】从game类获取当前游戏信息   #json
	vector<TPlayerID>& getRank() { return Rank; }                     //【FC18】获取当前所有玩家的排名
	int getTotalPlayerNum() { return totalPlayers; }             //【FC18】获取当前玩家数目
	int getTotalPlayerAlive() { return playerAlive; }
	TRound getCurrentRound() { return totalRounds; }                  //【FC18】获取当前回合数
	void DebugPhase();                                                //@@@【FC18】输出调试信息
	Info generatePlayerInfo(TPlayerID id);                            //@@@【FC18】得到分配给每个ai代码dll的参数info向量
	bool isValid();                                                   //【FC18】判定是否结束
	//@@@【FC18】根据规则每回合分过程的执行与结算函数
	void commandPhase(vector<CommandList>& command_list);             //@@@【FC18】处理玩家指令
	bool isAlive(TPlayerID id) { return data.players[id].isAlive(); } //【FC18】判断玩家是否活着
	void saveJson(ofstream& infoFile);                                                  //【FC18】写入这一轮的Json数据
	void printJson();                                                 //【FC18】将JSON写入文件
	void addRound() { totalRounds++; }               //@@@【FC18】回合数递增
	bool goNext();                                                   //【FC18】判断还能否继续进行
	void setGameOver() {}

	void setPlayerID(TPlayerID id){ curPlayer = id; }				//【FC18】设置当前回合玩家ID
	void printGameMap();                                             //【FC18】输出当前游戏地图
	void regeneratePhase();                                          //【FC18】重生阶段，兵团生命力恢复
	void beginPhase();                                               //【FC18】开始阶段，塔的生产任务结算
	void endPhase(vector<vector<Command>>& cmd2operate, TPlayerID id);             //【FC18】每位玩家指令结算后的操作
	bool recordOldCommand(string command_path);                     //【FC18】从旧的指令文档中恢复
	//std::ofstream& cmdFile;                                           //【FC18】存放指令数据的文件
	//std::ofstream& infoFile;                                          //【FC18】存放信息数据的文件
	int gameState = Normal;                                          //【FC18】游戏运行的状态




protected:

	DATA::Data data;                                                  //【FC18】所有的数据存放处
	TRound totalRounds;                                               //【FC18】当前回合数
	size_t totalPlayers;                                              //【FC18】玩家总数
	size_t playerAlive;                                               //【FC18】存活人数
	vector<TPlayerID> Rank;                                           //【FC18】根据规则进行排名


	string cmd_txt_filename;                                         //【FC18】指令TXT文件名
	string info_txt_filename;                                        //【FC18】信息TXT文件名
	string mapinfo_txt_filename;                                     //【FC18】地图信息TXT文件名

	TPlayerID curPlayer;	//【FC18】本回合正在执行的玩家 吕梦欣
};

#endif // !GAME_H
