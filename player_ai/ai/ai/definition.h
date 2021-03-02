#pragma once
#ifndef DEFINITION_H
#define DEFINITION_H
#define FC15_DEBUG
#define NO_SILENT_MODE
#define NO_JSON
#define INF 100000000
#define TRANSITION -1   //过渡地形区域
#define PUBLIC 0      //公共地形区域
#define NOTOWER -1    //当前方格没有防御塔
#define OUTOFRANGE -2  //当前方格在地图之外
#define FC15_SAVEMODE


#include <vector>
#include <string>
#include <initializer_list>
#include <stdexcept>
#include <map>
#include <set>
#include <iostream>

using namespace std;
typedef int    TMovePoint;     //【FC18】行动力
typedef int    TBattlePoint;   //【FC18】战斗力
typedef int    THealthPoint;   //【FC18】生命值
typedef int    TBuildPoint;    //【FC18】劳动力：工程建设兵团属性
typedef int    TProductPoint;  //【FC18】塔的生产力
typedef int	   TExperPoint;    //【FC18】塔的经验值
typedef int    TOccupyPoint;   //【FC18】塔对周围方格施加的占有属性值
typedef int    TIntPara;       //【FC18】整型系数值
typedef double TDoublePara;    //【FC18】双精度浮点型系数值
typedef int    TPlayer;        //【FC18】玩家个数
typedef int    TTower;         //【FC18】塔的个数
typedef int    TCorps;         //【FC18】兵团个数
typedef int    TPlayerID;      //【FC18】玩家ID号
typedef int    TTowerID;       //【FC18】防御塔ID号
typedef int    TCorpsID;       //【FC18】兵团ID号
typedef string TMapID;         //【FC18】地图ID号
typedef int    TMap;           //【FC18】地图参数（高度/宽度）
typedef int    TRound;         //【FC18】回合数
typedef int    TScore;         //【FC18】玩家得分
typedef int    TDist;          //【FC18】游戏中距离的定义
typedef int    TOperaNum;      //【FC18】操作数的个数
typedef int    TPosition;      //【FC18】游戏中二维坐标点的定义
typedef double TLength;        //【FC18】游戏中长度的定义
typedef int    TLevel;         //【FC18】各项属性等级


const int MAX_CORPS_LEVEL = 3;    //【FC18】最大的兵团等级
const int MAX_TOWER_LEVEL = 8;    //【FC18】最大的防御塔等级
const int TERRAIN_TYPE_NUM = 5;    //【FC18】地形的种类数
const int BATTLE_CORPS_TYPE_NUM = 3;    //【FC18】作战兵团的种类数
const int CONSTRUCT_CORPS_TYPE_NUM = 2;    //【FC18】工程兵团的种类数
const int TOWER_PRODUCT_TASK_NUM = 6;     //【FC18】塔的生产任务种类数
const int TOWER_EXPER_GAIN_SCALE = 3;     //【FC18】塔的每回合经验值增加等级数
const int OCCUPY_POINT_DIST_SCALE = 5;    //【FC18】塔对周围方格施加占有属性值的距离等级有几个
const int CORPS_ACTION_TYPE_NUM = 10;    //【FC18】兵团能进行的操作种类数
const int TOWER_ACTION_TYPE_NUM = 3;     //【FC18】防御塔进行的操作种类数
const int MAX_ROUND = 300;               //【FC18】游戏全程的最大回合数，确定的，不像FC15从外部文件读入
const int TOWER_SCORE = 10;              //【FC18】计算玩家得分时每个防御塔每个等级得分
const int BATTLE_CORP_SCORE = 2;         //【FC18】战斗兵团每个星级得分
const int CONSTRUCT_CORP_SCORE = 4;      //【FC18】工程兵团每个得分
const int KILL_SCORE = 5;                //【FC18】击杀分：占领塔/俘虏兵团/击杀兵团得分
const int CORP_SCORE = 4;                //【FC18】单个兵团得分
const int MAX_CMD_NUM = 50;              //【FC18】限制每个玩家每次最大命令数
const int MAX_TOWER_NUM = 10;            //【FC18】玩家最大防御塔数
const int MAX_BATTLE_NUM = 10;           //【FC18】玩家最大作战兵团数目
const int MAX_CONSTRUCT_NUM = 10;        //【FC18】玩家最大工程兵团数目


class Crops;
struct CorpsInfo;
typedef vector<CorpsInfo>	CorpsInfoUnit; //【FC18】一个单元格上所有兵团信息


//【FC18】二维坐标点结构体
struct TPoint
{
	TPosition  m_x;
	TPosition  m_y;
};

