#include "ai.h"
#include "definition.h"
#include "user_toolbox.h"
#include <iostream>
#include <vector>
#include <time.h>
//#include <fstream>
#include <stdlib.h>
#include <set>
#include <algorithm>

//4.3.2 add: A->D condition : at least 2 target tower's strategy==Attack
//		add: cut reinf to enrich target's resource
//		add: G->D condition: at least 1 attacker's strategy==Attack
//		modify: D->A condition: D->A is banned when at least 1 attacker's strategy==attack
//		add: N->D function
//4.3.3 add: est_resource : cutline estimated harm: * (suppress_power+1)/2\
//		modify: est_resource: when arrived, line_resource regarded to be equal to line_max_resource
//		add: family_lines: defense_stage taken into account
//		modify: D->A condition: num of attacker whose strategy != Attack >= num of attacker whose strategy== Attack
//4.3.4 modify: est_resource: skip when D attacks A & arrived 
//4.3.6 modify: is_enemy_arrived -> is_powerful_enemy_arrived
//5.0.1 add: complete defence
//5.0.2 add: defence condition: D->A or A->!D
//5.0.3 add: is_G_under_atk_of_A
//		add: X->D condition: target.owner!=myID
//5.0.4	add: attack is banned when round<50
//5.0.4a     regeneration upgrade is banned when round<20
//5.0.4b     D->!A cut line
//5.0.5      attack is recognized when line_resource > 6 * extendingspeedstage
//           defence is taken into account when enemy will arrive in 5 rounds
//			 cut line when D->A && !A->D
//5.0.6 modify: is_enemy_arrived is true when enemy will arrrive in 2 rounds
//		upgrade reinforcing system
//5.0.7 add: is_atk_line
//		add
//		add: cut all reinforcement to enrich its target's resource
//		add: simplify addCommmand
//		modify: est_resource: / family_line_num (enemy & myself )
//		modify: cut line D->A
//5.0.8	modify: G atk system & addline system
//		modify: change strategy condition
//		add: is_formal_attack, is_general_attack
//5.0.9 update: G cutline system
//		G->A/D when enemy arrived
//5.1.0 //modify: X->G
//		confront soon
//		correct bug in cnt_G_change
//5.1.1 
//5.1.2 defence strategy: frequently add & cut confront line to defense
//5.1.3 sort_my_towers_under_atk_first
//5.1.4 improve upgrading system
//5.1.5 is_neutral_tower & is_neutral_player & get_extend_speed
//		sort_towers before reinforcement
//5.1.6 biased when clear up
//5.1.7 est_resource when reinforcing, -type*30
//		reinf before atk
//		sort
//		bug resolved in is_enemy_arrive_soon
//		bug resolved in cutline when confront soon

// call self-protection when enemy cutline
// if arrive G, can change to A and cutline
// defence DO NOT attack towers expect A
// neutral tower
//cut line to enrich when my tower's resource is enough


// ATK_BEGIN_ROUND = 100; -> 648 722 ai 688

const int INITIALIZATION_ROUND = 10;
const int SOON_ARRIVE_ROUND = 2;
const int SOON_CONFRONT_ROUND = 2;
const int UNDER_ATK_LEAST_ROUND = 2;
const int ATK_BEGIN_ROUND = 50;
const double BASIC_RESOURCE = 30;
const double ADVANCED_RESOURCE = 40;
const double CUT_ATK_LINE_RATE = 1;
const double ADVANCED_STRATEGY_SWITCH_COST = 5;
const double STOP_REINF_LEAST_RESOURCE = 80;
const double ATK_RATE = 2;
const double MIN_EST_REINF_RESOURCE = -200;
const double MIN_CUT_REINF_RESOURCE = 100;
const double CALL_REINF_WHEN_BELOW = 120;
// SSFD == UBW > ZUC
bool UBW_flag1[4], UBW_flag2[4], UBW_flag3[4], UBW_flag4[4], UBW_flag5[4], UBW_flag6[4];
bool ZUC_flag1[4], ZUC_flag2[4], ZUC_flag3[4], ZUC_flag4[4], ZUC_flag5[4], ZUC_flag6[4];
bool SSFD_flag1[4], SSFD_flag2[4], SSFD_flag3[4], SSFD_flag4[4];

int UBW = -1;
int ZUC = -1;
int SSFD = -1;


/*
//print function
void print_vec_int(ostream& fout,vector<int>& vi){
	int sz=vi.size();
	fout<<'[';
	for(int i=0;i<sz;i++){
		fout<<vi[i]<<' ';
	}
	fout<<']';

}

void print_set_towers(ostream& fout,set<TTowerID>& towers){
	int sz=towers.size();
	fout<<" towers:[";
	for(set<TTowerID>::iterator it=towers.begin();it!=towers.end();it++){
		fout<<*it<<' ';
	}
	fout<<']';

}
void print_vec_playerinfo(ostream& fout,vector<PlayerInfo>& playerinfo){
	int sz=playerinfo.size();
	for(int i=0;i<sz;i++){
		fout<<"id:"<<playerinfo[i].id<<" rank:"<<playerinfo[i].rank<<" tp:"<<playerinfo[i].technologyPoint<<" regenlevel:"<<playerinfo[i].RegenerationSpeedLevel;
		print_set_towers(fout,playerinfo[i].towers);
		fout<<endl;
	}
}
void print_mycommandlist(ostream& fout,CommandList& myCommandList){

}
void print_vec_towerinfo(ostream& fout,vector<TowerInfo> towerinfo){
	int sz=towerinfo.size();
	for(int i=0;i<sz;i++){
		fout<<"towerid:"<<towerinfo[i].id<<" resource:"<<towerinfo[i].resource<<" type:"<<towerinfo[i].type<<" strategy:"<<towerinfo[i].strategy
			<<" maxLine:"<<towerinfo[i].maxLineNum<<" curLine:"<<towerinfo[i].currLineNum<<endl;
	}
}
void print_vec_whole_lineinfo(ostream& fout,vector<vector<LineInfo> >& lineInfo){
	fout<<"lineinfo: ";
	for(int i=0;i<lineInfo.size();i++){
		for(int j=0;j<lineInfo[i].size();j++){
		//	if(lineInfo[i][j].exist){
				fout<<'['<<lineInfo[i][j].sourceTower<<' '<<lineInfo[i][j].targetTower<<"] ";
		//	}
		}
		fout<<endl;
	}
	fout<<endl;
}
void print_vec_lineinfo(ostream& fout,vector<vector<LineInfo> >& lineInfo){
	fout<<"lineinfo: ";
	for(int i=0;i<lineInfo.size();i++){
		for(int j=0;j<lineInfo[i].size();j++){
			if(lineInfo[i][j].exist){
				fout<<'['<<lineInfo[i][j].sourceTower<<' '<<lineInfo[i][j].targetTower<<"] ";
			}
		}
	}
	fout<<endl;
}
void print_info(ostream& fout,Info& info){
	fout<<"\nround:"<<info.round<<endl;
	print_mycommandlist(fout,info.myCommandList);
	print_vec_playerinfo(fout,info.playerInfo);
	print_vec_towerinfo(fout,info.towerInfo);
	print_vec_lineinfo(fout,info.lineInfo);
	//
//	print_vec_whole_lineinfo(fout,info.lineInfo);
}
template<typename T>
void print_numeric_vec(ostream& fout,vector<T>& vec){
	fout<<'[';
	for(vector<T>::iterator it=vec.begin();it!=vec.end();it++){
		fout<<*it<<' ';
	}
	fout<<"]\n";
}*/

void cutline(Info& info,int source,int target,double pos){
	info.myCommandList.addCommand(cutLine,source,target,pos);

//		fout<<"command: cutline "<<my_towers[i]<<' '<<target<<endl;

//modify info
	info.lineInfo[source][target].state = AfterCut;
	info.lineInfo[source][target].frontResource = info.lineInfo[source][target].resource - pos;
	info.lineInfo[source][target].backResource = pos;
/*	if(pos == 0){
		info.towerInfo[source].currLineNum--;
	}
	*/
}
void addline(Info& info,int source,int target){
		info.myCommandList.addCommand(addLine,source,target);

	//	fout<<"command: addline "<<source<<' '<<target<<endl;			
				
	//modify info
		info.lineInfo[source][target].exist = true;
		info.lineInfo[source][target].sourceTower = source;
		info.lineInfo[source][target].targetTower = target;
		info.lineInfo[source][target].resource = 0;
		info.lineInfo[source][target].state = Extending;
		info.towerInfo[source].currLineNum++;
}
void changestrategy(Info& info,int tower,TowerStrategy strategy){
	info.playerInfo[info.myID].technologyPoint -= StrategyChangeCost[info.towerInfo[tower].strategy][strategy];
	info.myCommandList.addCommand(changeStrategy,tower,strategy);

//	fout<<"command:"<<"changeStrategy "<<my_towers[i]<<" Grow\n";
				
	//modify info

	info.towerInfo[tower].strategy = strategy;

	return;
}


//calc function
inline double min(double a,double b){
	return (a < b ? a : b);
}
inline double max(double a,double b){
	return (a > b ? a : b);
}
int family_line_num(Info& info,int playerID,int target){
	int cnt=0;
	for(int i=0;i<info.towerNum;i++){
		if(info.towerInfo[i].owner==playerID && info.lineInfo[i][target].exist && info.lineInfo[i][target].state!=AfterCut){
			cnt++;
		}
	}
	return cnt;
}

inline double line_max_resource(Info& info,int tower1,int tower2){
	return getDistance(info.towerInfo[tower1].position,info.towerInfo[tower2].position)*Density;
}
bool is_G_enough(Info& info,vector<int>& my_towers){
/*	int sz=my_towers.size();
	int cnt=0;
	for(int i=0;i<sz;i++){
		if(info.towerInfo[my_towers[i]].strategy == Grow){
			cnt++;
		}
	}
	if(cnt >= (sz+1)/2){
		return true;
	}
	else{
		return false;	
	}*/
	return false;
}


//op function
void complete_defence(Info& info,int my_tower){
	if(info.towerInfo[my_tower].strategy!=Defence && info.playerInfo[info.myID].technologyPoint >= StrategyChangeCost[info.towerInfo[my_tower].strategy][Defence]){
		info.myCommandList.addCommand(changeStrategy,my_tower,Defence);

		//modify info
		info.towerInfo[my_tower].strategy = Defence;
		info.playerInfo[info.myID].technologyPoint -= StrategyChangeCost[info.towerInfo[my_tower].strategy][Defence];
	}
	for(int i=0;i<info.towerNum;i++){
		if(info.lineInfo[my_tower][i].exist && info.lineInfo[my_tower][i].state != AfterCut){
			info.myCommandList.addCommand(cutLine,my_tower,i,info.lineInfo[my_tower][i].resource);

			//modify info
			info.lineInfo[my_tower][i].state = AfterCut;
			info.lineInfo[my_tower][i].frontResource = 0;
			info.lineInfo[my_tower][i].backResource = info.lineInfo[my_tower][i].resource;
		}
	}
}
bool is_clearing_up(Info& info,vector<int>& my_towers){
	if(my_towers.size() + info.playerAlive - 1 == info.towerNum){
		return true;
	}
	return false;
	
}
inline bool is_neutral_tower(Info& info,int tower){
	if(info.towerInfo[tower].owner <= 3 && info.towerInfo[tower].owner >= 0){
		return false;
	}
	return true;
}
inline bool is_neutral_player(Info& info,int player){
	if(player <= 3 && player >= 0){
		return false;
	}
	return true;
}
double get_extend_speed(Info& info,int playerid){
	if(is_neutral_player(info,playerid)){
		return BaseExtendSpeed;
	}
	return BaseExtendSpeed * SpeedStage[info.playerInfo[playerid].ExtendingSpeedLevel];
}

