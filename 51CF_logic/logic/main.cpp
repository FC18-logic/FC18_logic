//#2021-1-19 <JYP> 熟悉代码，添加注释


#include "game.h"
#include "player_code.h"
#include "../controller/Controller.h"
#include <time.h>


using namespace DAGAN;

//【FC18】输出结果排名文件
void outputResult(Game& game, vector<Player_Code>& players) {
	ofstream ofs("../log_txt/result.txt");//打开txt文件

	vector<TPlayerID> rank = game.getRank();   //游戏结果排名(vector)
	for (size_t i = 0; i < rank.size(); ++i) {
		if (rank[i] > players.size() || rank[i] <= 0) continue;
		ofs << "rank：" << rank[i] << " --> " << players[rank[i] - 1].getName() << endl;   //按排名输出每个名次的玩家name
	}
}

int main(int argc, char** argv)
{
	char buffer[1024];
	time_t t = time(0);//记录时间
	strftime(buffer, sizeof(buffer), "../log_txt/log_%Y%m%d_%H%M%S.txt", localtime(&t));
#ifdef FC15_DEBUG
	//freopen(buffer, "w", stdout);  //Debug模式，写入新的txt空文件
#endif // FC15_DEBUG
	char json_filename[1024];
	//旧代码//strftime(json_filename, sizeof(json_filename), "../log_json/log_%Y%m%d_%H%M%S.json", localtime(&t));
	strftime(json_filename, sizeof(json_filename), "../log_json/log_%Y%m%d_%H%M%S", localtime(&t));
	string  config_filename =
#ifdef _MSC_VER
		//旧代码//"../config_msvc.ini";
		"../config18_msvc.ini";
#endif
#ifdef __GNUC__
		//旧代码//"../config_gnu.ini";
		"../config18_gnu.ini";
#endif

	if (argc == 2) {
		config_filename = argv[1];
	}
	else if (argc >= 2) {
		cout << "usage:												" << endl
			<< "DAGAN						Load config file	" << endl;
	}


	vector<Player_Code>  players;                       //【FC18】玩家AI代码类
	string          map_filename;                       //【FC18】游戏地图文件名
	vector<string>  players_filename;                   //【FC18】玩家AI文件名
	vector<string>  players_name;                       //【FC18】玩家姓名，用于初始化
	int player_size;                                    //【FC18】Game中的玩家数量

	// load config file
	ifstream ifs(config_filename.c_str());
	if (!ifs.is_open()) {
		cout << "Failed to load \"" << config_filename << "\". Aborted. " << endl;
		return 1;
	}

	// read config file
	//配置文件
	//第一行：地图文件路径
	//第二行：玩家数量
	//第三行：每个玩家的ai路径
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


	//读入玩家AI代码
	int valid_cnt = 0; //有效ai的数量
	for (size_t i = 0; i < players_filename.size(); ++i) {
		//旧代码//Player_Code player(players_filename[i], i);
		Player_Code player(players_filename[i], i + 1);                     //玩家序号从1 开始
		string player_name;
		if (ifs >> player_name && !player_name.empty())
			player.setName(player_name);
		if (player.isValid())                //玩家ai的all加载成功会返回true
		{
			players.push_back(player);
			players_name.push_back(player_name);
			valid_cnt++;
		}
		else {
			players.push_back(player); //压进去充数，此时玩家ai代码类的Valid为false
			cout << "[Warning] failed to load player_ai " << players_filename[i] << endl;
			player.setName(player.getName() + "//NOT_VALID");
			players_name.push_back(player_name+"[N/A]");
#if (defined _MSC_VER && defined _DEBUG)
			system("pause");
#endif
		}
	}
	//旧代码//if (players.size() <= 1) {
		//旧代码//cout << "[Error] Not enough player_ais to start the game." << endl;
		//旧代码//return 5;
	//旧代码//}
	if (players.size() != 4) 
	{
		cout << "[Error] Player num not accord with requirements!\n";
		cout << "Player num should be 4, however, there are " << players.size() << " players." << endl;
		return 5;
	}
	cout << "[Info] " << valid_cnt << " players loaded." << endl;


	// load map
	//初始化地图、玩家的势力区域、防御塔、兵团的作战关系表等等
	Game G;
	if (!G.init(map_filename, json_filename, players_name)) {
		cout << "[Error] failed to load " << map_filename << endl;
		return 4;
	}


	// game and controller

	Controller controller(G, players);

	//旧代码//for (int i = 0; i < players.size(); i++)
	//旧代码//{
		//旧代码//controller.getData()->root["head"]["playerInfo"][i]["name"] = players[i].getName();
		//旧代码//controller.getData()->commandJsonRoot["head"]["playerInfo"][i]["name"] = players[i].getName();
	//旧代码//}
	// main
	//一个回合一个回合的跑
	system("pause");
	while (controller.isValid())
	{
		for (int i = 1; i <= 4; i++)
		{
			controller.run(i);   //每个玩家依次运行，传入玩家ID，运行该玩家指令
			if (G.goNext() == false ) {
				break;
			}
		}
		//旧代码//controller.run(json_filename);
	}


	// output the result
	outputResult(G, players);

	return 0;

}
