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
			, players_(p){ }                   //【FC18】游戏主控的构造函数，传入参数为当前游戏进程对象，以及玩家ai的vector（代码及相关信息）
											   //【FC18】默认输出文件，游戏下一回合可以运行
		                                       //【FC18】NO_SILENT_MODE和FC15_DEBUG两个宏定义决定是否输出debug信息
		void run(char* json_filename);         //@@@【FC18】游戏单回合运行的主要部分，同时向json_filename进行文件写入

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
	};
}