bool is_line_confront_soon(Info& info,int source,int target){
	if(info.lineInfo[source][target].exist && info.lineInfo[source][target].state!=AfterCut && 
		info.lineInfo[target][source].exist && info.lineInfo[target][source].state!=AfterCut
		&& info.towerInfo[source].owner!=info.towerInfo[target].owner){
			if(info.lineInfo[source][target].state == Confrontation || info.lineInfo[source][target].state == Attacking 
				|| info.lineInfo[source][target].state == Backing){
				return true;
			}
			if(info.lineInfo[source][target].state == Extending && info.lineInfo[target][source].state == Extending
				&& info.lineInfo[source][target].resource + info.lineInfo[target][source].resource + 
				(get_extend_speed(info,info.towerInfo[source].owner) + get_extend_speed(info,info.towerInfo[target].owner))
				* BaseExtendSpeed * SOON_CONFRONT_ROUND >= line_max_resource(info,source,target)){
				return true;
			}
	}
	return false;
}

bool is_confront_with_A_soon(Info& info,int target){
	for(int i=0;i<info.towerNum;i++){
		if(is_line_confront_soon(info,i,target) && info.towerInfo[i].strategy==Attack){
			return true;
		}
	}	
	return false;
}


bool is_formal_atk_line(Info& info,int source,int target){
	if(info.lineInfo[source][target].exist && info.lineInfo[source][target].state!=AfterCut && info.towerInfo[source].owner!=info.towerInfo[target].owner){
		if(info.towerInfo[target].owner!=info.myID || info.lineInfo[source][target].state != Extending){
			return true;
		}
		if(!is_neutral_tower(info,source) && info.lineInfo[source][target].resource >= 
		min(line_max_resource(info,source,target) - BaseExtendSpeed * SOON_ARRIVE_ROUND * SpeedStage[info.playerInfo[info.towerInfo[source].owner].ExtendingSpeedLevel],
		BaseExtendSpeed * UNDER_ATK_LEAST_ROUND * SpeedStage[info.playerInfo[info.towerInfo[source].owner].ExtendingSpeedLevel])){
			return true;
		}
	}
	return false;
}
bool is_general_atk_line(Info& info,int source,int target){
	if(info.lineInfo[source][target].exist && info.lineInfo[source][target].state!=AfterCut && info.towerInfo[source].owner!=info.towerInfo[target].owner){
		return true;
	}
	return false;
}
bool is_under_general_attack(Info& info,int target){
	for(int i=0;i<info.towerNum;i++){
		if(is_general_atk_line(info,i,target)){
			return true;
		}
	}
	return false;
}
bool is_under_general_attack_of_A(Info& info,int target){
	for(int i=0;i<info.towerNum;i++){
		if(is_general_atk_line(info,i,target) && info.towerInfo[i].strategy == Attack){
			return true;
		}
	}
	return false;
}
double est_resource(Info& info,int tower){
	vector<int> target;
	// tower is the sourcetower
	double ans=info.towerInfo[tower].resource;
	for(int i=0;i<info.towerNum;i++){
		if(!info.lineInfo[tower][i].exist) continue;

		if(info.lineInfo[tower][i].state == AfterCut){
			ans += info.lineInfo[tower][i].backResource;
		}
		//reinforcing
		else if(info.towerInfo[tower].owner == info.towerInfo[i].owner){
			ans += info.lineInfo[tower][i].resource - line_max_resource(info,tower,i) - info.towerInfo[tower].type * 10;
		}
		//attacking
		else{// target's resource* rate!!!! + line_max_resource - line_cur_resource
			int fln = family_line_num(info,info.towerInfo[tower].owner,i);
			if(fln == 0) fln = 1;

			if(is_line_confront_soon(info,tower,i)){
			ans -= info.towerInfo[i].resource / fln
					* ConfrontPower[info.towerInfo[i].strategy][info.towerInfo[tower].strategy] 
				/ ConfrontPower[info.towerInfo[tower].strategy][info.towerInfo[i].strategy];

				target.push_back(i);
			}
			else {//if(info.lineInfo[tower][i].state==Arrived || info.lineInfo[tower][i].state==Extending){
				ans -= info.towerInfo[i].resource / fln
				/ SupressPower[info.towerInfo[tower].strategy][info.towerInfo[i].strategy] 
				+ line_max_resource(info,tower,i) - info.lineInfo[tower][i].resource;
				target.push_back(i);
			}

		/*	ans -= info.towerInfo[i].resource / fln
				* ConfrontPower[info.towerInfo[i].strategy][info.towerInfo[tower].strategy] 
			/ ConfrontPower[info.towerInfo[tower].strategy][info.towerInfo[i].strategy]
			+ line_max_resource(info,tower,i) - info.lineInfo[tower][i].resource;
			target.push_back(i);*/

		}
	}
	// tower is the target
	int cnt=0;
	for(int i=0;i<info.towerNum;i++){
		if(cnt < target.size() && i == target[cnt]){
			cnt++;
			continue;
		}
		if(!info.lineInfo[i][tower].exist ) continue;

		//under reinforcement
		if(info.towerInfo[tower].owner == info.towerInfo[i].owner){
			if(info.lineInfo[i][tower].state==AfterCut){
				ans+=info.lineInfo[i][tower].frontResource;
			}
			else if(info.lineInfo[i][tower].state==Arrived){
				ans+=info.towerInfo[i].resource ;
				//	- getDistance(info.towerInfo[tower].position,info.towerInfo[i].position)*Density + info.lineInfo[i][tower].resource;
			}
		}
		//under attack
		else{

			if(info.towerInfo[i].strategy==Defence && info.towerInfo[tower].strategy==Attack && info.lineInfo[i][tower].state==Arrived){
				continue;
			}
			if(info.lineInfo[i][tower].state==AfterCut){
				ans-=info.lineInfo[i][tower].frontResource * (SupressPower[info.towerInfo[i].strategy][info.towerInfo[tower].strategy]+1)/2;
			}
			else{
				ans-=info.towerInfo[i].resource /** ConfrontPower[info.towerInfo[i].strategy][info.towerInfo[tower].strategy] 
				/ ConfrontPower[info.towerInfo[tower].strategy][info.towerInfo[i].strategy]*/
				- line_max_resource(info,tower,i) + info.lineInfo[i][tower].resource;
			}
		}
	}
	return ans;
}
int enemy_arrive_round(Info& info,int target){
	int rnd = -1;
	for(int i=0;i<info.towerNum;i++){
		if(!is_general_atk_line(info,i,target)){
			continue;
		}
		if(is_formal_atk_line(info,target,i)){
			continue;
		}
		if(info.lineInfo[i][target].state == Arrived){
			return 0;
		}
		if(info.lineInfo[i][target].state == Extending ){
			int tmprnd = (line_max_resource(info,i,target) - info.lineInfo[i][target].resource) / get_extend_speed(info,info.towerInfo[i].owner);
			if(rnd == -1 || tmprnd < rnd){
				rnd = tmprnd;
			}
		}
	}
	return rnd;
}

bool is_under_formal_attack(Info& info,int target){
	//attacker_towers.clear()
	for(int i=0;i<info.towerNum;i++){
		if(is_formal_atk_line(info,i,target)){
			return true;
		}
	}
	return false;
}
bool is_under_formal_attack_of_A(Info& info,int target){
	//attacker_towers.clear()
	for(int i=0;i<info.towerNum;i++){
		if(is_formal_atk_line(info,i,target) && info.towerInfo[i].strategy==Attack){
			return true;
		}
	}
	return false;
}

/*
bool is_under_attack(Info& info,int target){
	//attacker_towers.clear()
	for(int i=0;i<info.towerNum;i++){
		if(info.lineInfo[i][target].exist && info.lineInfo[i][target].state!= AfterCut && info.towerInfo[i].owner != info.towerInfo[target].owner){
			if(!is_neutral_tower(info,i) && info.lineInfo[i][target].resource > 6 * SpeedStage[info.playerInfo[info.towerInfo[i].owner].ExtendingSpeedLevel] ){
				return true;
			}
		}
	}
	return false;
}*/

bool is_under_third_party_atk(Info& info,int target){
	for(int i=0;i<info.towerNum;i++){
		if(is_formal_atk_line(info,i,target)
			&& info.towerInfo[i].owner != info.myID){
			return true;
		}
	}
	return false;
}
bool is_third_party_arrived(Info& info,int target){
	for(int i=0;i<info.towerNum;i++){
		if(i==target) continue;
		if(info.lineInfo[i][target].exist && info.lineInfo[i][target].state==Arrived && info.towerInfo[i].owner != info.towerInfo[target].owner
			&& info.towerInfo[i].owner != info.myID){
			return true;
		}
	}
	return false;
}
bool is_under_reinf(Info& info,int target){
	for(int i=0;i<info.towerNum;i++){
		if(info.lineInfo[i][target].exist && info.lineInfo[i][target].state!= AfterCut && info.towerInfo[i].owner == info.towerInfo[target].owner){
			return true;
		}
	}
	return false;
}

bool is_line_valid(Info& info,int source,int target){
	if(info.lineInfo[source][target].exist && info.lineInfo[source][target].state!=AfterCut){
		return true;
	}
	return false;
}
int get_cur_valid_line_num(Info& info,int tower){

	int cnt=0;
	for(int i=0;i<info.towerNum;i++){
		if(is_line_valid(info,tower,i)){
			cnt++;
		}
	}
	return cnt;

}
bool is_line_approaching(Info& info,int source,int target){
	if(info.lineInfo[source][target].exist && info.lineInfo[source][target].state!=AfterCut){
		if(info.lineInfo[source][target].state==Arrived){
			return true;
		}
		if(info.lineInfo[source][target].resource >= 
			line_max_resource(info,source,target) - get_extend_speed(info,info.towerInfo[source].owner) * SOON_ARRIVE_ROUND ){
			return true;
		}
	}
	return false;
}
bool is_enemy_approaching(Info& info,int target){
	for(int i=0;i<info.towerNum;i++){
		if(info.lineInfo[i][target].exist && info.towerInfo[i].owner != info.towerInfo[target].owner && 
			is_line_approaching(info,i,target)){
				return true;			
		}
	}
	return false;
}
bool is_line_arrive(Info& info,int source,int target){
	if(info.lineInfo[source][target].exist && info.lineInfo[source][target].state==Arrived){
		return true;
	}
	return false;
}

bool is_line_arrive_soon(Info& info,int source,int target){
	if(info.lineInfo[source][target].exist && info.lineInfo[source][target].state!=AfterCut){
		if(info.lineInfo[source][target].state==Arrived){
			return true;
		}
		if(!is_formal_atk_line(info,target,source) && info.lineInfo[source][target].resource >= 
			line_max_resource(info,source,target) - get_extend_speed(info,info.towerInfo[source].owner) * SOON_ARRIVE_ROUND ){
			return true;
		}
	}
	return false;
}
bool is_enemy_arrived(Info& info,int target){
	for(int i=0;i<info.towerNum;i++){
		if(info.lineInfo[i][target].exist && info.towerInfo[i].owner != info.towerInfo[target].owner){
			if(info.lineInfo[i][target].state== Arrived){
					return true;			
			}
		}
	}
	return false;
}

