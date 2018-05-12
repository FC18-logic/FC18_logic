#include "ai.h"
#include "definition.h"
#include "user_toolbox.h"
#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <algorithm>
using namespace std;

//权重类
#define weight_distance 0.2
#define weight_attline 20
#define weight_conline 3
#define weight_enemy 4
#define weight_helper 3
#define weight_being_arrived_cal 0.6
#define weight_stop_att 3
#define weight_for_group_att 1.6
#define weight_bias 10

//数值类
#define better_to_defence 2.5
#define better_to_attack 2
#define better_to_grow 0.5
#define num_to_attack 50
#define num_to_help 40
#define num_need_help 30
#define num_cut_to_help 23

//回合数类
#define round_to_start_attack 48
#define round_to_start_help 15
#define round_for_extra_tech 0
#define round_threat 20
#define round_to_confront 6
#define round_need_help 10
#define round_to_help 18
#define round_to_stop_help 12
#define round_to_attack 26
#define round_to_group_att 22
#define round_to_cut_to_att 2
#define round_to_att_save 21
#define round_to_cut_to_help 6
#define round_to_back_support 16
#define round_to_stop_attack 15
#define round_to_stop_confront 3
#define round_not_att_denfence 10
#define round_between_str_change 4

TPoint mycore,encore;
int *go_att,*be_att,*go_help,*be_help,*go_con,*be_con,*go_cut,*be_cut,*my_tower,*en_tower,*life,*already,**relation,*group_can_att,*group_can_cut,*tower_change;
int used_tech;
int flag_player[4],flag_tower[13],last_str_change[13];

double max(double a,double b)
{
	if(a>b) return a;
	return b;
}

double regeneration_v(Info& info,int tower)
{
	return info.playerInfo[info.towerInfo[tower].owner].RegenerationSpeedLevel*0.05+1;
}

double extending_v(Info& info,int tower)
{
	return 3*(info.playerInfo[info.towerInfo[tower].owner].ExtendingSpeedLevel*0.1+1);
}

double proportion(Info& info,int i,int j)
{
	return (double)info.lineInfo[i][j].resource*10/(double)getDistance(info.towerInfo[i].position,info.towerInfo[j].position);
}

double remained_time(Info& info,int i,int j)
{
	return ((double)getDistance(info.towerInfo[i].position,info.towerInfo[j].position)/10.0-(double)info.lineInfo[i][j].resource)/extending_v(info,i);
}

bool valid_line(Info& info,int i,int j)
{
	if(info.lineInfo[i][j].exist&&info.lineInfo[i][j].state!=Backing&&info.lineInfo[i][j].state!=AfterCut
		&&remained_time(info,i,j)<round_threat) return true;
	return false;
}

bool valid_line_for_confront(Info& info,int i,int j)
{
	if(info.lineInfo[i][j].exist&&info.lineInfo[i][j].state!=Backing&&info.lineInfo[i][j].state!=AfterCut)
	{
		if(info.round<30)
		{
			if(remained_time(info,i,j)<=1||proportion(info,i,j)>0.8) return true;
			return false;
		}
		if(remained_time(info,i,j)<=round_to_confront||proportion(info,i,j)>0.8) return true;
	}
	return false;
}

double wall_degree(Info& info,int tower)
{
	return double(15-info.playerInfo[info.towerInfo[tower].owner].DefenceLevel)/15.0;
}

double basic_recover_v(Info& info,int tower)
{
	double n=info.towerInfo[tower].resource,rate=regeneration_v(info,tower);
	if(n<10) return 1.0*rate;
	if(10<=n&&n<40) return 1.5*rate;
	if(40<=n&&n<80) return 2.0*rate;
	if(80<=n&&n<150) return 2.5*rate;
	return 3.0*rate;
}

double real_recover_v(Info& info,int tower)
{
	if(info.towerInfo[tower].strategy==Grow) return basic_recover_v(info,tower)*1.5;
	if(info.towerInfo[tower].strategy==Defence) return basic_recover_v(info,tower)*0.5;
	return 1*basic_recover_v(info,tower);
}

double basic_v(Info& info,int tower)
{
	if(info.towerInfo[tower].currLineNum<=1) return basic_recover_v(info,tower);
	if(info.towerInfo[tower].currLineNum==2) return 0.8*basic_recover_v(info,tower);
	return 0.6*basic_recover_v(info,tower);
}

double tech_v(Info& info,int id)
{
	double v=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner==id) v+=real_recover_v(info,i)/10.0;
	}
	return v;
}

int confrontation_v(Info& info,int my,int en)
{
	if(info.towerInfo[my].strategy==Normal) return 1;
	if(info.towerInfo[my].strategy==Attack)
	{
		if(info.towerInfo[en].strategy==Normal) return 2;
		else if(info.towerInfo[en].strategy==Grow) return 4;
		return 1;
	}
	if(info.towerInfo[my].strategy==Defence)
	{
		if(info.towerInfo[en].strategy==Attack) return 3;
		return 1;
	}
	if(info.towerInfo[en].strategy==Normal) return 2;
	return 1;
}

double arrived_v(Info& info,int my,int en)
{
	if(info.towerInfo[my].strategy==Defence) return 1*wall_degree(info,en);
	return 1.5*confrontation_v(info,my,en)*wall_degree(info,en);
}

double cut_arrived_v(Info& info,int i,int j)
{
	return (arrived_v(info,i,j)-1)/2.0+1;
}

double confrontation_cost(Info& info,int my,int en)//my的损失
{
	return max(confrontation_v(info,en,my)*extending_v(info,en),confrontation_v(info,my,en)*extending_v(info,my))/confrontation_v(info,my,en);
}

double arrrived_cost(Info& info,int my)
{
	return basic_v(info,my);
}

double help_cost(Info& info,int tower)
{
	double cost=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner==info.myID&&info.lineInfo[tower][i].exist)
		{
			if(info.lineInfo[tower][i].state==Arrived) cost+=basic_v(info,tower);
			else if(info.lineInfo[tower][i].state==Extending) cost+=extending_v(info,tower);
		}
	}
	return cost;
}

double attack_cost(Info& info,int tower)
{
	double cost=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner!=info.myID&&info.lineInfo[tower][i].exist)
		{
			if(info.lineInfo[tower][i].state==Confrontation) cost+=confrontation_cost(info,tower,i);
			else if(info.lineInfo[tower][i].state==Arrived) cost+=arrrived_cost(info,tower);
			else if(info.lineInfo[tower][i].state==Extending) cost+=extending_v(info,tower);
		}
	}
	return cost;
}

double being_arrived_cost(Info& info,int tower)
{
	double cost=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner!=info.myID&&info.lineInfo[i][tower].exist&&info.lineInfo[i][tower].state==Arrived)
			cost+=arrived_v(info,i,tower)*basic_v(info,i);
	}
	return cost;
}

double being_extending_cost(Info& info,int tower)
{
	double cost=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner!=info.myID&&info.lineInfo[i][tower].exist&&info.lineInfo[i][tower].state==Extending
			&&!info.lineInfo[tower][i].exist&&valid_line_for_confront(info,i,tower)&&remained_time(info,i,tower)>round_to_cut_to_att)
			cost+=arrived_v(info,i,tower)*basic_v(info,i)*proportion(info,i,tower)*proportion(info,i,tower);
	}
	return cost;
}

