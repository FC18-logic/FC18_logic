#ifndef MAP_H_
#define MAP_H_
#include<vector>
#include <string>
#include "definition.h"
using namespace std;
namespace DATA 
{
	struct Data;
}


class Map:public BaseMap
{
public:
	Map():data(nullptr){}
	void setData(DATA::Data* d) { data = d; }
	//~Map();

	//void   setWidth(TMap );
	//void   setHeiget(TMap );
	
	bool init(const TMapID& filename, TResourceI _MAX_RESOURCE_);
	bool init(ifstream& inMap, TResourceI _MAX_RESOURCE_); //通过文件流初始化信息;
private:
	DATA::Data* data;
};

#endif //_MAP_H_
