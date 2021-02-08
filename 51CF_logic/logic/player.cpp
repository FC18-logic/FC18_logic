#include "player.h"
using std::vector;


Player::Player()
{
	alive = true;
	deadRound = -1;
	m_id = -1;
	data = nullptr;
	conqueTowerNum = 0;
	eliminateCorpsNum = 0;
	captureCorpsNum = 0;
	rank = -1;
	score = -1;
}

//#json
Player::Player(Player& _player)// copy构造player
{
	m_crops = _player.getCrops();
	m_tower = _player.getTower();
	alive = _player.isAlive();
	m_id = _player.getId();
	conqueTowerNum = _player.getCqTowerNum();
	eliminateCorpsNum = _player.getElCorpsNum();
	captureCorpsNum = _player.getCqCorpsNum();
	name = _player.getName();
	rank = _player.getRank();
	score = _player.getScore();
	deadRound = _player.getdeadRound();

	data = _player.data;   //避免浅复制?

}


Player::~Player()
{
}


//杀死玩家
void Player::Kill()
{
	alive = false;
	deadRound = std::ceil(data->getRound() / 4.0); //返回死亡回合数是大回合数
}

/***********************************************************************************************
*函数名 :【FC18】getPlayScore获取玩家得分
*函数功能描述 : 根据玩家的防御塔和兵团信息，计算玩家当前得分，用于排名
*函数参数 : 无，直接由Player类调用
*函数返回值 : <int>分值
*作者 : 姜永鹏
***********************************************************************************************/
int Player::getPlayerScore() {

	if (isAlive() == false)   //玩家已经出局，根据出局的回合数记录进行分值
		return deadRound - MAX_ROUND;
	TScore corpsScore, towerScore, killScore;
	corpsScore = towerScore = killScore = 0;
	/*for (TCorpsID i : m_crops)
	{
		if (data->myCorps[i].getType() == Battle)  //兵团星级从0开始[!!!反复确认]
			corpsScore += BATTLE_CORP_SCORE * (data->myCorps[i].getLevel() + 1);
		else if (data->myCorps[i].getType() == Construct)
			corpsScore += CONSTRUCT_CORP_SCORE * 1;
	}*/
	corpsScore += CORP_SCORE * m_crops.size();
	for (TTowerID i : m_tower)
	{
		towerScore += TOWER_SCORE * (data->myTowers[i].getLevel());             //防御塔等级从1开始[!!!反复确认]
	}
	killScore += (conqueTowerNum + eliminateCorpsNum + captureCorpsNum) * KILL_SCORE;
	TScore totalScore = corpsScore + towerScore + killScore;
	//setScore(totalScore);
	return totalScore;
}

/***********************************************************************************************
*函数名 :【FC18】addCrops
*函数功能描述 : 在m_crops中添加ID，在兵团构造时调用
*函数参数 : 添加的兵团ID
*函数返回值 : 无
*作者 : 吕梦欣
***********************************************************************************************/
void Player::addCrops(TCorpsID ID)
{
	m_crops.insert(ID);
}

/***********************************************************************************************
*函数名 :【FC18】deleteCrops
*函数功能描述 : 在m_crops中删除ID，在兵团内部检测到HP为0时调用
*函数参数 : 删除的兵团ID
*函数返回值 : 无
*作者 : 吕梦欣
***********************************************************************************************/
void Player::deleteCrops(TCorpsID ID)
{
	m_crops.erase(ID);
}
/***********************************************************************************************
*函数名 :【FC18】towerNumControl判断玩家防御塔数是否超限
*函数功能描述 : 判断玩家防御塔数目是否超过最大防御塔数限制
*函数参数 : 空
*函数返回值 : <bool>---是否超过限制（true---是，false---否）
*作者 : 姜永鹏
***********************************************************************************************/
bool Player::towerNumControl() {
	if (getTower().size() >= MAX_TOWER_NUM) return true;
	else return false;
}
/***********************************************************************************************
*函数名 :【FC18】corpsNumControl判断玩家兵团数是否超限
*函数功能描述 : 判断玩家兵团数目是否超过最大兵团数限制
*函数参数 : 空
*函数返回值 : <bool>---是否超过限制（true---是，false---否）
*作者 : 姜永鹏
***********************************************************************************************/
bool Player::battleNumControl() {
	int battleNum = 0;
	for (TCorpsID c : getCrops())
	{
		if (data->myCorps[c].getType() == Battle) battleNum++;
		if (battleNum >= MAX_BATTLE_NUM)
		{
			return true;
		}
	}
	return false;
}
/***********************************************************************************************
*函数名 :【FC18】corpsNumControl判断玩家兵团数是否超限
*函数功能描述 : 判断玩家兵团数目是否超过最大兵团数限制
*函数参数 : 空
*函数返回值 : <bool>---是否超过限制（true---是，false---否）
*作者 : 姜永鹏
***********************************************************************************************/
bool Player::constructNumControl() {
	int constructNum = 0;
	for (TCorpsID c : getCrops())
	{
		if (data->myCorps[c].getType() == Construct) constructNum++;
		if (constructNum >= MAX_CONSTRUCT_NUM)
		{
			return true;
		}
	}
	return false;
}