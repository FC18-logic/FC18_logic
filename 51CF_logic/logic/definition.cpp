#include "definition.h"
#include <cmath>
#include <algorithm>
TPoint operator-(const TPoint& p1, const TPoint& p2)
{
	TPoint ret;
	ret.m_x = p1.m_x - p2.m_x;
	ret.m_y = p1.m_y - p2.m_y;
	return ret;
}

TLength getDistance(const TPoint& p1, const TPoint& p2)
{
	TPoint distance = p1 - p2;
	return sqrt(distance.m_x*distance.m_x + distance.m_y*distance.m_y);
}

int generateRanInt(int start, int end) {
	return rand() % (end - start + 1) + start;
}

std::ostream& operator<<(std::ostream& os, const CommandList& cl)
{
	vector<string> stg2str{ "Normal","Attack","Defend","Grow" };
	vector<string> upgrade2str{ "Regeneration","ExtendSpeed","ExtraControl","CellWall" };
	for (const Command& c : cl)
	{
		switch (c.type)
		{
		case addTentacle:
			os << "Add a tentacle from cell " << c.parameters[0] << " to cell " << c.parameters[1] << endl;
			break;
		case cutTentacle:
			os << "Cut the tentacle from cell " << c.parameters[0] << " to cell " << c.parameters[1] << " at the postion " << c.parameters[2] << endl;
			break;
		case changeStrategy:
			os << "Change the strategy of cell " << c.parameters[0] << " to " << stg2str[c.parameters[1]] << endl;
			break;
		case upgrade:
			os << "Upgrade the ability of cell " << stg2str[c.parameters[0]] << endl;
		default:
			break;
		}
	}
	return os;
}


/***********************************************************************************************
*函数名 :【FC18】getDist获取距离函数函数
*函数功能描述 : 计算FC18规则所述两个点之间的距离
*函数参数 : p1<const TPoint&>--第1个点，p2<const TPOint&>--第2个点
*函数返回值 : <TPoint>距离
*作者 : 姜永鹏
***********************************************************************************************/
TDist getDist(const TPoint& p1, const TPoint& p2) {
	TPoint dp = p1 - p2;
	int dx = dp.m_x;
	int dy = dp.m_y;
	return max(abs(dx), abs(dy));
}


/***********************************************************************************************
*函数名 :【FC18】getDist获取距离函数函数
*函数功能描述 : 通过文件读入当前游戏的地图数据、玩家数量数据，并初始化玩家数组，写入零回合命令
				Json
*函数参数 : inMap<ifstream&> 输入文件流对象，enableOutput<bool> 是否输出调试信息（true--允许，
			false--不允许）
*函数返回值 : false--读入地图失败，true--读入地图成功
*作者 : 姜永鹏
***********************************************************************************************/
TDist getDist(const int p1_x, const int p1_y, const int p2_x, const int p2_y) {
	int dx = p1_x - p2_x;
	int dy = p1_y - p2_y;
	return max(abs(dx), abs(dy));
}