double help_gain(Info& info,int tower)
{
	double gain=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner==info.myID&&info.lineInfo[i][tower].exist)
		{
			if(info.lineInfo[i][tower].state==Arrived) gain+=basic_v(info,i);
			else if(valid_line_for_confront(info,i,tower)) gain+=basic_v(info,i)*proportion(info,i,tower)*proportion(info,i,tower);
		}
	}
	return gain;
}

double total_cost(Info& info,int tower)
{
	return attack_cost(info,tower)+help_cost(info,tower)+being_arrived_cost(info,tower)+being_extending_cost(info,tower)-help_gain(info,tower)-real_recover_v(info,tower);
}

double reach_time(Info& info,int my,int en)
{
	return (double)getDistance(info.towerInfo[my].position,info.towerInfo[en].position)/10.0/extending_v(info,my);
}

double my_arrived_resource(Info& info,int tower)
{
	double resource=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner==info.myID&&info.lineInfo[i][tower].exist&&remained_time(info,i,tower)<=round_to_cut_to_att)
			resource+=info.lineInfo[i][tower].resource*cut_arrived_v(info,i,tower)*proportion(info,i,tower)*proportion(info,i,tower);
	}
	return resource*weight_being_arrived_cal;
}

double en_arrived_resource(Info& info,int tower)
{
	double resource=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner!=info.myID&&info.towerInfo[i].owner!=info.towerInfo[tower].owner&&info.lineInfo[i][tower].exist
			&&remained_time(info,i,tower)<=round_to_cut_to_att)
			resource+=info.lineInfo[i][tower].resource*cut_arrived_v(info,i,tower)*proportion(info,i,tower)*proportion(info,i,tower);
	}
	return resource*weight_being_arrived_cal;
}

int my_arrived_num(Info& info,int tower)
{
	int num=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.lineInfo[i][tower].exist&&info.towerInfo[i].owner==info.myID&&remained_time(info,i,tower)<round_to_cut_to_att) num++;
	}
	return num;
}

int en_arrived_num(Info& info,int tower)
{
	int num=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.lineInfo[i][tower].exist&&info.towerInfo[i].owner!=info.myID&&info.towerInfo[i].owner!=info.towerInfo[tower].owner&&remained_time(info,i,tower)<round_to_cut_to_att) num++;
	}
	return num;
}

double arrived_help(Info& info,int tower)
{
	double resource=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner==info.towerInfo[tower].owner&&info.lineInfo[i][tower].exist
			&&remained_time(info,i,tower)<=round_to_cut_to_att)
			resource+=info.lineInfo[i][tower].resource*proportion(info,i,tower)*proportion(info,i,tower);
	}
	return resource*weight_being_arrived_cal;
}

TPoint my_core(Info& info)
{
	double resource=0;
	TPoint a;
	a.m_x=0;
	a.m_y=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner==info.myID)
		{
			resource+=info.towerInfo[i].resource;
			a.m_x+=info.towerInfo[i].position.m_x*info.towerInfo[i].resource;
			a.m_y+=info.towerInfo[i].position.m_y*info.towerInfo[i].resource;
		}
	}
	if(resource==0) resource+=0.1;
	a.m_x/=resource;
	a.m_y/=resource;
	return a;
}

TPoint en_core(Info& info)
{
	double resource=0;
	TPoint a;
	a.m_x=0;
	a.m_y=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner!=info.myID)
		{
			resource+=info.towerInfo[i].resource;
			a.m_x+=info.towerInfo[i].position.m_x*info.towerInfo[i].resource;
			a.m_y+=info.towerInfo[i].position.m_y*info.towerInfo[i].resource;
		}
	}
	if(resource==0) resource+=0.1;
	a.m_x/=resource;
	a.m_y/=resource;
	return a;
}

TPoint group_core(Info& info)
{
	double resource=0;
	TPoint a;
	a.m_x=0;
	a.m_y=0;
	int len=0;
	while(group_can_att[len]!=-1) len++;
	for(int i=0;i<len;i++)
	{
		resource+=info.towerInfo[group_can_att[i]].resource;
		a.m_x+=info.towerInfo[group_can_att[i]].position.m_x*info.towerInfo[group_can_att[i]].resource;
		a.m_y+=info.towerInfo[group_can_att[i]].position.m_y*info.towerInfo[group_can_att[i]].resource;
	}
	if(resource==0) resource+=0.1;
	a.m_x/=resource;
	a.m_y/=resource;
	return a;
}

double line_resource(Info &info,int tower)
{
	double resource=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.lineInfo[tower][i].exist)
		{
			if(info.lineInfo[tower][i].state==AfterCut)
				resource+=info.lineInfo[tower][i].backResource;
			else resource+=info.lineInfo[tower][i].resource;
		}
	}
	return resource;
}

double player_resource(Info& info,int id)
{
	double resource=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner==id) resource+=(info.towerInfo[i].resource+line_resource(info,i));
	}
	return resource;
}

double total_resource(Info& info)
{
	return player_resource(info,0)+player_resource(info,1)+player_resource(info,2)+player_resource(info,3);
}

int helper(Info& info,int tower)
{
	int x=0;
	for(int i=0;i<info.towerNum;i++)
		if(info.towerInfo[i].owner==info.towerInfo[tower].owner&&valid_line(info,i,tower)) x++;
	return x;
}

int enemies(Info& info,int tower)
{
	int x=0;
	for(int i=0;i<info.towerNum;i++)
		if(info.towerInfo[i].owner!=info.towerInfo[tower].owner&&(valid_line(info,i,tower)||info.lineInfo[i][tower].state==Confrontation)) x++;
	return x;
}

int both_enemies(Info& info,int tower)
{
	int x=0;
	for(int i=0;i<info.towerNum;i++)
		if(info.towerInfo[i].owner!=info.towerInfo[tower].owner&&info.towerInfo[i].owner!=info.myID&&valid_line(info,i,tower)) x++;
	return x;
}

int att_enemies(Info& info,int tower)
{
	int x=0;
	for(int i=0;i<info.towerNum;i++)
		if(info.towerInfo[i].owner!=info.towerInfo[tower].owner&&(valid_line(info,i,tower)||info.lineInfo[i][tower].state==Confrontation)&&info.towerInfo[i].strategy==Attack) x++;
	return x;
}

int den_enemies(Info& info,int tower)
{
	int x=0;
	for(int i=0;i<info.towerNum;i++)
		if(info.towerInfo[i].owner!=info.towerInfo[tower].owner&&(valid_line(info,i,tower)||info.lineInfo[i][tower].state==Confrontation)&&info.towerInfo[i].strategy==Defence) x++;
	return x;
}

int grow_enemies(Info& info,int tower)
{
	int x=0;
	for(int i=0;i<info.towerNum;i++)
		if(info.towerInfo[i].owner!=info.towerInfo[tower].owner&&(valid_line(info,i,tower)||info.lineInfo[i][tower].state==Confrontation)&&info.towerInfo[i].strategy==Grow) x++;
	return x;
}

int counted_lines(Info& info,int tower)
{
	int num=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.lineInfo[tower][i].exist&&info.lineInfo[tower][i].state!=Backing&&info.lineInfo[tower][i].state!=AfterCut)
			num++;
	}
	return num;
}

int att_lines(Info& info,int tower)
{
	int num=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner!=info.towerInfo[tower].owner&&valid_line(info,tower,i)&&!info.lineInfo[i][tower].exist)
			num++;
	}
	return num;
}

