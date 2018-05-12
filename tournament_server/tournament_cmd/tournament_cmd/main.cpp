/*tournament_cmd
**服务器端总瑞士轮程序
**by Chiba
**2018-4-10
*/

#include<ctime>

#include<iostream>  
#include<fstream>
#include<string>
#include<algorithm>
#include<vector>
#define CIRCLE 6
int circle[CIRCLE][4] = { {0,1,2,3},{0,3,2,1},{0,2,1,3},{0,3,1,2},{1,0,2,3},{2,0,1,3} };


using namespace std;

const int PLAYERS_PER_GAME = 4;
const int score_by_rank[PLAYERS_PER_GAME] = {5,3,2,1};

struct ScoredString {
	string filename;
	string name;
	float score;
	int bug_time;
	int id;
	ScoredString(string n,float s,int i)
		:filename(n),score(s),bug_time(0),id(i){
		name = filename;
		if (name.rfind('/') != string::npos) {
			name = name.substr(name.rfind('/') + 1);
		}
		if (name.rfind('\\') != string::npos) {
			name = name.substr(name.rfind('\\') + 1);
		}
	}

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
	try {
		string  config_filename =
#ifdef _MSC_VER
			"../config_ranking_mscv.ini";
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

		vector<ScoredString>  players;
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
		) < 0) {
			return -1;
		}

		// load player and dummy player
		for (size_t i = 0; i < players_filename.size(); ++i) {
			players.push_back(ScoredString(players_filename[i], rand_f(),i));
		}
		if (players.size() == 0) { cout << "[Error] No valid player ai Loaded. Aborted." << endl; return -1; }
		cout << "[Info] " << players.size() << " players loaded." << endl;
		cout << "[Info] loading dummy players" << endl;
		int cnt_dummy = PLAYERS_PER_GAME - players.size() % PLAYERS_PER_GAME;
		if (cnt_dummy == PLAYERS_PER_GAME) cnt_dummy = 0;
		int cnt_player = players.size();
		for (size_t i = 0; i < cnt_dummy; ++i) {
			players.push_back(ScoredString(players_filename[i], rand_f(),cnt_player+i));
		}


		// init score ------------------------------------------------------------------------------------------------------------------
		srand(time(NULL));
		for (int i = 0; i < players.size(); ++i)
			if (i < init_score.size())
			{
				players[i].score = init_score[i];
			}
			else players[i].score = rand_f();
			//sort the scores
			std::sort(players.begin(), players.end(),
				[](const ScoredString &a, const ScoredString&b) -> bool { return a.score > b.score; }
			);
			for (ScoredString& player : players) player.score = 0;

			// ------------------------------------------------------------------------------------------------------------------

			for (int curr_round = 0; curr_round < total_round; curr_round++)
			{
				// 将当前结果输出到文件
				
				/*ofs << curr_round << endl;
				for (scoredAI& player : players) {
					player.score = (int)player.score;
					ofs << player.p.getName() << "\t" << (int)player.score << endl;
				}
				ofs.close();*/

				cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
				cout << "-=-=-=-=-=-=-=-=-= SWISS-SYSTEM TOURNAMENT ROUND " << curr_round << endl;
				cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;

				vector<string> game_players;
				game_players.clear();
				int c = 0;
				for (int i = 0; i < players.size(); i++) {
					game_players.push_back(players[i].filename);
					if (game_players.size() == PLAYERS_PER_GAME) {
						for (int k = 0; k != CIRCLE; ++k) {
							cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-  PART" << c << "-" << k << "  -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
							int score[4]{ 0,0,0,0 };//得分
							bool bugger[4]{ false ,false,false,false };//是否出bug
							int no_bug = 4;//未出bug
							while (1)//直到打完
							{
								string round_config_filename =
#ifdef _MSC_VER
									"../config_tournament_round_mscv.ini";
#endif // _MSC_VER
#ifdef __GNUC__
								"../config_tournament_round_gnuc.ini";
#endif

								ofstream ofs_round(round_config_filename);
								ofs_round << map_filename << "\n" << 4 << "\n";
								for (int i = 0; i != 4; ++i)
									if (!bugger[i])
									{
										ofs_round << game_players[circle[k][i]] << "\n";
										cout << "Calling Player {" << players[4 * c + circle[k][i]].id << "} " << game_players[circle[k][i]] << "\n";
									}
									else
									{
										ofs_round << weak_ai_filename << "\n";//已出bug 用凑数替代
										cout << "Calling " << weak_ai_filename << "\n";
									}
								cout << "... Running...\n";
								ofs_round.close();
								//调用回合程序
								system("tournament_round");
								cout << "Finished\n";
								ifstream ifs_round;
								ifs_round.open("../log_txt/round_result.txt");
								string line, last_line;
								getline(ifs_round, line);
								if (!line.empty())
								{
									for (char ch : line)
									{
										if (ch >= '0'&&ch < '4')
										{
											bugger[circle[k][ch - '0']] = true;
											no_bug--;
											players[4 * c + circle[k][ch - '0']].bug_time++;
										}
									}
								}
								while (!ifs_round.eof())
								{
									last_line = line;
									getline(ifs_round, line);
								}
								if (line.empty())
									line = last_line;
								//line为最后一行
								if (line.empty())
								{
									throw(runtime_error("回合进行失败！"));
								}
								else if (line.find("rank") == string::npos)//未正常结束
								{
									int cnt_bugger[4]{ 0,0,0,0 };
									for (char c : line)
									{
										if (c >= '0'&& c < '4')
											cnt_bugger[c - '0']++;
									}
									for (int i = 0; i != 4; ++i)
										if (cnt_bugger[i] % 2)
										{
											bugger[circle[k][i]] = true;
											no_bug--;
											players[4 * c + circle[k][i]].bug_time++;
										}
									if (no_bug == 0)//直接结束
										break;
									else continue;
								}
								else//正常进行！
								{
									int rank = 0;//最高
									for (char c : line)
										if (c >= '0'&& c < '4')
											if (!bugger[circle[k][c - '0']])
												score[circle[k][c - '0']] = score_by_rank[rank++];
									break;
								}
							}
							// player score
							for (size_t j = 0; j < 4; ++j) {
								players[4 * c + j].score += score[j];
							}
						}
						c++;
						game_players.clear();
					}
				}
				cout << "RESULT:" << endl;
				for (ScoredString& player : players)
					player.score += rand_f();
				//sort the scores
				std::sort(players.begin(), players.end(),
					[](const ScoredString&a, const ScoredString &b) -> bool { return a.score > b.score; }
				);
				for (ScoredString& player : players) {
					player.score = (int)player.score;
					cout << player.name << "\t" << (int)player.score << endl;
				}

			}

			// ------------------------------------------------------------------------------------------------------------------



			// 将当前结果输出到文件
			ofstream ofs_result("../log_txt/result_ranking.txt");
			for (ScoredString& player : players) {
				player.score = (int)player.score;
				ofs_result << player.name << "\n" << (int)player.score << endl;
			}
			ofs_result.close();
			//记录bug次数
			ofstream ofs_bug("../log_txt/tournament_bugger.txt");
			for (ScoredString& player : players) {
				ofs_bug << player.name << "\n" << (int)player.bug_time << endl;
			}
	}
	catch (exception e)
	{
		cerr << e.what() << endl;
	}
	return 0;
}
