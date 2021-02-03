#include "Crops.h"
#include "tower.h"

TCorpsID Crops::m_staticID = 0;

Crops::Crops():m_data(nullptr)
{
	
}


Crops::~Crops(void)
{
}

Crops::Crops(DATA::Data* _data, corpsType type, battleCorpsType battletype, constructCorpsType buildtype, TPlayerID PlayerID, TPoint pos)
	:m_data(_data)
{
	m_bAlive = true;
	m_type = type;
	m_BattleType = battletype;
	m_BuildType = buildtype;
	m_PlayerID = PlayerID;
	m_position = pos;
	m_myID = m_staticID;
	m_staticID++;
	m_PeaceNum = 0;
	m_level = 0;
	m_StationTower = nullptr;//鍏靛洟鐢熶骇鍑烘潵鍚庯紝闇€瑕佹寚浠ゆ墠椹诲畧褰撳湴鐨勫
	m_BuildPoint = 0;
	m_HealthPoint = 0;

	if (type == Battle)
	{
		m_MovePoint = battleMovePoint[battletype][m_level];
		m_HealthPoint = battleHealthPoint[battletype][m_level];
	}
	else if (type == Construct)
	{
		m_MovePoint = constructMovePoint[buildtype];
		m_BuildPoint = 3;
	}
	m_bResting = true;//鍏靛洟鐢熶骇鍑烘潵鍚庨粯璁や紤鏁?
	m_data->corps[m_position.m_y][m_position.m_x].push_back(this);
	m_data->players[m_PlayerID - 1].addCrops(m_myID);
	m_data->totalCorps++;
	m_data->newCorps.insert(m_myID);//璁板綍鏂颁骇鐢熺殑鍏靛洟搴忓彿
}




/*
Move
鍏靛洟鍦ㄥ湴鍥惧唴杩涜绉诲姩锛屾棤娉曞垽鏂洰鏍囧湴鐐规槸鍚﹀瓨鍦ㄥ埆鐨勫叺鍥?
dx锛歺鍧愭爣鍙樺寲閲?dy锛歽鍧愭爣鍙樺寲閲?
杩斿洖鏄惁绉诲姩鎴愬姛锛屾垚鍔熷垯鏇存柊浣嶇疆
*/
bool Crops::Move(int dir)
{
	if(!m_bAlive)
	{
		return false;
	}
	int curpos_x = m_position.m_x;
	int curpos_y = m_position.m_y;
	int next_x = curpos_x;
	int next_y = curpos_y;
	switch(dir)
	{
	case CUp:
		{
			next_y--;
		}
		break;
	case CDown:
		{
			next_y++;
		}
		break;
	case CLeft:
		{
			next_x--;
		}
		break;
	case CRight:
		{
			next_x++;
		}
		break;
	default:
		{
			return false;
		}
	}
	TPoint next_pos;
	next_pos.m_x = next_x;
	next_pos.m_y = next_y;
	if (m_data->gameMap.withinMap(next_pos) == false) return false;  //by jyp瑕佸墠寰€鐨勪綅缃笉鍦ㄥ湴鍥惧唴锛屽垽鏂け璐?

	//鍒ゆ柇鐩爣浣嶇疆鏄惁瀛樺湪宸辨柟濉?
	bool haveTower = false;
	int index = m_data->gameMap.map[next_y][next_x].TowerIndex;
	if(index != NOTOWER)
	{
		class Tower targettower = m_data->myTowers[index];
		if(targettower.getPlayerID() != m_PlayerID)
		{
			return false;
		}
		else
		{
			haveTower = true;
		}
	}
	if(!haveTower)
	{
		//鐩爣浣嶇疆鏄惁瀛樺湪鍒殑鍏靛洟
		CorpsUnit targetPos = m_data->corps[next_y][next_x];
		
		//锟斤拷锟教憋拷锟狡讹拷
		if(m_type == Construct)
		{
			for(int i = 0; i<targetPos.size(); i++)
			{
				if(targetPos[i]->m_type == Battle && targetPos[i]->m_PlayerID == m_PlayerID)
				{
					continue;
				}
				return false;
			}
		}
		//鎴樻枟鍏?
		else
		{
			for(int i = 0; i<targetPos.size(); i++)
			{
				if(targetPos[i]->m_type == Construct && targetPos[i]->m_PlayerID == m_PlayerID)
				{
					continue;
				}
				return false;
			}
		}
	}
	terrainType curtype = m_data->gameMap.map[curpos_y][curpos_x].type;
	terrainType nexttype = m_data->gameMap.map[next_y][next_x].type;
	float dMP = (float(CorpsMoveCost[curtype])+float(CorpsMoveCost[nexttype]))/2.0f;
	int temp = m_MovePoint - ceil(dMP);
	//琛屽姩鍔涗笉澶?
	if(temp<0)
	{
		return false;
	}
	m_MovePoint = temp;

	//淇敼data涓殑浣嶇疆
	UpdatePos(next_pos);
	return true;
}


