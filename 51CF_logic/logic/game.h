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
	bool init(string filename, char* json_filename,vector<string> players_name);                  //@@@【FC18】从文件读入信息，初始化游戏，并写第0轮的JSON
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
	void killPlayer(TPlayerID id);                                    //@@@【FC18】杀死玩家 
	bool isAlive(TPlayerID id) { return data.players[id].isAlive(); } //【FC18】判断玩家是否活着
	void saveJson(DATA::Data & dataLastRound, DataSupplement & dataSuppleMent);//？？？【FC18】写入这一轮的JSON文档
	void saveJson();                                                  //【FC18】写入这一轮的Json数据
	void addRound() { totalRounds++; }               //@@@【FC18】回合数递增
	bool goNext();                                                   //【FC18】判断还能否继续进行
	void setGameOver() {}

	void setPlayerID(TPlayerID id){ curPlayer = id; }				//【FC18】设置当前回合玩家ID

	//FC15的
	vector<Info> generateInfo();
	TRound getRound() { return currentRound; }//获取回合数
	int getPlayerSize() { return data.PlayerNum; }//获取玩家数量
	void regeneratePhase();    //恢复阶段
	void movePhase();          //触手移动
	void transPhase();         //传输/攻击阶段
	void beginPhase();      //减小各种回合数
	void endPhase();  //删除无用触手

protected:

	std::ofstream LogFile;
	DATA::Data data;                                                  //【FC18】所有的数据存放处
	TRound totalRounds;                                               //【FC18】当前回合数
	size_t totalPlayers;                                              //【FC18】玩家总数
	size_t playerAlive;                                               //【FC18】存活人数
	vector<TPlayerID> Rank;                                           //【FC18】根据规则进行排名


	string cmd_json_filename;                                         //【FC18】指令JSON文件名
	string info_json_filename;                                        //【FC18】信息JSON文件名
	string mapinfo_json_filename;                                     //【FC18】地图信息JSON文件名

	TPlayerID curPlayer;	//【FC18】本回合正在执行的玩家 吕梦欣

	//FC15的
	TRound currentRound;  //当前回合数
	size_t playerSize;    //游戏规模
	TResourceI _MAX_RESOURCE_;//每局特定的最大资源
	vector<int> controlCount;//记录玩家已执行的操作数
	TRound _MAX_ROUND_;//最大回合数
	void takeEffect(TransEffect& te); //将te施用到目标上
	void handleExtending(TransEffect& t);
	void OwnerChange(TransEffect** TE);
};

#endif // !GAME_H
