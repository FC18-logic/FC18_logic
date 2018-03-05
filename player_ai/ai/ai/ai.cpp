#include "ai.h"
#include "definition.h"
#include "user_toolbox.h"
#include <iostream>
#include <vector>
void player_ai(Info& info)
{
	TPlayerID myid = info.myID;
	//将各种指令压入堆中，根据得分选取指令
	MaxHeap<float, Command> evalution; 
	for(TCellID mycell:info.playerInfo[info.])
}
