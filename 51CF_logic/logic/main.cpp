#include"game.h"
#include "player_code.h"
#include "../controller/Controller.h"
#include <time.h>
void outputResult(Game& game, vector<string> players_filename) {
	ofstream ofs("../log/result.txt");

	vector<TPlayerID> rank = game.getRank();
	for (size_t i = 0; i < rank.size(); ++i) {
		ofs << players_filename[rank[i]] << endl;
	}
}
using namespace DAGAN;
	int main(int argc, char** argv)
	{
		/*打开输出文件*/
		char buffer[1024];
		time_t t = time(0);
		strftime(buffer, sizeof(buffer), "../log_txt/log_%Y%m%d_%H%M%S.txt", localtime(&t));
		freopen(buffer, "w", stdout);
		char json_filename[1024];
		strftime(json_filename, sizeof(buffer), "../log_json/log_%Y%m%d_%H%M%S.json", localtime(&t));

		string  config_filename =
#ifdef _MSC_VER
			"../config_msvc.ini";
#endif
#ifdef __GNUC__
		"../config_gnu.ini";
#endif

		if (argc == 2) {
			config_filename = argv[1];
		}
		else if (argc >= 2) {
			cout << "usage:												" << endl
				<< "DAGAN						Load config file	" << endl;
		}


		vector<Player_Code>  players;
		string          map_filename;
		vector<string>  players_filename;
		int player_size;

		// load config file
		ifstream ifs(config_filename.c_str());
		if (!ifs.is_open()) {
			cout << "Failed to load \"" << config_filename << "\". Aborted. " << endl;
			return -1;
		}

		// read config file
		ifs >> map_filename >> player_size;
		int cnt = 0;
		while (!ifs.eof() && cnt < player_size) {
			string player_filename;
			ifs >> player_filename;
			if (!player_filename.empty()) players_filename.push_back(player_filename);
			cnt++;
		}
		if (players_filename.size() == 0) {
			cout << "[Error] player_ai file names expected." << endl;
			return -1;
		}
		else if (players_filename.size() != player_size)
		{
			cout << "[Error]" <<player_size<< " player_ai file required." << endl;
			return -1;
		}
		// load map
		Game G;
		if (!G.init(map_filename, json_filename)) {
			cout << "[Error] failed to load " << map_filename << endl;
			return -1;
		}

		// load players
		for (size_t i = 0; i < players_filename.size(); ++i) {
			Player_Code player(players_filename[i], i);
			if (player.isValid())
				players.push_back(player);
			else {
				cout << "[Warning] failed to load player_ai " << players_filename[i] << endl;
#if (defined _MSC_VER && defined _DEBUG)
				system("pause");
#endif
			}
		}
		if (players.size() <= 1) {
			cout << "[Error] Not enough player_ais to start the game." << endl;
			return -1;
		}
		cout << "[Info] " << players.size() << " players loaded." << endl;

		// game and controller

		Controller controller(G, players);

		// main
		while (controller.isValid())
		{
			controller.run(json_filename);
		}

		// output the result
		outputResult(G, players_filename);

		return 0;

	}