//【FC18】塔的配置数据结构体
struct TowerConfig {
	int initBuildPoint;       //起始生产力
	int initProductPoint;      //起始战斗力
	int initHealthPoint;      //起始生命值
	int upgradeExper;         //升到下一级所需经验值
	int battleRegion;         //攻击距离
};


//【FC18】兵团种类
enum corpsType
{
	Battle = 0,          //作战兵团
	Construct = 1        //工程建设兵团
};


//【FC18】作战兵团种类的枚举类
enum battleCorpsType
{
	Warrior = 0,         //战士
	Archer = 1,         //弓箭手
	Cavalry = 2,         //骑兵
};


//【FC18】工程建设兵团的枚举类
enum constructCorpsType
{
	Builder = 0,           //建造者
	Extender = 1           //开拓者
};




//【FC18】命令类型
enum commandType {
	corpsCommand = 0,         //兵团命令
	towerCommand = 1,         //防御塔命令
};


//【FC18】兵团操作类型（C前缀表示Corps）
enum CorpsCommandEnum
{//                                                          作战兵团        工程兵团
	CMove = 0,       //在地图上移动                     √              √
	//CStation = 1,       //驻扎在地图方格（无自己塔）       √              √
	CStationTower = 2,       //驻扎在塔（自己势力的塔）         √              √
	CAttackCorps = 3,       //攻击对方势力的兵团               √
	CAttackTower = 4,       //攻击对方势力的塔                 √
	//CRegroup = 5,       //兵团整编                         √
	//CDissolve = 6,       //兵团解散                         ×              ×
	CBuild = 7,       //修建新防御塔                                     √
	CRepair = 8,       //修理原防御塔                                     √
	CChangeTerrain = 9,       //改变方格地形                                     √
};

//【FC18】命令在Json中的代号(J表示Json代号)
enum CmdJsonNumber
{
	JMove = 0,
	JStation = 1,
	JStationTower = 2,
	JAttackCorps = 3,
	JAttackTower = 4,
	JRegroup = 5,
	JDissolve = 6,
	JBuild = 7,
	JRepair = 8,
	JChangeTerrain = 9,
	JProduct = 10,
	JTowerAttackCorps = 11,
	JTowerSttackTower = 12
};

//【FC18】兵团移动的方向
enum corpsMoveDir
{
	CUp = 0,         //向上移动1格
	CDown = 1,         //向下移动1格
	CLeft = 2,         //向左移动1格
	CRight = 3          //向右移动1格
};

//【FC18】塔的操作类型（T前缀表示tower）
enum towerCommand
{
	TProduct = 0,       //生产任务
	TAttackCorps = 1,       //攻击兵团任务
	TAttackTower = 2        //攻击塔任务
};


//【FC18】塔的生产任务类型（P表示product）
enum productType
{//                                           生产回报
	PWarrior = 0,       //生产战士           1star-战士
	PArcher = 1,       //生产弓箭手         1star-弓箭手
	PCavalry = 2,       //生产法师           1star-法师
	PBuilder = 3,       //生产建造者        1-建造者兵团
	PExtender = 4,       //生产开拓者       1-开拓者兵团
	PUpgrade = 5,       //塔升级任务      塔等级+1（max=8)
	NOTASK = -1
};


//【FC18】地形的枚举类（TR前缀表示地形）
enum terrainType
{
	TRTower = 0,       //塔
	TRPlain = 1,       //平原
	TRMountain = 2,       //山地
	TRForest = 3,       //森林
	TRSwamp = 4,       //沼泽
	TRRoad = 5,       //道路
	TRGate = 6,       //二校门
	TRHall = 7,       //大礼堂
	TRClassRoom1 = 8,  //清华学堂x+z+
	TRClassRoom2 = 9,  //清华学堂x-z+
	TRClassRoom3 = 10, //清华学堂x-z-
	TRClassRoom4 = 11, //清华学堂x+z-
	TRTHUEmpty = 12    //清华元素占据的空地
};


//【FC18】游戏运行的状态
enum gameState
{
	Normal = 0,          //正常运行，不存档
	RecoverMap = 1,      //只按地图存档复现模式
	RecoverRound = 2,    //按回合（包括地图）存档恢复模式
};


//【FC18】作战兵团行动力（与作战兵团的枚举类在序号上对应，且考虑了等级）
const TMovePoint battleMovePoint[BATTLE_CORPS_TYPE_NUM][MAX_CORPS_LEVEL] =
{
	{2,   2,   2},    //战士
	{2,   2,   2},    //弓箭手
	{4,   4,   4}     //骑兵
};

