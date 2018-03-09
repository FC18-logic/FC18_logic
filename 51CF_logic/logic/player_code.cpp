/* DAGAN 2017, player_code.cpp
*
* Game <-- Player_Ai ---> AI
*
*/

#include <ctime>
#include "player_code.h"
#include <iostream>
namespace DAGAN {
	using namespace std;
	Player_Code::Player_Code()
	{
	}

	Player_Code::Player_Code(string file_name, int id)
		: id(id), file_name(file_name)
	{
		name = file_name;
		if (name.rfind('/') != string::npos) {
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
		if (NULL != hDLL)
		{
			_CLOSEDLL(hDLL);
			player_ai = NULL;
		}
		hDLL = _LOADDLL(file_name.c_str());
		if (NULL != hDLL)
		{
			player_ai = (TPlayerAi)_GETFUNC(hDLL, "player_ai");
		}

		Valid = true;
		Valid = Valid && (NULL != player_ai);

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
		int time_a = GetTickCount();
		int time_b;
		
#if (defined _MSC_VER) //|| (defined _DEBUG)
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
			player_ai(info);
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