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
int canfight(Info& info, int q, int x, int y, int myID) {//判断能否攻打并返回敌方兵团ID
	if (!(x >= 0 && x <= 14 && y >= 0 && y <= 14)) return -1;
	vector<int> corps = (*info.gameMapInfo)[y][x].corps;//如果没有设置兵团这个vector是什么样的呢？
	for (int i = 0; i < corps.size(); i++) {
		if (info.corpsInfo[corps[i]].owner != myID) return info.corpsInfo[corps[i]].ID;//该位置有其他玩家的兵团
	}
	return -1;
}
void player_ai(Info& info)
{
	int myID = info.myID;
	int command_num = 0;										//命令数
	int Tower_num = info.totalTowers;								//【FC18】总的防御塔个数
	int Corps_num = info.totalCorps;								//【FC18】总的兵团个数
	int my_Tower_num = info.playerInfo[myID - 1].tower.size();	//访问该玩家所有塔的数目
	int my_Corps_num = info.playerInfo[myID - 1].corps.size();	//访问该玩家所有兵团的数目														
	int my_Buider_num = 0;										// 我的防御塔总数
	int my_Extender_num = 0;
	int my_Warrior_num = 0;
	int my_Archer_num = 0;
	int my_Cavalry_num = 0;
	set<int> my_Tower = info.playerInfo[myID - 1].tower;         //我的防御塔序号们
	set<int> my_Corps = info.playerInfo[myID - 1].corps;		//我的兵团序号们
	vector<int> my_Buider;
	vector<int> my_Extender;
	vector<int> my_Warrior;
	vector<int> my_Archer;
	vector<int> my_Cavalry;
	set<int>::iterator tower_it;							//用于访问我的塔的迭代器
	set<int>::iterator corps_it;							//用于访问我的兵团的迭代器
	vector<TPoint> my_land;                                 //从属于我方势力的土地
	for (corps_it = my_Corps.begin(); corps_it != my_Corps.end(); corps_it++) {//遍历我的兵团，找出他们的种类
		int i = *corps_it;
		if (info.corpsInfo[i].exist) {
			if (info.corpsInfo[i].type == Construct) {
				if (info.corpsInfo[i].m_BuildType == Builder) {
					my_Buider_num++;
					my_Buider.push_back(i);
				}
				if (info.corpsInfo[i].m_BuildType == Extender) {
					my_Extender_num++;
					my_Extender.push_back(i);
				}
			}
			if (info.corpsInfo[i].type == Battle) {
				if (info.corpsInfo[i].m_BattleType == Warrior) {
					my_Warrior_num++;
					my_Warrior.push_back(i);
				}
				if (info.corpsInfo[i].m_BattleType == Archer) {
					my_Archer_num++;
					my_Archer.push_back(i);
				}
				if (info.corpsInfo[i].m_BattleType == Cavalry) {
					my_Cavalry_num++;
					my_Cavalry.push_back(i);
				}
			}
		}
	}
	//找到属于自己领地的坐标
	for (int x = 0; x < 15; x++) {
		for (int y = 0; y < 15; y++) {
			if ((*info.gameMapInfo)[y][x].owner == myID) {
				TPoint q;
				q.m_x = x;
				q.m_y = y;
				my_land.push_back(q);
			}
		}
	}
	//对Extender下命令
	for (int j = 0; j < my_Extender_num; j++) {
		int q = my_Extender[j];
		int E_x = info.corpsInfo[q].pos.m_x;
		int E_y = info.corpsInfo[q].pos.m_y;
		if (info.playerInfo[myID - 1].tower.size() < 10 && command_num < 50) {
			for (int k = 0; k < my_land.size(); k++) {
				int x = my_land[k].m_x;
				int y = my_land[k].m_y;
				if ((*info.gameMapInfo)[y][x].TowerIndex == -1) {//本方领土没有塔

					//我觉得应该加一个返回兵团每移动一次所消耗的行动力的函数
					//在样例里我先让它移动一格好了
					if (x == E_x && y == E_y) {
						info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[q].ID });
						command_num++;
						my_Extender_num--;
						my_Tower_num++;
					}
					if (x == E_x + 1 && y == E_y) {
						info.myCommandList.addCommand(corpsCommand, { CMove, info.corpsInfo[q].ID, CRight });
						if (info.corpsInfo[q].movePoint >= 1) {
							info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[q].ID });
							command_num = command_num + 2;
						}
						else command_num++;
						my_Extender_num--;
						my_Tower_num++;
					}
					if (x == E_x - 1 && y == E_y) {
						info.myCommandList.addCommand(corpsCommand, { CMove, info.corpsInfo[q].ID, CLeft });
						if (info.corpsInfo[q].movePoint >= 1) {
							info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[q].ID });
							command_num = command_num + 2;
						}
						else command_num++;
						my_Extender_num--;
						my_Tower_num++;
					}
					if (x == E_x && y == E_y + 1) {//我没太搞清楚坐标变换和上下左右的关系，有可能写错
						info.myCommandList.addCommand(corpsCommand, { CMove, info.corpsInfo[q].ID, CDown });
						if (info.corpsInfo[q].movePoint >= 1) {
							info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[q].ID });
							command_num = command_num + 2;
						}
						else command_num++;
						my_Extender_num--;
						my_Tower_num++;
					}
					if (x == E_x && y == E_y - 1) {
						info.myCommandList.addCommand(corpsCommand, { CMove, info.corpsInfo[q].ID, CUp });
						if (info.corpsInfo[q].movePoint >= 1) {
							info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[q].ID });
							command_num = command_num + 2;
						}
						else command_num++;
						my_Extender_num--;
						my_Tower_num++;
					}

				}
			}
		}
		else break;
	}
	//对Buider下命令
	for (int j = 0; j < my_Buider_num; j++) {
		if (command_num > 50) break;
		int q = my_Buider[j];
		int x = info.corpsInfo[q].pos.m_x;
		int y = info.corpsInfo[q].pos.m_y;
		if ((*info.gameMapInfo)[y][x].TowerIndex != -1) {
			int TI = (*info.gameMapInfo)[y][x].TowerIndex;
			if (info.towerInfo[TI].healthPoint < 0.33 * (info.towerInfo[TI].level * 20 + 80)) {
				info.myCommandList.addCommand(corpsCommand, { CRepair, info.corpsInfo[q].ID });
				command_num++;
			}
		}
		else if ((*info.gameMapInfo)[y][x].type == TRPlain) {  //地形为平原
			info.myCommandList.addCommand(corpsCommand, { CChangeTerrain,info.corpsInfo[q].ID, TRForest });
			command_num++;
		}
	}
	//对作战兵团下命令
	for (int j = 0; j < my_Warrior_num; j++) {
		if (command_num > 50) break;
		int q = my_Warrior[j];//在自己的攻击范围内搜寻其他玩家的兵团
		int x = info.corpsInfo[q].pos.m_x;
		int y = info.corpsInfo[q].pos.m_y;
		for (int dx = -TBattleRange[0]; dx <= TBattleRange[0]; dx++) {
			for (int dy = -TBattleRange[0]; dy <= TBattleRange[0]; dy++) {
				int p = canfight(info, q, x + dx, y + dy, myID);
				if (p != -1) {
					info.myCommandList.addCommand(corpsCommand, { CAttackCorps,info.corpsInfo[q].ID, p });
					command_num++;
				}
			}
		}
	}
	for (int j = 0; j < my_Archer_num; j++) {
		if (command_num > 50) break;
		int q = my_Archer[j];//在自己的攻击范围内搜寻其他玩家的兵团
		int x = info.corpsInfo[q].pos.m_x;
		int y = info.corpsInfo[q].pos.m_y;
		for (int dx = -TBattleRange[1]; dx <= TBattleRange[1]; dx++) {
			for (int dy = -TBattleRange[1]; dy <= TBattleRange[1]; dy++) {
				int p = canfight(info, q, x + dx, y + dy, myID);
				if (p != -1) {
					info.myCommandList.addCommand(corpsCommand, { CAttackCorps,info.corpsInfo[q].ID, p });
					command_num++;
				}
			}
		}
	}
	for (int j = 0; j < my_Cavalry_num; j++) {
		if (command_num > 50) break;
		int q = my_Cavalry[j];//在自己的攻击范围内搜寻其他玩家的兵团
		int x = info.corpsInfo[q].pos.m_x;
		int y = info.corpsInfo[q].pos.m_y;
		for (int dx = -TBattleRange[2]; dx <= TBattleRange[2]; dx++) {
			for (int dy = -TBattleRange[2]; dy <= TBattleRange[2]; dy++) {
				int p = canfight(info, q, x + dx, y + dy, myID);
				if (p != -1) {
					info.myCommandList.addCommand(corpsCommand, { CAttackCorps,info.corpsInfo[q].ID, p });
					command_num++;
				}
			}
		}
	}
	//命令塔
	for (tower_it = my_Tower.begin(); tower_it != my_Tower.end(); tower_it++) {
		if (command_num > 50) break;
		int i = *tower_it;
		int x = info.towerInfo[i].position.m_x;
		int y = info.towerInfo[i].position.m_y;
		int flag = 0;//由于每个塔只能添加一个命令，所以标记一下
		if (/*info.towerInfo[i].pdtType == -1*/1) {
			if (info.towerInfo[i].healthPoint < 0.67 * (info.towerInfo[i].level * 20 + 80)) {//如果生命值不够就生产建造者
				info.myCommandList.addCommand(towerCommand, { TProduct, info.towerInfo[i].ID, PBuilder });
				command_num++;
				flag = 1;
			}
			if (!flag && my_Tower_num < 2) {//如果塔不够就生产开拓者
				info.myCommandList.addCommand(towerCommand, { TProduct, info.towerInfo[i].ID, PExtender });
				command_num++;
				flag = 1;
			}
			if (!flag && my_Warrior_num < 2) {//如果战士不够就生产战士
				info.myCommandList.addCommand(towerCommand, { TProduct, info.towerInfo[i].ID, PWarrior });
				command_num++;
				flag = 1;
			}
			if (!flag && my_Archer_num < 2) {//战士够了但是弓箭手不够
				info.myCommandList.addCommand(towerCommand, { TProduct, info.towerInfo[i].ID, PArcher });
				command_num++;
				flag = 1;
			}
			if (!flag && my_Cavalry_num < 2) {//只有法师不够
				info.myCommandList.addCommand(towerCommand, { TProduct, info.towerInfo[i].ID, PCavalry });
				command_num++;
				flag = 1;
			}
		}
		if (!flag) {//实在没事做就升级
			if (info.towerInfo[i].level < 8) {
				info.myCommandList.addCommand(towerCommand, { TProduct, info.towerInfo[i].ID, PUpgrade });
			}
		}
		for (int dx = -2; dx <= 2; dx++) {//如果敌方兵团在攻击范围内，则攻打
			for (int dy = -2; dy <= 2; dy++) {
				int p = canfight(info, i, x + dx, y + dy, myID);
				if (p != -1) {
					info.myCommandList.addCommand(towerCommand, { CAttackCorps,info.towerInfo[i].ID, p });
					command_num++;
					flag = 1;
				}
			}
		}
	}

}