/*
AttackCrops
璇ュ叺鍥㈠鍙︿竴鍏靛洟鍙戣捣鏀诲嚮锛岃礋璐ｈ绠楁敾鍑诲姏銆佹敾鍑诲叺鍥€佸苟绉诲姩
鍙傛暟 enemy 鍙楁敾鍑诲叺鍥㈡寚閽?
杩斿洖鍊?int 鍑忓皯鐨凥P
lmx
*/
void Crops::AttackCrops(Crops* enemy)
{
	int enemylost = 0;
	int mylost = 0;
	//濡傛灉鏁屾柟鏄伐绋嬪叺
	if(enemy->m_type == Construct)
	{
		Crops* colleage = NULL;
		//鏄惁瀛樺湪鎶ゅ崼
		for(int i = 0; i<m_data->corps[m_position.m_y][m_position.m_x].size(); i++)
		{
			colleage = m_data->corps[m_position.m_y][m_position.m_x][i];
			if(colleage->m_type == Battle&&colleage->m_PlayerID == enemy->m_PlayerID)
				enemy = colleage;
		}
	}

	//濡傛灉鏁屼汉鏄垬鏂楀叺
	if(enemy->m_type == Battle)
	{
		//璁＄畻鎴樻枟鍔?
		TBattlePoint myCE = getCE();
		TBattlePoint enemyCE = enemy->getCE();
		//璁＄畻鎹熷け
		double deta = 0.04*((double)myCE-enemyCE);
		mylost = floor(28*pow(2.71828,-deta));
		enemylost = floor(30*pow(2.71828,deta));
	}
	//璁＄畻鐢熷懡鍔?
	if (m_BattleType != Archer)
	{
		m_HealthPoint -= mylost;
	}
	//濡傛灉宸查樀浜?
	if (m_HealthPoint <= 0)
	{
		KillCorps();
	}
	//濡傛灉瀵规柟姝讳骸 鍒欑Щ鍔ㄤ綅缃?
	if(!enemy->BeAttacked(enemylost,m_PlayerID,m_bAlive))
	{
		if(m_BattleType!= Archer&&m_bAlive)
		{
			//绉诲姩
			UpdatePos(enemy->m_position);
		}
	}
}

