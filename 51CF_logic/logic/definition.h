#pragma once
#ifndef DEFINITION_H
#define DEFINITION_H
#define FC15_DEBUG
#define NO_SILENT_MODE
#define NO_JSON
#define INF 100000000
#define TRANSITION -1   //���ɵ�������
#define PUBLIC 0      //������������
#define NOTOWER -1    //��ǰ����û�з�����
#define NOTASK  -1    //��ǰ����������������
#define OUTOFRANGE -2  //��ǰ�����ڵ�ͼ֮��


#include <vector>
#include <string>
#include <initializer_list>
#include <stdexcept>
#include <map>
#include <set>
#include <iostream>

using namespace std;
typedef int    TMovePoint;     //��FC18���ж���
typedef int    TBattlePoint;   //��FC18��ս����
typedef int    THealthPoint;   //��FC18������ֵ
typedef int    TBuildPoint;    //��FC18���Ͷ��������̽����������
typedef int    TProductPoint;  //��FC18������������
typedef int	   TExperPoint;    //��FC18�����ľ���ֵ
typedef int    TOccupyPoint;   //��FC18��������Χ����ʩ�ӵ�ռ������ֵ
typedef int    TIntPara;       //��FC18������ϵ��ֵ
typedef double TDoublePara;    //��FC18��˫���ȸ�����ϵ��ֵ
typedef int    TPlayer;        //��FC18����Ҹ���
typedef int    TTower;         //��FC18�����ĸ���
typedef int    TCorps;         //��FC18�����Ÿ���
typedef int    TPlayerID;      //��FC18�����ID��
typedef int    TTowerID;       //��FC18��������ID��
typedef int    TCorpsID;       //��FC18������ID��
typedef string TMapID;         //��FC18����ͼID��
typedef int    TMap;           //��FC18����ͼ�������߶�/��ȣ�
typedef int    TRound;         //��FC18���غ���
typedef int    TScore;         //��FC18����ҵ÷�
typedef int    TDist;          //��FC18����Ϸ�о���Ķ���
typedef int    TOperaNum;      //��FC18���������ĸ���


const int MAX_CORPS_LEVEL = 3;    //��FC18�����ı��ŵȼ�
const int MAX_TOWER_LEVEL = 8;    //��FC18�����ķ������ȼ�
const int TERRAIN_TYPE_NUM = 5;    //��FC18�����ε�������
const int BATTLE_CORPS_TYPE_NUM = 3;    //��FC18����ս���ŵ�������
const int CONSTRUCT_CORPS_TYPE_NUM = 2;    //��FC18�����̱��ŵ�������
const int TOWER_PRODUCT_TASK_NUM = 6;     //��FC18��������������������
const int TOWER_EXPER_GAIN_SCALE = 3;     //��FC18������ÿ�غϾ���ֵ���ӵȼ���
const int OCCUPY_POINT_DIST_SCALE = 5;    //��FC18��������Χ����ʩ��ռ������ֵ�ľ���ȼ��м���
const int CORPS_ACTION_TYPE_NUM = 10;    //��FC18�������ܽ��еĲ���������
const int TOWER_ACTION_TYPE_NUM = 3;     //��FC18�����������еĲ���������
const int MAX_ROUND = 500;               //��FC18����Ϸȫ�̵����غ�����ȷ���ģ�����FC15���ⲿ�ļ�����
const int TOWER_SCORE = 10;              //��FC18��������ҵ÷�ʱÿ��������ÿ���ȼ��÷�
const int BATTLE_CORP_SCORE = 2;         //��FC18��ս������ÿ���Ǽ��÷�
const int CONSTRUCT_CORP_SCORE = 4;      //��FC18�����̱���ÿ���÷�
const int MAX_CMD_NUM = 10;              //��FC18������ÿ�����ÿ�����������

class Crops;
struct CorpsInfo;
typedef vector<Crops*>		CorpsUnit;	//��FC18��һ����Ԫ�������б���
typedef CorpsUnit**			Army;		//��FC18��������ͼ�ϵ����б��� [i][j]��ʾx=i,y=jλ�õ����б���
typedef vector<CorpsInfo>	CorpsInfoUnit; //��FC18��һ����Ԫ�������б�����Ϣ

