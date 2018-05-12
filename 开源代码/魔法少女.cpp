#include "ai.h"
#include "definition.h"
#include "user_toolbox.h"
#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <cmath>

struct tower
{
	int id;
	int owner;
	int stra;
	double reso;
	double maxreso;
	int line;
	int maxline;
	double distance;
	int enemy;
	double priority;
	double last_round_reso;
};

tower tow[13];
int myid;
int mytow = 0;
bool giveup = 0;
void player_ai(Info& info)
{	
	myid = info.myID;
	mytow =0;
	double playerreso[4];
	playerreso[0] = 0;
	playerreso[1] = 0;
	playerreso[2] = 0;
	playerreso[3] = 0;
	srand(time(0) * (int(info.towerInfo[info.myID].resource)*info.towerInfo[info.myID + 1].resource) + info.myID); //乱打的只为随机
	for (int i = 0; i <= 12; i++)
	{
		tow[i].id = info.towerInfo[i].id;
		tow[i].owner = info.towerInfo[i].owner;
		tow[i].reso = info.towerInfo[i].resource;
		tow[i].line = info.towerInfo[i].currLineNum;
		tow[i].stra = info.towerInfo[i].strategy;
		tow[i].maxline = info.towerInfo[i].maxLineNum;
		tow[i].maxreso = info.towerInfo[i].maxResource;
		tow[i].enemy = 0;
		tow[i].distance = 0;
		tow[i].priority = 0;
	}
	for (int i = 0; i <= 12; i++)
	{
		if (tow[i].owner == myid)
			mytow++;
	}
	for (int i = 0; i <= 12; i++)
	{
		if (tow[i].owner == myid)
		{
			for (int j = 0; j <= 12; j++)
			{
				if (tow[j].owner != myid) { tow[i].distance += 1/(getDistance(info.towerInfo[i].position,info.towerInfo[j].position)); }
			}
			tow[i].distance = 1 / tow[i].distance;
		}
		else 
		{
			for (int j = 0; j <= 12; j++)
			{
				if (tow[j].owner == myid) { tow[i].distance +=1/ (getDistance(info.towerInfo[i].position, info.towerInfo[j].position)); }
			}
			//if (tow[i].stra == 2)
			//	tow[i].distance += 3000;
			tow[i].distance = 1 / tow[i].distance;
		}
	}
	for (int i = 0; i <= 12; i++)
	{
		for (int j = i+1; j <= 12; j++)
		{
			if (tow[i].owner != myid && tow[j].owner == myid)
			{
				tower t;
				t = tow[i];
				tow[i]=tow[j];
				tow[j] = t;
			}
		}
	}
	for (int i = 0; i < mytow; i++)
	{
		for (int j = mytow; j <= 12; j++)
		{
			if (info.lineInfo[tow[j].id][tow[i].id].exist == 1)
				tow[i].enemy++;
		}
	}
	for (int i = 0; i < mytow; i++)
	{
		for (int j = i + 1; j < mytow; j++)
		{
			if (tow[i].reso > tow[j].reso)
			{
				tower t;
				t = tow[i];
				tow[i] = tow[j];
				tow[j] = t;
			}
		}
	}
	for (int i = 0; i <= 12; i++)
	{
		tow[i].priority += tow[i].distance*0.2;
	}
	for (int i = mytow; i <= 12; i++)
	{
		double k;
		if (tow[i].stra == 0)k = 0.5;
		if (tow[i].stra == 1)k = 1.0;
		if (tow[i].stra == 2)k = 3.0;
		if (tow[i].stra == 3)k = 0.2;
		tow[i].priority += k * tow[i].reso;
	}
	for (int i = mytow; i <= 12; i++)
	{
		if (tow[i].stra != 2)
		{
			for (int j = 0; j < mytow; j++)
			{
				if (info.lineInfo[tow[i].id][tow[j].id].exist == 1&& info.lineInfo[tow[i].id][tow[j].id].state!=AfterCut)
					tow[i].priority -= 30;
				if (info.lineInfo[tow[j].id][tow[i].id].exist == 1 && info.lineInfo[tow[j].id][tow[i].id].state != AfterCut)
					tow[i].priority -= 30;
			}
		}
	}
	for (int i = mytow; i <= 12; i++)
	{
		double linereso = 0;
		for (int j = 0; j <= 12; j++)
		{
			if (info.lineInfo[tow[i].id][tow[j].id].exist == 1)
			{
				linereso += info.lineInfo[tow[i].id][tow[j].id].resource;
			}
		}
		tow[i].priority -= linereso;
	}
	for (int i = mytow; i <= 12; i++)
	{
		double playerreso = 0;
		for (int j = mytow; j <= 12; j++)
		{
			if (tow[i].owner == tow[j].owner)
				playerreso += tow[j].reso;
		}
		tow[i].priority += 0.05*playerreso;
	}
	for (int i = mytow; i <= 12; i++)
	{
		double v = tow[i].reso - tow[i].last_round_reso;
		const double e = 2.718281828;
		double delta_priority = pow(e, v);
		tow[i].priority += delta_priority;
	}
	for (int i = mytow; i <= 12; i++)
	{
		for (int j = i + 1; j <= 12; j++)
		{
			if (tow[i].priority > tow[j].priority)
			{
				tower t;
				t = tow[i];
				tow[i] = tow[j];
				tow[j] = t;
			}
		}
	}
	for (int i = 0; i <= 12; i++)
	{
		if(tow[i].owner>=0)
		playerreso[tow[i].owner] += tow[i].reso;
	}
	double sumreso = playerreso[0] + playerreso[1] + playerreso[2] + playerreso[3];
	if (info.round > 80)
	{
		for (int i = 0; i <= 3; i++)
		{
			if (i != myid)
			{
				if (playerreso[i] / sumreso > 0.6)
					giveup = 1;
			}
		}
		if (playerreso[myid] / sumreso < 0.1)
		{
			giveup = 1;
		}
	}
	/*if(info.round==4)
	{
		info.myCommandList.addCommand(addLine, myid * 3 + 1, 12);
		info.myCommandList.addCommand(addLine, myid * 3 + 2, 12);
	}
	if (info.round > 12 && info.round < 18)
	{
		if (info.towerInfo[12].owner == myid)
		{
			if (info.lineInfo[12][myid * 3 + 1].exist == 0)
				info.myCommandList.addCommand(addLine, 12, myid * 3 + 1);
			if (info.lineInfo[12][myid * 3 + 2].exist == 0)
				info.myCommandList.addCommand(addLine, 12, myid * 3 + 2);
		}
	}*/
	if (giveup == 1)
	{
		for (int i = 0; i < mytow; i++)
		{
			for (int j = mytow; j <= 12; j++)
			{
				if (info.lineInfo[tow[i].id][tow[j].id].exist == 1 && info.lineInfo[tow[i].id][tow[j].id].state != AfterCut)
				{
					int place = (int)info.lineInfo[tow[i].id][tow[j].id].maxlength / 10;
					info.myCommandList.addCommand(cutLine, tow[i].id, tow[j].id, place);
				}
			}//缩回所有兵线
			if (info.playerInfo[myid].technologyPoint >= 5.0 && tow[i].stra != 2)
			{
				info.myCommandList.addCommand(changeStrategy, tow[i].id, 2);//改成防御
			}
			double x = tow[i].maxreso;
			double value2 = 0.0026956128*x*x - 0.214699231*x + 44.40298507;
			if (tow[i].reso <= value2)//需要求救
			{
				for (int j = 0; j <mytow; j++)
				{
					if (info.lineInfo[tow[i].id][tow[j].id].exist == 1 && info.lineInfo[tow[i].id][tow[j].id].state != AfterCut)
					{
						int place = (int)info.lineInfo[tow[i].id][tow[j].id].maxlength / 10;
						info.myCommandList.addCommand(cutLine, tow[i].id, tow[j].id, place);
					}
				}//缩回所有兵线
				for (int j = 0; j < mytow; j++)
				{
					double y = tow[j].maxreso;
					double valuej = (80.0 + (y - 120.0)*(y - 120.0)*60.0 / 6400.0);
					if (tow[j].reso >= valuej)
					{
						if (info.lineInfo[tow[j].id][tow[i].id].exist == 0 && tow[j].line < tow[j].maxline)
						{
							info.myCommandList.addCommand(addLine, tow[j].id, tow[i].id);
						}
					}
				}//求救
			}
			double value1 = (80.0 + (x - 120.0)*(x - 120.0)*60.0 / 6400.0);
			if (tow[i].reso > value1)//开始进攻
			{
				for (int j = 0; j < mytow; j++)
				{
					if (info.lineInfo[tow[j].id][tow[i].id].exist == 1 && info.lineInfo[tow[j].id][tow[i].id].state != AfterCut)
					{
						int place = (int)info.lineInfo[tow[i].id][tow[j].id].maxlength / 10;
						info.myCommandList.addCommand(cutLine, tow[j].id, tow[i].id, place);
					}
				}//切断支援
			}
		}
		//return;
	}


	if (giveup == 0)
	{
		for (int i = 0; i < mytow; i++)
		{
			double x = tow[i].maxreso;
			double value0 = 0.00416666666666*x*x - 0.4583333333*x + 95;
			double value1 = (80.0 + (x - 120.0)*(x - 120.0)*60.0 / 6400.0);
			double value2 = 0.0026956128*x*x - 0.214699231*x + 44.40298507;
			double value3 = 0.2*x + 20;
			if (tow[i].reso <= value1 && tow[i].reso > value2)//防守反击
			{
				if (tow[i].enemy == 0)
				{
					if (tow[i].stra != 3 && info.playerInfo[myid].technologyPoint >= 5.0)
						info.myCommandList.addCommand(changeStrategy, tow[i].id, 3);

				}//没敌人时增长
				if (tow[i].enemy > 0)
				{
					bool p = 0;
					for (int j = mytow; j <= 12; j++)
					{
						if (info.lineInfo[tow[j].id][tow[i].id].exist == 1 && info.towerInfo[tow[j].id].strategy == 1)
							p = 1;
					}//如果有人attack我
					if (info.playerInfo[myid].technologyPoint >= 5.0 && tow[i].stra != 2 && (p = 1))
						info.myCommandList.addCommand(changeStrategy, tow[i].id, 2);//改成防御
					for (int j = mytow; j <= 12; j++)
					{
						if (info.lineInfo[tow[j].id][tow[i].id].exist == 1 && info.lineInfo[tow[i].id][tow[j].id].exist == 0 && tow[i].line < tow[i].maxline)
							info.myCommandList.addCommand(addLine, tow[i].id, tow[j].id);
					}//谁打我就打回去
				}
				for (int j = mytow; j <= 12; j++)
				{
					if (info.lineInfo[tow[i].id][tow[j].id].exist == 1 && info.lineInfo[tow[i].id][tow[j].id].state == 4)
					{
						if (tow[j].reso < info.lineInfo[tow[i].id][tow[j].id].maxlength / 10)
							info.myCommandList.addCommand(cutLine, tow[i].id, tow[j].id, 0);
					}
				}//有机会的话偷塔
			}
			if (tow[i].reso <= value3)//需要求救
			{
				for (int j = 0; j <= 12; j++)
				{
					if (info.lineInfo[tow[i].id][tow[j].id].exist == 1 && info.lineInfo[tow[i].id][tow[j].id].state != AfterCut)
					{
						int place = (int)info.lineInfo[tow[i].id][tow[j].id].maxlength / 10;
						info.myCommandList.addCommand(cutLine, tow[i].id, tow[j].id, place);
					}
				}//缩回所有兵线
				if (tow[i].enemy == 0)
				{
					if (tow[i].stra != 3 && info.playerInfo[myid].technologyPoint >= 5.0)
						info.myCommandList.addCommand(changeStrategy, tow[i].id, 3);

				}//没人打我就增长
				if (tow[i].enemy > 0)
				{
					bool p = 0;
					for (int j = mytow; j <= 12; j++)
					{
						if (info.lineInfo[tow[j].id][tow[i].id].exist == 1 && info.towerInfo[tow[j].id].strategy == 1)
							p = 1;
					}
					if (info.playerInfo[myid].technologyPoint >= 5.0 && tow[i].stra != 2)
						info.myCommandList.addCommand(changeStrategy, tow[i].id, 2);
					//改成防御
				}
				for (int j = 0; j < mytow; j++)
				{
					double y = tow[j].maxreso;
					double valuej = (80.0 + (y - 120.0)*(y - 120.0)*60.0 / 6400.0);
					if (tow[j].reso >= valuej)
					{
						if (info.lineInfo[tow[j].id][tow[i].id].exist == 0 && tow[j].line < tow[j].maxline)
						{
							info.myCommandList.addCommand(addLine, tow[j].id, tow[i].id);
						}
					}
				}
			}
			if (tow[i].reso <= value2 && tow[i].reso > value3)//需要自保
			{
				for (int j = 0; j <= 12; j++)
				{
					if (info.lineInfo[tow[j].id][tow[i].id].exist == 0 && info.lineInfo[tow[i].id][tow[j].id].exist == 1 && info.lineInfo[tow[i].id][tow[j].id].state != AfterCut)
					{
						int place = (int)info.lineInfo[tow[i].id][tow[j].id].maxlength / 10;
						info.myCommandList.addCommand(cutLine, tow[i].id, tow[j].id, place);
					}
				}//对方撤军我也撤军
				if (tow[i].enemy == 0)
				{
					if (tow[i].stra != 3 && info.playerInfo[myid].technologyPoint >= 5.0)
						info.myCommandList.addCommand(changeStrategy, tow[i].id, 3);
					//没人打我就grow
				}
				if (tow[i].enemy > 0)
				{
					bool p = 0;
					for (int j = mytow; j <= 12; j++)
					{
						if (info.lineInfo[tow[j].id][tow[i].id].exist == 1 && info.towerInfo[tow[j].id].strategy == 1)
							p = 1;
					}//有人attack我
					if (info.playerInfo[myid].technologyPoint >= 5.0 && tow[i].stra != 2 && (p == 1))
						info.myCommandList.addCommand(changeStrategy, tow[i].id, 2);//改防御
					for (int j = mytow; j <= 12; j++)
					{
						if (info.lineInfo[tow[j].id][tow[i].id].exist == 1 && info.lineInfo[tow[i].id][tow[j].id].exist == 0 && tow[i].line < tow[i].maxline)
							info.myCommandList.addCommand(addLine, tow[i].id, tow[j].id);
					}//谁打我就打谁
				}

			}
			for (int j = mytow; j <= 12; j++)
			{
				if (tow[j].reso < 25)
				{
					if (info.lineInfo[tow[i].id][tow[mytow].id].exist == 1 && info.lineInfo[tow[i].id][tow[j].id].state == 4)
					{
						info.myCommandList.addCommand(cutLine, tow[i].id, tow[mytow].id, 0);
					}
				}
			}
			if (tow[i].reso > value0)//开始进攻
			{
				if (info.playerInfo[myid].technologyPoint >= 5.0 && tow[i].stra != 1)
					info.myCommandList.addCommand(changeStrategy, tow[i].id, 1);//改进攻
				for (int j = 0; j < mytow; j++)
				{
					if (info.lineInfo[tow[j].id][tow[i].id].exist == 1 && info.lineInfo[tow[j].id][tow[i].id].state != AfterCut)
					{
						int place = (int)info.lineInfo[tow[i].id][tow[j].id].maxlength / 10;
						info.myCommandList.addCommand(cutLine, tow[j].id, tow[i].id, place);
					}
				}//切断支援
				for (int j = mytow; j <= 12; j++)
				{
					if (info.lineInfo[tow[j].id][tow[i].id].exist == 1 && info.lineInfo[tow[i].id][tow[j].id].exist == 0 && tow[i].line < tow[i].maxline)
						info.myCommandList.addCommand(addLine, tow[i].id, tow[j].id);
				}//谁打我就打谁
				if (info.lineInfo[tow[i].id][tow[mytow].id].exist == 0 && tow[i].line < tow[i].maxline)
				{
					info.myCommandList.addCommand(addLine, tow[i].id, tow[mytow].id);
				}
				for (int j = mytow + 1; j <= 12; j++)
				{
					if (info.lineInfo[tow[i].id][tow[j].id].exist == 1 && info.lineInfo[tow[i].id][tow[j].id].state != AfterCut && tow[j].priority > 300)
					{
						int place = (int)info.lineInfo[tow[i].id][tow[j].id].maxlength / 10;
						info.myCommandList.addCommand(cutLine, tow[i].id, tow[j].id, place);
					}
				}
			}
			if (tow[i].reso <= value0 && tow[i].reso > value1)
			{
				if (tow[i].stra == 1)
				{
					for (int j = 0; j < mytow; j++)
					{
						if (info.lineInfo[tow[j].id][tow[i].id].exist == 1 && info.lineInfo[tow[j].id][tow[i].id].state != AfterCut)
						{
							int place = (int)info.lineInfo[tow[i].id][tow[j].id].maxlength / 10;
							info.myCommandList.addCommand(cutLine, tow[j].id, tow[i].id, place);
						}
					}//切断支援
					for (int j = mytow; j <= 12; j++)
					{
						if (info.lineInfo[tow[j].id][tow[i].id].exist == 1 && info.lineInfo[tow[i].id][tow[j].id].exist == 0 && tow[i].line < tow[i].maxline)
							info.myCommandList.addCommand(addLine, tow[i].id, tow[j].id);
					}//谁打我就打谁
					if (info.lineInfo[tow[i].id][tow[mytow].id].exist == 0 && tow[i].line < tow[i].maxline)
					{
						info.myCommandList.addCommand(addLine, tow[i].id, tow[mytow].id);
					}
					for (int j = mytow + 1; j <= 12; j++)
					{
						if (info.lineInfo[tow[i].id][tow[j].id].exist == 1 && info.lineInfo[tow[i].id][tow[j].id].state != AfterCut && tow[j].priority > 250)
						{
							int place = (int)info.lineInfo[tow[i].id][tow[j].id].maxlength / 10;
							info.myCommandList.addCommand(cutLine, tow[i].id, tow[j].id, place);
						}
					}
				}
				else
				{
					for (int j = 0; j < mytow; j++)
					{
						if (info.lineInfo[tow[j].id][tow[i].id].exist == 1 && info.lineInfo[tow[j].id][tow[i].id].state != AfterCut)
						{
							int place = (int)info.lineInfo[tow[i].id][tow[j].id].maxlength / 10;
							info.myCommandList.addCommand(cutLine, tow[j].id, tow[i].id, place);
						}
					}//切断支援
					if (tow[i].enemy == 0)
					{
						if (tow[i].stra != 3 && info.playerInfo[myid].technologyPoint >= 5.0)
							info.myCommandList.addCommand(changeStrategy, tow[i].id, 3);

					}//没敌人时增长
					if (tow[i].enemy > 0)
					{
						bool p = 0;
						for (int j = mytow; j <= 12; j++)
						{
							if (info.lineInfo[tow[j].id][tow[i].id].exist == 1 && info.towerInfo[tow[j].id].strategy == 1)
								p = 1;
						}//如果有人attack我
						if (info.playerInfo[myid].technologyPoint >= 5.0 && tow[i].stra != 2 && (p = 1))
							info.myCommandList.addCommand(changeStrategy, tow[i].id, 2);//改成防御
						for (int j = mytow; j <= 12; j++)
						{
							if (info.lineInfo[tow[j].id][tow[i].id].exist == 1 && info.lineInfo[tow[i].id][tow[j].id].exist == 0 && tow[i].line < tow[i].maxline)
								info.myCommandList.addCommand(addLine, tow[i].id, tow[j].id);
						}//谁打我就打回去
					}
					for (int j = mytow; j <= 12; j++)
					{
						if (info.lineInfo[tow[i].id][tow[j].id].exist == 1 && info.lineInfo[tow[i].id][tow[j].id].state == 4)
						{
							if (tow[j].reso < info.lineInfo[tow[i].id][tow[j].id].maxlength / 10)
								info.myCommandList.addCommand(cutLine, tow[i].id, tow[j].id, 0);
						}
					}//有机会的话偷塔
				}
			}
		}
	}
	if (info.playerInfo[myid].technologyPoint > 3)
	{
		if (info.playerInfo[myid].RegenerationSpeedLevel == 0)
			info.myCommandList.addCommand(upgrade, 0);//2
		if (info.playerInfo[myid].ExtraControlLevel == 0)
			info.myCommandList.addCommand(upgrade, 2);//3
		if (info.playerInfo[myid].ExtendingSpeedLevel == 0)
			info.myCommandList.addCommand(upgrade, 1);//2
	}
	if (info.playerInfo[myid].technologyPoint > 4)
	{
		if (info.playerInfo[myid].RegenerationSpeedLevel == 1)
			info.myCommandList.addCommand(upgrade, 0);//4
		if (info.playerInfo[myid].DefenceLevel == 0)
			info.myCommandList.addCommand(upgrade, 3);//2
	}
	if (info.playerInfo[myid].technologyPoint > 10)
	{
		if (info.playerInfo[myid].RegenerationSpeedLevel == 2)
			info.myCommandList.addCommand(upgrade, 0);//6
		if (info.playerInfo[myid].ExtraControlLevel == 1)
			info.myCommandList.addCommand(upgrade, 2);//5
		if (info.playerInfo[myid].ExtendingSpeedLevel == 1)
			info.myCommandList.addCommand(upgrade, 1);//4
		if (info.playerInfo[myid].DefenceLevel == 1)
			info.myCommandList.addCommand(upgrade, 3);//4
	}
	if (info.playerInfo[myid].technologyPoint > 15)
	{
		if (info.playerInfo[myid].RegenerationSpeedLevel == 3)
			info.myCommandList.addCommand(upgrade, 0);//8
		if (info.playerInfo[myid].ExtendingSpeedLevel == 2)
			info.myCommandList.addCommand(upgrade, 1);//6
		if (info.playerInfo[myid].ExtraControlLevel == 2)
			info.myCommandList.addCommand(upgrade, 2);//7
	}
	if (info.playerInfo[myid].technologyPoint > 20)
	{
		if (info.playerInfo[myid].RegenerationSpeedLevel == 4)
			info.myCommandList.addCommand(upgrade, 0);//10
		if (info.playerInfo[myid].ExtendingSpeedLevel == 3)
			info.myCommandList.addCommand(upgrade, 1);//8
		if (info.playerInfo[myid].ExtendingSpeedLevel == 4)
			info.myCommandList.addCommand(upgrade, 1);//10
		if (info.playerInfo[myid].DefenceLevel == 2)
			info.myCommandList.addCommand(upgrade, 3);//6
	}
	/*for (int i = 0; i <= info.myMaxControl; i++)
	{
		Command C;
		C.type = static_cast<CommandType>(rand() % 4);
		switch (C.type)
		{
		case addLine:
			info.myCommandList.addCommand(addLine, rand() % info.towerNum, rand() % info.towerNum);
			break;
		case cutLine:
			info.myCommandList.addCommand(addLine, rand() % info.towerNum, rand() % info.towerNum, rand() % 50);
			break;
		case changeStrategy:
			info.myCommandList.addCommand(changeStrategy, rand() % info.towerNum, rand() % 4);
			break;
		case upgrade:
			info.myCommandList.addCommand(upgrade, rand() % 4);
			break;
		default:
			break;
		}
	}*/
	for (int i = 0; i <= 12; i++)
	{
		tow[i].last_round_reso = tow[i].reso;
	}
}
