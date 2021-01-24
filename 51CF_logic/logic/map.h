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
	Map():data(nullptr){}//map也可以访问全员共享的Data
	void setData(DATA::Data* d) { data = d; }//map也可以修改全员共享的Data
	//~Map();
	bool init(const TMapID& filename, TResourceI _MAX_RESOURCE_);
	bool init(ifstream& inMap, TResourceI _MAX_RESOURCE_, bool enableOutput); //通过文件流初始化信息;
private:
	DATA::Data* data;
};

#endif //_MAP_H_