//��������FC15������Ӧ���ò�����
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef double TSpeed;
typedef double TResourceD;  //double ����Դ���������ڲ�����
typedef int    TResourceI;  //int    ����Դ����������ʾ
typedef double TTechPoint;  //�Ƽ�����
typedef double TLength;
typedef int    TCellID;
typedef int    TTentacleID;
typedef int    TPosition;
typedef int    TLevel;  //�������Եȼ�
typedef double TPower;  //����
typedef int    TTentacleNum;

//�����ܶ�
const double       Density = 0.1;
const TSpeed       BaseExtendSpeed = 3;
const TSpeed       BaseFrontSpeed = 20;
const TSpeed       BaseBackSpeed = 12;
const TLevel       STUDENT_LEVEL_COUNT = 5;
const TResourceI   MAX_RESOURCE = 200;
const TSpeed       BASE_REGENERETION_SPEED[STUDENT_LEVEL_COUNT]{ 1,1.5,2,2.5,3 };
const TTentacleNum MAX_TENTACLE_NUMBER[STUDENT_LEVEL_COUNT]{ 1,2,2,3,3 };  //���촥������
const TResourceI   STUDENT_STAGE[STUDENT_LEVEL_COUNT + 1]{ 0 ,10,40,80,150,MAX_RESOURCE };
const int          NO_DATA = -1;
const TPlayerID        Neutral = NO_DATA;

//����ܵȼ�
const TLevel MAX_REGENERATION_SPEED_LEVEL = 5;
const TLevel MAX_EXTENDING_SPEED_LEVEL = 5;
const TLevel MAX_EXTRA_CONTROL_LEVEL = 3;
const TLevel MAX_DEFENCE_LEVEL = 3;

//�����ܵȼ���Ӧ��ֵ
const TPower RegenerationSpeedStage[MAX_REGENERATION_SPEED_LEVEL + 1] = { 1,1.05,1.1,1.15,1.2,1.25 };
const TPower SpeedStage[MAX_EXTENDING_SPEED_LEVEL + 1] = { 1,1.1,1.2,1.3,1.4,1.5 };
const TPower ExtraControlStage[MAX_EXTRA_CONTROL_LEVEL + 1] = { 0,0.5,1,1.5 };
const TPower DefenceStage[MAX_DEFENCE_LEVEL + 1] = { 1.5,1.4,1.3,1.2 };

//����������������ƴ�����
const TResourceD RegenerationSpeedUpdateCost[MAX_REGENERATION_SPEED_LEVEL] = { 2,4,6,8,10 };
const TResourceD ExtendingSpeedUpdateCost[MAX_EXTENDING_SPEED_LEVEL] = { 2,4,6,8,10 };
const TResourceD ExtraControlStageUpdateCost[MAX_EXTRA_CONTROL_LEVEL] = { 3,5,7 };
const TResourceD DefenceStageUpdateCost[MAX_DEFENCE_LEVEL] = { 3,5,7 };
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//��FC18����ά�����ṹ��
struct TPoint
{
	TPosition  m_x;
	TPosition  m_y;
};

//��FC18�������������ݽṹ��
struct TowerConfig {
	int initBuildPoint;       //��ʼ������
	int initProductPoint;      //��ʼս����
	int initHealthPoint;      //��ʼ����ֵ
	int upgradeExper;         //������һ�����辭��ֵ
	int battleRegion;         //��������
};


//��FC18����������
enum corpsType
{
	Battle = 0,          //��ս����
	Construct = 1        //���̽������
};


//��FC18����ս���������ö����
enum battleCorpsType
{
	Warrior = 0,         //սʿ
	Archer  = 1,         //������
	Cavalry = 2,         //���
};


//��FC18�����̽�����ŵ�ö����
enum constructCorpsType
{
	Builder = 0,           //������
	Extender = 1           //������
};




//��FC18����������
enum commandType {
	corpsCommand = 0,         //��������
	towerCommand = 1,         //����������
};


//��FC18�����Ų������ͣ�Cǰ׺��ʾCorps��
enum CorpsCommandEnum
{//                                                          ��ս����        ���̱���
	CMove          = 0,       //�ڵ�ͼ���ƶ�                     ��              ��
	CStation       = 1,       //פ���ڵ�ͼ�������Լ�����       ��              ��
	CStationTower  = 2,       //פ���������Լ�����������         ��              ��
	CAttackCorps   = 3,       //�����Է������ı���               ��
	CAttackTower   = 4,       //�����Է���������                 ��
	CRegroup       = 5,       //��������                         ��
	CDissolve      = 6,       //���Ž�ɢ                         ��              ��
	CBuild         = 7,       //�޽��·�����                                     ��
	CRepair        = 8,       //����ԭ������                                     ��
	CChangeTerrain = 9,       //�ı䷽�����                                     ��
};