/*
BeAttacked
鍏靛洟鍙楀埌鏀诲嚮鏃惰皟鐢紝鍐呴儴璋冪敤
鍙傛暟锛歛ttack 鍙楀埌鐨勪激瀹冲€硷紝enemy 鍙戝姩鏀诲嚮鐨勫叺鍥㈡寚閽? bAlive 鏀诲嚮鏂规槸鍚﹀瓨娲?
杩斿洖鍊硷細鏄惁瀛樻椿 瀛樻椿杩斿洖true
*/
bool Crops::BeAttacked(int attack, TPlayerID ID, bool bAlive)
{
	//淇樿檹宸ョ▼鍏?
	if(m_type == Construct)
	{
		if(bAlive){
			ChangeOwner(ID);
			int num = m_data->players[ID - 1].getCqCorpsNum() + 1;
			m_data->players[ID - 1].setCqCorpsNum(num);
		}
		return true;
	}
	//鎴樻枟鍏?
	m_PeaceNum = 0;
	m_HealthPoint -= attack;
	if(m_HealthPoint<=0)
	{
		KillCorps();
		int num = m_data->players[ID - 1].getElCorpsNum() + 1;
		m_data->players[ID - 1].setElCorpsNum(num);
		//鍚屼竴浣嶇疆鐨勫伐绋嬪叺琚繕铏?
		Crops* colleage = NULL;
		for(int i = 0; i<m_data->corps[m_position.m_y][m_position.m_x].size(); i++)
		{
			colleage = m_data->corps[m_position.m_y][m_position.m_x][i];
			//濡傛灉宸ョ▼鍏靛拰姝ｅ湪琚敾鍑荤殑鎴樻枟鍏垫槸鍚屼竴鐜╁鐨勫叺鍥?
			if(colleage->m_type == Construct&&colleage->m_PlayerID == m_PlayerID)
				colleage->BeAttacked(0,ID,bAlive);
		}
	}
	m_bResting = false;
	m_PeaceNum = 0;
	return m_bAlive;
}

bool Crops::bAlive()
{
	return m_bAlive;
}

/*
getCE
杩斿洖璇ュ叺鍥㈢殑鎴樻枟鍔?
*/
TBattlePoint Crops::getCE()
{
	//鎴樻枟鍔涘鐩?
	terrainType type = m_data->gameMap.map[m_position.m_y][m_position.m_x].type;
	TBattlePoint Attack = CorpsBattleGain[type];
	//鎬绘垬鏂楀姏
	Attack += (TBattlePoint)ceil(
		(float)(corpsBattlePoint[m_BattleType][m_level]*m_HealthPoint)
		/(float)battleHealthPoint[m_BattleType][m_level]);
	return Attack;
}


/*
Recover
鏍规嵁涓婁竴鍥炲悎鐘舵€佸叺鍥㈠洖澶嶇敓鍛藉姏锛屽嚱鏁板唴閮ㄥ垽鏂槸鍚﹁繘琛屾仮澶?
鏃犲弬鏁拌繑鍥炲€?
*/
void Crops::Recover()
{
	if(m_type == Construct)
	{
		return;
	}
	if(!m_bResting)
	{
		return;
	}
	if(m_PeaceNum == 3)
	{
		m_HealthPoint += (int)floor(battleHealthPoint[m_BattleType][m_level]/3);
		if(m_HealthPoint > battleHealthPoint[m_BattleType][m_level])
		{
			m_HealthPoint = battleHealthPoint[m_BattleType][m_level];
		}
		return;
	}
	m_PeaceNum ++;
}