int con_lines(Info& info,int tower)
{
	int num=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner!=info.towerInfo[tower].owner&&info.lineInfo[tower][i].exist&&info.lineInfo[i][tower].exist&&info.lineInfo[i][tower].state!=AfterCut
			&&info.lineInfo[i][tower].state!=Backing&&info.lineInfo[tower][i].state!=AfterCut&&info.lineInfo[tower][i].state!=Backing)
			num++;
	}
	return num;
}

int help_line(Info& info,int tower)
{
	int num=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner==info.towerInfo[tower].owner&&info.lineInfo[tower][i].exist&&info.lineInfo[tower][i].state!=AfterCut&&info.lineInfo[tower][i].state!=Backing)
			num++;
	}
	return num;
}

double my_basic_value(Info& info,int tower)
{
	return info.towerInfo[tower].resource+line_resource(info,tower)*0.8+weight_helper*helper(info,tower)-weight_attline*att_lines(info,tower)
		-weight_conline*con_lines(info,tower)-en_arrived_resource(info,tower)+arrived_help(info,tower)-weight_enemy*enemies(info,tower);
}

double en_basic_value(Info& info,int tower)
{
	return info.towerInfo[tower].resource+line_resource(info,tower)*0.8+weight_helper*helper(info,tower)-weight_attline*att_lines(info,tower)
		-weight_conline*con_lines(info,tower)+en_arrived_resource(info,tower)-my_arrived_resource(info,tower)-weight_enemy*enemies(info,tower)+arrived_help(info,tower);
}

double my_after_change_value(Info& info,int tower,int num)
{
	return my_basic_value(info,tower)+weight_conline*(go_cut[tower]-go_help[tower]-go_con[tower]-num)-weight_attline*go_att[tower]-weight_enemy*be_att[tower]+weight_helper*(be_help[tower]-be_cut[tower]);
}

double en_after_change_value(Info& info,int tower,int num)
{
	return en_basic_value(info,tower)+weight_conline*(go_cut[tower]-go_help[tower]-go_con[tower]-num)-weight_attline*go_att[tower]-weight_enemy*be_att[tower]+weight_helper*(be_help[tower]-be_cut[tower]);
}

double value(Info& info,int tower,TPoint my,int num)
{
	return en_after_change_value(info,tower,num)+getDistance(my,info.towerInfo[tower].position)*weight_distance;
}

void Life(Info& info)
{
	for(int i=0;i<info.towerNum;i++)
	{
		if(total_cost(info,i)>0)
		{
			if(info.towerInfo[i].owner==info.myID) life[i]=my_after_change_value(info,i,0)/(total_cost(info,i)+0.1);
			else life[i]=en_after_change_value(info,i,0)/(total_cost(info,i)+0.1);
		}
		else life[i]=1000-total_cost(info,i);
	}
	return ;
}

int after_change_life(Info& info,int tower,int num)
{
	double cost=total_cost(info,tower)+(num+go_help[tower]-be_help[tower]+be_cut[tower]-go_cut[tower]+go_att[tower]+be_att[tower]+go_con[tower])*basic_v(info,tower);
	
	if(cost<=0) return 1000-cost;

	if(info.towerInfo[tower].owner==info.myID) return my_after_change_value(info,tower,num)/(cost+0.1);
	return en_after_change_value(info,tower,num)/(cost+0.1);
}

int after_confront_life(Info& info,int my,int en,int num)
{
	double cost=total_cost(info,my)+(num+go_help[my]-be_help[my]+be_cut[my]-go_cut[my]+go_att[my]+be_att[my]+go_con[my])*basic_v(info,my);
	double value=my_basic_value(info,my)+weight_conline*(go_cut[my]-go_help[my]-go_con[my]-num)-weight_attline*go_att[my]-weight_enemy*be_att[my]+weight_helper*(be_help[my]-be_cut[my]);
	value+=info.lineInfo[en][my].resource*cut_arrived_v(info,en,my)*weight_being_arrived_cal;
	value-=confrontation_cost(info,my,en);
	if(cost<=0) return 1000-cost;
	return value/(cost+0.1);
}

void cal_relation(Info& info)
{
	for(int i=0;i<4;i++)
	{
		if(!info.playerInfo[i].alive) continue;
		for(int j=0;j<info.towerNum;j++)
		{
			if(info.towerInfo[j].owner!=i) continue;
			for(int k=0;k<info.towerNum;k++)
			{
				if(info.towerInfo[k].owner==i) continue;
				if(info.towerInfo[k].owner==-1) continue;
				if(valid_line(info,j,k)) relation[i][info.towerInfo[k].owner]++;
			}
		}
	}
	return ;
}

int my_bubble(Info& info)
{
	int i,len=0;
	for(i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner==info.myID)
		{
			my_tower[len]=i;
			len++;
		}
	}
	for(i=0;i<len-1;i++)
	{
		for(int j=0;j<len-i-1;j++)
		{
			if(my_basic_value(info,my_tower[j])<my_basic_value(info,my_tower[j+1]))
			{
				int temp;
				temp=my_tower[j];
				my_tower[j]=my_tower[j+1];
				my_tower[j+1]=temp;
			}
		}
	}
	return len;
}

int en_bubble(Info& info)
{
	int i,len=0;
	for(i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner!=info.myID)
		{
			en_tower[len]=i;
			len++;
		}
	}
	for(i=0;i<len-1;i++)
	{
		for(int j=0;j<len-i-1;j++)
		{
			
			if(getDistance(info.towerInfo[en_tower[j]].position,mycore)>getDistance(info.towerInfo[en_tower[j+1]].position,mycore))
			{
				int temp;
				temp=en_tower[j];
				en_tower[j]=en_tower[j+1];
				en_tower[j+1]=temp;
			}
		}
	}
	return len;
}

bool all_confront(Info& info,int tower)
{
	for(int i=0;i<info.towerNum;i++)
		if(valid_line_for_confront(info,i,tower)&&info.towerInfo[i].owner!=info.myID&&remained_time(info,i,tower)<round_to_confront/2)
		{
			if((!info.lineInfo[tower][i].exist||info.lineInfo[tower][i].state==Backing||info.lineInfo[tower][i].state==AfterCut)&&after_change_life(info,tower,1)>round_to_stop_confront)
				return false;
		}
	return true;
}

bool back_support(Info& info,int i,int j)
{
	if(info.round<round_to_start_attack) return false;
	if(info.towerInfo[i].strategy==Defence) return false;
	if(info.towerInfo[i].id==12||info.towerInfo[i].maxResource<80) return false;
	if(info.towerInfo[j].strategy==Grow&&enemies(info,j)<1) return false;
	if(info.towerInfo[j].resource>info.towerInfo[j].maxResource*0.8&&info.towerInfo[j].resource>120) return false;
	if(((flag_tower[i]==1024&&enemies(info,i)<2&&after_change_life(info,i,1)>15)&&info.towerInfo[j].resource<100
		||(info.towerInfo[i].strategy==Grow&&info.towerInfo[i].resource-enemies(info,i)*weight_enemy>=50&&enemies(info,i)<=1))&&helper(info,j)-be_cut[j]<3
		&&getDistance(info.towerInfo[i].position,info.towerInfo[j].position)/10.0<=extending_v(info,i)*round_to_back_support) return true;
	return false;
}