//��FC18�������ƶ��ķ���
enum corpsMoveDir
{
	CUp            = 0,         //�����ƶ�1��
	CDown          = 1,         //�����ƶ�1��
	CLeft          = 2,         //�����ƶ�1��
	CRight         = 3          //�����ƶ�1��
};

//��FC18�����Ĳ������ͣ�Tǰ׺��ʾtower��
enum towerCommand
{
	TProduct       = 0,       //��������
	TAttackCorps   = 1,       //������������
	TAttackTower   = 2        //����������
};


//��FC18�����������������ͣ�P��ʾproduct��
enum productType
{//                                                 �����ر�
	PWarrior       = 0,       //����սʿ         1star-սʿ����
	PArcher        = 1,       //����������      1star-�����ֱ���
	PCavalry       = 2,       //�������         1star-�������
	PBuilder       = 3,       //����������        1-�����߱���
	PExtender      = 4,       //����������        1-�����߱���
	PUpgrade       = 5        //����������      ���ȼ�+1��max=8)
};


//��FC18�����ε�ö���ࣨTRǰ׺��ʾ���Σ�
enum terrainType
{
	TRTower     = 0,       //��
	TRPlain     = 1,       //ƽԭ
	TRMountain  = 2,       //ɽ��
	TRForest    = 3,       //ɭ��
	TRSwamp     = 4,       //����
	TRRoad      = 5,       //��·
};

//��FC18�����Ų�����
const string CorpsCmd[CORPS_ACTION_TYPE_NUM] = 
{
	"move","station","station at tower","attack corps","attack tower","regroup","dissolve","build tower","repair tower","change terrain"
};

//��FC18����������
const string TowerCmd[TOWER_ACTION_TYPE_NUM] = 
{
	"produce","attack corps","attack tower"
};

//��FC18��������������
const string ProductCmd[TOWER_PRODUCT_TASK_NUM] = 
{
	"warrior","archer","caster","builder","extender","upgrade"
};

//��FC18���ƶ�������
const string Direction[4] = 
{
	"up","down","left","right"
};

//��FC18���ı�ĵ�����
const string Terrain[TERRAIN_TYPE_NUM] = 
{
	"plain","mountain","forest","swamp","road"
};

//��FC18����ս��������
const string BattleName[3] =
{
	"warrior","archer","caster"
};

//��FC18�����̱�������
const string ConstructName[2] =
{
	"Builder","Explorer"
};

//��FC18����ս�����ж���������ս���ŵ�ö����������϶�Ӧ���ҿ����˵ȼ���
const TMovePoint battleMovePoint[BATTLE_CORPS_TYPE_NUM][MAX_CORPS_LEVEL] = 
{
	{2,   2,   2},    //սʿ
	{2,   2,   2},    //������
	{4,   4,   4}     //���
};

//��FC18�����̱��ж�����������û�У��ȼ�����������
const TMovePoint constructMovePoint[CONSTRUCT_CORPS_TYPE_NUM] = {2,2};

//��FC18����ս���ų�ʼս����������ս���ŵ�ö����������϶�Ӧ���ҿ����˵ȼ���
const TBattlePoint corpsBattlePoint[BATTLE_CORPS_TYPE_NUM][MAX_CORPS_LEVEL] = 
{
	{36,   44,   52},    //սʿ
	{30,   38,   46},    //������
	{44,   52,   60}     //���
};


//��FC18����ս���ŵ�ս��������ϵ��������ս���ŵ�ö����������϶�Ӧ���ҿ����˵ȼ���
const TIntPara corpsBattleGain[BATTLE_CORPS_TYPE_NUM][MAX_CORPS_LEVEL] = 
{
	{2,   2,   2},    //սʿ
	{2,   2,   2},    //������
	{4,   4,   4}     //���
};