/*
MergeCrops
鍏靛洟鏁寸紪锛屽彂璧锋暣缂栫殑鍏靛洟璋冪敤锛屽嚱鏁板唴閮ㄥ垹闄ゅ鏂瑰叺鍥?
鍙傛暟锛氬鏂瑰叺鍥㈡寚閽堬紝寰呭畾 涔熷彲浠ユ槸浣嶇疆鎴栬€匢D
杩斿洖鏄惁鏁寸紪鎴愬姛
*/
bool Crops::MergeCrops(TCorpsID targetID)
{
	Crops* targetCrops = NULL;
	targetCrops = &(m_data->myCorps[targetID]);
	//濡傛灉鐩爣闃熶紞涓嶅瓨鍦?
	if(targetCrops == NULL)
		return false;

	//濡傛灉鏃犺鍔ㄥ姏
	if(m_MovePoint == 0)
		return false;

	//濡傛灉涓€鏂瑰凡缁忛樀浜?
	if(!m_bAlive||!(targetCrops->m_bAlive))
		return false;

	//濡傛灉涓嶅悓闃佃惀
	if(m_PlayerID!=targetCrops->m_PlayerID)
		return false;

	//濡傛灉鑷冲皯鍏朵腑涓€鏂逛笉鏄垬鏂楀叺鍥紝涓嶅悓绫诲瀷
	if(m_type!=Battle||targetCrops->m_type!=Battle)
		return false;
	if(m_BattleType!=targetCrops->m_BattleType)
		return false;

	//濡傛灉涓嶆槸閭诲眳锛屽彲浠ュ湪褰撳墠鏍兼垨鑰呭懆鍥寸殑8涓牸閲?
	if(!IsNeighbor(targetCrops->m_position))
		return false;

	//濡傛灉鏄熺骇涓嶇鍚堣姹?
	int level1 = m_level+1;
	int level2 = targetCrops->m_level+1;
	if(level2>level1)
		return false;
	if(level1+level2 >3)
		return false;

	//鏁寸紪
	float HPSum = m_HealthPoint+targetCrops->m_HealthPoint;
	float TotalSum = battleHealthPoint[m_BattleType][m_level]
					+battleHealthPoint[m_BattleType][targetCrops->m_level];
	m_level = level1+level2-1;
	m_HealthPoint = battleHealthPoint[m_BattleType][m_level]*HPSum/TotalSum;

	m_MovePoint = 0;
	targetCrops->KillCorps();
	return true;
}

/*
ResetMP
鍥炲悎寮€濮嬫椂閲嶇疆琛屽姩鍔?
*/
void Crops::ResetMP()
{
	if(m_type == Battle)
	{
		m_MovePoint = battleMovePoint[m_BattleType][m_level];
	}
	else
	{
		m_MovePoint = constructMovePoint[m_BattleType];
	}
}

/*
ChangeTerrain
寤虹瓚鍏垫敼閫犳墍鍦ㄥ崟鍏冨湴褰?
鍙傛暟锛歵arget 鐩爣鍦板舰
杩斿洖鏄惁鏀归€犳垚鍔?
*/
bool Crops::JudgeChangeTerrain(Command& c)
{
	terrainType target = (terrainType)c.parameters[2];
	c.parameters.clear();
	c.parameters.push_back(CChangeTerrain);
	c.parameters.push_back(m_myID);
	c.parameters.push_back(target);
	c.parameters.push_back(m_position.m_x);
	c.parameters.push_back(m_position.m_y);
	if(!m_bAlive)
		return false;
	if(m_type != Construct)
		return false;
	if(m_BuildType != Builder)
		return false;
	int curID = m_data->gameMap.map[m_position.m_y][m_position.m_x].owner;
	if(curID != m_PlayerID)
		return false;
	terrainType curType = m_data->gameMap.map[m_position.m_y][m_position.m_x].type;
	if (!((curType == TRPlain && target == TRForest) || (curType == TRForest && target == TRPlain))) return false;    //by jyp 只能平原改成森林，或者森林改成平原
	if(m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex == NOTOWER)
	{
		return true;
	}
	return false;
}

/*
newRound
鏂板洖鍚堝紑濮嬶紝鍥炲HP鍜孧P
*/
void Crops::newRound()
{
	if(m_bAlive)
	{
		ResetMP();
		Recover();
		if(!m_bResting)
		{
			m_PeaceNum = 0;//寮€濮嬭鏁?
			m_bResting = true;
		}
	}
}

/*
GoRest
杩涘叆椹绘墡浼戞暣鐘舵€?
濡傛灉璇ヤ綅缃病鏈夊鍒欒繑鍥瀟rue锛屾湁濉斿垯杩斿洖false
*/
bool Crops::GoRest()
{
	//濡傛灉鏈夊
	if(m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex!=NOTOWER)
	{
		return false;
	}

	//濡傛灉涔嬪墠娌℃湁澶勫湪椹绘墡鐘舵€佷腑锛屽垯鍏堝皢鍙橀噺娓呴浂骞惰繘鍏ラ┗鎵庣姸鎬?
	if(!m_bResting)
	{
		m_PeaceNum = 0;//寮€濮嬭鏁?
		m_bResting = true;
	}
	return true;
}

