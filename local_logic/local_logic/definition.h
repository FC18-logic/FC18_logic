#ifndef DEFINITION_H
#define DEFINITION_H

#include <vector>
#include <cmath>
#include <string>
#include <initializer_list>
#include <stdexcept>
#include <map>
#include <set>
using namespace std;
typedef double TSpeed;
typedef double TResourceD;  //double 的资源数，用于内部操作
typedef int    TResourceI;  //int    的资源数，用于显示
typedef int    TId;
typedef double TLength;
typedef int    TStudentID;
typedef double TResourceD;
typedef int    TPosition;
typedef int    TCamp;
typedef string TMapID;
typedef int    TMap;
typedef int    TLevel;  //各项属性等级
typedef int    TRound;  //回合数
typedef double TPower;  //倍率
typedef int    TTentacleNum;
typedef int    TTentacleID;

//兵力密度
const double       Density = 0.1;
const TSpeed       BaseExtendSpeed = 3;
const TSpeed       BaseFontSpeed = 12;
const TSpeed       BaseBackSpeed = 20;
const TLevel       STUDENT_LEVEL_COUNT = 5;
const TResourceI   MAX_RESOURCE = 200;
const TSpeed       BASE_REGENERETION_SPEED[STUDENT_LEVEL_COUNT]{ 1,1.5,2,2.5,3 };
const TTentacleNum TENTACLE_NUMBER[STUDENT_LEVEL_COUNT]{1,2,2,3,3};  //可伸触手数量
const TResourceI   STUDENT_STAGE[STUDENT_LEVEL_COUNT + 1]{ 0 ,10,40,80,150,MAX_RESOURCE };
const int          NO_DATA = -1;
const TCamp        Neutral = NO_DATA;

//最大技能等级
const TLevel MAX_REGENERATION_SPEED_LEVEL = 5;
const TLevel MAX_EXTENDING_SPEED_LEVEL = 5;
const TLevel MAX_EXTRA_CONTROL_LEVEL = 3;
const TLevel MAX_DEFENCE_LEVEL = 3;

//各技能等级对应数值
const TPower RegenerationSpeedStage[MAX_REGENERATION_SPEED_LEVEL + 1] = { 1,1.05,1.1,1.15,1.2,1.25 };
const TPower SpeedStage[MAX_EXTENDING_SPEED_LEVEL + 1] = { 1,1.1,1.2,1.3,1.4,1.5 };
const TPower ExtraControlStage[MAX_EXTRA_CONTROL_LEVEL + 1] = { 0,0.5,1,1.5 };
const TPower DefenceStage[MAX_DEFENCE_LEVEL + 1] = { 1.5,1.4,1.3,1.2 };

//各个技能升级所需科创点数
const TResourceD RegenerationSpeedUpdateCost[MAX_REGENERATION_SPEED_LEVEL] = {2,4,6,8,10};
const TResourceD ExtendingSpeedUpdateCost[MAX_EXTENDING_SPEED_LEVEL] = {2,4,6,8,10};
const TResourceD ExtraControlStageUpdateCost[MAX_EXTRA_CONTROL_LEVEL] = {3,5,7};
const TResourceD DefenceStageUpdateCost[MAX_DEFENCE_LEVEL] = {3,5,7};

enum TDepartment
{
	Aeronautics          //航院
	, Medical            //医学
	, Energy             //能源
	, Electronic         //电子
	, Economy            //经管
	, Computer           //计算机
	, Automation         //自动化
	, Environment        //环境
};

enum TPlayerPowerProperty
{
	RegenerationSpeed    //再生速度
	, ExtendingSpeed //延伸速度
	, ExtraControl   //额外控制数
	, Defence        //防御等级
};

enum TentacleStatus
{
	Extending           //延伸中
	,Attacking          //攻击中（面对对方触手）
	,Backing            //退后中（被打退）
	,Confrontation      //对峙中
	,Arrived            //已到达目的地
	,AfterCut           //被切断
	,Finished           //已经结算完毕，消失
};

enum PointState
{
	Normal,
	Barrier,
	Disables
};

struct TPoint
{
	TPosition  m_x;
	TPosition  m_y;
	PointState m_state;
};


TPoint operator-(const TPoint& p1, const TPoint& p2)
{
	TPoint temp;
	temp.m_x = p1.m_x - p2.m_x;
	temp.m_y = p1.m_y - p2.m_y;
	temp.m_state = Disables;
	return temp;
}

//计算欧式距离
TLength getDistance(const TPoint& p1, const TPoint& p2)
{
	return sqrt(pow(p1.m_x - p2.m_x, 2) + pow(p1.m_y - p2.m_y, 2));
}

enum StudentType  //学生种类的枚举
{ 
    XianYu=0,
	XueZha,
	XueBa,
	DaLao,
	JuLao
};  

struct StudentInfo
{
	TId id;
	StudentType type;
	TId playerId;
	vector<TTentacleID> tentacleIds;
	TResourceD resource;
	TResourceD occupyPoint; 
	vector<TTentacleID> attackBy;
	TPoint position;
};

struct PlayerInfo
{
	TId id;