bool open_fire(Info& info,int my_tower_num)
{
	if(my_tower_num>1+info.towerNum/2) return true;
	for(int i=0;i<4;i++)
	{
		if(i==info.myID) continue;
		if(player_resource(info,i)*3>total_resource(info)) return false;
		if(player_resource(info,i)*1.25>player_resource(info,info.myID)) return false;
	}
	if(player_resource(info,info.myID)*2.5>total_resource(info)) return true;
	return false;
}

bool valid_to_raid(Info& info,int tower)
{
	if(counted_lines(info,tower)>=info.towerInfo[tower].maxLineNum&&info.towerInfo[tower].resource<60) return true;
	return false;
}

int being_group_att(Info& info,int tower,int id)
{
	int num=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner==id&&info.lineInfo[i][tower].exist&&info.lineInfo[i][tower].state!=Backing&&info.lineInfo[i][tower].state!=AfterCut)
			num++;
	}
	return num;
}

bool special_case_cut(Info& info)
{
	if(info.round<60&&info.round>50&&info.towerInfo[12].owner==-1&&my_arrived_num(info,12)>=2&&en_arrived_num(info,12)<=3&&both_enemies(info,12)<=5)
		return true;
	return false;
}

bool att_mid(Info& info,int my_tower_num)
{
	if(info.towerInfo[12].resource<35&&helper(info,12)<2) return true;
	if(my_tower_num<5) return false;
	for(int i=0;i<4;i++)
	{
		if(i==info.myID) continue;
		if(player_resource(info,i)*2.5>total_resource(info)) return false;
		if(player_resource(info,i)*1.25>player_resource(info,info.myID)) return false;
	}
	if(player_resource(info,info.myID)*3>total_resource(info)&&my_tower_num>=5) return true;
	return false;
}

bool group_att(Info& info,int tower,int my_tower_num,int r)
{
	if(info.towerInfo[tower].owner!=-1&&flag_player[info.towerInfo[tower].owner]!=1&&!valid_to_raid(info,tower)) return false;
	if(info.towerInfo[tower].id==12&&!att_mid(info,my_tower_num)) return false;
	int max_num=0;
	for(int i=0;i<4;i++)
	{
		if(i!=info.myID&&i!=info.towerInfo[tower].owner)
		{
			if(being_group_att(info,tower,i)>=max_num)
				max_num=being_group_att(info,tower,i);
		}
	}
	double bias=0;
	if(my_tower_num>info.towerNum/2) bias+=weight_bias*(my_tower_num-info.towerNum/2+1);

	for(int j=0;j<info.towerNum;j++) group_can_att[j]=-1;
	int group=0,len=0,total_len=0;
	for(int i=0;i<my_tower_num;i++)
	{
		if(con_lines(info,my_tower[i])+att_lines(info,my_tower[i])>=1&&info.towerInfo[my_tower[i]].resource<50) continue;
		if(con_lines(info,my_tower[i])+att_lines(info,my_tower[i])>=2&&info.towerInfo[my_tower[i]].resource<80) continue;
		if(flag_tower[my_tower[i]]==1024&&!open_fire(info,my_tower_num)&&info.towerInfo[my_tower[i]].strategy==Grow&&info.towerInfo[my_tower[i]].resource<175) continue;
		if(getDistance(info.towerInfo[tower].position,info.towerInfo[my_tower[i]].position)/10<=extending_v(info,my_tower[i])*round_to_group_att)
		{
			if(info.lineInfo[my_tower[i]][tower].exist&&info.lineInfo[my_tower[i]][tower].state!=Backing&&info.lineInfo[my_tower[i]][tower].state!=AfterCut)
			{
				group+=my_after_change_value(info,my_tower[i],0);
				total_len++;
			}
			else if(!info.lineInfo[my_tower[i]][tower].exist&&after_change_life(info,my_tower[i],1)>round_to_attack&&my_after_change_value(info,my_tower[i],1)>num_to_attack
				&&info.towerInfo[my_tower[i]].currLineNum+go_att[my_tower[i]]+go_con[my_tower[i]]+go_help[my_tower[i]]-go_cut[my_tower[i]]<info.towerInfo[my_tower[i]].maxLineNum)
			{
				group+=my_after_change_value(info,my_tower[i],1);
				group_can_att[len]=my_tower[i];
				len++;
				total_len++;
				r++;
				if(info.towerInfo[my_tower[i]].strategy==Attack&&info.towerInfo[tower].strategy==Grow) bias+=2*weight_bias;
				if(info.towerInfo[my_tower[i]].strategy==Attack&&info.towerInfo[tower].strategy==Defence) bias-=info.towerInfo[tower].resource;
				if(info.towerInfo[my_tower[i]].strategy==Grow&&info.towerInfo[tower].strategy==Attack) bias-=2*weight_bias;
				if(info.towerInfo[my_tower[i]].strategy==Grow&&info.towerInfo[tower].strategy==Defence) bias+=2*weight_bias;
				if(info.towerInfo[my_tower[i]].strategy==Defence&&info.towerInfo[tower].strategy==Grow) bias-=2*weight_bias;
				if(info.towerInfo[my_tower[i]].strategy==Defence&&info.towerInfo[tower].strategy==Attack) bias+=2*weight_bias;
				if(r>=info.myMaxControl) break;
			}
		}
	}
	if(len==0) return false;
	if(len==1&&total_len==1&&(valid_to_raid(info,tower)||(info.towerInfo[tower].resource<=30&&both_enemies(info,tower)<=1))) return true;
	if(total_len==1||total_len<max_num-1) return false;
	if(total_len>=2&&group+bias>=weight_for_group_att*value(info,tower,group_core(info),len)) return true;
	return false;
}

bool group_cut(Info& info,int tower,int my_tower_num,int r)
{
	if(en_arrived_num(info,tower)>=1&&my_arrived_resource(info,tower)-info.towerInfo[tower].resource<en_arrived_resource(info,tower)*2) return false;
	if(en_arrived_resource(info,tower)>my_arrived_resource(info,tower)&&my_arrived_num(info,tower)<=1) return false;
	if(en_arrived_resource(info,tower)>my_arrived_resource(info,tower)-info.towerInfo[tower].resource) return false;

	for(int j=0;j<info.towerNum;j++) group_can_cut[j]=-1;
	int group=0,len=0,total_len=0;

	for(int i=0;i<my_tower_num;i++)
	{
		if(info.lineInfo[my_tower[i]][tower].exist&&info.lineInfo[my_tower[i]][tower].state==AfterCut&&info.lineInfo[my_tower[i]][tower].frontResource>0)
		{
			group+=info.lineInfo[my_tower[i]][tower].frontResource*cut_arrived_v(info,my_tower[i],tower)*wall_degree(info,tower);
			total_len++;
		}
		else if(info.lineInfo[my_tower[i]][tower].exist&&remained_time(info,my_tower[i],tower)<round_to_cut_to_att/2)
		{
			if(info.towerInfo[my_tower[i]].strategy==Attack&&info.towerInfo[tower].strategy==Grow
			&&info.playerInfo[info.towerInfo[tower].owner].technologyPoint+tech_v(info,info.towerInfo[tower].owner)*info.lineInfo[my_tower[i]][tower].resource/20.0>=5.0)
			{
				group+=info.lineInfo[my_tower[i]][tower].resource*1.25*wall_degree(info,tower)*proportion(info,my_tower[i],tower);
			}
			else group+=info.lineInfo[my_tower[i]][tower].resource*cut_arrived_v(info,my_tower[i],tower)*wall_degree(info,tower)*proportion(info,my_tower[i],tower);
			total_len++;
			if(info.lineInfo[my_tower[i]][tower].state==Arrived)
			{
				group_can_cut[len]=my_tower[i];
				len++;
				r++;
				if(r>=info.myMaxControl) break;
			}
		}
	}
	if(group>info.towerInfo[tower].resource+line_resource(info,tower)*0.75+arrived_help(info,tower)*0.75+weight_enemy*both_enemies(info,tower)+en_arrived_resource(info,tower)/weight_being_arrived_cal+2*weight_bias) return true;
	return false;
}

