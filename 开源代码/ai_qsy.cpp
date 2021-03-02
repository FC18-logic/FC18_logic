#include "ai.h"
#include "definition.h"
#include "user_toolbox.h"
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <queue>
#include <time.h>
//玩家ai代码写在player_ai中，此函数每回合运行1次，用于每回合获取玩家的命令
//游戏通过Info型实参给玩家player_ai函数传递信息，包括场上塔、兵团、地图等部分信息
//玩家通过info.myCommandList.addCommmand(<命令类型>,<参数列表:参数1,参数2...>)添加命令
//每回合命令最多不超过50条，myCommandList的addCommand方法已经对此有限制，超过50条的命令自动抛弃
//如果玩家修改参数，使得最后传回游戏的myCommandlist中有超过50条命令，游戏会自动判定玩家出局

struct myCorpsinfo {

	TPoint despos;
	TPoint	pos;		//兵团坐标
	int		level;		//兵团等级
	TCorpsID		ID;	//兵团ID
	THealthPoint	HealthPoint;	//生命值
	TBuildPoint		BuildPoint;		//劳动力
	TPlayerID		owner;			//所属玩家ID
	corpsType       type;           //兵团种类
	TMovePoint      movePoint;      //行动力
	battleCorpsType		m_BattleType;	//战斗兵用
	constructCorpsType	m_BuildType;	//建造兵用
};
struct myCorpsinfo convertinfo(CorpsInfo corp) {
	struct myCorpsinfo mycorp;
	mycorp.pos = corp.pos;
	mycorp.ID = corp.ID;
	mycorp.HealthPoint = corp.HealthPoint;
	mycorp.BuildPoint = corp.BuildPoint;
	mycorp.owner = corp.owner;
	mycorp.type = corp.type;
	mycorp.movePoint = corp.movePoint;
	mycorp.m_BattleType = corp.m_BattleType;
	mycorp.m_BuildType = corp.m_BuildType;
	mycorp.despos.m_x = INF;
	mycorp.despos.m_y = INF;
	return mycorp;
}
struct mymapBlock                                 //【FC18】地图方格类
{
	terrainType type;                           //【FC18】地块类型，对应terrainType枚举类
	vector<int> occupyPoint;                    //【FC18】各玩家的占有属性值，秩为玩家序号-1
	int owner;                                  //【FC18】所属玩家序号，-1为过渡TRANSITION，-2为公共PUBLIC
	int TowerIndex;								//@@@【FC18】位于该单元格的塔的下标，对应data里的myTowers
	int isdes;									//是否是目的地，避免重复，该变量在五六十回合后不再有用
	bool iscons;								//是否已经建好了或到达了
	int movePoint;								//移动难度
	TPoint pos;
};
struct tmpMap {
	struct tmpMap* father;
	struct tmpMap* son;
	int point;
};
class myculture {
public:
	myculture() {}
	void updatemyculture(Info& info) {
		for (int i = 0; i < info.corpsInfo.size(); i++) {
			if (info.corpsInfo[i].owner == info.myID) {
				if (info.corpsInfo[i].type == 0) {
					myBattlecorps.push_back(convertinfo(info.corpsInfo[i]));
				}
				else {
					myConstructcorps.push_back(convertinfo(info.corpsInfo[i]));
				}
			}
		}
		for (int i = 0; i < info.towerInfo.size(); i++) {
			if (info.towerInfo[i].ownerID == info.myID) {
				mytower.push_back(info.towerInfo[i]);
			}
		}
	}
	vector<myCorpsinfo> myBattlecorps;
	vector<myCorpsinfo> myConstructcorps;
	vector<TowerInfo> mytower;
};
TPoint hqpos;
int horizdir;
int vertdir;

void updatehq(Info& info) {
	for (int i = 0; i < info.towerInfo.size(); i++) {
		if (info.towerInfo[i].ownerID == info.myID) {
			hqpos = info.towerInfo[i].position;
		}
	}
}

//获取目标位置
TPoint getdestiny(TPoint curpos, int mid_x, int mid_y, Info info) {
	bool newp = 0;
	for (int j = 0; j < 30; j++) {
		for (int i = 0; i < info.totalTowers; i++) {
			if (getDist(info.towerInfo[i].position, curpos) < 2) {
				newp = 1;
				break;
			}
		}
		if (newp == 1) {
			newp = 0;
			srand(j);
			curpos.m_x += (rand() % 2) * (horizdir == 3 ? 1 : -1);
			curpos.m_y += (rand() % 2) * (vertdir == 0 ? 1 : -1);
			if (curpos.m_x > 14 || curpos.m_y > 14 || curpos.m_x < 0 || curpos.m_y < 0) {
				curpos.m_x = (horizdir == 3 ? 0 : 14);
				curpos.m_y = (vertdir == 0 ? 0 : 14);
			}
		}
	}
	return curpos;

}