/*
StationInTower
濡傛灉璇ヤ綅缃瓨鍦ㄥ繁鏂瑰娍鍔涚殑濉旓紝鍒欒繑鍥瀟rue
*/
bool Crops::StationInTower()
{
	bool bStation = false;
	int index = m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex;
	//濡傛灉閫夋嫨椹绘墡鐨勪綅缃湁濉?鍒欓┗鎵庡湪濉斾腑
	if(index != NOTOWER)
	{
		//濡傛灉濉斿睘浜庡繁鏂瑰垯椹绘墡
		if(m_data->myTowers[index].getPlayerID() == m_PlayerID)
		{
			m_StationTower = &(m_data->myTowers[index]);
			m_StationTower->input_staycrops(this);
			bStation = true;
		}
	}
	return bStation;
}


/*
ShowInfo
鎻愪緵鍏靛洟淇℃伅
杩斿洖鍏靛洟淇℃伅缁撴瀯浣?
*/
struct CorpsInfo Crops::ShowInfo()
{
	struct CorpsInfo info;
	info.ID = m_myID;
	info.level = m_level;
	info.owner = m_PlayerID;
	info.pos = m_position;
	info.type = m_type;
	info.movePoint = m_MovePoint;
	info.HealthPoint = info.BuildPoint = 0;
	info.m_BattleType = Warrior;
	info.m_BuildType = Builder;
	if (info.type == Battle) {
		info.m_BattleType = m_BattleType;
		info.HealthPoint = m_HealthPoint;  //鐜╁鐢ㄧ敓鍛藉€艰嚜宸辩畻鎴樻枟鍔涳紝涓嶆彁渚?
	}
	else if (info.type == Construct) {
		info.m_BuildType = m_BuildType;
		info.BuildPoint = m_BuildPoint;
	}
	return info;
}

/*
AttackTower
*/
void Crops::AttackTower(class Tower *enemy)
{
	int TowerCE;//濉旂殑鎴樻枟鍔?
	//鑾峰彇濉旂殑鎴樻枟鍔?
	TowerCE = enemy->get_towerbp();
	int myCE = getCE();//鍏靛洟鎴樻枟鍔?
	
	double deta = 0.04*((double)myCE-TowerCE);
	int mylost = floor(28*pow(2.71828,-deta));
	int enemylost = floor(30*pow(2.71828,deta))*corpsAttackTowerGain[m_BattleType][m_level];

	//璁＄畻鐢熷懡鍔?
	if (m_BattleType != Archer)
	{
		m_HealthPoint -= mylost;
	}
	//濡傛灉宸查樀浜?
	if (m_HealthPoint <= 0)
	{
		KillCorps();
	}

	bool IsTowerDestroy = false;
	//鍒ゆ柇濉旀槸鍚﹁鏀婚櫡(鍗犻銆佹懅姣侀兘绠?
	IsTowerDestroy = enemy->Be_Attacked(m_PlayerID, enemylost);
	if(IsTowerDestroy/*&&!(enemy->getexsit())*/)
	{
		int num = m_data->players[m_PlayerID - 1].getCqTowerNum() + 1;
		m_data->players[m_PlayerID - 1].setCqTowerNum(num);
	}

	//
	if(m_bAlive)
	{
		if(m_BattleType == Archer)
			IsTowerDestroy = false;
		if(IsTowerDestroy)
		{
			UpdatePos(enemy->getPosition());
		}
	}
}