bool is_enemy_arrive_soon(Info& info,int target){
	for(int i=0;i<info.towerNum;i++){
		if(info.lineInfo[i][target].exist && info.towerInfo[i].owner != info.towerInfo[target].owner && 
			is_line_arrive_soon(info,i,target)){
				return true;			
		}
	}
	return false;
}
int cnt_enemy_arrive_soon(Info& info,int target){
	int cnt=0;
	for(int i=0;i<info.towerNum;i++){
		if(info.lineInfo[i][target].exist && info.towerInfo[i].owner != info.towerInfo[target].owner && 
			is_line_arrive_soon(info,i,target)){
				cnt++;			
		}
	}
	return cnt;
}

bool is_powerful_enemy_arrived(Info& info,int target){
	for(int i=0;i<info.towerNum;i++){
		if(info.lineInfo[i][target].exist && info.lineInfo[i][target].state== Arrived && info.towerInfo[i].owner != info.towerInfo[target].owner 
			&& SupressPower[info.towerInfo[i].strategy][info.towerInfo[target].strategy] > 1){
			return true;
		}
	}
	return false;
}
bool is_powerful_enemy_arrive_soon(Info& info,int target){
	for(int i=0;i<info.towerNum;i++){
		if(is_line_arrive_soon(info,i,target) && info.towerInfo[i].owner != info.towerInfo[target].owner 
			&& SupressPower[info.towerInfo[i].strategy][info.towerInfo[target].strategy] > 1){
			return true;
		}
	}
	return false;
}
int cnt_G_change(Info& info,vector<int>& my_towers){
	int cnt=0;
	for(int i=0;i<my_towers.size();i++){
		if(info.towerInfo[my_towers[i]].strategy != Grow){
			continue;
		}
		if(is_under_general_attack_of_A(info,my_towers[i]) || is_enemy_arrive_soon(info,my_towers[i])){
			cnt++;
		}
	}
	return cnt;
}

bool is_any_tower_under_formal_attack(Info& info,vector<int>& my_towers,vector<int>& others_towers){
	//attacker_towers.clear()
	int szi=info.lineInfo.size();
	for(int i=0;i<my_towers.size();i++){
		for(int j=0;j<others_towers.size();j++){
			if(is_formal_atk_line(info,others_towers[j],my_towers[i])){
				return true;
			}
		}
	}
	return false;
}

bool is_attack_started(Info& info,vector<int>& my_towers,vector<int>& others_towers){
	static bool flag = false;
	if(info.round >= ATK_BEGIN_ROUND || is_any_tower_under_formal_attack(info,my_towers,others_towers)) 
		flag = true;
	return flag;
}
int cnt_not_defence_A(Info& info,int my_tower){
	int cnt=0;
	for(int i=0;i<info.towerNum;i++){
		if(is_formal_atk_line(info,i,my_tower)){
			if(info.towerInfo[i].strategy==Attack && !is_general_atk_line(info,my_tower,i)){
				cnt++;
			}
		}
	}
	return cnt;
}


bool is_able_G2D(Info& info,int my_tower){
	if(info.towerInfo[my_tower].strategy != Grow || !is_under_formal_attack(info,my_tower)){
		return false;
	}

	int in_A_cnt=0,out_A_cnt=0,GN_cnt=0;
	for(int i=0;i<info.towerNum;i++){
		if(is_general_atk_line(info,i,my_tower)){
			if(info.towerInfo[i].strategy==Attack){
				in_A_cnt++;
			}
			if(info.towerInfo[i].strategy==Grow || info.towerInfo[i].strategy==Normal){
				GN_cnt++;
			}
		}
		if(is_general_atk_line(info,my_tower,i) && !is_general_atk_line(info,i,my_tower)){
			if(info.towerInfo[i].strategy==Attack){
				out_A_cnt++;
			}
			if(info.towerInfo[i].strategy==Grow || info.towerInfo[i].strategy==Normal){
				GN_cnt++;
			}			
		}

	}
	if((is_under_formal_attack_of_A(info,my_tower) || is_enemy_arrive_soon(info,my_tower) || is_confront_with_A_soon(info,my_tower)) &&
		(in_A_cnt >= info.towerInfo[my_tower].maxLineNum || GN_cnt == 0 && in_A_cnt > 1)){
		return true;
	}

	return false;
}
//modify
bool is_able_G2A(Info& info,int my_tower){

	if(info.towerInfo[my_tower].strategy != Grow){
		return false;
	}
	if((is_under_formal_attack_of_A(info,my_tower) || is_enemy_arrive_soon(info,my_tower) || is_confront_with_A_soon(info,my_tower)) && !is_able_G2D(info,my_tower)){
		return true;
	}
	return false;

}
//tested
bool is_able_A2D(Info& info,int my_tower){
	if(info.towerInfo[my_tower].strategy!=Attack || est_resource(info,my_tower) >= 0)
		return false;
	int in_A_cnt=0,out_A_cnt=0,GN_cnt=0;
	for(int i=0;i<info.towerNum;i++){
		if(is_formal_atk_line(info,i,my_tower)){
			if(info.towerInfo[i].strategy==Attack){
				in_A_cnt++;
			}
			if(info.towerInfo[i].strategy==Grow || info.towerInfo[i].strategy==Normal){
				GN_cnt++;
			}
		}
		else if(is_formal_atk_line(info,my_tower,i)){
			if(info.towerInfo[i].strategy==Attack){
				out_A_cnt++;
			}
			if(info.towerInfo[i].strategy==Grow || info.towerInfo[i].strategy==Normal){
				GN_cnt++;
			}			
		}

	}
	if( in_A_cnt >= info.towerInfo[my_tower].maxLineNum ||  GN_cnt == 0 && in_A_cnt > 1){
		return true;
	}
	return false;
}

bool is_able_N2D(Info& info,int my_tower){
	if(info.towerInfo[my_tower].strategy!=Normal || est_resource(info,my_tower) >= 0)
		return false;
	int in_A_cnt=0,out_A_cnt=0,GN_cnt=0;
	for(int i=0;i<info.towerNum;i++){
		if(is_formal_atk_line(info,i,my_tower)){
			if(info.towerInfo[i].strategy==Attack){
				in_A_cnt++;
			}
			if(info.towerInfo[i].strategy==Grow || info.towerInfo[i].strategy==Normal){
				GN_cnt++;
			}
		}
		else if(is_formal_atk_line(info,my_tower,i)){
			if(info.towerInfo[i].strategy==Attack){
				out_A_cnt++;
			}
			if(info.towerInfo[i].strategy==Grow || info.towerInfo[i].strategy==Normal){
				GN_cnt++;
			}			
		}

	}
	if( in_A_cnt >= info.towerInfo[my_tower].maxLineNum ||  GN_cnt == 0 && in_A_cnt > 1){
		return true;
	}
	return false;
}
bool is_able_N2A(Info& info,vector<int>& my_towers,int i){
	if(info.towerInfo[my_towers[i]].strategy == Normal){
		return true;
	}
	return false;
}
//tested
bool is_able_N2G(Info& info,vector<int>& my_towers,int i){
	if(info.round < INITIALIZATION_ROUND && my_towers[i]%3){
		return false;
	}
	if(info.towerInfo[my_towers[i]].strategy != Normal ){
		return false;
	}
	if(!is_under_formal_attack_of_A(info,my_towers[i]) && !is_enemy_arrive_soon(info,my_towers[i]) && !is_confront_with_A_soon(info,my_towers[i])){
		return true;
	}
	return false;
}
//tested
bool is_able_A2G(Info& info,vector<int>& my_towers,int i){
	if(info.round < INITIALIZATION_ROUND && my_towers[i]%3 ){
		return false;
	}
	if(info.towerInfo[my_towers[i]].strategy != Attack){
		return false;
	}

	if(!is_under_formal_attack_of_A(info,my_towers[i]) && !is_enemy_arrive_soon(info,my_towers[i]) && !is_confront_with_A_soon(info,my_towers[i])){
		return true;
	}
	return false;
}
//tested
bool is_able_D2G(Info& info,vector<int>& my_towers,int i){
	if(info.round < INITIALIZATION_ROUND && my_towers[i]%3 ){
		return false;
	}
	if(info.towerInfo[my_towers[i]].strategy != Defence ){
		return false;
	}
	if(!is_under_formal_attack_of_A(info,my_towers[i]) && !is_enemy_arrive_soon(info,my_towers[i]) && !is_confront_with_A_soon(info,my_towers[i])){
		return true;
	}
	return false;
}
//tested
bool is_able_D2A(Info& info,int my_tower){
	if(info.towerInfo[my_tower].strategy!=Defence || info.towerInfo[my_tower].resource < ADVANCED_RESOURCE)
		return false;
	if(!is_under_formal_attack(info,my_tower)){
		return true;
	}
	int in_A_cnt=0,out_A_cnt=0,GN_cnt=0;
	for(int i=0;i<info.towerNum;i++){
		if(is_formal_atk_line(info,i,my_tower)){
			if(info.towerInfo[i].strategy==Attack){
				in_A_cnt++;
			}
			if(info.towerInfo[i].strategy==Grow || info.towerInfo[i].strategy==Normal){
				GN_cnt++;
			}
		}
		else if(is_formal_atk_line(info,my_tower,i)){
			if(info.towerInfo[i].strategy==Attack){
				out_A_cnt++;
			}
			if(info.towerInfo[i].strategy==Grow || info.towerInfo[i].strategy==Normal){
				GN_cnt++;
			}			
		}

	}
	if( in_A_cnt < info.towerInfo[my_tower].maxLineNum && in_A_cnt <= GN_cnt){
		return true;
	}
	return false;
}
double calc_3p_reinf_n_atk_resource(Info& info,int target){
	// tower is the target
	double ans=0;
	for(int i=0;i<info.towerNum;i++){
		if(!info.lineInfo[i][target].exist || info.towerInfo[i].owner==info.myID) continue;

		if(info.lineInfo[i][target].state==AfterCut){
			ans+=info.lineInfo[i][target].frontResource;
		}
		else if(is_line_arrive_soon(info,i,target)){
			ans+=info.towerInfo[i].resource;
		}
	}
	return ans;
}

