#include "Controller.h"
#include <ctime>

#ifdef FC15_DEBUG
#define _COMMAND_OUTPUT_ENABLED_
#endif // FC15_DEBUG
namespace DAGAN
{
	using namespace std;

	void Controller::run(char* json_filename)
	{
#ifdef GENERATE_JSON
		//#json getInitdata 
		data->currentRoundJson.clear();
		data->cutTentacleInfoJson.clear();
		data->cutTentacleBornJson.clear();
		data->cutTentacleJson.clear();
		data->cutTentacleBornJson.assign(data->CellNum,vector<bool>(data->CellNum,false));
		data->cutTentacleJson.assign(data->CellNum, vector<bool>(data->CellNum, false));
		data->cutTentacleInfoJson.resize(data->CellNum,vector<CutTentacleInfoJson>(data->CellNum));

		DATA::Data dataCopyLastRound = *data;
		DataSupplement dataSuppleMent;
		{
			dataCopyLastRound.PlayerNum = data->PlayerNum;
			dataCopyLastRound.TentacleNum = data->TentacleNum;
			dataCopyLastRound.CellNum = data->CellNum;

			dataCopyLastRound.players = new Player[data->PlayerNum];
			for (int i = 0; i != data->PlayerNum; i++)
			{
				dataCopyLastRound.players[i] = Player(data->players[i]);
			}

			dataCopyLastRound.cells = new Cell[data->CellNum];
			dataSuppleMent.cellTechPoint.resize(data->CellNum);
			for (int i = 0; i != data->CellNum; i++)
			{
				dataCopyLastRound.cells[i] = Cell(data->cells[i]);
				dataSuppleMent.cellTechPoint[i] = data->cells[i].techRegenerateSpeed(); //科技点数
			}

			dataCopyLastRound.tentacles = new Tentacle**[data->CellNum];
			for (int i = 0; i != data->CellNum; i++)
			{
				dataCopyLastRound.tentacles[i] = new Tentacle*[data->CellNum];
				for (int j = 0; j != data->CellNum; j++)
				{
					if (data->tentacles[i][j])
					{
						Tentacle* tem = new Tentacle(*data->tentacles[i][j]);
						dataCopyLastRound.tentacles[i][j] = tem;
					}
					else
						dataCopyLastRound.tentacles[i][j] = nullptr;
				}
			}
		}
		data->currentRoundJson["currentRound"] = Json::Value(game_.getRound() + 1 );
#endif

		int playerSize = game_.getPlayerSize();
		volatile TRound round = game_.getRound();
//将结果输出到文件
		ofstream ofs;
		ofs.open("../log_txt/round_result.txt",ios::app);

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
//此处不同，输出到文件
				ofs << id << " "<<flush;
				if(players_[id].run(info_list[id]))
					ofs << id << " "<<flush;
				commands.push_back(info_list[id].myCommandList);
			}
			else
			{
				players_[id].kill();
				commands.push_back(CommandList());
			}
		}
		ofs << "\n";
		ofs.close();

		isValid_ = game_.isValid();
		if (!isValid())
		{

		}
		else //isValid
		{
			game_.commandPhase(commands);
			game_.movePhase();
			game_.transPhase();
			game_.endPhase();
		}
		// check if killed
		for (TCellID i = 0; i < playerSize; ++i)
			if (!game_.isAlive(i))
				players_[i].kill();
		game_.addRound();

		//#json save
#ifdef GENERATE_JSON
		{
			game_.saveJson(dataCopyLastRound, dataSuppleMent);
			game_.roundTime.push_back(clock()); 
			data->currentRoundJson["runDuration"] =
				int(game_.roundTime[game_.roundTime.size() - 1] - game_.roundTime[game_.roundTime.size() - 2]);

			data->root["head"]["totalRounds"] = round + 1; 
			data->root["body"].append(data->currentRoundJson);

			//输出到文件 #json 
			Json::FastWriter fw;
			ofstream json_os;
			json_os.open(json_filename);
			json_os << fw.write(data->root);
			json_os.close();
		}
#endif
	}
}
