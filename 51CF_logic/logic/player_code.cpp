/* DAGAN 2017, player_code.cpp
*
* Game <-- Player_Ai ---> AI
*
*/
#include <time.h>
#include "player_code.h"
#include <iostream>

#ifdef __GNUC__
unsigned long GetTickCount()
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}
#endif //__GNUC__

namespace DAGAN {
	using namespace std;
	Player_Code::Player_Code()
	{
	}

	Player_Code::Player_Code(string file_name, int id)
		: id(id), file_name(file_name)
	{
		name = file_name;
		if (name.rfind('/') != string::npos) {          //rfind是返回待查字符的最后一次位置索引，即ai代码路径的最底层
			name = name.substr(name.rfind('/') + 1);
		}
		if (name.rfind('\\') != string::npos) {
			name = name.substr(name.rfind('\\') + 1);
		}

		player_ai = NULL;
		hDLL = NULL;
		load();
	}


	Player_Code::~Player_Code()
	{
	}

	bool Player_Code::load()
	{
		if (NULL != hDLL)                                          //重新加载，删除原来已经加载的动态链接库
		{
			_CLOSEDLL(hDLL);
			player_ai = NULL;
		}
		hDLL = _LOADDLL(file_name.c_str());                        //加载AI代码动态链接库（DLL）
		if (NULL != hDLL)
		{
			player_ai = (TPlayerAi)_GETFUNC(hDLL, "player_ai");    //加载DLL成功，更新玩家ai代码的函数指针
		}

		Valid = true;
		Valid = Valid && (NULL != player_ai);                      //检查玩家ai代码的函数指针是否可用

		if (NULL == hDLL)
			cout << "[ERROR] failed to load \"" << file_name << "\"" << endl;
		else
			cout << "Succeed to load \"" << file_name << "\". ";
		if (isValid())
			cout << "Player{" << (int)id << "} has been loaded. " << endl;
		else
			cout << "[ERROR] failed to load Player{" << (int)id << "} !" << endl;

		return Valid;
	}

	bool Player_Code::run(Info &info)
	{
		int time_a = GetTickCount();              //运行某位玩家ai代码开始、结束时有时间戳
		int time_b;
		//运行某位玩家ai代码，如果接到异常，则gameover，并报出异常代码玩家的序号
#if (!defined _MSC_VER) //|| (defined _DEBUG)
		try {
			player_ai(info);
		}
		catch (exception e) {
			cout << "[ERROR] Player " << (int)id << " raised an exception in run()." << endl;
			cout << e.what() << endl;
			kill(); return false;
		}
#else 
		__try {
			player_ai(info);//通过函数指针直接运行玩家ai代码
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			cout << "[ERROR] Player " << (int)id << " raised an exception in run()." << endl;
			kill(); return false;
		}
#endif

		time_b = GetTickCount();
		//if (time_b - time_a > 2000) kill();

		return true;

	}

}
