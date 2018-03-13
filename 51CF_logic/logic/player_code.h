#pragma once
/* XiangGuHuaJi 2016, player.h
*
*/

#ifndef _XIANGGUHUAJI_PLAYER_H__
#define _XIANGGUHUAJI_PLAYER_H__


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

	typedef void(*TPlayerAi)(Info& info);

	class Player_Code
	{
	public:
		Player_Code();
		Player_Code(string file_name, int id);
		~Player_Code();

		bool run(Info &info);

		bool load();

		inline void kill() { Valid = false; }
		inline string getName() { return name; }
		inline bool isValid() { return Valid; }

	private:
		TPlayerAi        player_ai;
		string           file_name;
		string           name;
		TPlayerID        id;
		bool             Valid;
		_DLLHANDLE hDLL;
	};

}
#ifdef __GNUC__
unsigned long GetTickCount();
#endif // !__GNUC__




#endif