void player_ai(Info& info)
{
	int tasknum = 0;//记录执行了多少条命令
	myculture cul;
	cul.updatemyculture(info);
	/*if (info.totalRounds == 1) {
		for (int i = 0; i < info.gameMapInfo->size(); i++) {
			for (int j = 0; j < (*info.gameMapInfo)[0].size(); j++) {
				myMap[i][j].isdes = 0;
				myMap[i][j].iscons = 0;
			}
		}
		myMap[hqpos.m_y][hqpos.m_x].isdes = 1;	//已被占据或被设为目标
	}*/
	if (info.myID == 1)
	{
		hqpos.m_x = 0;
		hqpos.m_y = 0;
	}
	else if (info.myID == 2)
	{
		hqpos.m_x = 14;
		hqpos.m_y = 0;
	}
	else if (info.myID == 3)
	{
		hqpos.m_x = 14;
		hqpos.m_y = 14;
	}
	else
	{
		hqpos.m_x = 0;
		hqpos.m_y = 14;
	}
	const int height = 15;
	const int width = 15;
	if (info.totalRounds == 1) {
		if (hqpos.m_x < width / 2)
			horizdir = 3;		//向右
		else
			horizdir = 2;
		if (hqpos.m_y < height / 2)
			vertdir = 0;        //向上
		else
			vertdir = 1;
	}
	//不够十座塔就造到十座塔
	if (cul.mytower.size() + cul.myConstructcorps.size() < 10) {
		for (unsigned int i = 0; i < cul.mytower.size() && i < 10 - cul.mytower.size() - cul.myConstructcorps.size(); ) {
			if (cul.mytower[i].pdtType == -1)
			{
				info.myCommandList.addCommand((commandType)1, vector<int>{0, cul.mytower[i].ID, 4});
				i++;
			}
		}
	}
	for (unsigned int i = 0; i < cul.myConstructcorps.size(); i++) {
		if ((*info.gameMapInfo)[cul.myConstructcorps[i].pos.m_y][cul.myConstructcorps[i].pos.m_x].TowerIndex == -1)
			info.myCommandList.addCommand((commandType)0, vector<int>{7, cul.myConstructcorps[i].ID});
		else {
			TPoint target = getdestiny(cul.myConstructcorps[i].pos, 8, 8, info);
			enum corpsMoveDir dir;
			if (rand() % 2 == 0) {
				if (target.m_x - cul.myConstructcorps[i].pos.m_x > 0) 
					dir = CRight;
				else
					dir = CLeft;
			}
			else {
				if (target.m_y - cul.myConstructcorps[i].pos.m_y > 0) 
					dir = CUp;
				else
					dir = CDown;
			}
			info.myCommandList.addCommand((commandType)0, vector<int>{0, cul.myConstructcorps[i].ID, dir});
		}
	}
	for (unsigned int i = 0; i < cul.mytower.size(); i++) {
		if (cul.mytower[i].pdtType == -1) {
			info.myCommandList.addCommand((commandType)1, vector<int>{0, cul.mytower[i].ID, rand()%3});
		}
	}
	for (unsigned int i = 0; i < cul.myBattlecorps.size(); i++) {
		int RAND = rand() % 2;
		if ( RAND == 0) {
		//兵团移动
			TPoint target = getdestiny(cul.myBattlecorps[i].pos, 8, 8, info);
			corpsMoveDir dir;
			if (rand() % 2 == 0) {
				if (target.m_x - cul.myBattlecorps[i].pos.m_x > 0)
					dir = CRight;
				else
					dir = CLeft;
			}
			else {
				if (target.m_y - cul.myBattlecorps[i].pos.m_y > 0)
					dir = CUp;
				else
					dir = CDown;
			}
			info.myCommandList.addCommand(corpsCommand, vector<int>{CMove, cul.myBattlecorps[i].ID, dir});
		}
		else if (RAND == 1) {
			//兵团驻扎
			if ((*info.gameMapInfo)[cul.myBattlecorps[i].pos.m_y][cul.myBattlecorps[i].pos.m_x].TowerIndex != -1)
				info.myCommandList.addCommand(corpsCommand, vector<int>{CStationTower, cul.myBattlecorps[i].ID, info.towerInfo[(*info.gameMapInfo)[cul.myBattlecorps[i].pos.m_y][cul.myBattlecorps[i].pos.m_x].TowerIndex].ID});
		}
	}
	for (unsigned int i = 0; i < cul.mytower.size(); i++) {
		//塔攻击
		for (unsigned int j = 0; j < info.totalCorps; j++) {
			if (info.corpsInfo[j].owner != info.myID)
				if (getDist(info.corpsInfo[j].pos, cul.mytower[i].position) < 3)
					info.myCommandList.addCommand(towerCommand, { CAttackCorps, cul.mytower[i].ID, info.corpsInfo[j].ID });
		}
	}
	for (unsigned int i = 0; i < cul.myBattlecorps.size(); i++) {
		//兵团攻击
		for (unsigned int j = 0; j < info.totalCorps; j++) {
			if (info.corpsInfo[j].owner != info.myID)
				if (getDist(info.corpsInfo[j].pos, cul.myBattlecorps[i].pos) < 2)
					info.myCommandList.addCommand(corpsCommand, { CAttackCorps, cul.myBattlecorps[i].ID, info.corpsInfo[j].ID });
		}
	}

}