//��FC18����ս���ŵĹ���ϵ��������ս���ŵ�ö����������϶�Ӧ���ҿ����˵ȼ���
const TDoublePara corpsAttackTowerGain[BATTLE_CORPS_TYPE_NUM][MAX_CORPS_LEVEL] = 
{
	{0.4,   0.4,   0.4},    //սʿ
	{0.7,   0.7,   0.7},    //������
	{0.5,   0.5,   0.5}     //���
};


//��FC18����ս���ų�ʼ����ֵ������ս���ŵ�ö����������϶�Ӧ���ҿ����˵ȼ���
const THealthPoint battleHealthPoint[BATTLE_CORPS_TYPE_NUM][MAX_CORPS_LEVEL] = 
{
	{60,   80,   90},    //սʿ
	{50,   70,   90},    //������
	{70,   90,   110}    //���
};

//��FC18��ս��������̾��� սʿ1 ������2 ���1
const int TBattleRange[BATTLE_CORPS_TYPE_NUM] = { 1, 2, 1 };

//��FC18�����Ų�������������������ж�ָ��Ϸ��ԣ�����Ų�����ö����������϶�Ӧ)
const TOperaNum CorpsOperaNumNeed[CORPS_ACTION_TYPE_NUM] =
{
	3,    //�ƶ�
	2,    //פ��
	2,    //פ����
	3,    //��������
	3,    //������
	3,    //��������
	INF,  //���Ž�ɢ��ȥ����
	2,    //�޽���
	2,    //ά����
	3     //�ĵ���
};


//��FC18�����̽�����Ų������Ͷ������ģ�����Ų�����ö����������϶�Ӧ)
const TBuildPoint constructBuildCost[CORPS_ACTION_TYPE_NUM] = 
{
	0,       //�ڵ�ͼ���ƶ�
	0,       //פ���ڵ�ͼ�������Լ�����
	0,       //פ���������Լ�����������
	0,       //�����Է������ı���
	0,       //�����Է���������
	0,       //��������
	0,       //���Ž�ɢ
	0,       //�޽��·�����
	1,       //����ԭ������
	1,       //�ı䷽�����
};

const TOperaNum towerOperaNumNeed[TOWER_ACTION_TYPE_NUM] =
{
	3,       //��������
	3,       //����������
	3        //��������
};

//��FC18������������������������ֵ
const TProductPoint TowerProductCost[TOWER_PRODUCT_TASK_NUM] = 
{//                                 �����ر�                                           ����˵��
	40,       //����սʿ         1star-սʿ����
	60,       //����������      1star-�����ֱ���
	100,      //�������         1star-�������
	40,       //����������        1-�����߱���
	40,       //����������        1-�����߱���
	40        //����������      ���ȼ�+1��max=8)       ����40����Сֵ��ʵ��ֵΪִ�и�����ĵ�һ�غ����ĵȼ�*40��Ҫ����ʵ�������  
};


//��FC18��ÿ�غ����ľ���ֵ������غ����Ĺ�ϵ
//��GC18��ע��ʵ�ʷ��ʵ�ʱ�򣬴���غ���/100��ȡ����300�غϴ���2,301�غ�����3
//��FC18����ֱ�ӹ�ʽ���㣺5 + 5 * floor(currentRound / 100)��ע��߽�ֵҪ����Դ�
const TExperPoint TowerExperGain[TOWER_EXPER_GAIN_SCALE + 1] = 
{//               �غ���
	5,//          [0,100)
	10,//         [100,200)
	15,//         [200,300]
	0//           (300,+��)
};


//��FC18��������Χ����ʩ�ӵ�ռ������ֵ�������Ĺ�ϵ��
const TOccupyPoint TowerOccupyPoint[OCCUPY_POINT_DIST_SCALE + 1] = 
{//                ��������ľ���
	100,//              1��
	80,//               2��
	50,//               3��
	20,//               4��
	10,//               5��
	0//               6����Զ
};


//��FC18����ÿ���ȼ��ʼ��ʱ��������Ե�ֵ�����ڳ�ʼ������
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


//��FC18���ж������ģ�����ε�ö����������϶�Ӧ��
const TMovePoint CorpsMoveCost[TERRAIN_TYPE_NUM + 1] = 
{
	0,    //��
	2,    //ƽԭ
	4,    //ɽ��
	3,    //ɭ��
	4,    //����
	1,    //��·
};