	int rank;          //排名/按总学力/包括触手上的
	vector<TId> studentIds;  //学生ID
	TDepartment department;            //院系种类
	TResourceD technologyPoint;        //科技点数

	TLevel RegenerationSpeedLevel;      //再生倍率等级
	TLevel ExtendingSpeedLevel;         //延伸速度等级
	TLevel ExtraControlLevel;           //额外操作数等级
	TLevel DefenceLevel;          //防御等级

	TRound skillLeftRound = 0;       //剩余主动技能回合
	size_t maxControlNumber;    //最大控制数

	TRound hackedLeftRound;                //是否被黑
	bool alive;                  //是否还活着
};

struct TentacleInfo
{
	TTentacleID     id;
	TStudentID            sourceStudent;              //源同学
	TStudentID             targetStudent;              //目标同学
	TentacleStatus  status;                     //触手状态
	TLength         length;                     //触手长度（由源/目标决定）
	TResourceI      resource;                   //当前资源      （切断前有效）
	TResourceI      frontResource;              //切断后前方资源（切断后有效）
	TResourceI      backResource;               //切断后后方资源（切断后有效）
	TTentacleID            enemyTentacle;              //对方触手
};

struct TBarrier
{
	TPoint m_beginPoint;
	TPoint m_endPoint;
};

class BaseMap
{
public:
	void   setID(TMapID _id) { id = _id;        }
	TMap   getWidth()  const { return m_width;  }
	TMap   getHeigth() const { return m_height; }
	bool   passable(TPoint p1, TPoint p2)   //判断触手能否连接这两个点
	{
		for (auto b : m_barrier)
		{
			//快速排斥实验
			int minFX = max(min(p1.m_x, p2.m_x), min(b.m_beginPoint.m_x, b.m_endPoint.m_x));
			int maxFX = min(max(p1.m_x, p2.m_x), max(b.m_beginPoint.m_x, b.m_endPoint.m_x));
			int minFY = max(min(p1.m_y, p2.m_y), min(b.m_beginPoint.m_y, b.m_endPoint.m_y));
			int maxFY = min(max(p1.m_y, p2.m_y), max(b.m_beginPoint.m_y, b.m_endPoint.m_y));
			if (minFX > maxFX || minFY > maxFY)
				return false;
			//跨越实验
			if (cross(p1 - b.m_beginPoint, b.m_endPoint - b.m_beginPoint)*cross(b.m_endPoint - b.m_beginPoint, p2 - b.m_beginPoint) >= 0
				|| cross(b.m_beginPoint - p1, p2 - p1)*cross(p2 - p1, b.m_endPoint - p1) >= 0)
				return false;
		}
		return true;
	}
	bool   isPosValid(TPoint p) { return isPosValid(p.m_x, p.m_y); }             //判断点是否越界
	bool   isPosValid(int x, int y) { return x >= 0 && x < m_width&&y >= 0 && y <= m_height; }

	const  vector<TPoint>  &  getStudentPos() const { return m_studentPos; }
	const  vector<TBarrier>&  getBarriar()    const { return m_barrier;    }
protected:
	string             id;                  //记录地图的id，由game赋值，被init函数使用，选择对应的文件
	TMap               m_width;
	TMap               m_height;
	vector<TPoint>     m_studentPos;        //只设定细胞的坐标，之后的势力分配交给game
	vector<TBarrier>   m_barrier;
private:
	int cross(const TPoint& p1, const TPoint& p2) { return p1.m_x*p2.m_y - p1.m_y*p2.m_x; }//叉乘
	int min(int a, int b) { return a < b ? a : b; }
	int max(int a, int b) { return a < b ? b : a; }
};

//命令种类
enum CommandType
{
	activeSkill    //主动技能
	,passiveSkill  //被动技能
	,addTentacle   //添加
	,cutTentacle   //切断
};

//保存命令相关信息
struct Command
{
	Command(CommandType _type, initializer_list<int> _parameters):
		type(_type),parameters(_parameters){}
	CommandType type;
	vector<int> parameters;  //参数
};

//命令列表
class CommandList
{
public:
	void addCommand(CommandType _type, initializer_list<int> _parameters)
	{
		m_commands.emplace_back(_type, _parameters);
	}
	void removeCommand(int n)
	{
		m_commands.erase(m_commands.begin() + n);
	}
	int  size() const{ return m_commands.size(); }
	vector<Command>::iterator begin() { return m_commands.begin(); }
	vector<Command>::iterator end() { return m_commands.end(); }
	Command& operator[](int n)
	{
		if (n < 0 || size() <= n)
			throw std::out_of_range("访问命令时越界");
		return m_commands[n];
	}
private:
	vector<Command> m_commands;
};

struct Info
{
	int playerSize;  //总玩家数
	int playerAlive;  //剩余玩家数
	int id;           //选手ID号
	int myMaxControl;   //最大操作数
	TRound round;     //回合数
	CommandList myCommandList;
	map<TCamp,PlayerInfo> playerInfo;   //势力信息
	map<TStudentID,StudentInfo> studentInfo; //同学信息
	map<TTentacleID, TentacleInfo> tentacleInfo; //触手信息
	BaseMap* mapInfo;  //地图信息
};

#endif // DEFINITION_H