//tested
bool is_able_cut_atk_line(Info& info,vector<int>& my_towers,int i,vector<int>& others_towers,int j){
	if(!info.lineInfo[my_towers[i]][others_towers[j]].exist || info.lineInfo[my_towers[i]][others_towers[j]].state==AfterCut){ 
		return false;
	}
	if(info.towerInfo[my_towers[i]].resource < BASIC_RESOURCE ){
		return true;
	}
	//complete attacking
	if(!info.lineInfo[others_towers[j]][my_towers[i]].exist || info.lineInfo[others_towers[j]][my_towers[i]].state==AfterCut){
		if(is_powerful_enemy_arrive_soon(info,my_towers[i]) && est_resource(info,my_towers[i]) < 0){
			return true;
		}
		if(info.towerInfo[my_towers[i]].strategy==Defence && info.towerInfo[others_towers[j]].strategy==Attack){
			return true;
		}
		if(info.towerInfo[my_towers[i]].strategy!=Grow  && 
			info.towerInfo[my_towers[i]].resource * ConfrontPower[info.towerInfo[my_towers[i]].strategy][info.towerInfo[others_towers[j]].strategy] 
		< info.towerInfo[others_towers[j]].resource + calc_3p_reinf_n_atk_resource(info,others_towers[j])
			* ConfrontPower[info.towerInfo[others_towers[j]].strategy][info.towerInfo[my_towers[i]].strategy]
		/ family_line_num(info,info.myID,others_towers[j]) * CUT_ATK_LINE_RATE){
			return true;
		}
		if(info.towerInfo[my_towers[i]].strategy==Grow){
			if(info.towerInfo[my_towers[i]].resource < info.towerInfo[others_towers[j]].resource 
				/ family_line_num(info,info.myID,others_towers[j]) * CUT_ATK_LINE_RATE){
				return true;
			}
		}
	}
	// under atk
	if(info.lineInfo[others_towers[j]][my_towers[i]].exist && info.lineInfo[others_towers[j]][my_towers[i]].state!=AfterCut){
		if(info.towerInfo[my_towers[i]].strategy==Defence && info.towerInfo[others_towers[j]].strategy!=Attack 
			&& cnt_not_defence_A(info,my_towers[i]) + info.towerInfo[my_towers[i]].currLineNum < info.towerInfo[my_towers[i]].maxLineNum){
			return true;
		}
	}
	// confront soon
	if(info.towerInfo[my_towers[i]].strategy==Grow && is_line_confront_soon(info,my_towers[i],others_towers[j]) && !is_enemy_arrive_soon(info,my_towers[i]) &&			
		info.towerInfo[my_towers[i]].resource * ConfrontPower[info.towerInfo[my_towers[i]].strategy][info.towerInfo[others_towers[j]].strategy] 
	< info.towerInfo[others_towers[j]].resource * ConfrontPower[info.towerInfo[others_towers[j]].strategy][info.towerInfo[my_towers[i]].strategy]
	/ family_line_num(info,info.myID,others_towers[j]) * CUT_ATK_LINE_RATE){
		return true;
	}
	// confronting
	if(info.towerInfo[my_towers[i]].strategy == Defence && is_line_valid(info,others_towers[j],my_towers[i])){
		if(info.lineInfo[my_towers[i]][others_towers[j]].state == Confrontation 
		|| info.lineInfo[my_towers[i]][others_towers[j]].state == Attacking && info.lineInfo[my_towers[i]][others_towers[j]].resource >= 15){

		//		fout<<info.round<<' '<<my_towers[i]<<' '<<endl;
			return true;
		}
	}
	if(is_line_valid(info,others_towers[j],my_towers[i]) && family_line_num(info,info.myID,others_towers[j]) <= 1){
		if(info.lineInfo[my_towers[i]][others_towers[j]].state == Confrontation ){

		//		fout<<info.round<<' '<<my_towers[i]<<' '<<endl;
			return true;
		}
	}


/*	if(info.towerInfo[my_towers[i]].resource * ConfrontPower[info.towerInfo[my_towers[i]].strategy][info.towerInfo[others_towers[j]].strategy] 
	< info.towerInfo[others_towers[j]].resource * ConfrontPower[info.towerInfo[others_towers[j]].strategy][info.towerInfo[my_towers[i]].strategy]
	/ family_line_num(info,info.myID,others_towers[j]) * CUT_ATK_LINE_RATE &&
		info.lineInfo[my_towers[i]][others_towers[j]].exist && info.lineInfo[my_towers[i]][others_towers[j]].state == Confrontation){
			return true;
	}*/

	//problemmmmmmm
	if(info.towerInfo[my_towers[i]].strategy==Attack && info.towerInfo[others_towers[j]].strategy==Defence
		&& is_line_confront_soon(info,my_towers[i],others_towers[j]) && 			
		info.towerInfo[my_towers[i]].resource * ConfrontPower[info.towerInfo[my_towers[i]].strategy][info.towerInfo[others_towers[j]].strategy] 
	< info.towerInfo[others_towers[j]].resource * ConfrontPower[info.towerInfo[others_towers[j]].strategy][info.towerInfo[my_towers[i]].strategy]
	/ family_line_num(info,info.myID,others_towers[j]) * CUT_ATK_LINE_RATE){
		return true;
	}
	// line state is changeable
	if(info.towerInfo[my_towers[i]].strategy==Attack && info.towerInfo[others_towers[j]].strategy==Defence){
		if(info.towerInfo[my_towers[i]].resource * ConfrontPower[info.towerInfo[my_towers[i]].strategy][info.towerInfo[others_towers[j]].strategy] 
		< info.towerInfo[others_towers[j]].resource * ConfrontPower[info.towerInfo[others_towers[j]].strategy][info.towerInfo[my_towers[i]].strategy]
		/ family_line_num(info,info.myID,others_towers[j]) * CUT_ATK_LINE_RATE){
			return true;
		}
	}

	return false;
}



// not tested
double family_atk_line_resource(Info& info,int target){
	double res=0;
	for(int i=0;i<info.towerNum;i++){
		if(info.towerInfo[i].owner!=info.myID || !info.lineInfo[i][target].exist )//||
	//		info.towerInfo[i].resource<=ADVANCED_RESOURCE || est_resource(info,i)+info.towerInfo[i].resource < 0 ) 
			continue;

		if(is_line_arrive_soon(info,i,target) ){
			double unit_res=info.lineInfo[i][target].resource * (SupressPower[info.towerInfo[i].strategy][info.towerInfo[target].strategy] + 1)/2;
			if(info.towerInfo[target].owner>=0 && info.towerInfo[target].owner<=3){
				unit_res *= DefenceStage[info.playerInfo[info.towerInfo[target].owner].DefenceLevel] / 1.5;
			}
			res+=unit_res;
		}
		else if(info.lineInfo[i][target].state==AfterCut){
			double unit_res=info.lineInfo[i][target].frontResource * (SupressPower[info.towerInfo[i].strategy][info.towerInfo[target].strategy] + 1)/2;
			if(info.towerInfo[target].owner>=0 && info.towerInfo[target].owner<=3){
				unit_res *= DefenceStage[info.playerInfo[info.towerInfo[target].owner].DefenceLevel] / 1.5;
			}
			res+=unit_res;
		}
	}
	return res;
}
double family_reinf_line_resource(Info& info,int target){
	double res=0;
	for(int i=0;i<info.towerNum;i++){
		if(info.towerInfo[i].owner!=info.myID || !info.lineInfo[i][target].exist )//||
	//		info.towerInfo[i].resource<=ADVANCED_RESOURCE || est_resource(info,i)+info.towerInfo[i].resource < 0 ) 
			continue;

		if(is_line_arrive_soon(info,i,target) ){
			double unit_res=info.lineInfo[i][target].resource;
			res+=unit_res;
		}
		else if(info.lineInfo[i][target].state==AfterCut){
			double unit_res=info.lineInfo[i][target].frontResource;
			res+=unit_res;
		}
	}
	return res;
}
double est_resource_aftercut_reinf(Info& info,int target){
	double res=est_resource(info,target);
	for(int i=0;i<info.towerNum;i++){
		if(info.towerInfo[i].owner!=info.myID || !info.lineInfo[i][target].exist) continue;
		if(info.lineInfo[i][target].state!=AfterCut){
			res+=line_max_resource(info,i,target) - info.towerInfo[i].resource;
		}
	}
	return res;
}


double est_resource_after_reinf(Info& info,vector<int>& my_towers,int target){
	double ans=est_resource(info,target);
//	est_resource(info,my_towers[i]) - line_max_resource(info,my_towers[i],my_towers[j]) + est_resource(info,my_towers[j]) >= 10
	for(int i=0;i<my_towers.size();i++){
		if(info.towerInfo[my_towers[i]].resource <= ADVANCED_RESOURCE || info.towerInfo[my_towers[i]].currLineNum >= info.towerInfo[my_towers[i]].maxLineNum
			|| est_resource(info,my_towers[i]) < 0){
			continue;
		}

		if(my_towers[i] == target || info.lineInfo[my_towers[i]][target].exist || est_resource(info,target) >= 0 || !is_under_formal_attack(info,target)
			|| info.towerInfo[target].resource > 100 || info.towerInfo[my_towers[i]].resource <= line_max_resource(info,my_towers[i],target)+ADVANCED_RESOURCE){ 
			continue;
		}

		ans += info.towerInfo[my_towers[i]].resource - line_max_resource(info,my_towers[i],target);
	}
	return ans;
}

double key_others(Info& info,int my_tower,int other_tower){
	double key=est_resource(info,other_tower);
	if(key>=0) 
		key /= 2 * family_line_num(info,info.myID,other_tower)+1;
	else //testing
		key *= 2 * family_line_num(info,info.myID,other_tower)+1;
	key += line_max_resource(info,my_tower,other_tower);
	return key;
}
double key_mine(Info& info,int my_tower,vector<int>& atk_towers){
	double ans= info.towerInfo[my_tower].resource;
	for(int i=0;i<atk_towers.size();i++){
		ans-=info.towerInfo[atk_towers[i]].resource + 10;
	}
	return ans;
}

void sort_towers_asc(Info& info,vector<int>& towers,int left,int right){//right为最后一个下标+1
	for(int i=left+1;i<right;i++){
		for(int j=left;j<right-i;j++){
			if(est_resource(info,towers[j+1]) < est_resource(info,towers[j])){
				int tmp=towers[j];
				towers[j]=towers[j+1];
				towers[j+1]=tmp;
			}
		}
	}
}
void sort_towers_des(Info& info,vector<int>& towers,int left,int right){//right为最后一个下标+1
	for(int i=left+1;i<right;i++){
		for(int j=left;j<right-i;j++){
			if(est_resource(info,towers[j+1]) > est_resource(info,towers[j])){
				int tmp=towers[j];
				towers[j]=towers[j+1];
				towers[j+1]=tmp;
			}
		}
	}
}


void sort_others_towers_asc(Info& info,int my_tower,vector<int>& others_towers,int left,int right){//right为最后一个下标+1
	for(int i=left+1;i<right;i++){
		for(int j=left;j<right-i;j++){
			if(key_others(info,my_tower,others_towers[j+1]) < key_others(info,my_tower,others_towers[j]) ){
				int tmp=others_towers[j];
				others_towers[j]=others_towers[j+1];
				others_towers[j+1]=tmp;
			}
		}
	}
}
void sort_this_is_under_atk_of(Info& info,int my_tower,vector<int>& this_is_under_atk_of,int left,int right){//right为最后一个下标+1
	for(int i=left+1;i<right;i++){
		for(int j=left;j<right-i;j++){
			if(info.lineInfo[this_is_under_atk_of[j+1]][my_tower].state==Arrived && info.towerInfo[my_tower].strategy==Defence && info.towerInfo[this_is_under_atk_of[j+1]].strategy==Attack
				|| info.lineInfo[this_is_under_atk_of[j+1]][my_tower].state==Arrived && info.lineInfo[this_is_under_atk_of[j]][my_tower].state==Extending
				|| info.lineInfo[this_is_under_atk_of[j+1]][my_tower].state==Extending && info.lineInfo[this_is_under_atk_of[j]][my_tower].state==Extending
				&& line_max_resource(info,this_is_under_atk_of[j+1],my_tower) - info.lineInfo[this_is_under_atk_of[j+1]][my_tower].resource 
				< line_max_resource(info,this_is_under_atk_of[j],my_tower) - info.lineInfo[this_is_under_atk_of[j]][my_tower].resource ){
				int tmp=this_is_under_atk_of[j];
				this_is_under_atk_of[j]=this_is_under_atk_of[j+1];
				this_is_under_atk_of[j+1]=tmp;
			}
		}
	}
}
void sort_my_towers_under_extending_first(Info& info,vector<int>& my_towers,int left,int right){
	for(int i=left+1;i<right;i++){
		for(int j=left;j<right-i;j++){
			if(is_under_formal_attack(info,my_towers[j+1])){
				if(enemy_arrive_round(info,my_towers[j]) == -1 || enemy_arrive_round(info,my_towers[j+1]) < enemy_arrive_round(info,my_towers[j])){
					int tmp=my_towers[j];
					my_towers[j]=my_towers[j+1];
					my_towers[j+1]=tmp;
				}
			}
			// both are not under formal atk
			else if(!is_under_formal_attack(info,my_towers[j])){
				if(est_resource(info,my_towers[j+1]) < est_resource(info,my_towers[j])){
					int tmp=my_towers[j];
					my_towers[j]=my_towers[j+1];
					my_towers[j+1]=tmp;
				}
			}
		}
	}
}