bool con_no_cut(Info& info,int my,int en)
{
	if(info.lineInfo[my][en].state==Confrontation||(info.lineInfo[my][en].state==Attacking
		&&info.lineInfo[my][en].resource<getDistance(info.towerInfo[my].position,info.towerInfo[en].position)/20.0))
		return true;
	return false;
}

bool not_moved(Info& info,int id)
{
	for(int i=0;i<4;i++)
	{
		if(relation[id][i]!=0&&i!=id) return false;
	}
	if(player_resource(info,id)<=250&&info.round>100) return false;
	return true;
}

void initial_flag(Info& info)
{
	flag_player[info.myID]=0;
	for(int i=0;i<info.towerNum;i++)
	{
		if(flag_player[info.towerInfo[i].owner]!=0) flag_tower[i]=1;
	}
	for(int i=0;i<4;i++)
	{
		if(i-info.myID==2||info.myID-i==2) flag_player[i]=-1;
		else if(i!=info.myID) flag_player[i]=-10;
	}
	flag_tower[my_tower[0]]=1024;
	return ;
}

bool first_flag(Info& info)
{
	for(int i=0;i<4;i++)
	{
		if(i==info.myID) continue;
		if(flag_player[i]==1) return false;
	}
	return true;
}

void first_to_att(Info& info)
{
	int a=0,b=0,c=0,d=0;
	for(int i=0;i<4;i++)
	{
		a+=relation[0][i];
		b+=relation[1][i];
		c+=relation[2][i];
		d+=relation[3][i];
	}
	if(info.myID==0)
	{
		if(b<=2&&c<=2&&d<=2) return ;
		if(b>=c&&b>=d)
		{
			flag_player[1]=1;
			return ;
		}
		if(d>=b&&d>=c)
		{
			flag_player[3]=1;
			return ;
		}
		if(c>b&&c>d)
		{
			flag_player[2]=1;
			return ;
		}
	}
	else if(info.myID==1)
	{
		if(a<=2&&c<=2&&d<=2) return ;
		if(a>=c&&a>=d)
		{
			flag_player[0]=1;
			return ;
		}
		if(c>=a&&c>=d)
		{
			flag_player[2]=1;
			return ;
		}
		if(d>a&&d>c)
		{
			flag_player[3]=1;
			return ;
		}
	}
	else if(info.myID==2)
	{
		if(a<=2&&b<=2&&d<=2) return ;
		if(b>=a&&b>=d)
		{
			flag_player[1]=1;
			return ;
		}
		if(d>=a&&d>=b)
		{
			flag_player[3]=1;
			return ;
		}
		if(a>b&&a>d)
		{
			flag_player[0]=1;
			return ;
		}
	}
	else if(info.myID==3)
	{
		if(a<=2&&b<=2&&c<=2) return ;
		if(a>=b&&a>=c)
		{
			flag_player[0]=1;
			return ;
		}
		if(c>=a&&c>=b)
		{
			flag_player[2]=1;
			return ;
		}
		if(b>a&&b>c)
		{
			flag_player[1]=1;
			return ;
		}
	}
	return ;
}

void change_flag(Info& info,int my_tower_num)
{
	if(info.round<round_to_start_attack) return ;

	for(int i=0;i<info.towerNum;i++)
	{
		if(flag_tower[i]==1024&&info.towerInfo[i].owner==info.myID) continue;
		if(info.towerInfo[i].owner==info.myID&&flag_tower[i]!=1024)
		{
			flag_tower[i]=0;
			continue;
		}
		if(info.towerInfo[i].owner!=info.myID) flag_tower[i]=1;
	}

	if(open_fire(info,my_tower_num))
	{
		for(int i=0;i<4;i++)
		{
			if(i!=info.myID) flag_player[i]=1;
		}
		return ;
	}

	for(int i=0;i<4;i++)
	{
		if(i!=info.myID&&not_moved(info,i)&&info.round<100) flag_player[i]=-1;
		else if(i==info.myID) flag_player[i]=0;
	}

	if(first_flag(info))
	{
		first_to_att(info);
		return ;
	}

	int max=0,target=-1;
	for(int i=0;i<4;i++)
	{
		if(i==info.myID) continue;
		if(relation[i][info.myID]>max)
		{
			max=relation[i][info.myID];
			target=i;
		}
	}
	if(max>2&&target!=-1)
	{
		if(flag_player[target]!=1)
		{
			flag_player[target]=1;
			for(int i=0;i<4;i++)
			{
				if(i!=target&&i!=info.myID&&flag_player[i]==1)
					if(relation[i][info.myID]<=2||relation[info.myID][i]<=2)
						flag_player[i]=-1;
			}
		}
	}

	return ;
}

bool need_to_change(Info& info,int tower,int *type)
{
	double be_att=0;
	*type=0;
	for(int i=0;i<info.towerNum;i++) 
	{
		if(info.towerInfo[i].owner==info.myID) continue;
		if(info.lineInfo[i][tower].exist&&(remained_time(info,i,tower)<round_to_confront||info.lineInfo[tower][i].state==Confrontation))
		{
			if(info.towerInfo[i].strategy==Attack) be_att+=1.5;
			else be_att+=0.5;
		}
	}
	if(info.towerInfo[tower].strategy==Normal)
	{
		if(be_att>=better_to_grow&&be_att<=better_to_defence)
		{
			*type=1;
			return true;
		}
		if(be_att<better_to_grow)
		{
			*type=3;
			return true;
		}
		if(be_att>better_to_defence)
		{
			*type=2;
			return true;
		}
	}
	else if(info.towerInfo[tower].strategy==Grow)
	{
		if(be_att>better_to_grow)
		{
			if(be_att>better_to_defence&&att_enemies(info,tower)>=1)
			{
				*type=2;
				return true;
			}
			if(be_att<=better_to_defence&&den_enemies(info,tower)<=1)
			{
				*type=1;
				return true;
			}
		}
	}
	else if(info.towerInfo[tower].strategy==Attack)
	{
		if(be_att<better_to_grow&&att_enemies(info,tower)==0)
		{
			*type=3;
			return true;
		}
		if(att_enemies(info,tower)==0&&den_enemies(info,tower)>1)
		{
			*type=3;
			return true;
		}
		if(be_att>better_to_defence&&att_enemies(info,tower)>=1)
		{
			*type=2;
			return true;
		}
	}
	else if(info.towerInfo[tower].strategy==Defence)
	{
		if(be_att<=better_to_defence&&be_att>better_to_grow&&den_enemies(info,tower)<=1)
		{
			*type=1;
			return true;
		}
		if(be_att<better_to_grow||(den_enemies(info,tower)>1&&att_enemies(info,tower)==0))
		{
			*type=3;
			return true;
		}
	}
	return false;
}

