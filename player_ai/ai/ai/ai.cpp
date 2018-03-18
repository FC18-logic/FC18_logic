#include "ai.h"
#include "definition.h"
#include "user_toolbox.h"
#include <iostream>
#include <vector>

TTowerID getMyMaxTower(Info& info)
{
	TResourceD maxResource = 0.0;
	TTowerID maxTower = -1;
	for (TTowerID tower : info.playerInfo[info.myID].towers)
	{
		if (info.towerInfo[tower].resource > maxResource)
		{
			maxResource = info.towerInfo[tower].resource;
			maxTower = tower;
		}
	}
	return maxTower;
}

//¿ª¾Ö²ßÂÔ
void beginPhase(Info& info)
{
	TPlayerID id = info.myID;
	TTowerID maxTower = getMyMaxTower(info);
	if (info.round == 5)
	{
		for (TTowerID tower : info.playerInfo[id].towers)
		{
			if (tower != maxTower)
				info.myCommandList.addCommand(addLine, tower, maxTower);
		}
	}
	if (info.round == 8)
	{
		info.myCommandList.addCommand(addLine, maxTower, 12);
	}
	if (info.playerInfo[info.myID].technologyPoint > StrategyChangeCost[Normal][Grow])
	{
		info.myCommandList.addCommand(changeStrategy, maxTower, Grow);
	}
	if (info.lineInfo[maxTower][12].exist && info.lineInfo[maxTower][12].state == Arrived)
	{
		if (info.towerInfo[12].occupyOwner != id)
			info.myCommandList.addCommand(cutLine, maxTower, 12, info.lineInfo[maxTower][12].resource - (info.towerInfo[12].occupyPoint + info.towerInfo[12].resource / 3 + 2));
	}
}



void player_ai(Info& info)
{
	if (info.round <= 25 && info.playerInfo[info.myID].towers.size() == 3)
		beginPhase(info);
	else
	{
		TResourceD leftTech = info.playerInfo[info.myID].technologyPoint;
		for (TPlayerID i : info.playerInfo[info.myID].towers)
		{
			TowerInfo& curr = info.towerInfo[i];
			int enemycount = 0;
			for (int j = 0;j!=info.towerNum;++j)
			{
				if (info.lineInfo[j][i].exist)
					enemycount++;
			}
			if (enemycount >= 3 && curr.strategy == Normal
				&& StrategyChangeCost[Normal][Defence] < leftTech)
			{
				info.myCommandList.addCommand(changeStrategy, i, Grow);
				leftTech -= StrategyChangeCost[Normal][Grow];
			}
			if (curr.resource < curr.maxResource/5 *3 && (curr.strategy == Normal ||curr.strategy == Defence)
				&& enemycount == 0 && StrategyChangeCost[Normal][Grow] < leftTech)
			{
				info.myCommandList.addCommand(changeStrategy, i, Grow);
				leftTech -= StrategyChangeCost[Normal][Grow];
			}
			else if (curr.resource > curr.maxResource / 5 * 3
				&& StrategyChangeCost[curr.strategy][Grow] < leftTech)
			{
				info.myCommandList.addCommand(changeStrategy, i, Attack);
				leftTech -= StrategyChangeCost[Normal][Grow];
			}
		}

		for (TPlayerID i : info.playerInfo[info.myID].towers)
		{
			for (int j = 0; j != info.towerNum; ++j)
				if (getDistance(info.towerInfo[i].position, info.towerInfo[j].position)*Density < info.towerInfo[i].resource
					&& info.towerInfo[i].strategy == Attack && !info.lineInfo[i][j].exist && info.towerInfo[i].currLineNum < info.towerInfo[i].maxLineNum)
				{
					info.myCommandList.addCommand(addLine, i, j);
				}
				else if (info.lineInfo[i][j].exist &&info.lineInfo[i][j].state == Arrived &&
					info.towerInfo[j].owner != info.myID && info.towerInfo[j].resource < info.lineInfo[i][j].resource)
				{
					info.myCommandList.addCommand(cutLine, i, j,0);
				}
		}

		if (leftTech > RegenerationSpeedUpdateCost[info.playerInfo[info.myID].RegenerationSpeedLevel])
		{
			info.myCommandList.addCommand(upgrade, RegenerationSpeed);
			leftTech -= RegenerationSpeedUpdateCost[info.playerInfo[info.myID].RegenerationSpeedLevel];
		}
		if (leftTech > ExtraControlStageUpdateCost[info.playerInfo[info.myID].ExtraControlLevel])
		{
			info.myCommandList.addCommand(upgrade, ExtraControl);
			leftTech -= ExtraControlStageUpdateCost[info.playerInfo[info.myID].ExtraControlLevel];
		}
		if (leftTech > ExtendingSpeedUpdateCost[info.playerInfo[info.myID].ExtendingSpeedLevel])
		{
			info.myCommandList.addCommand(upgrade, ExtendingSpeed);
			leftTech -= ExtendingSpeedUpdateCost[info.playerInfo[info.myID].ExtendingSpeedLevel];
		}
		if (leftTech > DefenceStageUpdateCost[info.playerInfo[info.myID].DefenceLevel])
		{
			info.myCommandList.addCommand(upgrade, Wall);
			leftTech -= DefenceStageUpdateCost[info.playerInfo[info.myID].DefenceLevel];
		}
	}
}