//【FC18】工程兵行动力，规则中没有，先假设这两个数
const TMovePoint constructMovePoint[CONSTRUCT_CORPS_TYPE_NUM] = { 2,2 };

//【FC18】作战兵团初始战斗力（与作战兵团的枚举类在序号上对应，且考虑了等级）
const TBattlePoint corpsBattlePoint[BATTLE_CORPS_TYPE_NUM][MAX_CORPS_LEVEL] =
{
	{36,   44,   52},    //战士
	{30,   38,   46},    //弓箭手
	{44,   52,   60}     //骑兵
};


//【FC18】作战兵团的战斗力增益系数（与作战兵团的枚举类在序号上对应，且考虑了等级）
const TIntPara corpsBattleGain[BATTLE_CORPS_TYPE_NUM][MAX_CORPS_LEVEL] =
{
	{2,   2,   2},    //战士
	{2,   2,   2},    //弓箭手
	{4,   4,   4}     //骑兵
};


//【FC18】作战兵团的攻城系数（与作战兵团的枚举类在序号上对应，且考虑了等级）
const TDoublePara corpsAttackTowerGain[BATTLE_CORPS_TYPE_NUM][MAX_CORPS_LEVEL] =
{
	{0.4,   0.4,   0.4},    //战士
	{0.7,   0.7,   0.7},    //弓箭手
	{0.5,   0.5,   0.5}     //骑兵
};


//【FC18】作战兵团初始生命值（与作战兵团的枚举类在序号上对应，且考虑了等级）
const THealthPoint battleHealthPoint[BATTLE_CORPS_TYPE_NUM][MAX_CORPS_LEVEL] =
{
	{60,   80,   90},    //战士
	{50,   70,   90},    //弓箭手
	{70,   90,   110}    //骑兵
};

//【FC18】战斗兵团射程距离 战士1 弓箭手2 骑兵1
const int TBattleRange[BATTLE_CORPS_TYPE_NUM] = { 1, 2, 1 };

//【FC18】兵团操作所需操作数个数，判断指令合法性（与兵团操作的枚举类在序号上对应)
const TOperaNum CorpsOperaNumNeed[CORPS_ACTION_TYPE_NUM] =
{
	3,    //移动
	2,    //驻扎
	2,    //驻扎塔
	3,    //攻击兵团
	3,    //攻击塔
	INF,  //兵团整编（去掉）
	INF,  //兵团解散（去掉）
	2,    //修建塔
	2,    //维护塔
	3     //改地形
};


//【FC18】工程建设兵团操作的劳动力消耗（与兵团操作的枚举类在序号上对应)
const TBuildPoint constructBuildCost[CORPS_ACTION_TYPE_NUM] =
{
	0,       //在地图上移动
	0,       //驻扎在地图方格（无自己塔）
	0,       //驻扎在塔（自己势力的塔）
	0,       //攻击对方势力的兵团
	0,       //攻击对方势力的塔
	0,       //兵团整编
	0,       //兵团解散
	0,       //修建新防御塔
	1,       //修理原防御塔
	1,       //改变方格地形
};

const TOperaNum towerOperaNumNeed[TOWER_ACTION_TYPE_NUM] =
{
	3,       //生产任务
	3,       //攻击防御塔
	3        //攻击兵团
};

//【FC18】塔的生产任务生产力消耗值
const TProductPoint TowerProductCost[TOWER_PRODUCT_TASK_NUM] =
{//                                 生产回报                                           特殊说明
	40,       //生产战士         1star-战士
	60,       //生产弓箭手      1star-弓箭手
	100,      //生产法师         1star-法师
	40,       //生产建造者        1-建造者兵团
	40,       //生产开拓者        1-开拓者兵团
	40        //塔升级任务      塔等级+1（max=8)       这里40是最小值，实际值为执行该任务的第一回合塔的等级*40，要根据实际情况算  
};


//【FC18】每回合塔的经验值增加与回合数的关系
//【GC18】注意实际访问的时候，传入回合数/100下取整，300回合传入2,301回合起传入3
//【FC18】或直接公式计算：5 + 5 * floor(currentRound / 100)，注意边界值要特殊对待
const TExperPoint TowerExperGain[TOWER_EXPER_GAIN_SCALE + 1] =
{//               回合数
	5,//          [0,100)
	10,//         [100,200)
	15,//         [200,300]
	0//           (300,+∞)
};


//【FC18】塔对周围方格施加的占有属性值，与距离的关系表
const TOccupyPoint TowerOccupyPoint[OCCUPY_POINT_DIST_SCALE + 1] =
{//                方格跟塔的距离
	100,//              1格
	80,//               2格
	50,//               3格
	20,//               4格
	10,//               5格
	0//               6格或更远
};