int upgrade_need(Info& info)
{
	if(info.playerInfo[info.myID].RegenerationSpeedLevel<5) return 2*info.playerInfo[info.myID].RegenerationSpeedLevel+2;
	if(info.playerInfo[info.myID].DefenceLevel<3) return 2*info.playerInfo[info.myID].DefenceLevel+2;
	if(info.playerInfo[info.myID].ExtendingSpeedLevel<5) return 2*info.playerInfo[info.myID].ExtendingSpeedLevel+2;
	if(info.playerInfo[info.myID].ExtraControlLevel<3) return 2*info.playerInfo[info.myID].ExtraControlLevel+3;
	return 0;
}

bool valid_to_upgrade(Info& info)
{
	int need=0,type=0;
	double extra=0;
	if(info.round>20)
	{
		extra=round_for_extra_tech*tech_v(info,info.myID);
		for(int i=0;i<info.towerNum;i++)
		{
			if(info.towerInfo[i].owner!=info.myID) continue;
			if(need_to_change(info,i,&type)&&(last_str_change[i]>=round_between_str_change||info.towerInfo[i].strategy==Grow))
			{
				if(info.towerInfo[i].strategy==Normal) need+=3;
				else need+=5;
			}
		}
	}
	if(upgrade_need(info)!=0&&info.playerInfo[info.myID].technologyPoint>=upgrade_need(info)+max(need-extra,0))
	{
		used_tech=upgrade_need(info);
		return true;
	}
	return false;
}

int Upgrade(Info& info,int r)
{
	if(info.playerInfo[info.myID].RegenerationSpeedLevel<5)
	{
		info.myCommandList.addCommand(upgrade,0);
		r++;
		return r;
	}
	if(info.playerInfo[info.myID].DefenceLevel<3)
	{
		info.myCommandList.addCommand(upgrade,3);
		r++;
		return r;
	}
	if(info.playerInfo[info.myID].ExtendingSpeedLevel<5)
	{
		info.myCommandList.addCommand(upgrade,1);
		r++;
		return r;
	}
	if(info.playerInfo[info.myID].ExtraControlLevel<3)
	{
		info.myCommandList.addCommand(upgrade,2);
		r++;
		return r;
	}
	return r;
}

int Help(Info &info,int r,int my_tower_num)
{
	for(int i=my_tower_num-1;i>=0;i--)
	{
		for(int j=0;j<my_tower_num;j++)
		{
			if(i==j) continue;
			if(info.lineInfo[my_tower[i]][my_tower[j]].exist||info.lineInfo[my_tower[j]][my_tower[i]].exist) continue;
			if(enemies(info,my_tower[i])<1&&info.towerInfo[my_tower[i]].resource>50&&getDistance(info.towerInfo[my_tower[i]].position,encore)>400) continue;
			if(flag_tower[my_tower[i]]==1024&&(enemies(info,my_tower[i])<=1||after_change_life(info,my_tower[i],0)>round_need_help)) continue;
			if(info.towerInfo[my_tower[j]].strategy==Defence||enemies(info,my_tower[j])>=3) continue;
			if(info.towerInfo[my_tower[i]].id==12&&info.towerInfo[my_tower[i]].resource>=45) continue;
			if(after_change_life(info,my_tower[j],1)>round_to_help&&my_after_change_value(info,my_tower[j],1)>num_to_help)
			{
				if(my_after_change_value(info,my_tower[i],0)<num_need_help||after_change_life(info,my_tower[i],0)-reach_time(info,my_tower[j],my_tower[i])*0.5<round_need_help
					||(back_support(info,my_tower[j],my_tower[i])&&my_tower_num<info.towerNum-3)||enemies(info,my_tower[j])>info.towerInfo[my_tower[j]].maxLineNum)
				{
					if(info.towerInfo[my_tower[j]].currLineNum+go_att[my_tower[j]]+go_con[my_tower[j]]+go_help[my_tower[j]]-go_cut[my_tower[j]]<info.towerInfo[my_tower[j]].maxLineNum)
					{
						info.myCommandList.addCommand(addLine,my_tower[j],my_tower[i]);
						r++;
						be_help[my_tower[i]]++;
						go_help[my_tower[j]]++;
						if(r>=info.myMaxControl) return r;
					}
				}
			}
		}
	}
	return r;
}

int Counter(Info& info,int r,int my_tower_num,int en_tower_num)
{
	for(int i=my_tower_num-1;i>=0;i--)
	{
		for(int j=en_tower_num-1;j>=0;j--)
		{
			if(valid_line_for_confront(info,en_tower[j],my_tower[i])&&!info.lineInfo[my_tower[i]][en_tower[j]].exist)
			{
				if(info.towerInfo[en_tower[j]].strategy==Defence&&info.towerInfo[my_tower[i]].strategy==Attack) continue;
				if(after_confront_life(info,my_tower[i],en_tower[j],1)<round_to_stop_confront) continue;
				if(info.towerInfo[my_tower[i]].currLineNum+go_att[my_tower[i]]+go_con[my_tower[i]]+go_help[my_tower[i]]-go_cut[my_tower[i]]<info.towerInfo[my_tower[i]].maxLineNum)
				{
					info.myCommandList.addCommand(addLine,my_tower[i],en_tower[j]);
					r++;
					go_con[my_tower[i]]++;
					be_con[en_tower[j]]++;
					if(r>=info.myMaxControl-1) return r;
				}
			}
		}
	}
	return r;
}

int ATtack(Info& info,int r,int my_tower_num,int en_tower_num)
{
	for(int i=0;i<en_tower_num;i++)
	{
		if(group_att(info,en_tower[i],my_tower_num,r))
		{
			int len=0;
			while(group_can_att[len]!=-1) len++;
			for(int j=0;j<len;j++)
			{
				info.myCommandList.addCommand(addLine,group_can_att[j],en_tower[i]);
				r++;
				go_att[group_can_att[j]]++;
				be_att[en_tower[i]]++;
			}
			for(int j=0;j<info.towerNum;j++) group_can_att[j]=-1;
			if(r>=info.myMaxControl-1&&r>=1) break;
		}
	}
	return r;
}

int Changestr(Info& info,int my_tower_num)
{
	int type=0,r=0,need=used_tech;
	for(int i=my_tower_num-1;i>=0;i--)
	{
		if(need_to_change(info,my_tower[i],&type))
		{
			if(type==2&&(last_str_change[my_tower[i]]>=round_between_str_change||info.towerInfo[my_tower[i]].strategy==Grow))
			{
				if(info.towerInfo[my_tower[i]].strategy==Normal) need+=3;
				else need+=5;
				if(need>info.playerInfo[info.myID].technologyPoint) return r;
				info.myCommandList.addCommand(changeStrategy,my_tower[i],Defence);
				r++;
				already[my_tower[i]]=1;
				last_str_change[my_tower[i]]=0;
				tower_change[my_tower[i]]=2;
			}
		}
	}
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner!=info.myID) continue;
		if(already[i]==1) continue;
		if(need_to_change(info,i,&type)&&(last_str_change[i]>=round_between_str_change||info.towerInfo[i].strategy==Grow))
		{
			if(info.towerInfo[i].strategy==Normal) need+=3;
			else need+=5;
			if(need>info.playerInfo[info.myID].technologyPoint) return r;
			if(type==1)
			{
				info.myCommandList.addCommand(changeStrategy,i,Attack);
				r++;
				tower_change[i]=1;
			}
			else if(type==2)
			{
				info.myCommandList.addCommand(changeStrategy,i,Defence);
				r++;
				tower_change[i]=2;
			}
			else if(type==3)
			{
				info.myCommandList.addCommand(changeStrategy,i,Grow);
				r++;
				tower_change[i]=3;
			}
			if(type!=0) last_str_change[i]=0;
		}
	}
	return r;
}

