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

const terrainType terrain[TERRAIN_TYPE_NUM + 1] =     //��FC18�����Σ���ռ�������С���������ε����㣬������������
{//                  enumֵ   ����
    Plain,       //    1       1
    Road,        //    5       2
    Mountain,    //    2       3
    Forest,      //    3       4
    Swamp,       //    4       5
    Tower        //    0       6
};

const TPoint paraOffset[8] =                                //��FC18���������������ͼʱ��ǿ��ͨ��
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

struct mapBlock                                 //��FC18����ͼ������
{
	terrainType type;                           //��FC18���ؿ����ͣ���ӦterrainTypeö����
	int owner;                                  //��FC18�����������ţ�-1Ϊ����TRANSITION��-2Ϊ����PUBLIC
	vector<int> occupyPoint;                    //��FC18������ҵ�ռ������ֵ����Ϊ����±�
	int TowerIndex;								//@@@��FC18��λ�ڸõ�Ԫ��������±꣬��Ӧ��map��data��������ָ����±�vector<Tower*>
};


class Map:public BaseMap
{
public:
	Map():data(nullptr){}                       //��FC18��map��Ĭ�Ϲ��캯��
	DATA::Data* getData(){ return data; }       //��FC18��mapҲ���Է���ȫԱ�����Data
	void setData(DATA::Data* d) { data = d; }   //��FC18��mapҲ�����޸�ȫԱ�����Data
	vector<vector<mapBlock>> map;               //��FC18����Ϸ��ʵ�ʴ洢�ĵ�ͼ
	//~Map();
	bool randomInitMap();                          //��FC18���������һ����ͼ�������������趨��ʼ����λ��
    bool readMap(ifstream& inMap, bool enableOutput);   //ͨ���ļ���ʼ����ͼ��Ϣ�����������ͼ���ݺͳ�ʼ���������
	void saveMapJson();          //@@@��FC18�������ͼ��json����


    //FC15��
    bool init(const TMapID& filename, TResourceI _MAX_RESOURCE_);             //ͨ���ļ���ʼ����ͼ��Ϣ
    bool init(ifstream& inMap, TResourceI _MAX_RESOURCE_, bool enableOutput); //ͨ���ļ�����ʼ����Ϣ;
private:
	DATA::Data* data;                            //��FC18��������Ϸ�е����ݴ���
};


class Perlin {                                                        //��FC18���������������ͼ������
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

    double Noise(int x, int y)    // ����(x,y)��ȡһ����������ֵ
    {
        int n = x + y * int(random(53, 57));
        n = (n << 13) ^ n;
        return (1.0 - ((n * (n * n * int(random(16000, 17000)) + int(random(830000, 870000))) + int(random(1300000000, 1700000000))) & 0x7fffffff) / 1073741824.0);
    }

    double SmoothedNoise(int x, int y)   //�⻬����
    {
        double corners = (Noise(x - 1, y - 1) + Noise(x + 1, y - 1) + Noise(x - 1, y + 1) + Noise(x + 1, y + 1)) / 16;
        double sides = (Noise(x - 1, y) + Noise(x + 1, y) + Noise(x, y - 1) + Noise(x, y + 1)) / 8;
        double center = Noise(x, y) / 4;
        return corners + sides + center;
    }

    double Cosine_Interpolate(double a, double b, double x)  // ���Ҳ�ֵ
    {
        double ft = x * 3.1415927;
        double f = (1 - cos(ft)) * 0.5;
        return a * (1 - f) + b * f;
    }

    double Linear_Interpolate(double a, double b, double x) //���Բ�ֵ
    {
        return a * (1 - x) + b * x;
    }

    double InterpolatedNoise(float x, float y)   // ��ȡ��ֵ����
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

    double PerlinNoise(float x, float y)    // ���յ��ã�����(x,y)������Ӧ��PerlinNoiseֵ
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