//【FC18】塔每个等级最开始的时候各项属性的值，用于初始化配置
const struct TowerConfig TowerInitConfig[MAX_TOWER_LEVEL] =
{
	{10,25,100,20, 2},
	{15,27,120,20, 2},
	{20,29,140,25, 2},
	{25,32,160,30, 2},
	{30,35,180,40, 2},
	{35,38,200,40, 2},
	{40,41,220,50, 2},
	{45,45,240,INF,2}
};


//【FC18】行动力消耗（与地形的枚举类在序号上对应）
const TMovePoint CorpsMoveCost[TERRAIN_TYPE_NUM + 8] =
{
	0,    //塔
	2,    //平原
	4,    //山地
	3,    //森林
	4,    //沼泽
	1,    //道路
	INF,  //二校门
	INF,  //大礼堂
	INF,  //清华学堂1
	INF,  //清华学堂2
	INF,  //清华学堂3
	INF,  //清华学堂4
	INF   //清华元素空地
};


//【FC18】战斗力增益（与地形的枚举类在序号上对应）
const TBattlePoint CorpsBattleGain[TERRAIN_TYPE_NUM + 8] =
{
	0,    //塔
	0,    //平原
	5,    //山地
	3,    //森林
	-3,   //沼泽
	0,    //道路
	0,    //二校门
	0,    //大礼堂
	0,    //清华学堂1
	0,    //清华学堂2
	0,    //清华学堂3
	0,    //清华学堂4
	0     //清华元素空地
};

struct TowerInfo {
	bool    exist;      //防御塔是否存在
	TTowerID      ID;   //防御塔ID
	TPlayerID     ownerID;  //所属玩家ID
	TPoint        position;    //位置
	TProductPoint productPoint;  //生产力
	productType   pdtType;     //塔上一次生产任务指令后尚未完成的任务种类（若上一次任务指令完成，则为-1）
	TProductPoint productConsume;  //塔上一次生产任务指令后未完成任务的剩余的工作余量
	TBattlePoint  battlePoint;   //战斗力
	THealthPoint  healthPoint;   //生命值
	TLevel        level;       //等级
};


//@@@【FC18】兵团结构体，有需要的信息再加
struct CorpsInfo
{
	bool	exist;		//是否存在
	TPoint	pos;		//兵团坐标
	TCorpsID		ID;	//兵团ID
	THealthPoint	HealthPoint;	//生命值
	TBuildPoint		BuildPoint;		//劳动力
	TPlayerID		owner;			//所属玩家ID
	corpsType       type;           //兵团种类
	TMovePoint      movePoint;      //行动力
	battleCorpsType		m_BattleType;	//战斗兵用
	constructCorpsType	m_BuildType;	//建造兵用
};


//@@@【FC18】玩家结构体
struct PlayerInfo
{
	TPlayerID id;                        //【FC18】玩家的序号，请注意玩家序号从1开始，访问玩家数组请用[ID-1]
	int rank;                            //【FC18】该选手排名（出局者直接由出局回合数给位次）|（存活者按防御塔得分和兵团得分来排名）|（同名次按防御塔攻占数、消灭敌方军团数、俘虏敌方军团数依次检索排名）|（仍有同名次者随机分配排名）
	bool alive;                          //【FC18】玩家是否还活着

	//@@@【FC18】玩家所有塔的序号，参照原来的set<TCellID> cells
	set<TTowerID> tower;

	//【FC18】玩家所有兵团的序号，建议也用set这种数据结构，内部按兵团序号升序来排序
	set<TCorpsID> corps; //所有兵团
};


//Info改传这个东西vector<vector<>> 方便快捷
struct mapBlock                                 //【FC18】地图方格类
{
	terrainType type;                           //【FC18】地块类型，对应terrainType枚举类
	vector<int> occupyPoint;                    //【FC18】各玩家的占有属性值，秩为玩家序号-1
	int owner;                                  //【FC18】所属玩家序号，-1为过渡TRANSITION，-2为公共PUBLIC
	int TowerIndex;								//@@@【FC18】位于该单元格的塔的下标，对应data里的myTowers，没有塔的时候为-1
	vector<TCorpsID> corps;						//该位置兵团
};


//类声明
class CommandList;
//常用数学运算
//二维坐标减运算
TPoint operator-(const TPoint& p1, const TPoint& p2);
//计算FC18中塔攻击范围的距离（FC18距离）
TDist getDist(const TPoint& p1, const TPoint& p2);
TDist getDist(const int p1_x, const int p1_y, const int p2_x, const int p2_y);
//生成指定闭区间的随机整数
int generateRanInt(int start, int end);