/*
Attack
鍏靛洟鍙戝姩鏀诲嚮 杩斿洖鏄惁鏀诲嚮鎴愬姛
鍙傛暟 type<CorpsCommandEnum> 鏀诲嚮鏁屾柟鍏靛洟鎴栧锛?ID 鏁屾柟ID
*/
bool Crops::Attack(int type, TCorpsID ID)
{
	//濡傛灉涓嶆槸鎴樻枟鍏?
	if(m_type!=Battle)
		return false;

	//琛屽姩鍔涗笉瓒?
	if(m_MovePoint == 0)
		return false;

	m_MovePoint = 0;

	//鎴戞柟HP鍑忓皯鍊?
	int mylost = 0;
	if(type == CAttackCorps)
	{
		Crops* enemy = NULL;
		//瀵绘壘鏁屼汉
		enemy = &m_data->myCorps[ID];
		//鏁屼汉宸查樀浜?
		if(!enemy->m_bAlive)
			return false;
		//鏁屼汉鍚岄樀钀?
		if(enemy->m_PlayerID == m_PlayerID)
			return false;
		//鏁屼汉涓嶅湪灏勭▼鑼冨洿鍐?
		if(!IsInRange(enemy->m_position))
			return false;

		//濡傛灉鏁屼汉鎵€鍦ㄤ綅缃瓨鍦ㄦ晫鏂瑰娍鍔涘 浼樺厛涓庡缁撶畻
		int x = enemy->m_position.m_x;
		int y = enemy->m_position.m_y;
		int index = m_data->gameMap.map[y][x].TowerIndex;
		if(index == NOTOWER)
			AttackCrops(enemy);
		else if(m_data->myTowers[index].getPlayerID() == enemy->m_PlayerID)
			AttackTower(&(m_data->myTowers[index]));
		else
			AttackCrops(enemy);
	}
	else if(type == CAttackTower)
	{
		class Tower* enemy = NULL;
		//瀵绘壘鏁屼汉
		enemy = &m_data->myTowers[ID];
		//鏁屼汉宸查樀浜?
		if(!enemy->getexsit())
			return false;
		//鏁屼汉鍚岄樀钀?
		if(enemy->getPlayerID() == m_PlayerID)
			return false;
		//鏁屼汉涓嶅湪灏勭▼鑼冨洿鍐?
		if(!IsInRange(enemy->getPosition()))
			return false;

		AttackTower(enemy);
	}
	else//鎸囦护鏈夎
		return false;

	return true;
}

/*
UpdatePos 鏇存柊浣嶇疆
鏇存柊鍦╩_data涓璫orps璁板綍鐨勪綅缃互鍙婄被鍐呴儴鐨勪綅缃?
鍙傛暟 鏂扮殑浣嶇疆
*/
void Crops::UpdatePos(TPoint targetpos)
{
	CorpsUnit* thisplace = &(m_data->corps[m_position.m_y][m_position.m_x]);
	CorpsUnit::iterator it;
	for(it = (*thisplace).begin(); it!=(*thisplace).end(); it++)
	{
		if(*it == this)
		{
			(*thisplace).erase(it);
			break;
		}
	}
	m_data->corps[targetpos.m_y][targetpos.m_x].push_back(this);
	m_position = targetpos;
	//濡傛灉椹诲畧鍦ㄥ涓?鍒欏垹闄?
	if(m_StationTower)
	{
		m_StationTower->remove_crop(m_myID);
		m_StationTower = NULL;
	}

}

/*
ChangeOwner
鏀瑰彉鎵€鏈夎€?淇敼data涓搴旂殑player涓殑m_corps
鍙傛暟 鏂颁富浜篒D
*/
void Crops::ChangeOwner(TPlayerID newowner)
{
	m_data->players[m_PlayerID - 1].deleteCrops(m_myID);
	m_PlayerID = newowner;
	m_data->players[m_PlayerID - 1].addCrops(m_myID);
}

