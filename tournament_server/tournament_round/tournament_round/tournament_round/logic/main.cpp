/*tournament_round
**瑞士轮的比赛，输出文件至round_result.txt
**by Chiba9
** 2018-4-10
*/

#include "Controller.h"
#include <time.h>
#include<fstream>
using namespace DAGAN;
//正常结束的输出
void outputResult(Game& game, vector<Player_Code>& players) {
	ofstream ofs("../log_txt/round_result.txt",ios::app);

	vector<TPlayerID> rank = game.getRank();
	ofs << "rank ";
	for (size_t i = 0; i < rank.size(); ++i) {
		ofs << rank[i] << " ";
	}
	ofs << endl;
}

int main(int argc, char** argv)
{
	time_t t = time(0);
#ifdef FC15_DEBUG
	freopen(buffer, "w", stdout);
#endif // FC15_DEBUG
	char json_filename[1024];
#ifdef GENERATE_JSON
	strftime(json_filename, sizeof(json_filename), "../log_json/log_%Y%m%d_%H%M%S.json", localtime(&t));
#endif
	string  config_filename =
#ifdef _MSC_VER
		"../config_tournament_round_mscv.ini";
#endif
#ifdef __GNUC__
	"../config_tournament_round_gnu.ini";
#endif
	vector<Player_Code>  players;
	string          map_filename;
	vector<string>  players_filename;
	int player_size;
//清空输出文件
	// load config file
	ifstream ifs(config_filename.c_str());
	if (!ifs.is_open()) {
		cout << "Failed to load \"" << config_filename << "\". Aborted. " << endl;
		return 1;
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
		return 2;
	}
	else if (players_filename.size() != player_size)
	{
		cout << "[Error]" << player_size << " player_ai file required." << endl;
		return 3;
	}
	// load map
	Game G;
	if (!G.init(map_filename, json_filename)) {
		cout << "[Error] failed to load " << map_filename << endl;
		return 4;
	}
	ofstream ofs("../log_txt/round_result.txt");
	// load players
	int valid_cnt = 0; //有效ai的数量
	for (size_t i = 0; i < players_filename.size(); ++i) {
		Player_Code player(players_filename[i], i);
		string player_name;
		if (ifs >> player_name && !player_name.empty())
			player.setName(player_name);
		if (player.isValid())
		{
			players.push_back(player);
			valid_cnt++;
		}
		else {
			cout << "[Warning] failed to load player_ai " << players_filename[i] << endl;
			player.setName(player.getName() + "//NOT_VALID");
			players.push_back(player); //压进去充数
			ofs << i<<" "<<flush;
#if (defined _MSC_VER && defined _DEBUG)
			system("pause");
#endif
		}
	}
	ofs << endl;
	ofs.close();
	if (valid_cnt <= 1) {
		cout << "[Error] Not enough player_ais to start the game." << endl;
		return 5;
	}
	cout << "[Info] " << valid_cnt << " players loaded." << endl;

	// game and controller

	Controller controller(G, players);
#ifdef FC15_DEBUG
	controller.setSilentMode(false);
#endif
#ifdef GENERATE_JSON
	for (int i = 0; i < players.size(); i++)
	{
		controller.getData()->root["head"]["playerInfo"][i]["name"] = players[i].getName();
	}
#endif
	// main
	while (controller.isValid())
	{
		controller.run(json_filename);
	}


	// output the result
	outputResult(G, players);
	//system("pause");
	return 0;

}