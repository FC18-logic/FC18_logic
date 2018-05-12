#include "ai.h"
#include "definition.h"
#include "user_toolbox.h"
#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>

typedef double distant;
int MyMaxControl = 0;
void jiaohuan(distant *a, distant *b)
{
	distant c;
	c = *a;
	*a = *b;
	*b = c;
}
int JuGuard(TTowerID n, TTowerID m[13])
{
	if (n == m[n]) return 1;
	else return 0;
}
void AddCommand(Info&info, CommandType _type, vector<int> _parameters)
{
	if (MyMaxControl > 0)
		info.myCommandList.addCommand(_type, _parameters);
	MyMaxControl -= 1;
}
void AddCommand(Info&info, CommandType _type, int para1)
{
	if (MyMaxControl > 0)
		info.myCommandList.addCommand(_type, para1);
	MyMaxControl -= 1;
}
void AddCommand(Info&info, CommandType _type, int para1, int para2)
{
	if (MyMaxControl > 0)
		info.myCommandList.addCommand(_type, para1, para2);
	MyMaxControl -= 1;
}
void AddCommand(Info&info, CommandType _type, int para1, int para2, int para3)
{
	if (MyMaxControl > 0)
		info.myCommandList.addCommand(_type, para1, para2, para3);
	MyMaxControl -= 1;
}
int linenum(Info&info, TTowerID i)
{
	int t = 0;
	for (int j = 0;j <= 12;j++)
		if (info.lineInfo[j][i].exist&&info.towerInfo[i].owner != info.towerInfo[j].owner&& info.towerInfo[j].owner!=info.myID)
			t++;
	t += 2;
	return t;
}    
int lineenNum(Info&info,TTowerID i)
{
	int t = 0;
	for (int j = 0;j <= 12;j++)
		if (info.lineInfo[j][i].exist&&info.towerInfo[i].owner != info.towerInfo[j].owner&&info.lineInfo[j][i].state == Arrived)
			t++;
	t += 2;
	return t;
}
int lineNum(Info&info, TTowerID i)
{
	int t = 0;
	if (info.towerInfo[i].owner != info.myID)
	{
		for (int j = 0;j <= 12;j++)
			if (info.lineInfo[j][i].exist&& info.towerInfo[j].owner == info.myID&&info.lineInfo[j][i].state == Arrived)
				t++;
	}
	else
	{
		for (int j = 0;j <= 12;j++)
			if (info.lineInfo[j][i].exist&& info.towerInfo[j].owner != info.myID&&info.lineInfo[j][i].state == Arrived)
				t++;
	}
	return t;
}
int Llinewith(Info&info, TTowerID i)
{
	int t = 0,m=0,n=0;
	for (int j = 0;j <= 12;j++)
		if (info.towerInfo[j].owner != info.myID&&info.lineInfo[i][j].exist&&info.lineInfo[i][j].state == Arrived)
			if (!info.lineInfo[j][i].exist)
				t++;
			else
				m++;
	for (int j = 0;j <= 12;j++)
		if (info.towerInfo[j].owner != info.myID&&info.lineInfo[j][i].exist&&info.lineInfo[j][i].state == Arrived)
			if (!info.lineInfo[i][j].exist)
				n++;
	if(t+m>0&&n==0)return 1;
	else return 0;
}
int Linesupport(Info&info, TTowerID i)
{
	int t = 0;
	for (int j = 0;j <= 12;j++)
		if (info.towerInfo[j].owner == info.myID&&info.lineInfo[j][i].exist)
			t++;
	if (t >= 3)return 0;
	else return 1;
}
TResourceD LineSum(Info&info, TTowerID i)
{
	TResourceD sum = 0;
	for (int j = 0;j <= 12;j++)
		if (info.towerInfo[j].owner == info.myID&&info.lineInfo[j][i].exist)
			sum += info.lineInfo[j][i].resource;
	return sum;
}
void player_ai(Info& info)
{
	int EnemyLine = 0, TowerGuard = 0,tower = 0;
	TResourceD Sum = 0.0;
	for(tower = 0;tower <= 12;tower++)
	{
		if (info.towerInfo[tower].owner == info.myID)
		{
			TowerGuard++;
			Sum += info.towerInfo[tower].resource;
			for (int i = 0;i <= 12;i++)
				if (info.towerInfo[i].owner != info.myID&&info.lineInfo[i][tower].exist)
					EnemyLine++;
		}
	}
	int EnemyAmount = 1, EnemyGuard = 2;
	if (Sum > 600)EnemyAmount++;
	const int TtowerMax = 100, TtowerMin = 49;
	MyMaxControl = info.myMaxControl;
	int mind = -1;
	int TtowerIdGuard[13] = { -1 }, TtowerIdEnguard[13] = { -1 }, safe[13] = { 0 };
	distant enemydist[13] = { 0 }, enemyNum[13] = { 0 }, EnemyNum[13] = { 0 };
	for (tower = 0;tower <= 12;tower++)
	{
		if (info.towerInfo[tower].owner == info.myID)                                                                                     //所有己方塔
			for (int i = 0;i <= 12;i++)
				if (info.towerInfo[i].owner != info.myID)                                                                                       //所有己方塔与敌方塔距离之和
				{
					enemydist[tower] += getDistance(info.towerInfo[tower].position, info.towerInfo[i].position);
				}
		if (info.towerInfo[tower].owner != info.myID)
			for (int i = 0;i <= 12;i++)
				if (info.towerInfo[i].owner == info.myID)
				{
					enemydist[tower] -= getDistance(info.towerInfo[tower].position, info.towerInfo[i].position);
				}
	}
	for (tower = 0;tower <= 12;tower++)
	{
		if (info.towerInfo[tower].owner == info.myID)
		{
			enemyNum[tower] = enemydist[tower];
			EnemyNum[tower] = enemydist[tower];
		}
		else
		{
			enemyNum[tower] = enemydist[tower] * info.towerInfo[tower].resource* info.towerInfo[tower].resource*linenum(info, tower)*linenum(info, tower)*linenum(info, tower);
			EnemyNum[tower] = enemydist[tower] * info.towerInfo[tower].resource* info.towerInfo[tower].resource*linenum(info, tower)*linenum(info, tower)*linenum(info, tower);
		}
	}
	for (int i = 0;i < 12;i++)
		for (int j = i + 1;j <= 12;j++)
			if (enemyNum[i] < enemyNum[j])  jiaohuan(&enemyNum[i], &enemyNum[j]);
	for (int i = 0;i <= 12;i++)
		if (enemyNum[i] > 0 && enemyNum[i + 1] < 0) mind = i;
	for (tower = 0;tower <= 12;tower++)
	{
		for (int i = 0;i <= 12;i++)
			if (info.towerInfo[tower].owner == info.myID)
				if (info.towerInfo[i].owner != info.myID&&info.lineInfo[i][tower].exist)
					safe[tower] = 1;
		//for (int j = 0;j < EnemyGuard;j++)
		//	if (mind - j >= 0)
		//	{
		//		if (EnemyNum[tower] == enemyNum[mind - j] && (safe[tower] == 0 || info.towerInfo[tower].resource > TtowerMin))TtowerIdGuard[tower] = tower;
		//		/*if (info.towerInfo[tower].resource <= TtowerMin && safe[tower] == 1)TtowerIdEnguard[tower] = tower;*/
		//	}
		if (info.towerInfo[tower].owner == info.myID&&Llinewith(info, tower))
			TtowerIdGuard[tower] = tower;
		for (int k = 0;k < EnemyAmount;k++)
			if (mind + 1 + k <= 12)
				if (EnemyNum[tower] == enemyNum[mind + 1 + k])TtowerIdEnguard[tower] = tower;
	}
	for (tower = 0;tower <= 12;tower++)
	{
		if (info.round <=50)
		{
			if (EnemyLine == 0 && info.towerInfo[tower].owner == info.myID)
			{
				if (info.towerInfo[tower].strategy != Grow && info.towerInfo[tower].resource <= TtowerMax)
					if ((info.towerInfo[tower].strategy == Normal && info.playerInfo[info.myID].technologyPoint >= 3) || info.playerInfo[info.myID].technologyPoint >= 5)
						AddCommand(info, changeStrategy, tower, Grow);
				for (int i = 0;i <= 12;i++)
					for (int j = 0;j <= 12;j++)
						if (info.towerInfo[i].owner == info.myID&&info.towerInfo[j].owner != info.myID&&info.lineInfo[i][j].exist)
							AddCommand(info, cutLine, i, j, 100);
			}
			else if (info.towerInfo[tower].owner == info.myID)
			{
				for (int i = 0;i <= 12;i++)
					if (info.towerInfo[i].owner != info.myID&&info.lineInfo[i][tower].exist)
					{
						if (info.lineInfo[tower][i].state != Confrontation && info.lineInfo[tower][i].state != Extending)
							if (info.towerInfo[tower].maxLineNum > info.towerInfo[tower].currLineNum)
								if (info.towerInfo[tower].resource > TtowerMin)
									AddCommand(info, addLine, tower, i);
						if (info.towerInfo[tower].strategy != Defence && lineNum(info, tower)>0)
							if ((info.towerInfo[tower].strategy == Normal && info.playerInfo[info.myID].technologyPoint >= 3) || info.playerInfo[info.myID].technologyPoint >= 5)
								AddCommand(info, changeStrategy, tower, Defence);
					}
			}
		}
		else
		{
			if (info.towerInfo[tower].owner == info.myID)
			{
				if (info.towerInfo[tower].resource < TtowerMin && safe[tower])
				{
					if (  info.towerInfo[tower].strategy != Defence)
						if ((info.towerInfo[tower].strategy == Normal && info.playerInfo[info.myID].technologyPoint >= 3) || info.playerInfo[info.myID].technologyPoint >= 5)
							AddCommand(info, changeStrategy, tower, Defence);
					for (int i = 0;i <= 12;i++)
					{
						if (info.towerInfo[i].owner == info.myID)
						{
							if (info.lineInfo[tower][i].exist)
								AddCommand(info, cutLine, tower, i, 100);
							if (info.lineInfo[i][tower].exist&&info.towerInfo[i].owner == info.myID&&info.lineInfo[i][tower].state == Arrived)
							{
								if (safe[i] == 0)
									AddCommand(info, cutLine, i, tower, 0);
								else if (info.towerInfo[i].resource >= 2 * TtowerMin)
									AddCommand(info, cutLine, i, tower, 30);
								else if (info.towerInfo[i].resource >= TtowerMin)
									AddCommand(info, cutLine, i, tower, 70);
							}
						}
						else
						{
							if (!info.lineInfo[i][tower].exist&&info.lineInfo[tower][i].exist)
								AddCommand(info, cutLine, tower, i, 50);
						}
						if (info.towerInfo[tower].resource < 30)
						{
							if (info.lineInfo[i][tower].exist&&info.towerInfo[i].owner == info.myID&&info.lineInfo[i][tower].state == Arrived)
								AddCommand(info, cutLine, i, tower, 0);
							if (info.lineInfo[tower][i].exist)
								AddCommand(info, cutLine, tower, i, 100);
						}
					}
				}
				else
				{
					if (tower != TtowerIdGuard[tower])
					{
						if (safe[tower] == 1)
						{
							if (info.towerInfo[tower].strategy != Defence&&lineNum(info,tower)>0)
								if ((info.towerInfo[tower].strategy == Normal && info.playerInfo[info.myID].technologyPoint >= 3) || info.playerInfo[info.myID].technologyPoint >= 5)
									AddCommand(info, changeStrategy, tower, Defence);
							for (int i = 0;i <= 12;i++)
								if (info.towerInfo[i].owner != info.myID&&info.lineInfo[i][tower].exist)
								{
									if (info.lineInfo[tower][i].state != Confrontation && info.lineInfo[tower][i].state != Extending)
										if (info.towerInfo[tower].maxLineNum > info.towerInfo[tower].currLineNum)
											if (info.towerInfo[tower].resource >  TtowerMin)
												AddCommand(info, addLine, tower, i);
									/*if (info.towerInfo[tower].resource <= TtowerMin)
										TtowerIdEnguard[tower] = tower;*/
								}
						}
						for (int i = 0;i <= 12;i++)
						{
							if (info.towerInfo[i].owner == info.myID && info.lineInfo[tower][i].exist&& info.lineInfo[tower][i].state == Arrived)
								if ( info.towerInfo[tower].resource<60)
									AddCommand(info, cutLine, tower, i, 100);
								else
									AddCommand(info, cutLine, tower, i, 0);
							if (info.towerInfo[i].owner != info.myID && info.lineInfo[tower][i].exist && !info.lineInfo[i][tower].exist && info.lineInfo[tower][i].state == Arrived)
							{
								if ((info.towerInfo[tower].resource > TtowerMin || safe[tower] == 0) && lineenNum(info, i) - lineNum(info, i) <= 3)
									AddCommand(info, cutLine, tower, i, 0);
								else
									AddCommand(info, cutLine, tower, i, 100);
							}
						}
						for (int i = 0;i <= 12;i++)
							if ((JuGuard(i, TtowerIdGuard) && !info.lineInfo[tower][i].exist&&info.towerInfo[i].resource <= TtowerMax-40) || (info.towerInfo[i].owner == info.myID&&info.towerInfo[i].resource <= TtowerMin && !info.lineInfo[tower][i].exist&&i!=12&&Linesupport(info,i)))
								if (info.towerInfo[tower].maxLineNum > info.towerInfo[tower].currLineNum&&info.towerInfo[tower].resource > TtowerMax&& info.round >= 50)//fdsjfhdskj
									AddCommand(info, addLine, tower, i);
						if (safe[tower] == 0 && info.towerInfo[tower].strategy != Grow)
							if ((info.towerInfo[tower].strategy == Normal && info.playerInfo[info.myID].technologyPoint >= 3) || info.playerInfo[info.myID].technologyPoint >= 5)
								AddCommand(info, changeStrategy, tower, Grow);
						for (int i = 0;i <= 12;i++)
							if (info.towerInfo[tower].resource > TtowerMax && !info.lineInfo[tower][i].exist&&TtowerIdEnguard[i] == i)
								AddCommand(info, addLine, tower, i);
					}
					else if (tower == TtowerIdGuard[tower])
					{
						if (info.towerInfo[tower].strategy != Attack && info.towerInfo[tower].resource > TtowerMax-30)
							if ((info.towerInfo[tower].strategy == Normal && info.playerInfo[info.myID].technologyPoint >= 3) || info.playerInfo[info.myID].technologyPoint >= 5)
								AddCommand(info, changeStrategy, tower, Attack);
						if (info.towerInfo[tower].strategy != Grow && info.towerInfo[tower].resource <= TtowerMax - 30 && safe[tower] == 0)
							if ((info.towerInfo[tower].strategy == Normal && info.playerInfo[info.myID].technologyPoint >= 3) || info.playerInfo[info.myID].technologyPoint >= 5)
								AddCommand(info, changeStrategy, tower, Grow);
						if (safe[tower] && info.towerInfo[tower].resource <= TtowerMin + 10 && info.towerInfo[tower].strategy != Defence)
							if ((info.towerInfo[tower].strategy == Normal && info.playerInfo[info.myID].technologyPoint >= 3) || info.playerInfo[info.myID].technologyPoint >= 5)
								AddCommand(info, changeStrategy, tower, Defence);
						for (int i = 0;i <= 12;i++)
						{
							if (JuGuard(i, TtowerIdEnguard) && !info.lineInfo[tower][i].exist && (info.towerInfo[i].strategy != Defence || !info.lineInfo[i][tower].exist))
								if (info.towerInfo[tower].maxLineNum > info.towerInfo[tower].currLineNum)
									if (info.towerInfo[tower].resource > TtowerMax || (tower == 12 && info.towerInfo[tower].resource == 50.0))
										AddCommand(info, addLine, tower, i);
							if (info.towerInfo[i].owner != info.myID&&info.lineInfo[i][tower].exist && !info.lineInfo[tower][i].exist)
								if (info.towerInfo[tower].maxLineNum > info.towerInfo[tower].currLineNum)
									if (info.towerInfo[tower].resource > TtowerMax - 30 || (tower == 12 && info.towerInfo[tower].resource == 50.0))
										AddCommand(info, addLine, tower, i);
							if (info.towerInfo[i].owner != info.myID&&info.lineInfo[tower][i].exist && !info.lineInfo[i][tower].exist&&info.lineInfo[tower][i].state == Arrived && (lineenNum(info, i) - lineNum(info, i))<=2)      //&&LineSum(info, i) > info.towerInfo[i].resource
								AddCommand(info, cutLine, tower, i, 0);
							/*if (info.towerInfo[i].owner != info.myID&&info.lineInfo[tower][i].exist && info.lineInfo[i][tower].exist&& info.towerInfo[i].strategy == Defence&&info.towerInfo[tower].strategy==Attack)
								AddCommand(info, cutLine, tower, i, 100);*/
							if(!JuGuard(i,TtowerIdEnguard)&&!info.lineInfo[i][tower].exist&&info.lineInfo[tower][i].exist)
								AddCommand(info, cutLine, tower, i, 100);
						}
						for (int i = 0;i <= 12;i++)
							if (info.towerInfo[i].owner == info.myID&&info.lineInfo[tower][i].exist && ((safe[i] == 0&&info.towerInfo[i].resource>TtowerMin) || info.towerInfo[i].resource > 2 * TtowerMin))
								AddCommand(info, cutLine, tower, i, 100);
						if (safe[tower] == 1)
						{
							for (int i = 0;i <= 12;i++)
								if (info.towerInfo[i].owner != info.myID&&info.lineInfo[i][tower].exist)
								{
									if (info.lineInfo[tower][i].state != Confrontation && info.lineInfo[tower][i].state != Extending&&info.towerInfo[i].strategy!=Defence)
										if (info.towerInfo[tower].maxLineNum > info.towerInfo[tower].currLineNum)
											if (info.towerInfo[tower].resource >  TtowerMin)
												AddCommand(info, addLine, tower, i);
									/*if (info.towerInfo[tower].resource <= TtowerMin)
									TtowerIdEnguard[tower] = tower;*/
								}
						}
					}
				}
			}
		}
	}
	if (info.playerInfo[info.myID].technologyPoint >((info.playerInfo[info.myID].RegenerationSpeedLevel + 1) * 2) && info.playerInfo[info.myID].RegenerationSpeedLevel < 5 && info.round % 3 == 0)
		AddCommand(info, upgrade, RegenerationSpeed);
	if (info.playerInfo[info.myID].technologyPoint > ((info.playerInfo[info.myID].ExtraControlLevel + 1) * 2) + 1 && info.playerInfo[info.myID].ExtraControlLevel < 3 && info.round % 3 == 0 && info.playerInfo[info.myID].RegenerationSpeedLevel ==5)
		AddCommand(info, upgrade, ExtraControl);
	if (info.playerInfo[info.myID].technologyPoint >((info.playerInfo[info.myID].ExtendingSpeedLevel + 1) * 2) && info.playerInfo[info.myID].ExtendingSpeedLevel < 5 && info.round % 3 == 2 && info.playerInfo[info.myID].ExtraControlLevel == 3 && info.playerInfo[info.myID].RegenerationSpeedLevel == 5)
		AddCommand(info, upgrade, ExtendingSpeed);	
	if (info.playerInfo[info.myID].technologyPoint >((info.playerInfo[info.myID].DefenceLevel + 1) * 2) && info.playerInfo[info.myID].DefenceLevel < 3 && info.round % 3 == 0 && info.playerInfo[info.myID].ExtendingSpeedLevel == 5 && info.playerInfo[info.myID].ExtraControlLevel == 3 && info.playerInfo[info.myID].RegenerationSpeedLevel == 5)
		AddCommand(info, upgrade, Wall);
}