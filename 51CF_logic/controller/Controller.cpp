#include "Controller.h"
#include <time.h>
namespace DAGAN
{
	using namespace std;

	void Controller::run()
	{

		int playerSize = game_.getPlayerSize();
		TRound round = game_.getRound();

		if (file_output_enabled_ && !ofs.is_open()) {
			char buffer[1024];
			time_t t = time(0);
			strftime(buffer, sizeof(buffer), "log_%Y%m%d_%H%M%S.txt", localtime(&t));
			ofs.open(buffer);
		}

		if (!silent_mode_) cout << "-=-=-=-=-=-=-=-=-=-=-= Controller: Round[" << round << "] =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
			// 每个玩家开始运行

		if (debug_mode)
			game_.DebugPhase();
		game_.beginPhase();
		game_.regeneratePhase();
		vector<Info> info_list = game_.generateInfo();
		vector<CommandList> commands; //选手命令
		for (TPlayerID id = 0; id < playerSize; ++id)
		{
			Player_Code& player = players_[id];
			if (player.isValid() && game_.isAlive(id))
			{
				// 单个玩家执行
				if (!silent_mode_) cout << "Calling Player " << (int)id << "'s Run() method" << endl;
				players_[id].run(info_list[id]);
				commands.push_back(info_list[id].myCommandList);
			}
			else
			{
				players_[id].kill();
				commands.push_back(CommandList());
			}
			// 直接输出此玩家的操作
			if (file_output_enabled_) 
			{
				for (TPlayerID id = 0; id != playerSize; ++id)
				{
					ofs << "Player " << id << "'s control:" << endl;
				}
			}
			if (file_output_enabled_) ofs << endl;

			isValid_ = game_.isValid();
			if (!isValid())
				if (!silent_mode_)
				{
					cout << "-=-=-=-=-=-=-=-=-=-=-= GAME OVER ! =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
					cout << "Rank:" << endl;
					for (TPlayerID id : game_.getRank())
					{
						cout << "Player " << id << " : " << players_[id].getName() << endl;
					}
				}
		}
		// check if killed
		for (TCellID i = 0; i < playerSize; ++i)
			if (!game_.isAlive(i))
				players_[i].kill();
	}
}