//【FC18】保存命令相关信息
struct Command
{
	Command(commandType _FC18type, initializer_list<int> _FC18parameters) :  //【FC18】由初始化列表构造的构造函数
		cmdType(_FC18type), parameters(_FC18parameters) {}
	Command(commandType _FC18type, vector<int> _FC18parameters) :            //【FC18】由默认向量数组构造的构造函数
		cmdType(_FC18type), parameters(_FC18parameters) {}
	Command() {}                                                             //【FC18】指令的析构函数
	commandType cmdType;                                                     //【FC18】命令种类
	vector<int> parameters;                                                  //【FC18】参数：注意所有参数是整型！
};

//【FC18】命令列表
class CommandList
{
public:
	void addCommand(commandType _FC18type, initializer_list<int> _FC18parameters)  //【FC18】由初始化列表直接添加命令
	{
		if (size() >= MAX_CMD_NUM) return;
		m_commands.emplace_back(_FC18type, _FC18parameters);
	}
	void addCommand(commandType _FC18type, vector<int> _FC18parameters)            //【FC18】由默认向量数组添加命令（需要拷贝构造）
	{
		if (size() >= MAX_CMD_NUM) return;
		Command newCmd;
		newCmd.cmdType = _FC18type;
		newCmd.parameters = _FC18parameters;
		m_commands.push_back(newCmd);
	}
	void removeCommand(int n)                                                      //【FC18】移除第n条命令
	{
		if (n < 0 || n >= size())
			throw std::out_of_range("移除命令时越界 ");
		m_commands.erase(m_commands.begin() + n);
	}
	vector<Command> getCommand() { return m_commands; }                            //【FC18】获取所有命令
	Command& operator[](int n)                                                     //【FC18】访问第n条命令，返回该命令的引用
	{
		if (n < 0 || size() <= n)
			throw std::out_of_range("访问命令时越界 ");
		return m_commands[n];
	}
	int size() const { return int(m_commands.size()); }                            //【FC18】获取总共的命令条数
	vector<Command>::iterator begin() { return m_commands.begin(); }               //【FC18】返回第一条命令的迭代器
	vector<Command>::iterator end() { return m_commands.end(); }                   //【FC18】返回最后一条命令的迭代器
	vector<Command>::const_iterator  begin() const { return m_commands.cbegin(); } //【FC18】返回第一条命令的常量迭代器
	vector<Command>::const_iterator end() const { return m_commands.cend(); }      //【FC18】返回最后一条命令的常量迭代器

private:
	vector<Command> m_commands;                                                    //【FC18】指令集vector存储
};


//@@@【FC18】用于与玩家共享场上的各项信息
struct Info
{
	TPlayer totalPlayers;                                   //【FC18】总玩家数（4人）                               
	TPlayer playerAlive;                                    //【FC18】剩余玩家数（还活着的）
	TRound totalRounds;                                     //【FC18】当前回合数（4个玩家依次执行一次操作为1回合，UI中是1个玩家执行操作记1回合）
	TTower totalTowers;                                     //【FC18】存活的总的防御塔个数
	TCorps totalCorps;                                      //【FC18】存活的总的兵团个数
	TPlayerID myID;                                         //【FC18】选手ID号（注意游戏中玩家ID都是从1开始，索引时请用[myID - 1]这种格式

	CommandList myCommandList;                              //【FC18】用于接收玩家发出的指令的指令集

	//@@@【FC18】返回所有势力信息的vector，可以参照原来的vector<PlayerInfo> playerInfo;
	vector<PlayerInfo> playerInfo;   //势力信息

	//@@@【FC18】返回所有防御塔信息的vector，可以参照原来的vector<CellInfo> cellInfo;
	vector<TowerInfo> towerInfo;

	//@@@【FC18】返回所有兵团信息的vector，可以参照原来的vector<vector<TentacleInfo> > tentacleInfo;
	//vector<vector<CorpsInfoUnit>> corpsInfo;//下标为ij的位置表示位置为x:i,y:j的兵团信息
	vector<CorpsInfo> corpsInfo;

	//【FC18】地图信息
	//vector<vector<mapBlockInfo>> mapInfo;
	//索引地图时 ，第一维为y坐标，第二维为x坐标，即gameMapInfo[y][x]表示（x,y）点的地图信息
	const vector<vector<mapBlock>>* gameMapInfo;

};

#endif // DEFINITION_H