void Judge_cut(Info& info,int my_tower_num,int r)
{
	if(special_case_cut(info))
	{
		for(int i=my_tower_num-1;i>=0;i--)
		{
			if(info.lineInfo[my_tower[i]][12].exist&&info.lineInfo[my_tower[i]][12].state==Arrived)
			{
				info.myCommandList.addCommand(cutLine,my_tower[i],12,0);
				go_cut[my_tower[i]]++;
				be_cut[12]++;
				r++;
				break;
			}
		}
	}
	for(int i=0;i<info.towerNum;i++)
	{
		if(special_case_cut(info)&&i==12&&info.towerInfo[i].owner==-1) continue;
		if(info.towerInfo[i].owner==info.myID) continue;
		if(group_cut(info,i,my_tower_num,r))
		{
			int len=0,resource=0;
			while(group_can_cut[len]!=-1) len++;
			for(int j=0;j<len;j++)
			{
				info.myCommandList.addCommand(cutLine,group_can_cut[j],i,0);
				r++;
				if(info.towerInfo[i].owner==-1) resource+=info.lineInfo[group_can_cut[j]][i].resource;
				go_cut[group_can_cut[j]]++;
				be_cut[i]++;
				if(r>=info.myMaxControl) return ;
				if(info.towerInfo[i].owner==-1&&resource>info.towerInfo[i].resource/cut_arrived_v(info,group_can_cut[j],i)+en_arrived_resource(info,i)+weight_enemy*both_enemies(info,i)+info.towerInfo[i].maxResource)
					break;
			}
			for(int j=0;j<info.towerNum;j++) group_can_cut[j]=-1;
		}
	}
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner==info.myID)
		{
			for(int j=0;j<info.towerNum;j++)
			{
				if(info.lineInfo[i][j].exist&&info.lineInfo[i][j].state!=Backing&&info.lineInfo[i][j].state!=AfterCut)
				{
					if(info.towerInfo[i].resource<25&&go_cut[i]<1)
					{
						info.myCommandList.addCommand(cutLine,i,j,0);
						r++;
						go_cut[i]++;
						be_cut[j]++;
						if(r>=info.myMaxControl) return ;
						continue;
					}
					if(info.towerInfo[j].owner!=info.myID&&info.towerInfo[j].owner!=-1)
					{
						if(info.lineInfo[j][i].exist&&(info.lineInfo[i][j].state==Extending||info.lineInfo[i][j].state==Confrontation)&&after_change_life(info,i,0)>=round_to_stop_confront
							&&!(info.towerInfo[i].strategy==Attack&&info.towerInfo[j].strategy==Defence)) continue;
						if(info.towerInfo[j].owner!=-1&&flag_player[info.towerInfo[j].owner]!=1&&!con_no_cut(info,i,j)&&info.towerInfo[j].resource>50)
						{
							info.myCommandList.addCommand(cutLine,i,j,info.lineInfo[i][j].resource);
							r++;
							go_cut[i]++;
							be_cut[j]++;
							if(r>=info.myMaxControl) return ;
						}
						else if(info.towerInfo[j].id==12&&!att_mid(info,my_tower_num)&&!con_no_cut(info,i,j)&&info.towerInfo[j].resource>30)
						{
							info.myCommandList.addCommand(cutLine,i,j,info.lineInfo[i][j].resource);
							r++;
							go_cut[i]++;
							be_cut[j]++;
							if(r>=info.myMaxControl) return ;
						}
						else if((after_change_life(info,i,0)<round_to_stop_attack||my_after_change_value(info,i,0)<value(info,j,info.towerInfo[i].position,0)/(being_group_att(info,j,info.myID)+0.2))
							&&!con_no_cut(info,i,j))
						{
							info.myCommandList.addCommand(cutLine,i,j,info.lineInfo[i][j].resource);
							r++;
							go_cut[i]++;
							be_cut[j]++;
							if(r>=info.myMaxControl) return ;
						}
						else if(!con_no_cut(info,i,j)&&after_change_life(info,j,0)>6&&info.towerInfo[j].resource>30&&!all_confront(info,i)&&go_cut[i]<1)
						{
							info.myCommandList.addCommand(cutLine,i,j,info.lineInfo[i][j].resource);
							r++;
							go_cut[i]++;
							be_cut[j]++;
							if(r>=info.myMaxControl) return ;
						}
						else if(!con_no_cut(info,i,j)&&info.towerInfo[j].resource>30&&en_arrived_num(info,j)>my_arrived_num(info,j)+1)
						{
							info.myCommandList.addCommand(cutLine,i,j,info.lineInfo[i][j].resource);
							r++;
							go_cut[i]++;
							be_cut[j]++;
							if(r>=info.myMaxControl) return ;
						}
						else if(!con_no_cut(info,i,j)&&info.towerInfo[j].resource>80&&being_group_att(info,j,info.myID)<=1&&!valid_to_raid(info,j))
						{
							info.myCommandList.addCommand(cutLine,i,j,info.lineInfo[i][j].resource);
							r++;
							go_cut[i]++;
							be_cut[j]++;
							if(r>=info.myMaxControl) return ;
						}
						else if(info.lineInfo[i][j].state==Confrontation&&after_change_life(info,i,0)<round_to_stop_confront)
						{
							info.myCommandList.addCommand(cutLine,i,j,info.lineInfo[i][j].resource);
							r++;
							go_cut[i]++;
							be_cut[j]++;
							if(r>=info.myMaxControl) return ;
						}
						else if(info.lineInfo[i][j].state==Confrontation&&info.towerInfo[i].strategy==Attack&&info.towerInfo[j].strategy==Defence
							&&(helper(info,j)>=enemies(info,j)||(after_change_life(info,j,0)>round_not_att_denfence&&info.towerInfo[j].resource>30))
							&&being_group_att(info,j,info.myID)+both_enemies(info,j)<=info.towerInfo[j].maxLineNum&&tower_change[i]!=3)
						{
							info.myCommandList.addCommand(cutLine,i,j,info.lineInfo[i][j].resource);
							r++;
							go_cut[i]++;
							be_cut[j]++;
							if(r>=info.myMaxControl) return ;
						}
					}
					else if(info.towerInfo[j].owner==info.myID)
					{
						if(info.towerInfo[j].id==12&&info.towerInfo[j].resource>=45&&enemies(info,j)<2)
						{
							info.myCommandList.addCommand(cutLine,i,j,info.lineInfo[i][j].resource);
							r++;
							go_cut[i]++;
							be_cut[j]++;
							if(r>=info.myMaxControl) return ;
							continue;
						}
						if(info.towerInfo[j].resource>=info.towerInfo[j].maxResource-20&&enemies(info,j)<=2&&info.towerInfo[j].id!=12&&info.towerInfo[j].maxResource>80)
						{
							info.myCommandList.addCommand(cutLine,i,j,info.lineInfo[i][j].resource);
							r++;
							go_cut[i]++;
							be_cut[j]++;
							if(r>=info.myMaxControl) return ;
							continue;
						}
						if(enemies(info,j)>=2&&enemies(info,i)<2&&my_after_change_value(info,i,0)>num_to_help&&after_change_life(info,i,0)>round_to_help&&info.towerInfo[i].resource>60) continue;
						if(after_change_life(info,i,0)<round_to_stop_help)
						{
							info.myCommandList.addCommand(cutLine,i,j,info.lineInfo[i][j].resource);
							r++;
							go_cut[i]++;
							be_cut[j]++;
							if(r>=info.myMaxControl) return ;
						}
						else if(info.towerInfo[j].resource>75&&info.lineInfo[i][j].state==Arrived&&info.towerInfo[i].resource<info.towerInfo[j].resource)
						{
							info.myCommandList.addCommand(cutLine,i,j,info.lineInfo[i][j].resource);
							r++;
							go_cut[i]++;
							be_cut[j]++;
							if(r>=info.myMaxControl) return ;
						}
						else if(info.towerInfo[j].resource>info.towerInfo[j].maxResource*0.8&&info.towerInfo[j].resource>120&&info.lineInfo[i][j].state==Arrived)
						{
							info.myCommandList.addCommand(cutLine,i,j,info.lineInfo[i][j].resource);
							r++;
							go_cut[i]++;
							be_cut[j]++;
							if(r>=info.myMaxControl) return ;
						}
						else if((after_change_life(info,j,0)<=round_to_cut_to_help||info.towerInfo[j].resource<=num_cut_to_help)&&info.lineInfo[i][j].state==Arrived)
						{
							info.myCommandList.addCommand(cutLine,i,j,max(info.lineInfo[i][j].resource-(info.towerInfo[j].maxResource-info.towerInfo[j].resource),0));
							r++;
							go_cut[i]++;
							be_cut[j]++;
							if(r>=info.myMaxControl) return ;
						}
					}
				}
			}
		}
	}
	return ;
}

