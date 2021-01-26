#ifndef MAP_H_
#define MAP_H_

#include <string>
#include <vector>
#include "definition.h"
#include <math.h>
#include <cmath>
#include <ctime>
#define TRANSITION -1
#define PUBLIC 0
#define UNALLOCATED -1
using namespace std;
namespace DATA 
{
	struct Data;
}

const terrainType terrain[TERRAIN_TYPE_NUM + 1] =     //【FC18】地形：按占地面积大小排序，塔地形单独算，不放在这里面
{//                  enum值   排序
    Plain,       //    1       1
    Road,        //    5       2
    Mountain,    //    2       3
    Forest,      //    3       4
    Swamp,       //    4       5
    Tower        //    0       6
};

const TPoint paraOffset[8] =                                //【FC18】用于生成随机地图时增强连通性
{
    {1 , 0},
    {1 , 1},
    {0 , 1},
    {-1, 1},
    {-1, 0},
    {-1,-1},
    {0 ,-1},
    {1 ,-1}
};

struct mapBlock                                 //【FC18】地图方格类
{
	terrainType type;                           //【FC18】地块类型，对应terrainType枚举类
	int owner;                                  //【FC18】所属玩家序号，-1为过渡TRANSITION，-2为公共PUBLIC
	vector<int> occupyPoint;                    //【FC18】各玩家的占有属性值，秩为玩家下标
	int TowerIndex;								//@@@【FC18】位于该单元格的塔的下标，对应在map的data里所有塔指针的下标vector<Tower*>
};


class Map:public BaseMap
{
public:
	Map():data(nullptr){}                       //【FC18】map的默认构造函数
	DATA::Data* getData(){ return data; }       //【FC18】map也可以访问全员共享的Data
	void setData(DATA::Data* d) { data = d; }   //【FC18】map也可以修改全员共享的Data
	vector<vector<mapBlock>> map;               //【FC18】游戏中实际存储的地图
	//~Map();
	bool randomInitMap();                          //【FC18】随机生成一幅地图，分配势力和设定初始塔的位置
    bool readMap(ifstream& inMap, bool enableOutput);   //通过文件初始化地图信息，包括读入地图数据和初始化玩家数组
	void saveMapJson();          //@@@【FC18】保存地图的json数据


    //FC15的
    bool init(const TMapID& filename, TResourceI _MAX_RESOURCE_);             //通过文件初始化地图信息
    bool init(ifstream& inMap, TResourceI _MAX_RESOURCE_, bool enableOutput); //通过文件流初始化信息;
private:
	DATA::Data* data;                            //【FC18】用于游戏中的数据传输
};


class Perlin {                                                        //【FC18】柏林噪声随机地图产生器
public:
    float persistence;
    int Number_Of_Octaves;

    Perlin() {
        srand(unsigned(time(0)));
        persistence = 0.0001;
        Number_Of_Octaves = 3;
        iterRound = 1;
        connectStandard = 5;
    }
    ~Perlin(){}

    double random(double start, double end)
    {
        return start + (end - start) * rand() / (RAND_MAX + 1.0);
    }

    double Noise(int x, int y)    // 根据(x,y)获取一个初步噪声值
    {
        int n = x + y * int(random(53, 57));
        n = (n << 13) ^ n;
        return (1.0 - ((n * (n * n * int(random(16000, 17000)) + int(random(830000, 870000))) + int(random(1300000000, 1700000000))) & 0x7fffffff) / 1073741824.0);
    }

    double SmoothedNoise(int x, int y)   //光滑噪声
    {
        double corners = (Noise(x - 1, y - 1) + Noise(x + 1, y - 1) + Noise(x - 1, y + 1) + Noise(x + 1, y + 1)) / 16;
        double sides = (Noise(x - 1, y) + Noise(x + 1, y) + Noise(x, y - 1) + Noise(x, y + 1)) / 8;
        double center = Noise(x, y) / 4;
        return corners + sides + center;
    }

    double Cosine_Interpolate(double a, double b, double x)  // 余弦插值
    {
        double ft = x * 3.1415927;
        double f = (1 - cos(ft)) * 0.5;
        return a * (1 - f) + b * f;
    }

    double Linear_Interpolate(double a, double b, double x) //线性插值
    {
        return a * (1 - x) + b * x;
    }

    double InterpolatedNoise(float x, float y)   // 获取插值噪声
    {
        int integer_X = int(x);
        float  fractional_X = x - integer_X;
        int integer_Y = int(y);
        float fractional_Y = y - integer_Y;
        double v1 = SmoothedNoise(integer_X, integer_Y);
        double v2 = SmoothedNoise(integer_X + 1, integer_Y);
        double v3 = SmoothedNoise(integer_X, integer_Y + 1);
        double v4 = SmoothedNoise(integer_X + 1, integer_Y + 1);
        double i1 = Cosine_Interpolate(v1, v2, fractional_X);
        double i2 = Cosine_Interpolate(v3, v4, fractional_X);
        return Cosine_Interpolate(i1, i2, fractional_Y);
    }

    double PerlinNoise(float x, float y)    // 最终调用：根据(x,y)获得其对应的PerlinNoise值
    {
        double noise = 0;
        double p = persistence;
        int n = Number_Of_Octaves;
        for (int i = 0; i < n; i++)
        {
            double frequency = pow(2, i);
            double amplitude = pow(p, i);
            noise = noise + InterpolatedNoise(x * frequency, y * frequency) * amplitude;
        }

        return noise;
    }

    friend class Map;
private:
    int iterRound;
    int connectStandard;
};

#endif //_MAP_H_
