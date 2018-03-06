#include "../logic/game.h"
#include "../logic/player_code.h"
#include <vector>
#include <string>
#include <fstream>

namespace DAGAN
{
	class Controller
	{
	public:
		Controller(Game& g, std::vector<Player_Code>& p)
			: game_(g)
			, data(&(g.getData()))
			, silent_mode_(false), file_output_enabled_(true)
			, isValid_(true), debug_mode(true)
			, players_(p){ }

		void run();

		inline void setSilentMode(bool flag) { silent_mode_ = flag; }
		inline void setFileOutputEnabled(bool flag) { file_output_enabled_ = flag; }
		inline bool isValid() const { return isValid_; }

	protected:
		bool debug_mode;
		Game& game_;
		DATA::Data *data;
		std::ofstream ofs;
		std::vector<Player_Code>& players_;
		bool file_output_enabled_;
		bool silent_mode_;
		bool isValid_;
	};
}