#include "ai.h"
#include "definition.h"
#include "user_toolbox.h"
#include <iostream>
#include <vector>
//玩家ai代码写在player_ai中，此函数每回合运行1次，用于每回合获取玩家的命令
//游戏通过Info型实参给玩家player_ai函数传递信息，包括场上塔、兵团、地图等部分信息
//玩家通过info.myCommandList.addCommmand(<命令类型>,<参数列表:参数1,参数2...>)添加命令
//每回合命令最多不超过10条，myCommandList的addCommand方法已经对此有限制，超过10条的命令自动抛弃
//如果玩家修改参数，使得最后传回游戏的myCommandlist中有超过10条命令，游戏会自动判定玩家出局
void player_ai(Info& info)
{

}