//��FC18��ս�������棨����ε�ö����������϶�Ӧ��
const TBattlePoint CorpsBattleGain[TERRAIN_TYPE_NUM + 1] = 
{
	0,    //��
	0,    //ƽԭ
	5,    //ɽ��
	3,    //ɭ��
	-3,   //����
	0,    //��·
};


//��������FC15������Ӧ���ò�����
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum CellStrategy
{
	Normal    //��ʼ״̬
	, Attack  //����  
	, Defence //����
	, Grow    //����
};

//ϸ�����Ըı仨�ѿƼ���
const TTechPoint CellChangeCost[4][4] ={
	//TO        N    A    D    G
	/*F  N */   0,   3,   3,   3,
	/*R  A */   2,   0,   5,   5,
	/*O  D */   2,   5,   0,   5,
	/*M  G */   2,   5,   5,   0
};

//ϸ���������ı���
const TPower CellConfrontPower[4][4] ={
	//TO        N    A    D    G
	/*F  N */  1.0, 1.0, 1.0, 1.0,
	/*R  A */  2.0, 1.0, 1.0, 5.0,
	/*O  D */  1.0, 3.0, 1.0, 1.0,
	/*M  G */  2.0, 1.0, 1.0, 1.0
};

//ϸ��ѹ�����ı���
const TPower CellSupressPower[4][4] ={
	//TO        N    A    D    G
	/*F  N */  1.5, 1.5, 1.0, 1.5,
	/*R  A */  3.0, 1.5, 1.0, 6.0,
	/*O  D */  1.0, 1.0, 1.0, 1.0,
	/*M  G */  3.0, 1.5, 1.0, 1.5
};

//ϸ����Դ��������
const TPower CellStrategyRegenerate[4] ={
	//    N    A    D    G
		 1.0, 1.0, 0.5, 1.5
};

const TPower TentacleDecay[4] = {
	//��������   0    1    2    3
	           1.0, 1.0, 0.8, 0.6
};

enum TPlayerProperty
{
	RegenerationSpeed    //�����ٶ�
	, ExtendingSpeed //�����ٶ�
	, ExtraControl   //���������
	, CellWall        //�����ȼ�
};

enum TentacleState
{
	Extending           //������
	, Attacking          //�����У���ԶԷ����֣�
	, Backing            //�˺��У������ˣ�
	, Confrontation      //������
	, Arrived            //�ѵ���Ŀ�ĵ�
	, AfterCut           //���ж�
};

enum CellType  //ϸ�������ö��
{
	Alpha = 0,
	Beta_1,
	Beta_2,
	Gamma_1,
	Gamma_2
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//@@@��FC18���������ṹ�壬����Ҫ����Ϣ�ټ�

struct TowerInfo {
	bool    exist;      //�������Ƿ����
	TTowerID      ID;   //������ID
	TPlayerID     ownerID;  //�������ID
	TPoint        position;    //λ��
	TProductPoint productPoint;  //������
	TProductPoint productConsume;  //��ǰ���������Ӧ������������ֵ	*���䶨��
	TBattlePoint  battlePoint;   //ս����
	THealthPoint  healthPoint;   //����ֵ
	TExperPoint   experPoint;    //����ֵ
	TLevel        level;       //�ȼ�
	productType   pdtType;    //������������
};


//@@@��FC18�����Žṹ�壬����Ҫ����Ϣ�ټ�
struct CorpsInfo
{
	//����Ҫ����������ھͲ�¼����Ϣ��bool	exist;		//�Ƿ����
	TPoint	pos;		//��������
	int		level;		//���ŵȼ�
	TCorpsID		ID;	//����ID
	THealthPoint	HealthPoint;	//����ֵ
	TBuildPoint		BuildPoint;		//�Ͷ���
	TPlayerID		owner;			//�������ID
	corpsType       type;           //��������
	TMovePoint      movePoint;      //�ж���
	battleCorpsType		m_BattleType;	//ս������
	constructCorpsType	m_BuildType;	//�������
};


//@@@��FC18����ҽṹ��
struct PlayerInfo
{
	TPlayerID id;                        //��FC18����ҵ���ţ��ӣ�[0 or 1]��ʼ��
	int rank;                            //��FC18����ѡ��������������ֱ���ɳ��ֻغ�����λ�Σ�|������߰��������÷ֺͱ��ŵ÷���������|��ͬ���ΰ���������ռ��������з�����������²�з����������μ���������|������ͬ�������������������
	bool alive;                          //��FC18������Ƿ񻹻���

	//@@@��FC18���������������ţ�����ԭ����set<TCellID> cells
	set<TTowerID> tower;


	//��FC18��������б��ŵ���ţ�����Ҳ��set�������ݽṹ���ڲ��������������������
	set<TCorpsID> corps; //���б���
	//@@@��FC18��������Ҫ��ӵ����Ի�ӿ�


	//FC15��
	TResourceD technologyPoint;        //�Ƽ�����
	TLevel RegenerationSpeedLevel;      //�������ʵȼ�
	TLevel ExtendingSpeedLevel;         //�����ٶȵȼ�
	TLevel ExtraControlLevel;           //����������ȼ�
	TLevel DefenceLevel;          //�����ȼ�
	size_t maxControlNumber;    //��������
	set<TCellID> cells; //���е�ϸ��
};


//Info�Ĵ��������vector<vector<>> ������
struct mapBlock                                 //��FC18����ͼ������
{
	terrainType type;                           //��FC18���ؿ����ͣ���ӦterrainTypeö����
	vector<int> occupyPoint;                    //��FC18������ҵ�ռ������ֵ����Ϊ������-1
	int owner;                                  //��FC18�����������ţ�-1Ϊ����TRANSITION��-2Ϊ����PUBLIC
	int TowerIndex;								//@@@��FC18��λ�ڸõ�Ԫ��������±꣬��Ӧdata���myTowers
};

//��FC18����ͼ��Ԫ����Ϣ�ṹ��
struct mapBlockInfo
{
	terrainType type;                           //��FC18���ؿ����ͣ���ӦterrainTypeö����
	int owner;                                  //��FC18�����������ţ�-1Ϊ����TRANSITION��-2Ϊ����PUBLIC
	vector<int> occupyPoint;                    //��FC18������ҵ�ռ������ֵ����Ϊ������-1
};

struct CellInfo
{
	TCellID id;
	CellType type;
	TPlayerID owner;
	CellStrategy strategy;

	TResourceD resource;
	TResourceD occupyPoint;  //ֻ������ʱ��������
	TPlayerID occupyOwner;//ֻ������ʱ��������

	TPoint position;

	TResourceD maxResource;
	int maxTentacleNum;  //���������
	int currTentacleNum;
	TPower techSpeed;    //�ƴ���������Դ�������ʵļ���
};

//��FC15��
struct TentacleInfo
{
	bool	exist;		//�Ƿ����
	TCellID         sourceCell;              //Դͬѧ
	TCellID         targetCell;              //Ŀ��ͬѧ
	TentacleState   state;                     //����״̬
	TLength         maxlength;                     //���ֳ��ȣ���Դ/Ŀ�������
	TResourceD      resource;                   //��ǰ��Դ      ���ж�ǰ��Ч��
	TResourceD      frontResource;              //�жϺ�ǰ����Դ���жϺ���Ч��
	TResourceD      backResource;               //�жϺ����Դ���жϺ���Ч��
};

//������
class CommandList;
//������ѧ����
//��ά���������
TPoint operator-(const TPoint& p1, const TPoint& p2);
//����ŷʽ����
TLength getDistance(const TPoint& p1, const TPoint& p2);
//����FC18����������Χ�ľ��루FC18���룩
TDist getDist(const TPoint& p1, const TPoint& p2);
TDist getDist(const int p1_x, const int p1_y, const int p2_x, const int p2_y);
//����ָ����������������
int generateRanInt(int start, int end);
//�������´��ָ�
std::ostream& operator << (std::ostream& os, const CommandList& cl);

struct TBarrier
{
	TPoint m_beginPoint;
	TPoint m_endPoint;
};


//@@@��FC18����ͼ����
class BaseMap
{
	public:
		void   setID(TMapID _id) { id = _id; }         //��FC18�����õ�ͼ�����
		TMap   getWidth()  const { return m_width; }   //��FC18����ȡ��ͼ���
		TMap   getHeigth() const { return m_height; }  //��FC18����ȡ��ͼ�߶�

