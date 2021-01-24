#ifndef MAP_H_
#define MAP_H_

#include <string>
#include <vector>
#include "definition.h"
#include <math.h>
#define TRANSITION -1
#define PUBLIC -2
using namespace std;
namespace DATA 
{
	struct Data;
}

struct mapBlock                                 //【FC18】地图方格类
{
	int type;                                   //【FC18】地块类型，对应terrainType枚举类
	int owner;                                  //【FC18】所属玩家序号，-1为过渡TRANSITION，-2为公共PUBLIC
	vector<int> occupyPoint;                    //【FC18】各玩家的占有属性值，秩为玩家下标
};

class Map:public BaseMap
{
public:
	Map():data(nullptr){}                       //【FC18】map的默认构造函数
	DATA::Data* getData(){ return data; }       //【FC18】map也可以访问全员共享的Data
	void setData(DATA::Data* d) { data = d; }   //【FC18】map也可以修改全员共享的Data
	vector<vector<mapBlock>> gameMap;           //【FC18】游戏中实际存储的地图
	//~Map();
	bool randomInit();                          //@@@【FC18】随机生成一幅地图
	bool init(const TMapID& filename, TResourceI _MAX_RESOURCE_);             //@@@【FC18】通过文件初始化地图信息
	bool init(ifstream& inMap, TResourceI _MAX_RESOURCE_, bool enableOutput); //@@@【FC18】通过文件流初始化信息;
	void saveMapJson(string file_name);          //@@@【FC18】保存地图的json数据
private:
	DATA::Data* data;                            //【FC18】用于游戏中的数据传输
};

#endif //_MAP_H_