void sort_my_towers_under_atk_first(Info& info,vector<int>& my_towers,int left,int right){
	for(int i=left+1;i<right;i++){
		for(int j=left;j<right-i;j++){
			if(!is_under_formal_attack(info,my_towers[j]) && is_under_formal_attack(info,my_towers[j+1])){
				int tmp=my_towers[j];
				my_towers[j]=my_towers[j+1];
				my_towers[j+1]=tmp;
			}
			else if(is_under_formal_attack(info,my_towers[j]) && is_under_formal_attack(info,my_towers[j+1])){
				if(enemy_arrive_round(info,my_towers[j]) == -1 || enemy_arrive_round(info,my_towers[j+1]) < enemy_arrive_round(info,my_towers[j])){
					int tmp=my_towers[j];
					my_towers[j]=my_towers[j+1];
					my_towers[j+1]=tmp;
				}
			}
			// both are not under formal atk
			else if(!is_under_formal_attack(info,my_towers[j]) && !is_under_formal_attack(info,my_towers[j+1])){
				if(est_resource(info,my_towers[j+1]) < est_resource(info,my_towers[j])){
					int tmp=my_towers[j];
					my_towers[j]=my_towers[j+1];
					my_towers[j+1]=tmp;
				}
			}
		}
	}
}

void sort_my_towers_close_first(Info& info,vector<int>& my_towers,int target,int left,int right){
	for(int i=left+1;i<right;i++){
		for(int j=left;j<right-i;j++){
			if(line_max_resource(info,my_towers[j+1],target) < line_max_resource(info,my_towers[j],target)){
				int tmp=my_towers[j];
				my_towers[j]=my_towers[j+1];
				my_towers[j+1]=tmp;
			}
		}
	}
}
void bubble_my_towers_des(Info& info,vector<int>& my_towers,vector<vector<int> >& atk_towers,int left,int right){//right为最后一个下标+1
	for(int i=left+1;i<right;i++){
		for(int j=left;j<right-i;j++){
			if(key_mine(info,my_towers[j+1],atk_towers[my_towers[j+1]]) > key_mine(info,my_towers[j],atk_towers[my_towers[j]])){
				int tmp=my_towers[j];
				my_towers[j]=my_towers[j+1];
				my_towers[j+1]=tmp;
			}
		}
	}
}
void sort_my_towers_line_resource_des(Info& info,vector<int>& my_towers,int target,int left,int right){
	for(int i=left+1;i<right;i++){
		for(int j=left;j<right-i;j++){
			if(info.lineInfo[my_towers[j+1]][target].exist && info.lineInfo[my_towers[j+1]][target].state!=AfterCut &&
				(!info.lineInfo[my_towers[j]][target].exist || info.lineInfo[my_towers[j]][target].state==AfterCut ||
				line_max_resource(info,my_towers[j+1],target) > line_max_resource(info,my_towers[j],target))){
				int tmp=my_towers[j];
				my_towers[j]=my_towers[j+1];
				my_towers[j+1]=tmp;
			}
		}
	}	
}




/*
double pr_regeneration(Info& info,int towerid){
	double ans=(info.towerInfo[towerid].type * 0.5 + 1);
	if(info.towerInfo[towerid].owner!=-1){
		 ans *= (info.playerInfo[info.towerInfo[towerid].owner].RegenerationSpeedLevel * 0.05 + 1);
	}
	if(info.towerInfo[towerid].strategy==Grow){
		ans*=1.5;
	}
	else if(info.towerInfo[towerid].strategy==Defence){
		ans*=0.5;
	}
	return ans;

}*/
/*
double pr_line_resource(Info& info,int towerid,vector<int>& my_towers){
	// towerid is the sourcetower
	double ans=0;
	for(int i=0;i<info.towerNum;i++){
		if(!info.lineInfo[towerid][i].exist) continue;

		double base_speed_i=(info.towerInfo[i].type * 0.5 + 1)* LineDecay[info.towerInfo[i].currLineNum];
		double base_speed_towerid=(info.towerInfo[towerid].type * 0.5 + 1)* LineDecay[info.towerInfo[towerid].currLineNum];

		if(info.lineInfo[towerid][i].state==Extending){
			if(info.towerInfo[towerid].owner>=0 && info.towerInfo[towerid].owner<=3){
				ans -= BaseExtendSpeed * SpeedStage[info.playerInfo[info.towerInfo[towerid].owner].ExtendingSpeedLevel];	
			}	
			else{
				ans -= BaseExtendSpeed;
			}
		}
		else if(info.lineInfo[towerid][i].state==Backing){
			ans += min(BaseBackSpeed, info.lineInfo[towerid][i].resource);
		}
		else if(info.lineInfo[towerid][i].state==Confrontation){
			double max_power = max(ConfrontPower[info.towerInfo[i].strategy][info.towerInfo[towerid].strategy] * base_speed_i,
				ConfrontPower[info.towerInfo[towerid].strategy][info.towerInfo[i].strategy] * base_speed_towerid);
			//	ofstream fout("E:\\Year 1\\spring\\freshmen_c\\user_package\\log_txt\\max_power.out",ios::app);
			//	fout<<"round"<<info.round<<"\ndddddddddddddddd\n"<<max_power<<"\nddddddddddddd\n";
			if(ConfrontPower[info.towerInfo[i].strategy][info.towerInfo[towerid].strategy] 
				> ConfrontPower[info.towerInfo[towerid].strategy][info.towerInfo[i].strategy]){
					ans -= max_power;
			}
			else{
				ans -= max_power / ConfrontPower[info.towerInfo[towerid].strategy][info.towerInfo[i].strategy];
			}
		}
		else if(info.lineInfo[towerid][i].state==Attacking){
			
		}


		else if(info.lineInfo[towerid][i].state == AfterCut){
			ans += min(BaseFrontSpeed,info.lineInfo[towerid][i].backResource);
		}
		else if(info.lineInfo[towerid][i].state == Arrived){
			ans -= base_speed_towerid;
		}
	}
	// towerid is the target
	for(int i=0;i<info.towerNum;i++){
		if(!info.lineInfo[i][towerid].exist || info.lineInfo[towerid][i].state == Extending) continue;

		double base_speed_i=(info.towerInfo[i].type * 0.5 + 1)* LineDecay[info.towerInfo[i].currLineNum];
		double base_speed_towerid=(info.towerInfo[towerid].type * 0.5 + 1)* LineDecay[info.towerInfo[towerid].currLineNum];

		if(info.lineInfo[i][towerid].state == Arrived){
			// enemy attacking
			double base_speed_i=(info.towerInfo[i].type * 0.5 + 1)* LineDecay[info.towerInfo[i].currLineNum];
			if(find(my_towers.begin(),my_towers.end(),i)==my_towers.end()){
				if(info.towerInfo[i].owner>=0 && info.towerInfo[i].owner<=3){
					ans -= SupressPower[info.towerInfo[i].strategy][info.towerInfo[towerid].strategy] * base_speed_i
						 * DefenceStage[info.playerInfo[info.towerInfo[i].owner].DefenceLevel] / 1.5 ;						
				}
				else{
					ans -= SupressPower[info.towerInfo[i].strategy][info.towerInfo[towerid].strategy] * base_speed_i;
				}
			}
			// family reinforcement
			else{
				ans += base_speed_i;
			}
		}
		else if(info.lineInfo[i][towerid].state == Confrontation){
			double max_power = max(ConfrontPower[info.towerInfo[i].strategy][info.towerInfo[towerid].strategy] * base_speed_i,
				ConfrontPower[info.towerInfo[towerid].strategy][info.towerInfo[i].strategy] * base_speed_towerid);
		//		ofstream fout("E:\\Year 1\\spring\\freshmen_c\\user_package\\log_txt\\max_power.out",ios::app);
		//		fout<<"round"<<info.round<<"\ndddddddddddddddd\n"<<max_power<<"\nddddddddddddd\n";
			if(ConfrontPower[info.towerInfo[i].strategy][info.towerInfo[towerid].strategy] 
				> ConfrontPower[info.towerInfo[towerid].strategy][info.towerInfo[i].strategy]){
					ans -= max_power;
			}
			else{
				ans -= max_power / ConfrontPower[info.towerInfo[towerid].strategy][info.towerInfo[i].strategy];
			}
		}
		else if(info.lineInfo[i][towerid].state == AfterCut){
			ans -= min(BaseFrontSpeed,info.lineInfo[i][towerid].frontResource);
		}
	}
	return ans;
}
*/
/*

void estimate_pr_resource(Info& info,vector<double>& est_resource,vector<int>& my_towers){
	for(int i=0;i<info.towerNum;i++){
		est_resource[i]= min(info.towerInfo[i].resource + pr_regeneration(info,i) + pr_line_resource(info,i,my_towers),
			info.towerInfo[i].maxResource);
	}
}
int est_atk_round(Info& info,int source,int target,vector<int>& my_towers){
	// target is the sourcetower
	double round_arrive = getDistance(info.towerInfo[source].position,info.towerInfo[target].position) * Density
		/ (BaseExtendSpeed * SpeedStage[info.playerInfo[info.towerInfo[target].owner].ExtendingSpeedLevel]);
	double target_resource_arrived = info.towerInfo[target].resource;
	double rate_arrive = 0;

	double regeneration = info.towerInfo[target].type * 0.5 +1;
	if(info.towerInfo[target].owner!=-1){
		 regeneration *= (info.playerInfo[info.towerInfo[target].owner].RegenerationSpeedLevel * 0.05 + 1);
	}
	if(info.towerInfo[target].strategy==Grow){
		regeneration*=1.5;
	}
	else if(info.towerInfo[target].strategy==Defence){
		regeneration*=0.5;
	}
	target_resource_arrived += round_arrive * regeneration;
	rate_arrive += regeneration;

	for(int i=0;i<info.towerNum;i++){
		if(!info.lineInfo[target][i].exist) continue;

		double base_speed_i=(info.towerInfo[i].type * 0.5 + 1)* LineDecay[info.towerInfo[i].currLineNum];
		double base_speed_target=(info.towerInfo[target].type * 0.5 + 1)* LineDecay[info.towerInfo[target].currLineNum];

		if(info.lineInfo[target][i].state==Extending){
			if(info.towerInfo[target].owner>=0 && info.towerInfo[target].owner<=3){
				target_resource_arrived -= min(getDistance(info.towerInfo[source].position,info.towerInfo[target].position) * Density,
					BaseExtendSpeed * SpeedStage[info.playerInfo[info.towerInfo[target].owner].ExtendingSpeedLevel] * round_arrive);
				if(getDistance(info.towerInfo[source].position,info.towerInfo[target].position) * Density <
					BaseExtendSpeed * SpeedStage[info.playerInfo[info.towerInfo[target].owner].ExtendingSpeedLevel] * round_arrive){
						rate_arrive-=BaseExtendSpeed * SpeedStage[info.playerInfo[info.towerInfo[target].owner].ExtendingSpeedLevel];
				}

			}	
			else{
				target_resource_arrived -= min(getDistance(info.towerInfo[source].position,info.towerInfo[target].position) * Density,
					BaseExtendSpeed * round_arrive);
				if(getDistance(info.towerInfo[source].position,info.towerInfo[target].position) * Density <
					BaseExtendSpeed * round_arrive){
						rate_arrive -= BaseExtendSpeed * round_arrive;
				}

			}
		}
		else if(info.lineInfo[target][i].state==Backing){
			target_resource_arrived += min(BaseBackSpeed * round_arrive, info.lineInfo[target][i].resource);
			if(BaseBackSpeed * round_arrive < info.lineInfo[target][i].resource){
				rate_arrive+=BaseBackSpeed;
			}
		}
		else if(info.lineInfo[target][i].state==Confrontation){
			double max_power = max(ConfrontPower[info.towerInfo[i].strategy][info.towerInfo[target].strategy] * base_speed_i,
				ConfrontPower[info.towerInfo[target].strategy][info.towerInfo[i].strategy] * base_speed_target);
			//	ofstream fout("E:\\Year 1\\spring\\freshmen_c\\user_package\\log_txt\\max_power.out",ios::app);
			//	fout<<"round"<<info.round<<"\ndddddddddddddddd\n"<<max_power<<"\nddddddddddddd\n";
			if(ConfrontPower[info.towerInfo[i].strategy][info.towerInfo[target].strategy] 
				> ConfrontPower[info.towerInfo[target].strategy][info.towerInfo[i].strategy]){
					target_resource_arrived -= max_power * round_arrive;
					rate_arrive-=max_power;
			}
			else{
				target_resource_arrived -= max_power / ConfrontPower[info.towerInfo[target].strategy][info.towerInfo[i].strategy] * round_arrive;
				rate_arrive-=max_power / ConfrontPower[info.towerInfo[target].strategy][info.towerInfo[i].strategy];
			}
		}
		else if(info.lineInfo[target][i].state==Attacking){
			target_resource_arrived -= min(BaseFrontSpeed * round_arrive, 
				getDistance(info.towerInfo[target].position,info.towerInfo[i].position) * Density - info.lineInfo[target][i].resource);
			if(BaseFrontSpeed * round_arrive <
				getDistance(info.towerInfo[target].position,info.towerInfo[i].position) * Density - info.lineInfo[target][i].resource){
					rate_arrive -= BaseFrontSpeed;
			}

		}


		else if(info.lineInfo[target][i].state == AfterCut){
			target_resource_arrived += min(BaseFrontSpeed * round_arrive,info.lineInfo[target][i].backResource);
			if(BaseFrontSpeed * round_arrive<info.lineInfo[target][i].backResource){
				rate_arrive += BaseFrontSpeed;
			}

		}
		else if(info.lineInfo[target][i].state == Arrived){
			target_resource_arrived -= base_speed_target * round_arrive;
			rate_arrive-=base_speed_target;
		}
	}
	// target is the target
	for(int i=0;i<info.towerNum;i++){
		if(!info.lineInfo[i][target].exist || info.lineInfo[target][i].state == Extending) continue;

		double base_speed_i=(info.towerInfo[i].type * 0.5 + 1)* LineDecay[info.towerInfo[i].currLineNum];
		double base_speed_target=(info.towerInfo[target].type * 0.5 + 1)* LineDecay[info.towerInfo[target].currLineNum];

		if(info.lineInfo[i][target].state == Arrived){
			// enemy attacking
			double base_speed_i=(info.towerInfo[i].type * 0.5 + 1)* LineDecay[info.towerInfo[i].currLineNum];
			if(find(my_towers.begin(),my_towers.end(),i)==my_towers.end()){
				if(info.towerInfo[i].owner>=0 && info.towerInfo[i].owner<=3){
					target_resource_arrived -= SupressPower[info.towerInfo[i].strategy][info.towerInfo[target].strategy] * base_speed_i
						* DefenceStage[info.playerInfo[info.towerInfo[i].owner].DefenceLevel] / 1.5 * round_arrive;		
					rate_arrive -= SupressPower[info.towerInfo[i].strategy][info.towerInfo[target].strategy] * base_speed_i
						* DefenceStage[info.playerInfo[info.towerInfo[i].owner].DefenceLevel] / 1.5;
				}
				else{
					target_resource_arrived -= SupressPower[info.towerInfo[i].strategy][info.towerInfo[target].strategy] * base_speed_i * round_arrive;
					rate_arrive-=SupressPower[info.towerInfo[i].strategy][info.towerInfo[target].strategy] * base_speed_i;
				}
			}
			// family reinforcement
			else{
				target_resource_arrived += base_speed_i * round_arrive;
				rate_arrive+=base_speed_i;
			}
		}
		else if(info.lineInfo[i][target].state == Confrontation){
			double max_power = max(ConfrontPower[info.towerInfo[i].strategy][info.towerInfo[target].strategy] * base_speed_i,
				ConfrontPower[info.towerInfo[target].strategy][info.towerInfo[i].strategy] * base_speed_target);
		//		ofstream fout("E:\\Year 1\\spring\\freshmen_c\\user_package\\log_txt\\max_power.out",ios::app);
		//		fout<<"round"<<info.round<<"\ndddddddddddddddd\n"<<max_power<<"\nddddddddddddd\n";
			if(ConfrontPower[info.towerInfo[i].strategy][info.towerInfo[target].strategy] 
				> ConfrontPower[info.towerInfo[target].strategy][info.towerInfo[i].strategy]){
					target_resource_arrived -= max_power * round_arrive;
					rate_arrive-=max_power;
			}
			else{
				target_resource_arrived -= max_power / ConfrontPower[info.towerInfo[target].strategy][info.towerInfo[i].strategy] * round_arrive;
				rate_arrive -= max_power / ConfrontPower[info.towerInfo[target].strategy][info.towerInfo[i].strategy];
			}
		}
		else if(info.lineInfo[i][target].state == AfterCut){
			target_resource_arrived -= min(BaseFrontSpeed * round_arrive,info.lineInfo[i][target].frontResource);
			if(BaseFronjtSpeed * round_arrive<info.lineInfo[i][target].frontResource){
				rate_arrive -= BaseFrontSpeed;
			}
		}
	}
	double base_speed_target=(info.towerInfo[target].type * 0.5 + 1)* LineDecay[info.towerInfo[target].currLineNum];

	double rate_harm=0;
	if(info.towerInfo[target].owner>=0 && info.towerInfo[target].owner<=3){
		rate_harm = SupressPower[info.towerInfo[source].strategy][info.towerInfo[target].strategy] * base_speed_target
			* DefenceStage[info.playerInfo[info.towerInfo[target].owner].DefenceLevel] / 1.5;
	}
	else{
		rate_harm = SupressPower[info.towerInfo[source].strategy][info.towerInfo[target].strategy] * base_speed_target;
	}

	if(rate_arrive + rate_harm){
		return -target_resource_arrived / (rate_arrive+rate_harm);
	}
	else{
		return -1;
	}

}
*/


