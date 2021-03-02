#include "ai.h"
#include "definition.h"
#include "user_toolbox.h"
#include <iostream>
#include <vector>
//玩家ai代码写在player_ai中，此函数每回合运行1次，用于每回合获取玩家的命令
//游戏通过Info型实参给玩家player_ai函数传递信息，包括场上塔、兵团、地图等部分信息
//玩家通过info.myCommandList.addCommmand(<命令类型>,<参数列表:参数1,参数2...>)添加命令
//每回合命令最多不超过50条，myCommandList的addCommand方法已经对此有限制，超过50条的命令自动抛弃
//如果玩家修改参数，使得最后传回游戏的myCommandlist中有超过50条命令，游戏会自动判定玩家出局

struct Memo {
	int myID = -1;
	int myTowerCount = 0;
	int myBattleCount = 0;
	int myConstructCount = 0;
	int commandCount = 0;
};

Memo MemoInitial(Info& info) {
	Memo memo;
	memo.myID = info.myID;
	for (int i = 0; i < info.corpsInfo.size(); ++i) {
		if (info.corpsInfo[i].owner == memo.myID && info.corpsInfo[i].exist) {
			if (info.corpsInfo[i].type == Battle) { memo.myBattleCount++; }
			else { memo.myConstructCount++; }
		}
	}
	for (int i = 0; i < info.towerInfo.size(); ++i) {
		if (info.towerInfo[i].ownerID == memo.myID && info.towerInfo[i].exist) {
			memo.myTowerCount++;
		}
	}
	return memo;
}

bool isMyTower(TowerInfo& tower, int myID) {
	return tower.ownerID == myID && tower.exist;
}

bool isValidTerritory(int x, int y) {
	return x >= 0 && x <= 14 && y >= 0 && y <= 14;
}

bool isMyTerritory(int x, int y, Info& info) {
	if (!isValidTerritory(x, y)) return false;
	return ((*info.gameMapInfo)[y][x].owner == info.myID);
}

std::vector<CorpsInfo> GetCorp(int x, int y, Info& info) {
	vector<CorpsInfo> ret;
	if (!isValidTerritory(x, y)) return ret;
	for (int i = 0; i < (*info.gameMapInfo)[y][x].corps.size(); ++i) {
		ret.push_back(info.corpsInfo[(*info.gameMapInfo)[y][x].corps.at(i)]);
	}
	return ret;
}

TowerInfo GetTower(int x, int y, Info& info) {
	TowerInfo tower;
	tower.exist = false;
	if (!isValidTerritory(x, y)) return tower;
	if ((*info.gameMapInfo)[y][x].TowerIndex == NOTOWER) return tower;
	return info.towerInfo[(*info.gameMapInfo)[y][x].TowerIndex];
}

bool CanBuildTowerAt(int x, int y, Info& info, Memo& memo) {
	if (memo.myTowerCount >= 10 || memo.commandCount >= 50) return false; // 已经达到建塔数量上限。
	if (!isMyTerritory(x, y, info)) return false; // 不是我的领地不能建造。
	if (GetTower(x, y, info).exist) return false; // 如果已经有塔了不能建造。
	return true;
}

bool TowerNeedRepair(TowerInfo& tower, Info& info) {
	if (tower.healthPoint <= 0.67 * (tower.level * 20 + 80)) { // 当塔的生命力小于等级满血的2/3时，需要修复
		return true;
	}
	return false;
}

TCorpsID FindTowerTarget(TowerInfo& tower, Info& info) { // 寻找tower周围在攻击范围内，属于敌方的兵团，返回其序号。如果不存在，则返回-1
	CorpsInfo corp;
	for (int ix = tower.position.m_x - 2 > 0 ? tower.position.m_x - 2 : 0; ix <= tower.position.m_x + 2 && ix < 15; ++ix) {
		for (int iy = tower.position.m_y - 2 > 0 ? tower.position.m_y - 2 : 0; iy <= tower.position.m_y + 2 && iy < 15; ++iy) {
			if (GetTower(ix, iy, info).exist) continue; // 如果这个单元格已经有塔了，那么就不能攻击这个单元格上的兵团。
			for (int i = 0; i < (*info.gameMapInfo)[iy][ix].corps.size(); ++i) {
				corp = GetCorp(ix, iy, info)[i];
				if (corp.exist && corp.owner != info.myID) {
					return corp.ID;
				}
			}
		}
	}
	return -1;
}

bool isMyExtender(CorpsInfo& corp, Info& info) {
	return corp.owner == info.myID && corp.exist && corp.type == Construct && corp.m_BuildType == Extender;
}

bool isMyBuilder(CorpsInfo& corp, Info& info) {
	return corp.owner == info.myID && corp.exist && corp.type == Construct && corp.m_BuildType == Builder;
}

TCorpsID FindBattlerTarget(CorpsInfo& corp, Info& info) {
	// TODO
	return -1;
}

