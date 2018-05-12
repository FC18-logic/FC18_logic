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

}