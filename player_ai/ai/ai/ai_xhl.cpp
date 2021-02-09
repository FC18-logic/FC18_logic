#include "ai.h"
#include "definition.h"
#include "user_toolbox.h"
#include <iostream>
#include <vector>
#include <ctime>
//玩家ai代码写在player_ai中，此函数每回合运行1次，用于每回合获取玩家的命令
//游戏通过Info型实参给玩家player_ai函数传递信息，包括场上塔、兵团、地图等部分信息
//玩家通过info.myCommandList.addCommmand(<命令类型>,<参数列表:参数1,参数2...>)添加命令
//每回合命令最多不超过50条，myCommandList的addCommand方法已经对此有限制，超过50条的命令自动抛弃
//如果玩家修改参数，使得最后传回游戏的myCommandlist中有超过50条命令，游戏会自动判定玩家出局
void player_ai(Info& info)
{
	//无限建塔流
	//cxy
	vector<int> MyTower;
	vector<int> MyCorps;
	srand(time(NULL));
	for (int i = 0; i < info.towerInfo.size(); i++)
		if (info.towerInfo[i].ownerID == info.myID)MyTower.push_back(i);
	for (int i = 0; i < info.corpsInfo.size(); i++)
		if (info.corpsInfo[i].owner == info.myID)MyCorps.push_back(i);
	for (int i = 0; i < MyTower.size(); i++)
		if (info.towerInfo[MyTower[i]].pdtType != -1)
		{
			if (MyTower.size() <= 9)info.myCommandList.addCommand(towerCommand, { TProduct, MyTower[i], PExtender });
			else info.myCommandList.addCommand(towerCommand, { TProduct, MyTower[i], PUpgrade });
		}
	for (int i = 0; i < MyCorps.size(); i++)
		if (info.corpsInfo[MyCorps[i]].movePoint > 0)
		{
			int nowx = info.corpsInfo[MyCorps[i]].pos.m_x, nowy = info.corpsInfo[MyCorps[i]].pos.m_y;
			if ((*info.gameMapInfo)[nowy][nowx].owner == info.myID && (*info.gameMapInfo)[nowy][nowx].type != TRTower)
			{
				info.myCommandList.addCommand(corpsCommand, { CBuild, MyCorps[i] });
			}
			else
			{
				info.myCommandList.addCommand(corpsCommand, { CMove, MyCorps[i], rand() % 4 });
			}
		}
}