void player_ai(Info& info)
{
	Memo memo = MemoInitial(info); // 用来实时判断当前塔数量是否达到上限，命令数量是否达到上限

	// 建塔
	for (int i = 0; i < info.corpsInfo.size(); ++i) {
		// 遍历兵团，找到了我的开拓者
		if (isMyExtender(info.corpsInfo[i], info)) {
			//在开拓者的周围找我能新建塔的敌方，并前往新建塔。
			int x = info.corpsInfo[i].pos.m_x;
			int y = info.corpsInfo[i].pos.m_y;
			if (CanBuildTowerAt(x, y, info, memo)) {
				info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[i].ID }); // 问，此处的info.corpsInfo[i].ID是不是始终等于i？
				memo.myTowerCount++;
				memo.myConstructCount--;
				memo.commandCount += 1;
			}
			else if (CanBuildTowerAt(x + 1, y, info, memo)) {
				info.myCommandList.addCommand(corpsCommand, { CMove, info.corpsInfo[i].ID, CRight });
				info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[i].ID });
				memo.myTowerCount++;
				memo.myConstructCount--;
				memo.commandCount += 2;
			}
			else if (CanBuildTowerAt(x - 1, y, info, memo)) {
				info.myCommandList.addCommand(corpsCommand, { CMove, info.corpsInfo[i].ID, CLeft });
				info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[i].ID });
				memo.myTowerCount++;
				memo.myConstructCount--;
				memo.commandCount += 2;
			}
			else if (CanBuildTowerAt(x, y + 1, info, memo)) {
				info.myCommandList.addCommand(corpsCommand, { CMove, info.corpsInfo[i].ID, CDown });
				info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[i].ID });
				memo.myTowerCount++;
				memo.myConstructCount--;
				memo.commandCount += 2;
			}
			else if (CanBuildTowerAt(x, y - 1, info, memo)) {
				info.myCommandList.addCommand(corpsCommand, { CMove, info.corpsInfo[i].ID, CUp });
				info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[i].ID });
				memo.myTowerCount++;
				memo.myConstructCount--;
				memo.commandCount += 2;
			}
		}
	}

	// 塔的操作
	TowerInfo tower;
	for (int x = 0; x < 15; ++x) {
		for (int y = 0; y < 15; ++y) { // 双层循环遍历地图
			tower = GetTower(x, y, info);
			//如果这个方格存在塔，是我的塔，且塔需要维修
			if (isMyTower(tower, memo.myID) && TowerNeedRepair(tower, info)) { 
				//首先找找看是否已经有我的Builder可以维修这个塔。
				for (int j = 0; j < GetCorp(x, y, info).size(); ++j) {
					CorpsInfo corp = GetCorp(x, y, info)[j];
					if (isMyBuilder(corp, info)) {
						info.myCommandList.addCommand(corpsCommand, { CRepair, corp.ID });
						memo.myConstructCount--;
						memo.commandCount++;
						break;
					}
				}
				// 生产builder
				info.myCommandList.addCommand(towerCommand, { TProduct, tower.ID, PBuilder });
				memo.commandCount++;
				break;
			}
			//其他情况：这个塔是我的塔，但不需要维修
			else if (isMyTower(tower, memo.myID)) {
				// 那么，如果塔周围可以供我建新塔
				if (CanBuildTowerAt(x + 1, y, info, memo)
					|| CanBuildTowerAt(x, y + 1, info, memo)
					|| CanBuildTowerAt(x - 1, y, info, memo)
					|| CanBuildTowerAt(x, y - 1, info, memo)) { // 小问题：其实不应该在这里判断命令数量的上限的，不过问题不大。
					info.myCommandList.addCommand(towerCommand, { TProduct, tower.ID, PExtender }); // 则新建开拓者，这个开拓者如果新建完成，则会在下一个回合直接移动到该单元格并新建塔。
					memo.commandCount++;
					// memo.myConstructCount++; 这个还没写好，因为并不是加一个命令之后每次都会新建出兵团，所以memo里兵团的数量并不可用，但问题不大。
				}
				// 如果我的周围不可以建塔，但有敌人兵团
				else if (int id_tmp = FindTowerTarget(tower, info) != -1) {
					info.myCommandList.addCommand(towerCommand, { TAttackCorps, tower.ID, id_tmp }); //则攻击这个兵团
					memo.commandCount++;
				}
				// 其他情况下，如果我还没有升到最高级
				else if (tower.level < 8) {
					info.myCommandList.addCommand(towerCommand, { TProduct, tower.ID, PUpgrade }); // 则继续升级本塔
					memo.commandCount++;
				}
				else {
					info.myCommandList.addCommand(towerCommand, { TProduct, tower.ID, PWarrior }); // 最后其他情况，就造造战士吧。
					memo.commandCount++;
				}
			}
		}
	}

	

}