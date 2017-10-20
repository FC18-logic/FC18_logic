#ifndef _DATA_H
#define _DATA_H
/*  
**  Data结构体
**  用于数据的存储和共享    
**  在外部存储相关类只需存储 ID 在 Data 中调用
*/
#include<vector>
#include<map>
#include "student.h"
#include "tentacle.h"
#include "map.h"
#include "player.h"
using std::map;
namespace DATA {
	struct Data
	{
		map<TStudentID, Student>  students;
		map<TTentacleID, Tentacle> tentacles;
		map<TCamp, Player>   players;
		Map              gameMap;
	};
}

#endif // !_DATA_H