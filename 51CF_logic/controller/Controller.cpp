#include "Controller.h"
#include <ctime>
#ifdef FC15_DEBUG
#define _COMMAND_OUTPUT_ENABLED_
#endif // FC15_DEBUG
namespace DAGAN
{
	using namespace std;

	void Controller::run(char* json_filename)  //@@@��FC18��
	{
		//#json getFC18InitData
		data->currentRoundCommandJson.clear();
		data->currentRoundPlayerJson.clear();
		data->currentRoundTowerJson.clear();
		data->currentRoundMapJson.clear();

		//#json getInitdata 
		/******************************************FC15�ɴ���**********************************************
		data->currentRoundJson.clear();
		data->cutTentacleInfoJson.clear();
		data->cutTentacleBornJson.clear();
		data->cutTentacleJson.clear();
		data->cutTentacleBornJson.assign(data->CellNum, vector<bool>(data->CellNum, false));
		data->cutTentacleJson.assign(data->CellNum, vector<bool>(data->CellNum, false));
		data->cutTentacleInfoJson.resize(data->CellNum, vector<CutTentacleInfoJson>(data->CellNum));
		***************************************************************************************************/

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
				dataSuppleMent.cellTechPoint[i] = data->cells[i].techRegenerateSpeed(); //�Ƽ�����
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
		data->currentRoundJson["currentRound"] = Json::Value(game_.getRound() + 1);


		int playerSize = game_.getPlayerSize();
		volatile TRound round = game_.getRound();

		if (file_output_enabled_ && !ofs.is_open())
		{
			//char buffer[1024];
			//time_t t = time(0);
			//strftime(buffer, sizeof(buffer), "log_%Y%m%d_%H%M%S.txt", localtime(&t)); // #?json
			//ofs.open(buffer);
		}

		if (!silent_mode_) cout << "-=-=-=-=-=-=-=-=-=-=-= Controller: Round[" << round << "] =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
		// ÿ����ҿ�ʼ����

		if (debug_mode)
			game_.DebugPhase();
		game_.beginPhase();  //�յĺ���
		game_.regeneratePhase();  //ÿ����/���Ű��չ������ָ�����

		//����ҡ����ͱ��ŵ���Ϣ�������ռ�����
		vector<Info> info_list = game_.generateInfo();
		vector<CommandList> commands; //ѡ������
		for (TPlayerID id = 0; id < playerSize; ++id)
		{
			Player_Code& player = players_[id];
			if (player.isValid() && game_.isAlive(id))
			{
				// �������ִ�У��������ai��ȡָ��
				if (!silent_mode_) cout << "Calling Player " << (int)id << "'s Run() method" << endl;
				//run����dll��Ȼ��Ѷ�Ӧ��myCommandList(��dll�޸�)�ش�������
				players_[id].run(info_list[id]);
				commands.push_back(info_list[id].myCommandList);
			}
			else
			{
				players_[id].kill();
				commands.push_back(CommandList());
			}

		}
		// ֱ���������ҵĲ���
#ifdef _COMMAND_OUTPUT_ENABLED_
		if (file_output_enabled_)
		{
			vector<string> stg2str{ "Normal","Attack","Defence","Grow" };
			vector<string> up2str{ "Regeneration","ExtendingSpeed","ExtraControl","Wall" };
			for (TPlayerID id = 0; id != playerSize; ++id)
			{
				if (!game_.isAlive(id))
					continue;
				cout << "Player " << id << "'s commands:" << endl;
				for (Command& c : commands[id])
				{
					switch (c.type)
					{
					case addTentacle:
						cout << "Add a line from " << c.parameters[0] << " to " << c.parameters[1] << endl;
						break;
					case cutTentacle:
						cout << "Cut the line from " << c.parameters[0] << " to " << c.parameters[1] << " at the position of " << c.parameters[2] << endl;
						break;
					case changeStrategy:
						cout << "Change the strategy of tower " << c.parameters[0] << " to " << stg2str[c.parameters[1]] << endl;
						break;
					case upgrade:
						cout << "Upgrade " << up2str[c.parameters[0]] << endl;
						break;
					default:
						break;
					}
				}
			}
		}
		if (file_output_enabled_) cout << endl;
#endif
		isValid_ = game_.isValid();
		if (!isValid())
		{
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
		//ִ���ƶ�������ת���͹�������Ĳ���
		else //isValid
		{
			//�������Ĳ���������
			game_.commandPhase(commands);//��ȡ������Ȼ����Ч����д��JSON����ȥ��Ȼ��ִ��һЩ�ӱ���֮��Ĳ���
			game_.movePhase();//���ߵ����ƺͽ���
			game_.transPhase();//���ߵĴ���ͽ���
			game_.endPhase();//���ߵ��жϺͽ���
		}
		// check if killed
		//��鲢�ж�����Ƿ����
		for (TCellID i = 0; i < playerSize; ++i)
			if (!game_.isAlive(i))
				players_[i].kill();
		//�غ�������1
		game_.addRound();

		//#json save
		{
			game_.saveJson(dataCopyLastRound, dataSuppleMent);
			game_.roundTime.push_back(clock());
			data->currentRoundJson["runDuration"] =
				int(game_.roundTime[game_.roundTime.size() - 1] - game_.roundTime[game_.roundTime.size() - 2]);

			data->root["head"]["totalRounds"] = round + 1;
			data->root["body"].append(data->currentRoundJson);

			//������ļ� #json 
			Json::FastWriter fw;
			ofstream json_os;
			json_os.open(json_filename);
			json_os << fw.write(data->root);
			json_os.close();
		}
	}
}