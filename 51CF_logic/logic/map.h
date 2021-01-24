#ifndef MAP_H_
#define MAP_H_

#include <string>
#include "definition.h"
#include <math.h>
using namespace std;
namespace DATA 
{
	struct Data;
}


class Map:public BaseMap
{
public:
	Map():data(nullptr){}//mapҲ���Է���ȫԱ�����Data
	void setData(DATA::Data* d) { data = d; }//mapҲ�����޸�ȫԱ�����Data
	//~Map();
	bool init(const TMapID& filename, TResourceI _MAX_RESOURCE_);
	bool init(ifstream& inMap, TResourceI _MAX_RESOURCE_, bool enableOutput); //ͨ���ļ�����ʼ����Ϣ;
private:
	DATA::Data* data;
};

#endif //_MAP_H_