		string             id;                         //��FC18����¼��ͼ��id����game��ֵ����init����ʹ�ã�ѡ���Ӧ���ļ�
		TMap               m_width;                    //��FC18����ͼ���
		TMap               m_height;                   //��FC18����ͼ�߶�

		//@@@��FC18���洢��ͼ�ϵ����з�������Ϣ��������Ԫ��Ϊ��������Ϣ�ṹ�壩�����Բ���vector<TPoint> m_studentPos
		vector<TPoint>     m_studentPos;               //ֻ�趨ϸ�������֮꣬����������佻��game

		//��FC18���洢��ͼ�ϵ����б�����Ϣ��������Ԫ��Ϊ������Ϣ�ṹ�壩�����Բ���vector<TPoint> m_studentPos
		vector<CorpsInfoUnit>     m_corpsinfo;

		//@@@��FC18����ȡ��ͼ�ϵ����з�������Ϣ���������Բ���const  vector<TPoint>& getStudentPos() const
		//@@@��FC18������һ����������Ϣ�ṹ���vector���ã������ⲿ�����޸�
		const  vector<TPoint>& getStudentPos() const { return m_studentPos; }

		//��FC18����ȡ��ͼ�ϵ����б�����Ϣ���������Բ���const  vector<TPoint>& getStudentPos() const
		//��FC18������һ��������Ϣ�ṹ���vector���ã������ⲿ�����޸�
		const  vector<CorpsInfoUnit>& getCropsInfo() const { return m_corpsinfo; }

		//FC15��
		vector<TBarrier>   m_barrier;                  //��¼�����ϰ�����Ϣ
		const  vector<TBarrier>& getBarriar()    const { return m_barrier; }
		bool   passable(TPoint p1, TPoint p2)          //�жϴ����ܷ�������������
		{
			for (auto b : m_barrier)
			{
				//�����ų�ʵ��
				int minFX = max(min(p1.m_x, p2.m_x), min(b.m_beginPoint.m_x, b.m_endPoint.m_x));
				int maxFX = min(max(p1.m_x, p2.m_x), max(b.m_beginPoint.m_x, b.m_endPoint.m_x));
				int minFY = max(min(p1.m_y, p2.m_y), min(b.m_beginPoint.m_y, b.m_endPoint.m_y));
				int maxFY = min(max(p1.m_y, p2.m_y), max(b.m_beginPoint.m_y, b.m_endPoint.m_y));
				if (minFX > maxFX || minFY > maxFY)
					return false;
				//��Խʵ��
				if (cross(p1 - b.m_beginPoint, b.m_endPoint - b.m_beginPoint)*cross(b.m_endPoint - b.m_beginPoint, p2 - b.m_beginPoint) >= 0
					|| cross(b.m_beginPoint - p1, p2 - p1)*cross(p2 - p1, b.m_endPoint - p1) >= 0)
					return false;
			}
			return true;
		}
		bool   isPosValid(TPoint p) { return isPosValid(p.m_x, p.m_y); }             //�жϵ��Ƿ�Խ��
		bool   isPosValid(int x, int y) { return x >= 0 && x < m_width&&y >= 0 && y < m_height; }
  //protected:
	private:
		int cross(const TPoint& p1, const TPoint& p2) { return p1.m_x*p2.m_y - p1.m_y*p2.m_x; }//���
		int min(int a, int b) { return a < b ? a : b; }
		int max(int a, int b) { return a < b ? b : a; }
};

//��������
enum CommandType
{
	upgrade          //��������
	, changeStrategy //�ı�ϸ������
	, addTentacle    //��Ӵ���
	, cutTentacle    //�жϴ���
};

//��FC18���������������Ϣ
struct Command
{
	Command(commandType _FC18type, initializer_list<int> _FC18parameters) :  //��FC18���ɳ�ʼ���б���Ĺ��캯��
		cmdType(_FC18type), parameters(_FC18parameters){}
	Command(commandType _FC18type, vector<int> _FC18parameters) :            //��FC18����Ĭ���������鹹��Ĺ��캯��
		cmdType(_FC18type), parameters(_FC18parameters){}
	Command() {}                                                             //��FC18��ָ�����������
	commandType cmdType;                                                     //��FC18����������
	vector<int> parameters;                                                  //��FC18��������ע�����в��������ͣ�


