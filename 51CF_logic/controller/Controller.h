//#2021-1-19 <JYP> 熟悉代码，添加注释
#include "../logic/game.h"
#include "../logic/player_code.h"
#include <vector>
#include <string>
#include <fstream>
#include "../json/json.h"

namespace DAGAN
{
	class Controller
	{
	public:
		Controller(Game& g, std::vector<Player_Code>& p)
			: game_(g)
			, data(&(g.getData()))
			, silent_mode_(
#ifdef NO_SILENT_MODE
				false
#else
				true
#endif
			), file_output_enabled_(true)
			, isValid_(true), debug_mode(
#ifdef FC15_DEBUG
		true
#else
		false
#endif
			)
			, players_(p) {
			Corpslastcmd.resize(5);
		}                              //【FC18】游戏主控的构造函数，传入参数为当前游戏进程对象，以及玩家ai的vector（代码及相关信息）
											              //【FC18】默认输出文件，游戏下一回合可以运行
		                                                  //【FC18】NO_SILENT_MODE和FC15_DEBUG两个宏定义决定是否输出debug信息
		void run(TPlayerID id,ofstream& cmdF, ofstream& infoF);      //@@@【FC18】运行每个玩家当前的游戏回合（小回合），然后记录玩家指令Json和变化后的信息Json
        //#Json有关Json更改函数
		void jsonChange(TPlayerID id,Command& c,ofstream& cmdFile);                      //【FC18】在命令执行完毕（保证有效）后，传入命令进来，会对应修改命令Json的数据
		bool moreCommand(TPlayerID id,set<TTowerID>& tBanned,set<TCorpsID>& cBanned);//【FC18】判断某位玩家是否能继续执行下一句指令
		void killPlayers();                                  //【FC18】判断玩家阵亡情况
		int commandRead = 0;                                      //【FC18】已读取命令数量
		void setValid(bool valid) { isValid_ = valid; }       //【FC18】设置游戏控制器不能继续运行了
		bool handleCorpsCommand(TPlayerID ID, Command& c);                       //【FC18】执行当前兵团指令，并返回是否执行成功
		bool handleTowerCommand(TPlayerID ID, Command& c);                       //【FC18】执行当前防御塔指令，并返回是否执行成功
		void getGameRank();                                                      //【FC18】获取更新游戏排名
		void outPutCommand(TPlayerID id,Command& c);                             //【FC18】输出当前玩家的指令，这个函数只输出被实际执行的指令

		void testPlayerCommand(Info& info);                                      //【FC18】用于调试，模拟AI给出玩家命令，放到游戏中执行


		inline void setSilentMode(bool flag) { silent_mode_ = flag; }                         //【FC18】重设silent_mode
		inline void setFileOutputEnabled(bool flag) { file_output_enabled_ = flag; }          //【FC18】文件输出开关，重设file_output_enabled_
		inline bool isValid() const { return isValid_; }                                      //【FC18】设置是够game_over
		inline DATA::Data* getData() { return data; }                                         //【FC18】通过Controller类访问游戏实时的数据Data

	protected:
		bool debug_mode;                       //【FC18】是否调试模式
		Game& game_;                           //【FC18】当前的游戏进程（结构体）
		DATA::Data *data;                      //【FC18】整个游戏中，数据的存储与共享
		std::ofstream ofs;                     //【FC18】一个输出流对象，用于把游戏进程中的信息输出                 
		std::vector<Player_Code>& players_;    //【FC18】存储玩家的ai（代码和相关信息）
		bool file_output_enabled_;             //【FC18】文件输出使能，是否允许输出相关文件
		bool silent_mode_;                     //【FC18】silent_mode_为false会输出回合数信息和每个玩家的操作信息等，用于debug
		bool isValid_;                         //【FC18】游戏是否还能继续进行，用于判断game_over
		vector<vector<Command>> Corpslastcmd;	//【FC18】兵团上一回合命令，用于需要延迟一回合生效的命令
	};
}
