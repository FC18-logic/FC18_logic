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

struct mapBlock                                 //��FC18����ͼ������
{
	int type;                                   //��FC18���ؿ����ͣ���ӦterrainTypeö����
	int owner;                                  //��FC18�����������ţ�-1Ϊ����TRANSITION��-2Ϊ����PUBLIC
	vector<int> occupyPoint;                    //��FC18������ҵ�ռ������ֵ����Ϊ����±�
};

class Map:public BaseMap
{
public:
	Map():data(nullptr){}                       //��FC18��map��Ĭ�Ϲ��캯��
	DATA::Data* getData(){ return data; }       //��FC18��mapҲ���Է���ȫԱ�����Data
	void setData(DATA::Data* d) { data = d; }   //��FC18��mapҲ�����޸�ȫԱ�����Data
	vector<vector<mapBlock>> gameMap;           //��FC18����Ϸ��ʵ�ʴ洢�ĵ�ͼ
	//~Map();
	bool randomInit();                          //@@@��FC18���������һ����ͼ
	bool init(const TMapID& filename, TResourceI _MAX_RESOURCE_);             //@@@��FC18��ͨ���ļ���ʼ����ͼ��Ϣ
	bool init(ifstream& inMap, TResourceI _MAX_RESOURCE_, bool enableOutput); //@@@��FC18��ͨ���ļ�����ʼ����Ϣ;
	void saveMapJson(string file_name);          //@@@��FC18�������ͼ��json����
private:
	DATA::Data* data;                            //��FC18��������Ϸ�е����ݴ���
};

#endif //_MAP_H_