	//FC15��
	Command(CommandType _type, initializer_list<int> _parameters) :
		type(_type), parameters(_parameters) {}
	Command(CommandType _type, vector<int> _parameters) :
		type(_type), parameters(_parameters) {}
                                                       
	CommandType type;
};

//��FC18�������б�
class CommandList
{
	public:
		void addCommand(commandType _FC18type, initializer_list<int> _FC18parameters)  //��FC18���ɳ�ʼ���б�ֱ���������
		{
			if (size() >= MAX_CMD_NUM) return;
			m_commands.emplace_back(_FC18type, _FC18parameters);
		}
		void addCommand(commandType _FC18type, vector<int> _FC18parameters)            //��FC18����Ĭ������������������Ҫ�������죩
		{
			if (size() >= MAX_CMD_NUM) return;
			Command newCmd;
			newCmd.cmdType = _FC18type;
			newCmd.parameters = _FC18parameters;
			m_commands.push_back(newCmd);
		}
		void removeCommand(int n)                                                      //��FC18���Ƴ���n������
		{
			if (n < 0 || n >= size())
				throw std::out_of_range("�Ƴ�����ʱԽ��");
			m_commands.erase(m_commands.begin() + n);
		}
		vector<Command> getCommand() { return m_commands; }                            //��FC18����ȡ��������
		Command& operator[](int n)                                                     //��FC18�����ʵ�n��������ظ����������
		{
			if (n < 0 || size() <= n)
				throw std::out_of_range("��������ʱԽ��");
			return m_commands[n];
		}
		int size() const { return int(m_commands.size()); }                            //��FC18����ȡ�ܹ�����������
		vector<Command>::iterator begin() { return m_commands.begin(); }               //��FC18�����ص�һ������ĵ�����
		vector<Command>::iterator end() { return m_commands.end(); }                   //��FC18���������һ������ĵ�����
		vector<Command>::const_iterator  begin() const { return m_commands.cbegin(); } //��FC18�����ص�һ������ĳ���������
		vector<Command>::const_iterator end() const { return m_commands.cend(); }      //��FC18���������һ������ĳ���������


		//FC15��
		void addCommand(CommandType _type, initializer_list<int> _parameters)
		{
			m_commands.emplace_back(_type, _parameters);
		}
		void addCommand(CommandType _type, vector<int> _parameters)
		{
			Command c;
			c.type = _type;
			c.parameters = _parameters;
			m_commands.push_back(c);
		}
	private:
		vector<Command> m_commands;                                                    //��FC18��ָ�vector�洢
};


//@@@��FC18����������ҹ����ϵĸ�����Ϣ
struct Info
{
	TPlayer totalPlayers;                                   //��FC18���������                               
	TPlayer playerAlive;                                    //��FC18��ʣ�������
	TRound totalRounds;                                     //��FC18����ǰ�غ���
	TTower totalTowers;                                     //��FC18���ܵķ���������
	TCorps totalCorps;                                      //��FC18���ܵı��Ÿ���
	TPlayerID myID;                                         //��FC18��ѡ��ID��

	CommandList myCommandList;                              //��FC18�����ڽ�����ҷ�����ָ���ָ�

	//@@@��FC18����������������Ϣ��vector�����Բ���ԭ����vector<PlayerInfo> playerInfo;
	vector<PlayerInfo> playerInfo;   //������Ϣ

	//@@@��FC18���������з�������Ϣ��vector�����Բ���ԭ����vector<CellInfo> cellInfo;
	vector<TowerInfo> towerInfo;

	//@@@��FC18���������б�����Ϣ��vector�����Բ���ԭ����vector<vector<TentacleInfo> > tentacleInfo;
	//vector<vector<CorpsInfoUnit>> corpsInfo;//�±�Ϊij��λ�ñ�ʾλ��Ϊx:i,y:j�ı�����Ϣ
	vector<CorpsInfo> corpsInfo;

	//��FC18����ͼ��Ϣ
	//vector<vector<mapBlockInfo>> mapInfo;
	const vector<vector<mapBlock>>* mapInfo;

	//FC15��
	BaseMap* mapInfo;
	TRound round;
	int playerSize;
	int cellNum;    //ϸ��������
	int myMaxControl;    //��������
	vector<vector<TentacleInfo> > tentacleInfo; //������Ϣ
	vector<CellInfo> cellInfo; //ͬѧ��Ϣ
};

#endif // DEFINITION_H
