#include "ai.h"
#include "definition.h"
#include "user_toolbox.h"
#include <iostream>
#include <vector>
#include <cmath>
//玩家ai代码写在player_ai中，此函数每回合运行1次，用于每回合获取玩家的命令
//游戏通过Info型实参给玩家player_ai函数传递信息，包括场上塔、兵团、地图等部分信息
//玩家通过info.myCommandList.addCommmand(<命令类型>,<参数列表:参数1,参数2...>)添加命令
//每回合命令最多不超过50条，myCommandList的addCommand方法已经对此有限制，超过50条的命令自动抛弃
//如果玩家修改参数，使得最后传回游戏的myCommandlist中有超过50条命令，游戏会自动判定玩家出局
void player_ai(Info& info)
{
	int towernum = info.totalTowers;
	int mytowernum = 0;
	int x;
	int y;
	vector<vector<int>> whethertower;
	for (int i = 0; i < 15; i++)
	{
		vector<int> q(15, 0);
		whethertower.push_back(q);
	}
	if (info.myID == 1)
	{
		x = 0;
		y = 0;
	}
	else if (info.myID == 2)
	{
		x = 15;
		y = 0;
	}
	else if (info.myID == 3)
	{
		x = 15;
		y = 15;
	}
	else
	{
		x = 0;
		y = 15;
	}
	vector<int> mytowerID;
	int firsttowernum;
	for (int i = 0; i < towernum; i++)
	{
		if (info.towerInfo[i].ownerID == info.myID - 1)
		{
			mytowerID.push_back(i);
			mytowernum++;
			if (info.towerInfo[i].position.m_x == x && info.towerInfo[i].position.m_y == y)
			{
				firsttowernum = i;
			}
			whethertower[info.towerInfo[i].position.m_x][info.towerInfo[i].position.m_y] = 1;
		}
	}
	int corpsnum = info.totalCorps;
	int mycorpsnum = 0;
	int myextendernum = 0;
	vector<int> myextenderID;
	int mybuildernum = 0;
	vector<int> mybuilderID;
	int myarchornum=0;
	vector<int> myarchorID;
	for (int i = 0; i < corpsnum; i++)
	{
		if (info.corpsInfo[i].owner == info.myID - 1)
		{
			if (info.corpsInfo[i].type == 1 && info.corpsInfo[i].m_BuildType == 1)
			{
				myextendernum++;
				myextenderID.push_back(i);
			}
			if (info.corpsInfo[i].type == 1 && info.corpsInfo[i].m_BuildType == 0)
			{
				mybuildernum++;
				mybuilderID.push_back(i);
			}
			if (info.corpsInfo[i].type == 0 && info.corpsInfo[i].m_BattleType == 1)
			{
				myarchornum++;
				myarchorID.push_back(i);
			}
		}
	}
	if (info.towerInfo[firsttowernum].level != 8)
	{
		info.myCommandList.addCommand(towerCommand, { 0,firsttowernum,PUpgrade });
	}
	else
	{
		if (mybuildernum + mytowernum < 9)
		{
			info.myCommandList.addCommand(towerCommand, { 0,firsttowernum,PExtender });
		}	
	}
	for (int i = 0; i < mytowernum; i++)
	{
		if (info.towerInfo[mytowerID[i]].level != 8)
		{
			info.myCommandList.addCommand(towerCommand, { 0, mytowerID[i],PUpgrade });
		}
		else 
		{
			if (info.towerInfo[mytowerID[i]].healthPoint != 240)
			{
				info.myCommandList.addCommand(towerCommand, { 0,mytowerID[i],PBuilder });
			}
			else
			{
				info.myCommandList.addCommand(towerCommand, { 0,mytowerID[i],PArcher });
			}
		}
	}
	for (int i = 0; i < myextendernum; i++)
	{
		int emptynum=0;
		int destinitionx;
		int destinitiony;
		if (x == 0 && y == 0)
		{
			int j = 1;
			int k = 0;
			while (emptynum != i + 1)
			{
				k = 0;
				while (emptynum != i + 1 && k <= j)
				{
					if (whethertower[k][j] == 0)
					{
						emptynum++;
					}
					k++;
				}
				j++;
			}
			destinitionx = j;
			destinitiony = k;
		}
		if (x == 15 && y == 0)
		{
			int j = 1;
			int k = 0;
			while (emptynum != i + 1)
			{
				k = 0;
				while (emptynum != i + 1 && k <= j)
				{
					if (whethertower[k][j] == 0)
					{
						emptynum++;
					}
					k++;
				}
				j++;
			}
			destinitionx = 15-j;
			destinitiony = k;
		}
		if (x == 0 && y == 15)
		{
			int j = 1;
			int k = 0;
			while (emptynum != i + 1)
			{
				k = 0;
				while (emptynum != i + 1 && k <= j)
				{
					if (whethertower[15-k][j] == 0)
					{
						emptynum++;
					}
					k++;
				}
				j++;
			}
			destinitionx = j;
			destinitiony = 15-k;
		}
		if (x == 15 && y == 15)
		{
			int j = 1;
			int k = 0;
			while (emptynum != i + 1)
			{
				k = 0;
				while (emptynum != i + 1 && k <= j)
				{
					if (whethertower[15-k][15-j] == 0)
					{
						emptynum++;
					}
					k++;
				}
				j++;
			}
			destinitionx = 15-j;
			destinitiony = 15-k;
		}
		if (info.corpsInfo[myextenderID[i]].pos.m_x < destinitionx)
		{
			info.myCommandList.addCommand(corpsCommand, { CMove,myextenderID[i],3 });
		}
		if (info.corpsInfo[myextenderID[i]].pos.m_x > destinitionx)
		{
			info.myCommandList.addCommand(corpsCommand, { CMove,myextenderID[i],2 });
		}
		if (info.corpsInfo[myextenderID[i]].pos.m_y < destinitiony)
		{
			info.myCommandList.addCommand(corpsCommand, { CMove,myextenderID[i],1 });
		}
		if (info.corpsInfo[myextenderID[i]].pos.m_y > destinitiony)
		{
			info.myCommandList.addCommand(corpsCommand, { CMove,myextenderID[i],0 });
		}
		if (info.corpsInfo[myextenderID[i]].pos.m_y == destinitiony && info.corpsInfo[myextenderID[i]].pos.m_x == destinitionx)
		{
			info.myCommandList.addCommand(corpsCommand, { CBuild,myextenderID[i] });
		}
	}
	for (int i = 0; i < mybuildernum; i++)
	{
		info.myCommandList.addCommand(corpsCommand, { CRepair,mybuilderID[i] });
	}
	for (int i = 0; i < info.totalCorps; i++)
	{
		if (info.corpsInfo[i].owner != info.myID - 1)
		{
			for (int j = 0; j < mytowernum; j++)
			{
				if (abs(info.corpsInfo[i].pos.m_x - info.towerInfo[mytowerID[j]].position.m_x) + abs(info.corpsInfo[i].pos.m_y - info.towerInfo[mytowerID[j]].position.m_y) <= 2)
				{
					info.myCommandList.addCommand(towerCommand, { TAttackCorps,mytowerID[j],i });
				}
			}
			for (int j = 0; j < myarchornum; j++)
			{
				if (abs(info.corpsInfo[i].pos.m_x - info.towerInfo[myarchorID[j]].position.m_x) + abs(info.corpsInfo[i].pos.m_y - info.towerInfo[myarchorID[j]].position.m_y) <= 2)
				{
					info.myCommandList.addCommand(corpsCommand, { CAttackCorps,myarchorID[j],i });
				}
			}
		}
	}
}