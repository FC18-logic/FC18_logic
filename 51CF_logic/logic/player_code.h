#pragma once
/* DAGAN 2017, player_code.h
*
*/

#ifndef _DAGAN_PLAYER_CODE_H__
#define _DAGAN_PLAYER_CODE_H__


#include<string>

#include"definition.h"

#include "ai.h"

#ifdef _MSC_VER
// Du 2017.2.6
// EXCLUDE windows sockets
#define WIN32_LEAN_AND_MEAN
#include"windows.h"
#define _LOADDLL(x) LoadLibraryA(x)
#define _CLOSEDLL(x) FreeLibrary(x)
#define _GETFUNC GetProcAddress
typedef HMODULE _DLLHANDLE;
#endif
#ifdef __GNUC__
#include<dlfcn.h>
#define _LOADDLL(x) dlopen(x, RTLD_LAZY)
#define _CLOSEDLL(x) dlclose(x)
#define _GETFUNC dlsym
typedef void* _DLLHANDLE;
#endif

namespace DAGAN {

	using namespace std;

	typedef void(*TPlayerAi)(Info& info);                   //【FC18】指向玩家ai代码的函数指针

	class Player_Code
	{
	public:
		Player_Code();                                      //【FC18】玩家ai代码类的默认构造函数
		Player_Code(string file_name, int id);              //【FC18】玩家ai代码类的构造函数，传入代码文件名与玩家序号
		~Player_Code();                                     //【FC18】玩家ai代码类的默认析构函数

		bool run(Info &info);                               //【FC18】传入场上数据，运行某位玩家ai代码一次

		bool load();                                        //【FC18】加载玩家ai代码动态链接库（DLL）

		inline void kill() { Valid = false; }               //【FC18】遇到异常，强制game_over这位玩家的代码，并且报错
		inline string getName() { return name; }            //【FC18】获取玩家ai代码dll的名称
		inline bool isValid() { return Valid; }             //【FC18】判断这位玩家的dll运行是否正常
		void setName(string _name) { name = _name; }        //【FC18】重置这位玩家dll的文件名
	private:
		TPlayerAi        player_ai;                         //【FC18】玩家ai代码的函数指针
		string           file_name;                         //【FC18】玩家ai代码带路径文件名
		string           name;                              //【FC18】玩家ai代码单独文件名
		TPlayerID        id;                                //【FC18】玩家的序号
		bool             Valid;                             //【FC18】玩家的dll是否运行正常
		_DLLHANDLE       hDLL;                              //【FC18】玩家dll读取时返回的句柄
	};

}
#ifdef __GNUC__
unsigned long GetTickCount();
#endif // !__GNUC__




#endif