/*
KillCorps
鍏靛洟姝讳骸 淇敼player銆乨ata浠ュ強map涓殑鏁版嵁 灏嗗叺鍥㈢姸鎬佽缃负姝讳骸
*/
void Crops::KillCorps()
{
	m_HealthPoint = 0;
	m_BuildPoint = 0;
	m_data->players[m_PlayerID - 1].deleteCrops(m_myID);
	m_data->totalCorps--;
	CorpsUnit* thisplace = &(m_data->corps[m_position.m_y][m_position.m_x]);
	CorpsUnit::iterator it;
	for(it = (*thisplace).begin(); it!=(*thisplace).end(); it++)
	{
		if(*it == this)
		{
			(*thisplace).erase(it);
			break;
		}
	}
	m_bAlive = false;
	m_data->dieCorps.insert(m_myID);    //璁板綍姝讳骸鍏靛洟ID
}

//寤虹瓚鍏典慨濉?
bool Crops::JudgeMendTower(Command& c)
{
	if(!m_bAlive)
		return false;
	if(m_type != Construct)
		return false;
	if(m_BuildType != Builder)
		return false;

	//鑾峰彇璇ヤ綅缃鐨勪俊鎭?
	int index = m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex;
	//娌℃湁濉?
	if(index == NOTOWER)
		return false;
	//涓嶆槸宸辨柟濉?
	if(m_data->myTowers[index].getPlayerID()!=m_PlayerID)
		return false;
	//濉斿凡琚懅姣?
	if(!m_data->myTowers[index].getexsit())
		return false;
	c.parameters.clear();
	c.parameters.push_back(CRepair);
	c.parameters.push_back(m_myID);
	c.parameters.push_back(index);
	return true;
}

//寮€鎷撹€呭缓濉?
bool Crops::BuildTower()
{
	if (!m_bAlive)//by jyp 褰撳墠鍏靛洟姝讳骸
		return false;
	if (m_type != Construct)//by jyp褰撳墠鍏靛洟涓嶆槸宸ョ▼鍏靛洟
		return false;
	if (m_BuildType != Extender)//by jyp褰撳墠鍏靛洟涓嶆槸寮€鎷撹€?
		return false;

	//鑾峰彇璇ヤ綅缃鐨勪俊鎭?
	int index = m_data->gameMap.map[m_position.m_y][m_position.m_x].TowerIndex;
	//濡傛灉璇ヤ綅缃凡瀛樺湪濉?
	if(index != NOTOWER)
		return false;

	//鑾峰彇璇ヤ綅缃墍鏈夎€呭睘鎬?
	TPlayerID OwnerID;
	OwnerID = m_data->gameMap.showOwner(m_position);
	//
	//濡傛灉璇ュ崟鍏冩牸灞炰簬宸辨柟
	if(OwnerID != OUTOFRANGE && OwnerID == m_PlayerID)
	{
		//鏂板缓涓€涓槻寰″
		Tower newTower(m_data, m_PlayerID, m_position);
		m_data->myTowers.push_back(newTower);
		KillCorps();
		return true;
	}
	return false;
}

bool Crops::IsNeighbor(TPoint point)
{
	if(abs(point.m_x-m_position.m_x) <= 1&&abs(point.m_y-m_position.m_y) <= 1)
		return true;
	return false;
}

bool Crops::IsInRange(TPoint point)
{
	int range = TBattleRange[m_BattleType];
	if(abs(point.m_x-m_position.m_x) <= range&&abs(point.m_y-m_position.m_y) <= range)
		return true;
	return false;
}


void Crops::haveCmd()
{
	m_bResting = false;
	m_PeaceNum = 0;
}

//
void Crops::doChangingTerrain(terrainType target, int x, int y)
{
	m_data->gameMap.map[y][x].type = target;
	m_BuildPoint--;
	if (m_BuildPoint == 0)
	{
		KillCorps();
	}
}

//
void Crops::doMending(int index)
{	
	m_BuildPoint--;
	if (m_BuildPoint <= 0)
	{
		KillCorps();
	}

	m_data->myTowers[index].Recover();
}