bool only_to_survive(Info& info,int my_tower_num)
{
	if(my_tower_num==0||my_tower_num>2) return false;
	if(my_tower_num==1)
	{
		if(info.towerInfo[my_tower[0]].strategy==Defence&&info.towerInfo[my_tower[0]].currLineNum==0) return true;
		if(info.playerInfo[info.myID].technologyPoint>=5)
		{
			info.myCommandList.addCommand(changeStrategy,my_tower[0],Defence);
		}
		for(int i=0;i<info.towerNum;i++)
		{
			if(info.lineInfo[my_tower[0]][i].exist&&info.lineInfo[my_tower[0]][i].state!=AfterCut&&info.lineInfo[my_tower[0]][i].state!=Backing)
			{
				info.myCommandList.addCommand(cutLine,my_tower[0],i,info.lineInfo[my_tower[0]][i].resource);
			}
		}
		return true;
	}
	else if(my_tower_num==2)
	{
		if(info.playerInfo[info.myID].technologyPoint>=5&&info.towerInfo[my_tower[0]].strategy!=Defence)
		{
			info.myCommandList.addCommand(changeStrategy,my_tower[0],Defence);
		}
		for(int i=0;i<info.towerNum;i++)
		{
			if(info.lineInfo[my_tower[0]][i].exist&&info.lineInfo[my_tower[0]][i].state!=AfterCut&&info.lineInfo[my_tower[0]][i].state!=Backing)
			{
				info.myCommandList.addCommand(cutLine,my_tower[0],i,info.lineInfo[my_tower[0]][i].resource);
			}
		}
		for(int i=0;i<info.towerNum;i++)
		{
			if(info.lineInfo[my_tower[1]][i].exist&&info.towerInfo[i].owner!=info.myID&&info.lineInfo[my_tower[1]][i].state!=AfterCut&&info.lineInfo[my_tower[1]][i].state!=Backing)
			{
				info.myCommandList.addCommand(cutLine,my_tower[1],i,info.lineInfo[my_tower[1]][i].resource);
			}
		}
		if(!info.lineInfo[my_tower[1]][my_tower[0]].exist)
		{
			info.myCommandList.addCommand(addLine,my_tower[1],my_tower[0]);
		}
		return true;
	}
	return false;
}

bool win(Info& info)
{
	for(int i=0;i<info.towerNum;i++)
	{
		if(info.towerInfo[i].owner!=info.myID) return false;
	}
	return true;
}

void player_ai(Info& info)
{
	if(win(info)) return ;

	used_tech=0;

	my_tower=(int *)malloc(info.towerNum*sizeof(int));
	en_tower=(int *)malloc(info.towerNum*sizeof(int));
	life=(int *)malloc(info.towerNum*sizeof(int));
	go_att=(int *)malloc(info.towerNum*sizeof(int));
	be_att=(int *)malloc(info.towerNum*sizeof(int));
	go_cut=(int *)malloc(info.towerNum*sizeof(int));
	be_cut=(int *)malloc(info.towerNum*sizeof(int));
	go_help=(int *)malloc(info.towerNum*sizeof(int));
	be_help=(int *)malloc(info.towerNum*sizeof(int));
	go_con=(int *)malloc(info.towerNum*sizeof(int));
	be_con=(int *)malloc(info.towerNum*sizeof(int));
	already=(int *)malloc(info.towerNum*sizeof(int));
	group_can_att=(int *)malloc(info.towerNum*sizeof(int));
	group_can_cut=(int *)malloc(info.towerNum*sizeof(int));
	tower_change=(int *)malloc(info.towerNum*sizeof(int));
	relation=(int **)malloc(4*sizeof(int *));


	for(int i=0;i<4;i++)
	{
		relation[i]=(int *)malloc(4*sizeof(int));
		for(int j=0;j<4;j++) relation[i][j]=0;
	}

	for(int i=0;i<info.towerNum;i++)
	{
		tower_change[i]=en_tower[i]=go_att[i]=be_att[i]=go_help[i]=be_help[i]=go_con[i]=be_con[i]=go_cut[i]=be_cut[i]=life[i]=already[i]=0;
		group_can_cut[i]=group_can_att[i]=-1;
	}

	int my_tower_num=my_bubble(info),en_tower_num=en_bubble(info),r=0;
	
	mycore=my_core(info),encore=en_core(info);
	
	if(only_to_survive(info,my_tower_num)) return ;

	cal_relation(info);

	Life(info);
	
	if(info.round<=1)
	{
		initial_flag(info);
		for(int i=0;i<13;i++) last_str_change[i]=0;
	}

	for(int i=0;i<13;i++) last_str_change[i]++;

	if(upgrade_need(info)==0)
	{
		for(int i=0;i<13;i++) last_str_change[i]=100;
	}

	change_flag(info,my_tower_num);

	if(valid_to_upgrade(info)) r=Upgrade(info,r);

	if(r<info.myMaxControl&&info.round>round_to_start_help) r=Help(info,r,my_tower_num);

	if(r<info.myMaxControl) r=Counter(info,r,my_tower_num,en_tower_num);

	if(info.round>round_to_start_attack&&r<info.myMaxControl) r=ATtack(info,r,my_tower_num,en_tower_num);

	if(r<info.myMaxControl&&info.playerInfo[info.myID].technologyPoint>=3) r+=Changestr(info,my_tower_num);

	if(r<info.myMaxControl) Judge_cut(info,my_tower_num,r);

	delete [] my_tower;
	delete [] en_tower;
	delete [] life;
	delete [] go_att;
	delete [] be_att;
	delete [] go_cut;
	delete [] be_cut;
	delete [] go_help;
	delete [] be_help;
	delete [] go_con;
	delete [] be_con;
	delete [] already;
	delete [] group_can_att;
	delete [] group_can_cut;
	delete [] relation;
	delete [] tower_change;

	return ;
}