void cmd(Info& info,int op){

	if(info.round<40 && is_neutral_tower(info,info.towerNum-1) && info.towerInfo[info.towerNum-1].resource > 40){
		info.towerInfo[info.towerNum-1].resource=-25;
	}

// create my towers (tested)
	vector<int> my_towers,others_towers;
/*	for(set<int>::iterator it=info.playerInfo[info.myID].towers.begin();it!=info.playerInfo[info.myID].towers.end();it++){
		my_towers.push_back(*it);
	}
	int my_towers_sz=my_towers.size();*/

	for(int i=0;i<info.towerNum;i++){
		if(info.towerInfo[i].owner == info.myID){
			my_towers.push_back(i);
		}
		else{
			others_towers.push_back(i);
		}
	}

	if(my_towers.size()==1){
		complete_defence(info,my_towers[0]);
		return ;
	}


// create others' towers (tested)

/*	for(int i=0;i<info.towerNum;i++){
		if(find(my_towers.begin(),my_towers.end(),i)==my_towers.end()){
			others_towers.push_back(i);
		}		
	}*/
//	bubble_tower_asc(info,others_towers,0,others_towers.size());
//create in_num, out_num

	vector<vector<int> > this_is_under_atk_of(info.towerNum);		// under_atk_of[2]=(3,4,5) means tower 3,4,5 is attacking tower 2
	vector<vector<int> > this_is_atking(info.towerNum);				// atking[2]=(3,4,5) means tower 2 is attacking tower 3,4,5
	vector<vector<int> > this_is_under_reinf_of(info.towerNum);
	vector<vector<int> > this_is_reinfing(info.towerNum);
	for(int i=0;i<info.towerNum;i++){		//i is source
		for(int j=0;j<info.towerNum;j++){   //j is target
			if(info.lineInfo[i][j].exist && info.lineInfo[i][j].state != AfterCut){
				if(is_formal_atk_line(info,i,j)){
					this_is_under_atk_of[j].push_back(i);
					this_is_atking[i].push_back(j);
				}
				else{
					this_is_reinfing[i].push_back(j);
					this_is_under_reinf_of[j].push_back(i);
				}
			}
		}
	}	

	for(int i=0;i<4;i++){
		//UBW
		if(info.round == 5 && info.playerInfo[i].RegenerationSpeedLevel == 1){
			UBW_flag1[i] = true;
		}	
		if(info.round == 11 && info.towerInfo[i*3].strategy == Grow){
			UBW_flag2[i] = true;
		}
		if(info.round == 16 && (info.towerInfo[i*3+1].strategy == Grow || info.towerInfo[i*3+2].strategy == Grow)){
			UBW_flag3[i] = true;
		}
		if(info.round == 21 && info.towerInfo[i*3+1].strategy == Grow && info.towerInfo[i*3+2].strategy == Grow){
			UBW_flag4[i] = true;
		}
		if(info.round == 25 && info.playerInfo[i].RegenerationSpeedLevel == 2){
			UBW_flag5[i] = true;
		}
	/*	if(info.round == 37 && info.playerInfo[i].RegenerationSpeedLevel == 4){

		}*/
		if(info.round == 44 && info.playerInfo[i].RegenerationSpeedLevel == 5){
			UBW_flag6[i] = true;
		}
	/*	if(info.playerInfo[i].alive && info.playerInfo[i].towers.size() == 1){
			set<int>::iterator it = info.playerInfo[i].towers.begin();
			if(info.towerInfo[*it].strategy == Defence){
				UBW_flag6[i] = true;
			}
		}*/

		//BLAZZERS / SSFD
		if(info.round == 7 && info.towerInfo[i*3].strategy == Grow){
			SSFD_flag1[i] = true;
		}
		if(info.round == 13 && (info.towerInfo[i*3+1].strategy == Grow || info.towerInfo[i*3+2].strategy == Grow)){
			SSFD_flag2[i] = true;
		}
		if(info.round == 18 && info.towerInfo[i*3+1].strategy == Grow && info.towerInfo[i*3+2].strategy == Grow){
			SSFD_flag3[i] = true;
		}
		if(info.round == 25 && info.playerInfo[i].RegenerationSpeedLevel == 1){
			SSFD_flag4[i] = true;
		}
		// ZUC
		if(info.round == 7 && (info.towerInfo[i*3+1].strategy == Grow || info.towerInfo[i*3+2].strategy == Grow)){
			ZUC_flag1[i] = true;
		}
		if(info.round == 13 && info.towerInfo[i*3].strategy == Grow){
			ZUC_flag2[i] = true;
		}
		if(info.round == 18 && info.towerInfo[i*3].strategy == Grow && info.towerInfo[i*3+1].strategy == Grow && info.towerInfo[i*3+2].strategy == Grow){
			ZUC_flag3[i] = true;
		}
		if(info.round == 23 && info.playerInfo[i].RegenerationSpeedLevel == 1){
			ZUC_flag4[i] = true;
		}		
		if(info.round == 39 && info.playerInfo[i].RegenerationSpeedLevel == 4){
			ZUC_flag5[i] = true;
		}
		if(info.round == 41 && info.playerInfo[i].DefenceLevel == 1){
			ZUC_flag6[i] = true;
		}	
	}

// create flag
	if(is_clearing_up(info,my_towers)){
		for(int i=0;i<4;i++){
			if(i != info.myID && UBW_flag1[i] && UBW_flag2[i] && UBW_flag3[i] && UBW_flag4[i] && UBW_flag5[i] && UBW_flag6[i]){
				UBW = i;
			//	fout<<"UBW is player "<<UBW<<endl;
			}
		}
		for(int i=0;i<4;i++){
			if(i != info.myID && SSFD_flag1[i] && SSFD_flag2[i] && SSFD_flag3[i] && SSFD_flag4[i]){
				SSFD = i;
			//	fout<<"SSFD is player "<<SSFD<<endl;
			}
		}
		for(int i=0;i<4;i++){
			if(i != info.myID && ZUC_flag1[i] && ZUC_flag2[i] && ZUC_flag3[i] && ZUC_flag4[i] && ZUC_flag5[i] && ZUC_flag6[i]){
				ZUC = i;
	//			fout<<"ZUC is player "<<ZUC<<endl;
			}
		}
	}

	vector<int> player_eliminate_first;
	if(ZUC != -1 && info.playerInfo[ZUC].alive){
		player_eliminate_first.push_back(ZUC);
	}
	if(SSFD != -1 && info.playerInfo[SSFD].alive){
		player_eliminate_first.push_back(SSFD);
	}
	if(UBW != -1 && info.playerInfo[UBW].alive){
		player_eliminate_first.push_back(UBW);
	}




//create my towers attacking resource.
	//add code


	
//test print
/*	fout<<"my towers:";
	print_vec_int(fout,my_towers);
	fout<<"others towers:";
	print_vec_int(fout,others_towers);*/

//test print
/*	fout<<"ddddddddddddddddd\n\n";
	print_numeric_vec(fout,this_is_under_atk_of[7]);
	fout<<"ddddddddddddddddd\n\n";*/

	//----------------------------------------------estimate next round tower resource-------------------------------------

/*	if(op==info.myMaxControl-1){
		ofstream fout_resource("E:\\Year 1\\spring\\freshmen_c\\user_package\\log_txt\\resource.out",ios::app);

		vector<double> est_resource(info.towerNum,0);
		estimate_resource(info,est_resource,my_towers);
		fout_resource<<"[";
		for(int i=0;i<info.towerNum;i++){
			fout_resource<<info.towerInfo[i].resource<<' ';
		}
		fout_resource<<"]\n";

		fout_resource<<"round:"<<info.round+1<<endl;
		print_numeric_vec(fout_resource,est_resource);
	}*/
	////////////est_resource
/*	fout<<'[';
	for(int i=0;i<info.towerNum;i++){
		fout<<est_resource(info,i)<<' ';
	}
	fout<<"]\n";*/
	
	
//--------------------------------ops-------------------------------

	sort_my_towers_under_atk_first(info,my_towers,0,my_towers.size());
//G->A
	if(info.playerInfo[info.myID].technologyPoint >= StrategyChangeCost[Grow][Attack]){
		for(int i=0;i<my_towers.size();i++){
			if(is_able_G2A(info,my_towers[i])){

				changestrategy(info,my_towers[i],Attack);
				return;
			}
		}
	}
//G->D
	if(info.playerInfo[info.myID].technologyPoint >= StrategyChangeCost[Grow][Defence]){
		for(int i=0;i<my_towers.size();i++){
			if(is_able_G2D(info,my_towers[i])){

				changestrategy(info,my_towers[i],Defence);
				return;
			}
		}
	}

	bool tp_flag = 1;
	if(cnt_G_change(info,my_towers) && info.playerInfo[info.myID].technologyPoint < 
		(cnt_G_change(info,my_towers) + 1) * ADVANCED_STRATEGY_SWITCH_COST){
		tp_flag=0;
	}
	// if tower growing or normal is under attack, other tp usage would be banned
	if(tp_flag){
	//N->G		
		if(info.playerInfo[info.myID].technologyPoint >= StrategyChangeCost[Normal][Grow]){
			for(int i=0;i<my_towers.size();i++){
				if(is_able_N2G(info,my_towers,i)){

					changestrategy(info,my_towers[i],Grow);
					return;
				}
			}
		}
	//D->G
		if(info.playerInfo[info.myID].technologyPoint >= StrategyChangeCost[Defence][Grow]){
			for(int i=0;i<my_towers.size();i++){
				if(is_able_D2G(info,my_towers,i)){

					changestrategy(info,my_towers[i],Grow);
					return;
				}
			}
		}
	//A->G
		if(info.playerInfo[info.myID].technologyPoint >= StrategyChangeCost[Attack][Grow]){
			for(int i=0;i<my_towers.size();i++){
				if(is_able_A2G(info,my_towers,i)){

					changestrategy(info,my_towers[i],Grow);
					return;
				}
			}
		}

	//N->D
		if(info.playerInfo[info.myID].technologyPoint >= StrategyChangeCost[Normal][Defence]){
			for(int i=0;i<my_towers.size();i++){
				if(is_able_N2D(info,my_towers[i])){

					changestrategy(info,my_towers[i],Defence);
					return;
				}
			}
		}
	//N->A
		if(info.playerInfo[info.myID].technologyPoint >= StrategyChangeCost[Normal][Attack]){
			for(int i=0;i<my_towers.size();i++){
				if(is_able_N2A(info,my_towers,i)){

					changestrategy(info,my_towers[i],Attack);
					return;				
				}
			}
		}
	//A->D
		if(info.playerInfo[info.myID].technologyPoint >= StrategyChangeCost[Attack][Defence]){
			for(int i=0;i<my_towers.size();i++){
				if(is_able_A2D(info,my_towers[i])){

					changestrategy(info,my_towers[i],Defence);
					return;
				}
			}
		}
	//D->A
		if(info.playerInfo[info.myID].technologyPoint >= StrategyChangeCost[Defence][Attack]){
			for(int i=0;i<my_towers.size();i++){
				if(is_able_D2A(info,my_towers[i])){

					changestrategy(info,my_towers[i],Attack);
					return;
				}
			}
		}

	//regeneration upgrade
		if(info.round >= 20 && info.playerInfo[info.myID].technologyPoint >= info.playerInfo[info.myID].RegenerationSpeedLevel*2+2 
			&& info.playerInfo[info.myID].RegenerationSpeedLevel < MAX_REGENERATION_SPEED_LEVEL){
			info.myCommandList.addCommand(upgrade,RegenerationSpeed);

			//modify info
			info.playerInfo[info.myID].technologyPoint -= info.playerInfo[info.myID].RegenerationSpeedLevel*2+2;
			info.playerInfo[info.myID].RegenerationSpeedLevel++;

		//	fout<<"command: upgrade RegenerationSpeed\n";

			return ;
		}
	// wall upgrade
		if(info.playerInfo[info.myID].RegenerationSpeedLevel == MAX_REGENERATION_SPEED_LEVEL &&
			info.playerInfo[info.myID].technologyPoint >= info.playerInfo[info.myID].DefenceLevel*2+3 && info.playerInfo[info.myID].DefenceLevel < MAX_DEFENCE_LEVEL){
			info.myCommandList.addCommand(upgrade,Wall);

			//modify info
			info.playerInfo[info.myID].technologyPoint -= info.playerInfo[info.myID].DefenceLevel*2+3;
			info.playerInfo[info.myID].DefenceLevel++;

		//	fout<<"command: upgrade DefenceLevel\n";
			return;
		}
	// extendingspeed upgrade
		if(info.playerInfo[info.myID].RegenerationSpeedLevel == MAX_REGENERATION_SPEED_LEVEL && info.playerInfo[info.myID].DefenceLevel == MAX_DEFENCE_LEVEL &&
			info.playerInfo[info.myID].technologyPoint >= info.playerInfo[info.myID].ExtendingSpeedLevel*2+2+5 && info.playerInfo[info.myID].ExtendingSpeedLevel < MAX_EXTENDING_SPEED_LEVEL){
			info.myCommandList.addCommand(upgrade,ExtendingSpeed);

			//modify info
			info.playerInfo[info.myID].technologyPoint -= info.playerInfo[info.myID].ExtendingSpeedLevel*2+2;
			info.playerInfo[info.myID].ExtendingSpeedLevel++;

		//fout<<"command: upgrade ExtendingSpeedLevel\n";
			return ;
		}
	}
	// extra control upgrade
		if(info.playerInfo[info.myID].RegenerationSpeedLevel == MAX_REGENERATION_SPEED_LEVEL && info.playerInfo[info.myID].DefenceLevel == MAX_DEFENCE_LEVEL &&
			info.playerInfo[info.myID].ExtendingSpeedLevel == MAX_EXTENDING_SPEED_LEVEL &&
			info.playerInfo[info.myID].technologyPoint >= info.playerInfo[info.myID].ExtraControlLevel*2+3+5 && info.playerInfo[info.myID].ExtraControlLevel < MAX_EXTRA_CONTROL_LEVEL){
			info.myCommandList.addCommand(upgrade,ExtraControl);

			//modify info
			info.playerInfo[info.myID].technologyPoint -= info.playerInfo[info.myID].ExtraControlLevel*2+3;
			info.playerInfo[info.myID].ExtraControlLevel++;

		//	fout<<"command: upgrade ExtraControlLevel\n";
			return ;
		}	
//cut line
	//cut to accelerate attack	
		sort_towers_des(info,my_towers,0,my_towers.size());
		sort_towers_asc(info,others_towers,0,others_towers.size());
	for(int i=0;i<my_towers.size();i++){
		for(int j=0;j<others_towers.size();j++){
			if(!info.lineInfo[my_towers[i]][others_towers[j]].exist || info.lineInfo[my_towers[i]][others_towers[j]].state != Arrived ||
				is_neutral_tower(info,others_towers[j]) && is_third_party_arrived(info,others_towers[j]) && info.towerInfo[others_towers[j]].occupyOwner!=info.myID){
				continue;
			}
			// clearing up
			if(is_clearing_up(info,my_towers) && player_eliminate_first.size() && info.towerInfo[others_towers[j]].owner != player_eliminate_first[0]){
				continue;
			}

			if( family_atk_line_resource(info,others_towers[j]) > (info.towerInfo[others_towers[j]].resource + calc_3p_reinf_n_atk_resource(info,others_towers[j]))*1.25
				&& info.towerInfo[my_towers[i]].resource>ADVANCED_RESOURCE && est_resource(info,my_towers[i])+info.towerInfo[others_towers[j]].resource >= 0 ){
				
				cutline(info,my_towers[i],others_towers[j],0);
				return;
			}
		}
	}

	//cut attacking line to protect myself
	for(int i=0;i<my_towers.size();i++){
		//testing clearing up
//		if(is_clearing_up(info,my_towers)) break;
		for(int j=0;j<others_towers.size();j++){

			if(!info.lineInfo[my_towers[i]][others_towers[j]].exist || info.lineInfo[my_towers[i]][others_towers[j]].state==AfterCut){
				continue;
			}

			if(is_clearing_up(info,my_towers)){
				if(player_eliminate_first.size() == 0){
					break;
				}
				else if(info.towerInfo[others_towers[j]].owner == player_eliminate_first[0]){
					continue;
				}
				else{
					cutline(info,my_towers[i],others_towers[j],info.lineInfo[my_towers[i]][others_towers[j]].resource);
					return ;
				}
				continue;
			}

			if(is_able_cut_atk_line(info,my_towers,i,others_towers,j)){

				cutline(info,my_towers[i],others_towers[j],info.lineInfo[my_towers[i]][others_towers[j]].resource);
				return;
			}
		}
	}
	// cut reinforcement to protect myself
	sort_towers_asc(info,my_towers,0,my_towers.size());
	for(int i=0;i<my_towers.size();i++){
		for(int j=0;j<my_towers.size();j++){
			if(i==j || !info.lineInfo[my_towers[i]][my_towers[j]].exist || info.lineInfo[my_towers[i]][my_towers[j]].state==AfterCut) continue;

			if((info.towerInfo[my_towers[j]].resource >= info.towerInfo[my_towers[j]].maxResource 
				|| info.towerInfo[my_towers[j]].resource >= STOP_REINF_LEAST_RESOURCE )
				&& est_resource(info,my_towers[j]) > info.towerInfo[my_towers[i]].resource + ADVANCED_RESOURCE
				|| info.towerInfo[my_towers[i]].resource < BASIC_RESOURCE
				|| cnt_enemy_arrive_soon(info,my_towers[i]) + get_cur_valid_line_num(info,my_towers[i]) > info.towerInfo[my_towers[i]].maxLineNum){
					//|| is_enemy_arrived(info,my_towers[i]) && est_resource(info,my_towers[i]) < 0 ){

				cutline(info,my_towers[i],my_towers[j],info.lineInfo[my_towers[i]][my_towers[j]].resource);
				return;
			}
		}
	}
	// cut reinforcement to enrich target's resource
	sort_towers_des(info,my_towers,0,my_towers.size());
	for(int i=0;i<my_towers.size();i++){
		if(info.towerInfo[my_towers[i]].resource <= ADVANCED_RESOURCE || est_resource(info,my_towers[i]) < 0){
			continue;
		}
		for(int j=my_towers.size()-1;j>=0;j--){
			if(i==j || !info.lineInfo[my_towers[i]][my_towers[j]].exist || info.lineInfo[my_towers[i]][my_towers[j]].state!=Arrived 
				|| info.towerInfo[my_towers[j]].resource >= MIN_CUT_REINF_RESOURCE)
				continue;

	/*		if(info.towerInfo[my_towers[i]].resource >= 120){
				cutline(info,my_towers[i],my_towers[j],0);
				return;
			}*/

			if(est_resource_aftercut_reinf(info,my_towers[j]) >= 0 || my_towers[j] != 12 &&
				family_reinf_line_resource(info,my_towers[j]) + info.towerInfo[my_towers[j]].resource >= MIN_CUT_REINF_RESOURCE){

				cutline(info,my_towers[i],my_towers[j],0);
				return;
			}
		}
	}

//add line
	//defence
	for(int i=0;i<my_towers.size();i++){
		if(info.towerInfo[my_towers[i]].resource <= ADVANCED_RESOURCE || info.towerInfo[my_towers[i]].currLineNum >= info.towerInfo[my_towers[i]].maxLineNum){
			continue;
		}

		sort_this_is_under_atk_of(info,my_towers[i],this_is_under_atk_of[my_towers[i]],0,this_is_under_atk_of[my_towers[i]].size());

		for(int j=0;j<this_is_under_atk_of[my_towers[i]].size();j++){

/*			int target = this_is_under_atk_of[my_towers[i]][j];
			if(is_clearing_up(info,my_towers) && player_eliminate_first.size() && info.towerInfo[target].owner != player_eliminate_first[0]){
				continue;
			}*/

			if(info.lineInfo[my_towers[i]][this_is_under_atk_of[my_towers[i]][j]].exist){
				continue;
			}
			if(info.towerInfo[my_towers[i]].resource < line_max_resource(info,my_towers[i],others_towers[j]) / 2 + BASIC_RESOURCE){
				continue;
			}
			if(!is_line_arrive_soon(info,this_is_under_atk_of[my_towers[i]][j],my_towers[i])){
				continue;
			}

			if(	info.towerInfo[my_towers[i]].strategy==Attack && info.towerInfo[this_is_under_atk_of[my_towers[i]][j]].strategy!=Defence 
				|| info.towerInfo[my_towers[i]].strategy==Defence && info.towerInfo[this_is_under_atk_of[my_towers[i]][j]].strategy==Attack
				|| info.towerInfo[my_towers[i]].strategy==Grow && info.towerInfo[this_is_under_atk_of[my_towers[i]][j]].strategy!=Attack){
	/*			ConfrontPower[this_is_under_atk_of[my_towers[i]][j]][my_towers[i]] < SupressPower[this_is_under_atk_of[my_towers[i]][j]][my_towers[i]]*/

				addline(info,my_towers[i],this_is_under_atk_of[my_towers[i]][j]);
				return ;
			}
		}
	}
	// reinforce	
	sort_towers_des(info,my_towers,0,my_towers.size());
	for(int i=0;i<my_towers.size();i++){

		if(info.towerInfo[my_towers[i]].resource <= ADVANCED_RESOURCE || info.towerInfo[my_towers[i]].currLineNum >= info.towerInfo[my_towers[i]].maxLineNum
			|| est_resource(info,my_towers[i]) < 0){
			continue;
		}

		for(int j=my_towers.size()-1;j>=0;j--){

			if(i==j || info.lineInfo[my_towers[i]][my_towers[j]].exist || est_resource(info,my_towers[j]) >= 0 || !is_under_formal_attack(info,my_towers[j])
				|| info.towerInfo[my_towers[j]].resource > CALL_REINF_WHEN_BELOW 
				|| info.towerInfo[my_towers[i]].resource <= line_max_resource(info,my_towers[i],my_towers[j])+ADVANCED_RESOURCE){ 


				continue;
			}

		//	if(est_resource(info,my_towers[i]) - line_max_resource(info,my_towers[i],my_towers[j]) + est_resource(info,my_towers[j]) >= 10){
			if(est_resource_after_reinf(info,my_towers,my_towers[j]) >= MIN_EST_REINF_RESOURCE){

				addline(info,my_towers[i],my_towers[j]);
				return ;
			}
		}
	}
	sort_towers_des(info,my_towers,0,my_towers.size());
	//attack
	for(int i=0;i<my_towers.size();i++){

		if(!is_attack_started(info,my_towers,others_towers)) break;

		if(info.towerInfo[my_towers[i]].resource <= ADVANCED_RESOURCE 
			|| info.towerInfo[my_towers[i]].currLineNum >= info.towerInfo[my_towers[i]].maxLineNum){
			continue;
		}

		sort_others_towers_asc(info,my_towers[i],others_towers,0,others_towers.size());

	//	print_numeric_vec(fout,others_towers);

		for(int j=0;j<others_towers.size();j++){
			//there is already attacking line
			if(info.lineInfo[my_towers[i]][others_towers[j]].exist 
				|| info.lineInfo[others_towers[j]][my_towers[i]].exist && info.lineInfo[others_towers[j]][my_towers[i]].state!=AfterCut){
					//|| info.towerInfo[others_towers[j]].strategy == Defence){
				continue;
			}
			//testing clearing up
		/*	if( is_clearing_up(info,my_towers) ){
				addline(info,my_towers[i],others_towers[j]);
				return ;
			}*/
			if(is_clearing_up(info,my_towers)){
				if(player_eliminate_first.size()){
					if(info.towerInfo[others_towers[j]].owner == player_eliminate_first[0]){
						addline(info,my_towers[i],others_towers[j]);
						return ;
					}
				}
				else{
					addline(info,my_towers[i],others_towers[j]);
					return ;
				}
			}


			if(info.towerInfo[my_towers[i]].strategy == Attack && est_resource(info,my_towers[i]) - line_max_resource(info,my_towers[i],others_towers[j]) 
		> (info.towerInfo[others_towers[j]].resource + calc_3p_reinf_n_atk_resource(info,others_towers[j])) 
		* ATK_RATE * ConfrontPower[info.towerInfo[others_towers[j]].strategy][info.towerInfo[my_towers[i]].strategy] /
			ConfrontPower[info.towerInfo[my_towers[i]].strategy][info.towerInfo[others_towers[j]].strategy] / (family_line_num(info,info.myID,others_towers[j])+1)
			&& info.towerInfo[my_towers[i]].resource > line_max_resource(info,my_towers[i],others_towers[j]) + BASIC_RESOURCE){

				addline(info,my_towers[i],others_towers[j]);
				return ;
			}
			if(info.towerInfo[my_towers[i]].strategy == Grow && est_resource(info,my_towers[i]) - line_max_resource(info,my_towers[i],others_towers[j])
			> info.towerInfo[others_towers[j]].resource * ATK_RATE / (family_line_num(info,info.myID,others_towers[j])+1)
			 && info.towerInfo[my_towers[i]].resource > line_max_resource(info,my_towers[i],others_towers[j]) + BASIC_RESOURCE){

				addline(info,my_towers[i],others_towers[j]);
				return ;
			}
		}
	}


/*	for(int i=0;i<my_towers.size();i++){
		int target = my_towers[i];

		if(!is_under_formal_attack(info,target) || info.towerInfo[target].resource > 100 || est_resource(info,target) >= 0){
			continue;
		}

		for(int j=my_towers.size()-1;j>=0;j--){

			int source = my_towers[j];

			if(source == target || info.towerInfo[source].resource <= ADVANCED_RESOURCE || 
				info.towerInfo[source].currLineNum >= info.towerInfo[source].maxLineNum || est_resource(info,source) < 0 ||
			info.lineInfo[source][target].exist || info.towerInfo[source].resource <= line_max_resource(info,source,target)+ADVANCED_RESOURCE){
				continue;
			}
			if(est_resource_after_reinf(info,my_towers,target) >= 0){

				addline(info,source,target);
				return ;
			}
		}
	}*/


}

/*
void correct_type(Info& info){
	for(int i=0;i<info.towerNum;i++){
		if(info.towerInfo[i].resource < 10){
			info.towerInfo[i].type=Alpha;
		}
		else if(info.towerInfo[i].resource < 40){
			info.towerInfo[i].type=Beta_1;
		}
		else if(info.towerInfo[i].resource < 80){
			info.towerInfo[i].type=Beta_2;
		}
		else if(info.towerInfo[i].resource < 150){
			info.towerInfo[i].type=Gamma_1;
		}
		else if(info.towerInfo[i].resource <= 200){
			info.towerInfo[i].type=Gamma_2;
		}
	}
}*/

void player_ai(Info& info)
{
	while(info.myCommandList.begin()!=info.myCommandList.end()){
		info.myCommandList.removeCommand(0);
	}
	
	//new code
//	correct_type(info);
/*	ofstream fout("E:\\Year 1\\spring\\freshmen_c\\user_package\\log_txt\\data333.out",ios::app);
	if(!fout.is_open()){
		cout<<"bad open\n";
	}*/
	
//new code
//	print_info(fout,info);

	for (int i = 0; i != info.myMaxControl; ++i)
	{
		cmd(info,i);
	}
}
