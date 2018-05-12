/* DAGAN 2017, main.cpp
* Chiba 2018-04-08
* 采用瑞士赛制 https://zh.wikipedia.org/wiki/%E7%91%9E%E5%A3%AB%E5%88%B6
*/
#include "../../51CF_logic/controller/Controller.h"
#include<ctime>
#include<cstdlib>

#include<iostream>  
#include<fstream>
#include<string>
#include<sstream>
#include<algorithm>

using namespace std;
using namespace DAGAN;

const int PLAYERS_PER_GAME = 4;
const int score_by_rank[PLAYERS_PER_GAME] = {5,3,2,1};

struct scoredAI
{
	Player_Code p;
	string name;
	int score;
	scoredAI(Player_Code &_p, const string& _name)
		:score(0), name(_name), p(_p)
	{ }
};

struct ScoredString {
	string name;
	float score;
};

float rand_f() {
	return (float)rand() / RAND_MAX;
}

void usage()
{
	cout << "the tournament neads a specific config path" << endl;
	cout << "../config_tournament_msvc.ini in MSVC" << endl;
	cout << "OR ../config_tournament_gnu.ini in GNU" << endl;
}

// 读取配置文件获得信息
int load_config_file(string config_filename,   // input: 配置文件名称
	string& map_filename,    // output:  地图文件名
	string& weak_ai_filename,         // 替补ai文件名
	vector<string>& players_filename, // 选手ai文件名
	int&  total_round,                 // 从哪个回合开始
	vector<int>& init_score           // 每个人的初始得分
)
{
	// load config file 
	ifstream ifs(config_filename.c_str());
	if (!ifs.is_open()) {
		cout << "Failed to load \"" << config_filename << "\". Aborted. " << endl;
		return -1;
	}
	// read config file
	ifs >> map_filename;
	ifs >> weak_ai_filename;
	ifs >> total_round;
	while (!ifs.eof()) {
		string player_filename;
		int player_init_score = 0;
		ifs >> player_filename;
		player_init_score = rand();
		if (!player_filename.empty()) {
			players_filename.push_back(player_filename);
			init_score.push_back(player_init_score);
		}
	}
	if (players_filename.size() == 0) {
		cout << "[Error] player_ai file names expected." << endl;
		return -1;
	}

	return 0;
}

int main(int argc, char** argv)
{
	string  config_filename =
#ifdef _MSC_VER
		"../config_ranking_msvc.ini";
#endif
#ifdef __GNUC__
	"../config_ranking.ini";
#endif

	// check arguments
	if (argc == 2) {
		config_filename = argv[1];
	}
	else if (argc != 1)
	{
		usage();
		return -1;
	}

	vector<scoredAI>  players;
	string          map_filename;
	string          weak_ai_filename;
	vector<string>  players_filename;
	int             total_round;
	vector<int>     init_score;

	// load ------------------------------------------------------------------------------------------------------------------
	// load config file 
	if (load_config_file(config_filename,
		map_filename,
		weak_ai_filename,
		players_filename,
		total_round,
		init_score
	)<0) {
		return -1;
	}
	
	// load player and dummy player
	for (size_t i = 0; i<players_filename.size(); ++i) {
		Player_Code player(players_filename[i], i);
		if (player.isValid()) players.push_back(scoredAI(player, players_filename[i] + "#" + std::to_string(i)));
	}
	if (players.size() == 0) { cout << "[Error] No valid player ai Loaded. Aborted." << endl; return -1; }
	cout << "[Info] " << players.size() << " players loaded." << endl;
	cout << "[Info] loading dummy players" << endl;
	int cnt_dummy = PLAYERS_PER_GAME - players.size() % PLAYERS_PER_GAME;
	if (cnt_dummy == PLAYERS_PER_GAME) cnt_dummy = 0;
	int cnt_player = players.size();
	for (size_t i = 0; i<cnt_dummy; ++i) {
		Player_Code player(weak_ai_filename, i + cnt_player);
		if (player.isValid()) players.push_back(scoredAI(player, "weak_ai_dummy#" + std::to_string(i)));
	}


	// init score ------------------------------------------------------------------------------------------------------------------
	srand(time(NULL));
	for (int i = 0; i<players.size(); ++i)
		if (i < init_score.size())
		{
			players[i].score = init_score[i];
		}
		else players[i].score = rand_f();
		//sort the scores
		std::sort(players.begin(), players.end(),
			[](const scoredAI &a, const scoredAI &b) -> bool { return a.score > b.score; }
		);
		for (scoredAI& player : players) player.score = 0;

		// ------------------------------------------------------------------------------------------------------------------

		for (int curr_round = 0; curr_round<total_round; curr_round++)
		{
			// 将当前结果输出到文件
			ofstream ofs("tournament.txt");
			/*ofs << curr_round << endl;
			for (scoredAI& player : players) {
				player.score = (int)player.score;
				ofs << player.p.getName() << "\t" << (int)player.score << endl;
			}
			ofs.close();*/

			cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
			cout << "-=-=-=-=-=-=-=-=-= SWISS-SYSTEM TOURNAMENT ROUND " << curr_round << endl;
			cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;

			vector<Player_Code> game_players;
			game_players.clear();
			int c = 0;
			for (int i = 0; i<players.size(); i++) {
				players[i].p.load();
				game_players.push_back(players[i].p);
				if (game_players.size() == PLAYERS_PER_GAME) {
					char buffer[1024];
					time_t t = time(0);
					char json_filename[1024];
					strftime(json_filename, sizeof(json_filename), "playgame/log_ranking/log_%Y%m%d_%H%M%S.json", localtime(&t));
					Game G;
					if (!G.init(map_filename, json_filename)) {
						cout << "[Error] failed to load " << map_filename << endl;
						return 4;
					}
					Controller controller(G, game_players);
					ScoredString scored_logfile;
					controller.setSilentMode(true);
					controller.setFileOutputEnabled(true);

					cout << "... Running... ";
					//run game
					while (controller.isValid()) controller.run(json_filename);
					cout << "Finished" << endl;

					// player score
					vector<int> rank = G.getRank();
					for (size_t j = 0; j<rank.size(); ++j) {
						players[4 * c + rank[j]].score += score_by_rank[j];
					}
					c++;
					game_players.clear();
				}
			}

			game_players.clear();

			cout << "RESULT:" << endl;
			for (scoredAI& player : players)
				player.score += rand_f();
			//sort the scores
			std::sort(players.begin(), players.end(),
				[](const scoredAI &a, const scoredAI &b) -> bool { return a.score > b.score; }
			);
			for (scoredAI& player : players) {
				player.score = (int)player.score;
				cout << player.p.getName() << "\t" << (int)player.score << endl;
			}

		}

		// ------------------------------------------------------------------------------------------------------------------



		// 将当前结果输出到文件
		ofstream ofs("playgame/result_ranking.txt");
		for (scoredAI& player : players) {
			player.score = (int)player.score;
			ofs << player.p.getName() << "\t" << (int)player.score << endl;
		}
		ofs.close();

		return 0;

}
