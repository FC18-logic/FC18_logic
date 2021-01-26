#include "Controller.h"
#include <ctime>
#include <math.h>
#ifdef FC15_DEBUG
#define _COMMAND_OUTPUT_ENABLED_
#endif // FC15_DEBUG
namespace DAGAN
{
	using namespace std;

	TPoint moveDir[4] =    //�ƶ�������enum corpsMoveDirö�����Ͷ�Ӧ
	{// dx, dy
		{0,-1},      //�ϣ�-y
		{0, 1},      //�£�+y
		{-1,0},      //��-x
		{1, 0}       //�ң�+x
	};

	void Controller::run(char* json_filename)
	{
	
		//#json getInitdata 
		data->currentRoundJson.clear();
		data->cutTentacleInfoJson.clear();
		data->cutTentacleBornJson.clear();
		data->cutTentacleJson.clear();
		data->cutTentacleBornJson.assign(data->CellNum, vector<bool>(data->CellNum, false));
		data->cutTentacleJson.assign(data->CellNum, vector<bool>(data->CellNum, false));
		data->cutTentacleInfoJson.resize(data->CellNum, vector<CutTentacleInfoJson>(data->CellNum));

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
	/***********************************************************************************************
	*������ :��FC18��run����һغ����к���
	*������������ : ���ɵ�ǰ��Ϸ��Ϣ�������ai���������������֮������ִ�У�ִ��ÿ����Ч������޸�
					Data�޸�DATA::Data�����б仯�����ݣ�Ȼ����ú���jsonChange�޸�����Json�����ݣ�
					ִ�����Ҫ�ж�����Ƿ�����Լ���Ϸ�Ƿ�������С���λ��ҵ�����ȫ��ִ�������
					�޸ĳ�����Ϣ��Json���ݡ�
	*�������� : id<TPlayerID>--��ǰ���id��json_filename<char*>--Json�ļ���ǰ׺
	*��������ֵ : ��
	*���� : ������
	***********************************************************************************************/
	void Controller::run(TPlayerID id, char* json_filename) {  //@@@��FC18��ÿ���������ִ��
		//�����һ�غϼ�¼��Json����
		data->currentRoundCommandJson.clear();
		data->currentRoundPlayerJson.clear();
		data->currentRoundTowerJson.clear();
		data->currentRoundCorpsJson.clear();
		data->currentRoundMapJson.clear();

		//data�ڵĻغ����ݸ���+1
		volatile TRound dataRound = data->addRound();
		//Json����data�еĻغ����ݣ���ʱgame�лغ����ݻ���������һ�غ�
		data->currentRoundCommandJson["round"] = dataRound;
		data->currentRoundPlayerJson["round"] = dataRound;
		data->currentRoundTowerJson["round"] = dataRound;
		data->currentRoundCorpsJson["round"] = dataRound;
		data->currentRoundMapJson["round"] = dataRound;
		int playerSize = game_.getTotalPlayerNum();
		volatile TRound round = game_.getCurrentRound() + 1;

		if (!silent_mode_) cout << "-=-=-=-=-=-=-=-=-=-=-= Controller: Round[" << round << "] =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
		if (debug_mode) {
			game_.DebugPhase();      //���Խ׶Σ����������Ϣ
		}
		game_.beginPhase();          //�����׶Σ����/��/����/��ͼÿ�غϿ�ʼǰ��׼��������������
		game_.regeneratePhase();     //�ָ��׶Σ����/��/����/��ͼ����Ҫ���еĻָ���������

		//Ϊ��ҵ�AI������������
		Info info2Player = game_.generatePlayerInfo(id);
		vector<CommandList> commands; //ѡ������
		Player_Code& player = players_[id - 1];  //ȡ����Ҷ�Ӧ��ai������
		if (player.isValid() && game_.isAlive(id))  //�������г����ai���룬�������г�����ҵ�ai����
		{
			// �������ִ�У��������ai��ȡָ��
			if (!silent_mode_) cout << "Calling Player " << (int)id << "'s Run() method" << endl;
			//run����dll��Ȼ��Ѷ�Ӧ��myCommandList(��dll�޸�)�ش�������
			players_[id].run(info2Player);
			commands.push_back(info2Player.myCommandList);
		}
		else
		{
			players_[id].kill();
			commands.push_back(CommandList());
		}


		// ֱ���������ҵĲ���
#ifdef _COMMAND_OUTPUT_ENABLED_
		if (file_output_enabled_ && game_.isAlive(id))
		{
			if (game_.isAlive(id)) {
				cout << "Player " << id << "'s commands:" << endl;
				for (Command& c : commands[id])
				{
					switch (c.type)
					{
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
			game_.roundTime.push_back(clock());
			data->currentRoundCommandJson["runDuration"] =
				int(game_.roundTime[game_.roundTime.size() - 1] - game_.roundTime[game_.roundTime.size() - 2]);
			game_.saveJson();

			//������ļ� #json 
			Json::FastWriter fw;
			ofstream json_os;
			json_os.open(json_filename);
			json_os << fw.write(data->root);
			json_os.close();
		}
	}

	/***********************************************************************************************
	*������ :��FC18��jsonChange����Json�޸ĺ���
	*������������ : ��IDΪid������´������c¼�뵱ǰ�غ�����Json��
	                ע�⣬����ҵ������ǰ����������˳������������
					�ġ����ҵ��ñ�����ǰ��ȷ������ִ�й���Ҳ����Ҫȷ
					��ÿ���������Ϸ��������볡�������Ӧ��
	*�������� : id<TPlayerID>--��ǰ��ҵ�ID��c<Command&>--��ǰָ��
	*��������ֵ : ��
	*���� : ������
	***********************************************************************************************/
	
	
	void Controller::jsonChange(TPlayerID id, Command& c) {
		if (c.cmdType == corpsCommand) {
			Json::Value newCmd;
			newCmd["cT"] = Json::Value(int(corpsCommand));
			newCmd["tp"] = Json::Value(int(c.parameters[0]));
			newCmd["id"] = Json::Value(int(c.parameters[1]));
			switch (c.parameters[0])  //��0������
			{
			case(CMove):
				newCmd["mv"] = Json::Value(int(c.parameters[2]));
				newCmd["dir"] = Json::Value(std::atan2(DAGAN::moveDir[c.parameters[2]].m_y, DAGAN::moveDir[c.parameters[2]].m_x));
				break;
			case(CStation):
				break;
			case(CStationTower):
				newCmd["dFT"] = Json::Value(int(c.parameters[2]));
				break;
			case(CAttackCorps):
				newCmd["dEC"] = Json::Value(int(c.parameters[2]));
				TPoint dirTPoint = data->myCorps[c.parameters[2]].getPos() - data->myCorps[c.parameters[1]].getPos();
				newCmd["dir"] = Json::Value(std::atan2(dirTPoint.m_y, dirTPoint.m_x));
				break;
			case(CAttackTower):
				newCmd["dET"] = Json::Value(int(c.parameters[2]));
				TPoint dirTPoint = data->myTowers[c.parameters[2]].getPosition() - data->myCorps[c.parameters[1]].getPos();
				newCmd["dir"] = Json::Value(std::atan2(dirTPoint.m_y, dirTPoint.m_x));
				break;
			case(CRegroup):
				newCmd["dFC"] = Json::Value(int(c.parameters[2]));
				break;
			case(CBuild):
				break;
			case(CRepair):
				newCmd["dFT"] = Json::Value(int(c.parameters[2]));
				break;
			case(CChangeTerrain):
				newCmd["dT"] = Json::Value(int(c.parameters[2]));
				break;
			default:;
			}
			data->currentRoundCommandJson["command"].append(newCmd);
		}
		else if (c.type == towerCommand) {
			Json::Value newCmd;
			newCmd["cT"] = Json::Value(int(towerCommand));
			newCmd["tp"] = Json::Value(int(c.parameters[0]));
			newCmd["id"] = Json::Value(int(c.parameters[1]));
			switch (c.parameters[0]) 
			{
			case(TProduct):
				newCmd["pT"] = Json::Value(int(c.parameters[2]));
				break;
			case(TAttackCorps):
				newCmd["dEC"] = Json::Value(int(c.parameters[2]));
				TPoint dirTPoint = data->myCorps[c.parameters[2]].getPos() - data->myTowers[c.parameters[1]].getPosition();
				newCmd["dir"] = Json::Value(std::atan2(dirTPoint.m_y, dirTPoint.m_x));
				break;
			case(TAttackTower):
				newCmd["dET"] = Json::Value(int(c.parameters[2]));
				TPoint dirTPoint = data->myTowers[c.parameters[2]].getPosition() - data->myTowers[c.parameters[1]].getPosition();
				newCmd["dir"] = Json::Value(std::atan2(dirTPoint.m_y, dirTPoint.m_x));
				break;
			default:;
			}
			data->currentRoundCommandJson["command"].append(newCmd);
		}
	}
}

