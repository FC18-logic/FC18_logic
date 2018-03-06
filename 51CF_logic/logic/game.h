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
void sample_ai(Info& info);

class Game
{
public:
	vector<time_t> roundTime; //#json
	bool init(string filename);//初始化信息
	DATA::Data& getData() { return data; } //#json
	vector<TPlayerID> getRank() { return Rank; }
	int getPlayerSize() { return data.PlayerNum; }
	TRound getRound() { return currentRound; }
	void DebugPhase();
	vector<Info> generateInfo();
	bool isValid();//判定是否结束
	void regeneratePhase();    //恢复阶段
	void movePhase();          //触手移动
	void transPhase();         //传输/攻击阶段
	void beginPhase();      //减小各种回合数
	void endPhase();  //删除无用触手
	void commandPhase(vector<CommandList>& command_list); //处理玩家指令
	void killPlayer(TPlayerID id); //杀死玩家 
	bool isAlive(TPlayerID id) { return data.players[id].isAlive(); }
	void saveJson(DATA::Data & dataLastRound, DataSupplement & dataSuppleMent);
	void addRound() { currentRound++; }
protected:
	void takeEffect(TransEffect& te); //将te施用到目标上
	void handleExtending(TransEffect& t);
	std::ofstream LogFile;
	DATA::Data data;      //所有的数据存放处
	TRound currentRound;  //当前回合数
	size_t playerSize;    //游戏规模
	size_t playerAlive;   //存活人数
	vector<TPlayerID> Rank;//根据总资源数排名
	TResourceI _MAX_RESOURCE_;//每局特定的最大资源
	TRound _MAX_ROUND_;
	vector<int> controlCount; //记录操作数
	void OwnerChange(TransEffect** TE);
};

#endif // !GAME_H