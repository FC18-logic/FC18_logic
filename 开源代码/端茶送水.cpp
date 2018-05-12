#include "ai.h"
#include "definition.h"
#include "user_toolbox.h"
#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>

void wetherunderattack(TTowerID ,vector<int> &,Info&);//在进攻上再想想办法，目标选取可以优先选取那些兵线全伸出去了的塔
void wetheraftercut(TTowerID&,vector<int>&,Info&);
void wethergiveup(TTowerID,vector<int>&,Info&);
void defencegiveup(TTowerID,vector<int>&,Info&);
void wetherconfrontation(TTowerID,vector<int>&,Info&);
void wetherattacking(TTowerID,vector<int>&,Info&);
void wetherattackingp(TTowerID,vector<int>&,Info&);
void wetherattackcut(TTowerID,vector<int>&,Info&);
void wetherexist(TTowerID,vector<int>&,Info&);
int currentlinenum(TTowerID,Info&);
TTowerID target(TTowerID,vector<int>,Info&);
TTowerID targetononeplayer(TTowerID,Info&);
int wetherupgrade(int,int&,Info&);
void wethercut(TTowerID,vector<int>&,Info&);
double multiply(double,int);
int regenerationspeedcompare(TTowerID,TTowerID,Info&);
int myopposite(int);
double resourcejudge(TTowerID,TTowerID,Info&);
double regenerationspeed(TTowerID,Info&);
double attacklineconsumption(TTowerID,Info&);
double attack_target_consumption(TTowerID,TTowerID,Info&);
void wetherextend(TTowerID,vector<int>&,Info&);
void defence(TTowerID,vector<int>,vector<int>,vector<int>,Info&);
int wetherlast(Info&);
int doggie(Info&);
int attackbythreefkingguys(Info&);
int damagecompare(TTowerID,vector<int>,Info&);
int allconfrontator(TTowerID,Info&);
struct wethersuccess{
	int booool;
	TTowerID tower;
};
using namespace std;
void player_ai(Info& info)
{	
	//cout<<"round"<<info.round<<endl;
	static vector<int> mytower;
	static wethersuccess what={1,-1};
	vector<int> mytowerbefore;
	if(info.round>1){
		vector<int>::iterator it=mytower.begin();
		for(;it!=mytower.end();it++){
			mytowerbefore.push_back(*it);
		}
	}
	int operate=info.playerInfo[info.myID].maxControlNumber;
	int myid=info.myID;
	int currentech=info.playerInfo[myid].technologyPoint;
	vector<int> indanger;
	set<TTowerID>::iterator Mytower=info.playerInfo[myid].towers.begin();
	mytower.clear();
	for(;Mytower!=info.playerInfo[myid].towers.end();Mytower++){
		mytower.push_back(*Mytower);													
	}					
	if(attackbythreefkingguys(info)==1){
		for(int i=0;i<mytower.size();i++){
			vector<int> attacker;
			wetherexist(mytower[i],attacker,info);
			if(info.towerInfo[mytower[i]].strategy!=Defence&&operate>0&&currentech>=5&&attacker[0]>1){
				info.myCommandList.addCommand(changeStrategy,mytower[i],Defence);
				operate--;
				currentech=currentech-5;
			}
		}
	}
	int num=mytower.size();
	for(int i=0;i<info.towerNum;i++){
		indanger.push_back(0);
	}
	int mynewtower=-1;
	if(info.round>1&&mytower.size()>mytowerbefore.size()){
		for(int i=0;i<mytower.size();i++){
			int a=0;
			for(int j=0;j<mytowerbefore.size();j++){
				if(mytower[i]==mytowerbefore[j]){
					a=1;
					break;
				}
			}
			if(a==0){
				mynewtower=mytower[i];
				break;
			}
		}
	}
	if(mynewtower!=-1&&info.towerInfo[mynewtower].resource<80){
		if(operate>0&&currentech>=3){
			info.myCommandList.addCommand(changeStrategy,mynewtower,Defence);
			operate--;
			currentech=currentech-3;
		}
		else{
			what.booool=0;
			what.tower=mynewtower;
		}
	}
	if(what.booool==0){
		if(operate>0&&currentech>=3){
			info.myCommandList.addCommand(changeStrategy,what.tower,Defence);
			operate--;
			currentech=currentech-3;
			what.booool=1;
			what.tower=-1;
		}
	}
	//cout<<"mytowernum"<<num<<endl;
	for(int i=0;i<num;i++){//危险性判断
		vector<int> extender;
		vector<int>&extending=extender;
		wetherextend(mytower[i],extender,info);
		vector<int> cutting;
		vector<int>&cutter=cutting;
		wethergiveup(mytower[i],cutting,info);
		vector<int> attacker;
		vector<int> &att=attacker;
		wetherunderattack(mytower[i],att,info);
		double attackresource=0.0;
		double lineresource=0.0;
		for(int j=0;j<extender.size()-1;j++){
			attackresource=attackresource+((info.lineInfo[extender[j+1]][mytower[i]].maxlength*Density*(SupressPower[info.towerInfo[extender[j+1]].strategy][info.towerInfo[mytower[i]].strategy]-1)/2)+1);
		}
		for(int j=0;j<attacker.size()-1;j++){
			attackresource=attackresource+((info.lineInfo[attacker[j+1]][mytower[i]].maxlength*Density*(SupressPower[info.towerInfo[attacker[j+1]].strategy][info.towerInfo[mytower[i]].strategy]-1)/2)+1);
		}
		for(int j=0;j<extender.size()-1;j++){
			lineresource=lineresource+((info.lineInfo[extender[j+1]][mytower[i]].resource*(SupressPower[info.towerInfo[extender[j+1]].strategy][info.towerInfo[mytower[i]].strategy]-1)/2)+1);
		}
		for(int j=0;j<attacker.size()-1;j++){
			lineresource=lineresource+((info.lineInfo[attacker[j+1]][mytower[i]].resource*(SupressPower[info.towerInfo[attacker[j+1]].strategy][info.towerInfo[mytower[i]].strategy]-1)/2)+1);
		}
		if(attackresource>info.towerInfo[mytower[i]].resource&&lineresource>=(attackresource*3/4)){
			indanger[mytower[i]]=1;
			if(cutting[0]==1){
				for(int j=0;j<cutting.size()-1;j++){
					int x=0;
					for(int k=0;k<extender.size()-1;k++){
						if(cutting[j+1]==extender[k+1])x=1;
					}
					if(x==0&&operate>0&&(info.lineInfo[mytower[i]][cutting[j+1]].state!=AfterCut)){
						info.myCommandList.addCommand(cutLine,mytower[i],cutting[j+1],info.lineInfo[mytower[i]][cutting[j+1]].resource);
						operate--;
					}
				}
				for(int j=0;j<extender.size()-1;j++){
					if((!info.lineInfo[mytower[i]][extender[j+1]].exist)&&operate>0&&info.lineInfo[extender[j+1]][mytower[i]].resource>=((info.lineInfo[mytower[i]][extender[j+1]].maxlength*Density*3)/(4))){
						info.myCommandList.addCommand(addLine,mytower[i],extender[j+1]);
						operate--;
					}
				}
			}
		}
	}
	for(int i=0;i<num;i++){//塔改变策略
		vector<int> attacker;
		vector<int>&att=attacker;
		vector<int> confrontator;
		vector<int>&con=confrontator;
		vector<int> attacking;
		vector<int>&atta=attacking;
		wetherunderattack(mytower[i],att,info);
		wetherconfrontation(mytower[i],con,info);
		wetherattacking(mytower[i],atta,info);
		if(attacker[0]==0&&confrontator[0]==0&&attacking[0]==0){
			if(info.towerInfo[mytower[i]].resource<=60){
				if(info.towerInfo[mytower[i]].strategy==Normal){
					if(currentech>=3&&operate>0&&indanger[mytower[i]]==0){
						info.myCommandList.addCommand(changeStrategy,mytower[i],Grow);
						operate--;
						currentech=currentech-3;
					}
				}
				if(info.towerInfo[mytower[i]].strategy==Attack){
					if(currentech>=5&&operate>0&&indanger[mytower[i]]==0){
						info.myCommandList.addCommand(changeStrategy,mytower[i],Grow);
						operate--;
						currentech=currentech-5;
					}
				}
				if(info.towerInfo[mytower[i]].strategy==Normal){
					if(currentech>=3&&operate>0&&indanger[mytower[i]]==1){
						info.myCommandList.addCommand(changeStrategy,mytower[i],Defence);
						operate--;
						currentech=currentech-3;
					}
				}
				if(info.towerInfo[mytower[i]].strategy==Attack){
					if(currentech>=5&&operate>0&&indanger[mytower[i]]==1){
						info.myCommandList.addCommand(changeStrategy,mytower[i],Defence);
						operate--;
						currentech=currentech-5;
					}
				}
			}
			if(info.towerInfo[mytower[i]].resource>=100){
				if(info.towerInfo[mytower[i]].strategy==Defence&&currentech>=5&&operate>0&&doggie(info)==0&&attackbythreefkingguys(info)==0){
					info.myCommandList.addCommand(changeStrategy,mytower[i],Attack);
					operate--;
					currentech=currentech-5;
				}
			}
			if(mytower[i]==12&&info.towerInfo[12].owner==myid&&info.towerInfo[12].resource==50&&info.towerInfo[12].strategy==Defence&&currentech>=5&&operate>0){
				info.myCommandList.addCommand(changeStrategy,12,Attack);
				operate--;
				currentech=currentech-5;
			}
		}
	}
	int defencenum=0;
	for(int i=0;i<mytower.size();i++){//防守放弃进攻
		if(info.towerInfo[mytower[i]].strategy==Defence){
			for(int j=0;j<info.towerNum;j++){
				if(info.lineInfo[mytower[i]][j].exist&&(!info.lineInfo[j][mytower[i]].exist)){
					if(operate>0){
						info.myCommandList.addCommand(cutLine,mytower[i],j,info.lineInfo[mytower[i]][j].resource);
						operate--;
					}
				}
			}
			defencenum++;
		}
	}
	for(int i=0;i<mytower.size();i++){//花式防守
		if(info.towerInfo[mytower[i]].strategy==Defence){
			vector<int> attacker;
			vector<int> confrontator;
			vector<int> extender;
			wetherunderattack(mytower[i],attacker,info);
			wetherconfrontation(mytower[i],confrontator,info);
			wetherextend(mytower[i],extender,info);
			defence(mytower[i],attacker,confrontator,extender,info);
		}
	}
	for(int i=0;i<info.towerNum;i++){//切兵线进攻
		vector<int> myattack;
		if(info.towerInfo[i].owner!=myid&&info.towerInfo[i].owner!=-1){
			wetherattackcut(i,myattack,info);
			if(myattack[0]!=-1&&operate>=myattack.size()-2){
				for(int j=0;j<myattack.size()-1;j++){
					if(myattack[j+1]!=myattack[0]){
						info.myCommandList.addCommand(cutLine,myattack[j+1],i,0);
						operate--;
					}
				}
			}
		}
	}
	for(int i=0;i<num;i++){//停止支援//
		vector<int> cutlist;
		vector<int>& cut=cutlist;
		wethercut(mytower[i],cut,info);
		if(cutlist[0]==1&&operate>0&&indanger[cutlist[1]]==0&&(info.lineInfo[mytower[i]][cutlist[1]].state!=AfterCut)){
			if(info.towerInfo[cutlist[1]].strategy!=Defence){
				info.myCommandList.addCommand(cutLine,mytower[i],cutlist[1],resourcejudge(mytower[i],cutlist[1],info));
				operate--;
			}
		}
	}
	for(int i=0;i<num;i++){//防守
		vector<int> attacker;
		vector<int>& att=attacker;
		vector<int> confrontator;
		vector<int>& con=confrontator;
		wetherconfrontation(mytower[i],con,info);
		wetherunderattack(mytower[i],att,info);
		if((confrontator[0]+attacker[0]==1)&&info.towerInfo[mytower[i]].resource>=100){//半防守
			if(attacker[0]==1){
				if(info.towerInfo[attacker[1]].strategy!=Defence){
					if(currentech>5&&operate>0&&info.towerInfo[mytower[i]].strategy!=Attack){
						info.myCommandList.addCommand(changeStrategy,mytower[i],Attack);
						operate--;
						currentech=currentech-5;
					}
					if(attacker[0]==1&&operate>0&&(!info.lineInfo[mytower[i]][attacker[1]].exist)){
						info.myCommandList.addCommand(addLine,mytower[i],attacker[1]);
						operate--;
					}
				}
			}
			if(confrontator[0]==1){
				if(info.towerInfo[mytower[i]].strategy==Grow){
					if(info.towerInfo[confrontator[1]].strategy==Attack){
						if(operate>0&&currentech>=5){
							info.myCommandList.addCommand(changeStrategy,mytower[i],Attack);
							operate--;
							currentech=currentech-5;
						}
					}
				}
				if(info.towerInfo[mytower[i]].strategy==Attack){
					if(info.towerInfo[confrontator[1]].strategy==Defence){
						if(operate>0&&(!regenerationspeedcompare(mytower[i],confrontator[1],info))&&currentech>=5){
							info.myCommandList.addCommand(cutLine,mytower[i],confrontator[1],info.lineInfo[mytower[i]][confrontator[1]].resource);
							operate--;
						}
						if(operate>0&&currentech>=5&&regenerationspeedcompare(mytower[i],confrontator[1],info)){
							info.myCommandList.addCommand(changeStrategy,mytower[i],Grow);
							currentech=currentech-5;
							operate--;
						}
						if(operate>0&&currentech>5&&regenerationspeedcompare(mytower[i],confrontator[1],info)&&doggie(info)==1){
							info.myCommandList.addCommand(changeStrategy,mytower[i],Defence);
							currentech=currentech-5;
							operate--;
						}
					}
				}
			}
		}
		if((confrontator[0]+attacker[0]==1)&&info.towerInfo[mytower[i]].resource<100){//全防守
			int a=0;
			if(currentech>5&&operate>0&&info.towerInfo[mytower[i]].strategy!=Defence){
				info.myCommandList.addCommand(changeStrategy,mytower[i],Defence);
				operate--;
				currentech=currentech-5;
				a=1;
			}
			if(a==0&&attacker[0]==1&&operate>0){
				info.myCommandList.addCommand(addLine,mytower[i],attacker[1]);
				operate--;
			}
		}
		if(attacker[0]+confrontator[0]>1){//全防守
			int a=0;
			if(currentech>=5&&info.towerInfo[mytower[i]].strategy!=Defence&&operate>0&&info.towerInfo[mytower[i]].resource<=info.towerInfo[mytower[i]].maxResource-50){
				info.myCommandList.addCommand(changeStrategy,mytower[i],Defence);
				currentech=currentech-5;
				a=1;
				operate--;
			}
			for(int k=0;k<attacker.size()-1;k++){
				if(info.towerInfo[attacker[k+1]].strategy==Attack&&operate>0&&a==1){
					info.myCommandList.addCommand(addLine,mytower[i],attacker[k+1]);
					operate--;
				}
			}
		}
	}
	for(int i=0;i<num;i++){//放弃进攻
		vector<int>attacking;
		vector<int>&atta=attacking;
		wetherattackingp(mytower[i],atta,info);
		if(attacking[0]==1&&info.towerInfo[attacking[1]].owner!=-1){
			if(!regenerationspeedcompare(mytower[i],attacking[1],info)&&operate>0){
				info.myCommandList.addCommand(cutLine,mytower[i],attacking[1],info.lineInfo[mytower[i]][attacking[1]].resource);
				operate--;
			}
		}
		if(attacking[0]==1&&info.towerInfo[attacking[1]].owner==-1){
			vector<int> playernum;
			playernum.push_back(0);
			playernum.push_back(0);
			playernum.push_back(0);
			playernum.push_back(0);
			int num=0;
			for(int b=0;b<info.towerNum;b++){
				if(info.lineInfo[b][attacking[1]].exist&&info.towerInfo[b].owner!=info.myID){
					playernum[info.towerInfo[b].owner]=1;
				}
			}
			vector<int>::iterator player=playernum.begin();
			for(;player!=playernum.end();++player){
				if(*player){
					num++;
				}
			}
			if(num>=2&&operate>0){
				info.myCommandList.addCommand(cutLine,mytower[i],attacking[1],info.lineInfo[mytower[i]][attacking[1]].resource);	
				operate--;
			}
		}
	}
	for(int i=0;i<num;i++){//兵线太多就切
		double maxresource=0.0;
		int target=mytower[i];
		if(currentlinenum(mytower[i],info)>info.towerInfo[mytower[i]].maxLineNum-1){
			for(int j=0;j<info.towerNum;j++){
				if(info.lineInfo[mytower[i]][j].exist&&(info.lineInfo[mytower[i]][j].state==Extending||info.lineInfo[mytower[i]][j].state==Arrived)){
					if(info.lineInfo[mytower[i]][j].resource>maxresource){
						maxresource=info.lineInfo[mytower[i]][j].resource;
						target=j;
					}
				}
			}
		}
		if(target!=mytower[i]&&operate>0){
			info.myCommandList.addCommand(cutLine,mytower[i],target,info.lineInfo[mytower[i]][target].resource);
			operate--;
		}
	}
	vector<int> firstattack;
	firstattack.push_back(0);
	for(int i=0;i<num;i++){//支援Defence
		vector<int> attacker;
		vector<int> &att=attacker;
		vector<int> confrontator;
		vector<int> &con=confrontator;
		vector<int> cutting;
		wethergiveup(mytower[i],cutting,info);
		wetherunderattack(mytower[i],att,info);
		wetherconfrontation(mytower[i],con,info);
		if(info.towerInfo[mytower[i]].strategy!=Defence&&indanger[mytower[i]]==0){
			int j=0,a=0;
			for(;j<num;j++){
				if(info.towerInfo[mytower[j]].strategy==Defence||indanger[mytower[j]]==1){
					a=1;
					break;
				}
			}
			if(a==1){
				int b=0;
				if(currentlinenum(mytower[i],info)<info.towerInfo[mytower[i]].maxLineNum&&operate>0&&info.towerInfo[mytower[i]].resource>=info.towerInfo[mytower[j]].resource&&(!info.lineInfo[mytower[i]][mytower[j]].exist)){
					if(attacker[0]==0&&confrontator[0]==0){
						info.myCommandList.addCommand(addLine,mytower[i],mytower[j]);
						b=1;
						operate--;
					}
					if(b==0){
						int wetherhavecut=0;
						int cuttar=0;
						for(int j=0;j<cutting.size()-1;j++){
							if(info.towerInfo[cutting[j+1]].owner!=myid){
								wetherhavecut=1;
								cuttar=cutting[j+1];
							}
						}
						if(cutting[0]==1&&operate>1){
							int a=0;
							if(wetherhavecut==1){
								info.myCommandList.addCommand(cutLine,mytower[i],cuttar,info.lineInfo[mytower[i]][cuttar].resource);
								operate--;
								a=1;
							}
							if(a==1){
								info.myCommandList.addCommand(addLine,mytower[i],mytower[j]);
								operate--;
							}
						}
					}
				}
			}
		}
	}
	int ok=0;
	int &ojbk=ok;
	operate=wetherupgrade(operate,ojbk,info);									//科技升级//
	for(int i=0;i<mytower.size();i++){//主动进攻
		vector<int> mytarget;
		for(int j=0;j<mytower.size();j++){
			for(int k=0;k<info.towerNum;k++){
				if(info.towerInfo[k].owner!=myid){
					if(info.lineInfo[mytower[j]][k].exist&&info.lineInfo[mytower[j]][k].state!=AfterCut){
						if(mytarget.size()==0){
							mytarget.push_back(k);
						}
						if(mytarget.size()==1){
							if(mytarget[0]!=k){
								mytarget.push_back(k);
							}
						}
					}
				}
			}
		}
		vector<int> attacker;
		vector<int>&att=attacker;
		wetherunderattack(mytower[i],att,info);
		double currsource=0.0;
		if(info.towerInfo[mytower[i]].resource>=info.towerInfo[mytower[i]].maxResource-50&&attacker[0]==0&&currentlinenum(mytower[i],info)<info.towerInfo[mytower[i]].maxLineNum-1&&indanger[mytower[i]]==0&&ok==1&&info.towerInfo[mytower[i]].strategy!=Defence){
			if(mytarget.size()==0&&target(mytower[i],indanger,info)!=-1){
					if(operate>0&&(!info.lineInfo[mytower[i]][target(mytower[i],indanger,info)].exist)&&target(mytower[i],indanger,info)!=-1){
						info.myCommandList.addCommand(addLine,mytower[i],target(mytower[i],indanger,info));
						operate--;
					}
				}
				if(mytarget.size()==1&&mytower.size()>3){
					int a=0;
					if(operate>0&&target(mytower[i],indanger,info)!=-1){
						info.myCommandList.addCommand(addLine,mytower[i],target(mytower[i],indanger,info));
						operate--;
						a=1;
					}
					if(a==0&&operate>0&&(!info.lineInfo[mytower[i]][mytarget[0]].exist)){
						info.myCommandList.addCommand(addLine,mytower[i],mytarget[0]);
						operate--;
					}
				}
				if(mytarget.size()==1&&mytower.size()<4){
					int num=0;
					vector<int> playernum;
					playernum.push_back(0);
					playernum.push_back(0);
					playernum.push_back(0);
					playernum.push_back(0);
					for(int b=0;b<info.towerNum;b++){
						if(info.lineInfo[b][mytarget[0]].exist&&info.towerInfo[b].owner!=info.myID){
							playernum[info.towerInfo[b].owner]=1;
						}
					}
					vector<int>::iterator player=playernum.begin();
					for(;player!=playernum.end();++player){
						if(*player){
							num++;
						}
					}
					if(operate>0&&num<2&&(!info.lineInfo[mytower[i]][mytarget[0]].exist)){
						info.myCommandList.addCommand(addLine,mytower[i],mytarget[0]);
						operate--;
					}
				}
				if(mytarget.size()==2){
					if((!info.lineInfo[mytower[i]][mytarget[0]].exist)&&operate>0){
						int num=0;
						vector<int> playernum;
						playernum.push_back(0);
						playernum.push_back(0);
						playernum.push_back(0);
						playernum.push_back(0);
						for(int b=0;b<info.towerNum;b++){
							if(info.lineInfo[b][mytarget[0]].exist&&info.towerInfo[b].owner!=info.myID){
								playernum[info.towerInfo[b].owner]=1;
							}
						}
						vector<int>::iterator player=playernum.begin();
						for(;player!=playernum.end();++player){
							if(*player){
								num++;
							}
						}
						if(num<2){
							if((!info.lineInfo[mytower[i]][mytarget[0]].exist)&&operate>0){
								info.myCommandList.addCommand(addLine,mytower[i],mytarget[0]);
								operate--;
							}
							if((!info.lineInfo[mytower[i]][mytarget[1]].exist)&&operate>0){
								info.myCommandList.addCommand(addLine,mytower[i],mytarget[1]);
								operate--;
							}
						}
					}
				}
		}
	}
}
void wetherunderattack(TTowerID mytower,vector<int>& attacker,Info& info)										//判断是否遭到攻击,第零个元素为bool，后面列出攻击者
{
	int myid=info.myID;
	attacker.push_back(0);
	int count=0,i=0;
	for(;i<info.towerNum;i++){
		if(info.towerInfo[i].owner!=myid&&info.lineInfo[i][mytower].exist){
			if(info.lineInfo[i][mytower].state==Arrived){
				attacker.push_back(i);
				count++;
			}
		}
	}
	attacker[0]=count;
}
void wetherextend(TTowerID mytower,vector<int>& attacker,Info& info)
{
	int myid=info.myID;
	attacker.push_back(0);
	int count=0,i=0;
	for(;i<info.towerNum;i++){
		if(info.towerInfo[i].owner!=myid&&info.lineInfo[i][mytower].exist){
			if(info.lineInfo[i][mytower].state==Extending){
				attacker.push_back(i);
				count++;
			}
		}
	}
	attacker[0]=count;
}
void wetherexist(TTowerID mytower,vector<int>& attacker,Info&info)
{
	int count=0;
	attacker.push_back(0);
	for(int i=0;i<info.towerNum;i++){
		if(info.towerInfo[i].owner!=info.myID){
			if(info.lineInfo[i][mytower].exist&&info.lineInfo[i][mytower].state!=AfterCut){
				attacker.push_back(i);
				count++;
			}
		}
	}
	attacker[0]=count;
}
void wetheraftercut(TTowerID &mytower,vector<int>& cutter,Info& info)										//判断是否遭到攻击,第零个元素为bool，后面列出攻击者
{
	int myid=info.myID;
	cutter.clear();
	cutter.push_back(0);
	int count=0,i=0;
	for(;i<info.towerNum;i++){
		if(info.towerInfo[i].owner!=myid&&info.lineInfo[i][mytower].exist){
			if(info.lineInfo[i][mytower].state==AfterCut){
				cutter.push_back(i);
				count++;
			}
		}
	}
	if(count>=1)cutter[0]=1;
}
void wethercut(TTowerID mytoweri,vector<int>&cut,Info&info)		//判断是否有需要切断的支援兵线
{
	int myid=info.myID;
	vector<int> mytower;																//输入自己兵塔信息
	set<TTowerID>::iterator Mytower=info.playerInfo[myid].towers.begin();				
	for(;Mytower!=info.playerInfo[myid].towers.end();Mytower++){
		mytower.push_back(*Mytower);													
	}
	cut.push_back(0);
	int count=0;
	for(int i=0;i<mytower.size();i++){
		if(info.lineInfo[mytoweri][mytower[i]].exist&&info.lineInfo[mytoweri][mytower[i]].state==Arrived){
			if((info.lineInfo[mytoweri][mytower[i]].resource+info.towerInfo[mytower[i]].resource>=info.towerInfo[mytoweri].resource)||info.lineInfo[mytoweri][mytower[i]].resource+info.towerInfo[mytower[i]].resource>=info.towerInfo[mytower[i]].maxResource){
				cut.push_back(mytower[i]);
				count++;
			}
		}
	}
	if(count!=0){
		cut[0]=1;
	}
}
int wetherupgrade(int operate,int&ok,Info&info)
{
	int a=1;
	if(operate>0){
		if(doggie(info)==1){
			info.myCommandList.addCommand(upgrade,Wall);
			operate--;
		}
		if(info.playerInfo[info.myID].RegenerationSpeedLevel==0&&info.playerInfo[info.myID].technologyPoint>=7){
			info.myCommandList.addCommand(upgrade,RegenerationSpeed);
			operate--;
		}
		if(info.playerInfo[info.myID].RegenerationSpeedLevel==1)a=1;
		else a=0;
		if(info.playerInfo[info.myID].ExtendingSpeedLevel==0&&info.playerInfo[info.myID].technologyPoint>=7&&a==1){
			info.myCommandList.addCommand(upgrade,ExtendingSpeed);
			operate--;
		}
		if(info.playerInfo[info.myID].ExtendingSpeedLevel==1)a=1;
		else a=0;
		if(info.playerInfo[info.myID].ExtendingSpeedLevel==1&&info.playerInfo[info.myID].technologyPoint>=9&&a==1){
			info.myCommandList.addCommand(upgrade,ExtendingSpeed);
			operate--;
		}
		if(info.playerInfo[info.myID].ExtendingSpeedLevel==2)a=1;
		else a=0;
		if(info.playerInfo[info.myID].RegenerationSpeedLevel==1&&info.playerInfo[info.myID].technologyPoint>=9&&a==1){
			info.myCommandList.addCommand(upgrade,RegenerationSpeed);
			operate--;
		}
		if(info.playerInfo[info.myID].RegenerationSpeedLevel==2)a=1;
		else a=0;
		if(info.playerInfo[info.myID].RegenerationSpeedLevel==2&&info.playerInfo[info.myID].technologyPoint>=11&&a==1){
			info.myCommandList.addCommand(upgrade,RegenerationSpeed);
			operate--;
		}
		if(info.playerInfo[info.myID].RegenerationSpeedLevel==3)a=1;
		else a=0;
		if(info.playerInfo[info.myID].DefenceLevel==0&&info.playerInfo[info.myID].technologyPoint>=8&&a==1){
			info.myCommandList.addCommand(upgrade,Wall);
			operate--;
		}
		if(info.playerInfo[info.myID].DefenceLevel==1)a=1;
		else a=0;
		if(info.playerInfo[info.myID].RegenerationSpeedLevel==3&&info.playerInfo[info.myID].technologyPoint>=13&&a==1){
			info.myCommandList.addCommand(upgrade,RegenerationSpeed);
			operate--;
		}
		if(info.playerInfo[info.myID].RegenerationSpeedLevel==4)a=1;
		else a=0;
		if(info.playerInfo[info.myID].ExtraControlLevel==0&&info.playerInfo[info.myID].technologyPoint>=8&&a==1){
			info.myCommandList.addCommand(upgrade,ExtraControl);
			operate--;
		}
		if(info.playerInfo[info.myID].ExtraControlLevel==1)a=1;
		else a=0;
		if(info.playerInfo[info.myID].RegenerationSpeedLevel==4&&info.playerInfo[info.myID].technologyPoint>=15&&a==1){
			info.myCommandList.addCommand(upgrade,RegenerationSpeed);
			operate--;
		}
		if(info.playerInfo[info.myID].RegenerationSpeedLevel==5)a=1;
		else a=0;
		if(info.playerInfo[info.myID].ExtraControlLevel==1&&info.playerInfo[info.myID].technologyPoint>=10&&a==1){
			info.myCommandList.addCommand(upgrade,ExtraControl);
			operate--;
		}
		if(info.playerInfo[info.myID].ExtendingSpeedLevel==2&&info.playerInfo[info.myID].technologyPoint>=11&&a==1){
			info.myCommandList.addCommand(upgrade,ExtendingSpeed);
			operate--;
		}
		if(info.playerInfo[info.myID].ExtendingSpeedLevel==3&&info.playerInfo[info.myID].technologyPoint>=13&&a==1){
			info.myCommandList.addCommand(upgrade,ExtendingSpeed);
			operate++;
		}
		if(info.playerInfo[info.myID].ExtendingSpeedLevel==4&&info.playerInfo[info.myID].technologyPoint>=15&&a==1){
			info.myCommandList.addCommand(upgrade,ExtendingSpeed);
			operate--;
		}
		if(info.playerInfo[info.myID].DefenceLevel==1&&info.playerInfo[info.myID].technologyPoint>=10&&a==1){
			info.myCommandList.addCommand(upgrade,Wall);
			operate--;
		}
		if(info.playerInfo[info.myID].DefenceLevel==2&&info.playerInfo[info.myID].technologyPoint>=12&&a==1){
			info.myCommandList.addCommand(upgrade,Wall);
			operate--;
		}
		if(info.playerInfo[info.myID].ExtraControlLevel==2&&info.playerInfo[info.myID].technologyPoint>=12&&a==1){
			info.myCommandList.addCommand(upgrade,ExtraControl);
			operate--;
		}
	}
	if(info.playerInfo[info.myID].RegenerationSpeedLevel==5){
		ok=1;
	}
	return operate;
}
TTowerID target(TTowerID mytower,vector<int>indanger,Info&info)
{
	int myid=info.myID;
	TTowerID target=mytower;
	double round=0.0;
	double min=9999.0;
	vector<int> mytowerinfo;																//输入自己兵塔信息
	set<TTowerID>::iterator Mytower=info.playerInfo[myid].towers.begin();				
	for(;Mytower!=info.playerInfo[myid].towers.end();Mytower++){
		mytowerinfo.push_back(*Mytower);													
	}
	vector<int> noextraline;
	for(int i=0;i<info.towerNum;i++){
		if(info.towerInfo[i].owner!=info.myID){
			if(currentlinenum(i,info)==info.towerInfo[i].maxLineNum&&((info.towerInfo[i].strategy==Defence&&info.towerInfo[i].resource<60)||info.towerInfo[i].strategy!=Defence)){
				noextraline.push_back(i);
			}
		}
	}
	if(noextraline.size()==0){
		for(int a=0;a<info.towerNum;a++){
			int num=0;
			vector<int> playernum;
			playernum.push_back(0);
			playernum.push_back(0);
			playernum.push_back(0);
			playernum.push_back(0);
			for(int b=0;b<info.towerNum;b++){
				if(info.lineInfo[b][a].exist&&info.towerInfo[b].owner!=info.myID){
					playernum[info.towerInfo[b].owner]=1;
				}
			}
			vector<int>::iterator player=playernum.begin();
			for(;player!=playernum.end();player++){
				if(*player){
					num++;
				}
			}
			if(info.towerInfo[a].owner!=info.myID&&info.towerInfo[a].owner!=-1){
				double hisregeneration=0.0;
				int enemy=a,myid=info.myID,hisid=info.towerInfo[a].owner;
				int normalline=0;
				for(int i=0;i<info.towerNum;i++){
					if(info.lineInfo[enemy][i].exist){
						if(info.lineInfo[enemy][i].state!=AfterCut){
							normalline++;
						}
					}
					if(info.towerInfo[i].owner==hisid&&info.lineInfo[i][enemy].exist){
						hisregeneration=hisregeneration+(BASE_REGENERETION_SPEED[info.towerInfo[i].type]*LineDecay[info.towerInfo[i].currLineNum]*RegenerationSpeedStage[info.playerInfo[hisid].RegenerationSpeedLevel]);
					}
				}
				hisregeneration=hisregeneration+(BASE_REGENERETION_SPEED[info.towerInfo[enemy].type]*RegenerationSpeedStage[info.playerInfo[hisid].RegenerationSpeedLevel]*StrategyRegeneratePower[info.towerInfo[enemy].strategy]);
				hisregeneration=hisregeneration-(normalline*BASE_REGENERETION_SPEED[info.towerInfo[enemy].type]*LineDecay[normalline]*RegenerationSpeedStage[info.playerInfo[hisid].RegenerationSpeedLevel]);
				for(int i=0;i<mytowerinfo.size();i++){
					vector<int> attacker;
					wetherunderattack(mytowerinfo[i],attacker,info);
					if(info.towerInfo[mytowerinfo[i]].resource>=info.towerInfo[mytowerinfo[i]].maxResource-50&&attacker[0]==0&&currentlinenum(mytowerinfo[i],info)<info.towerInfo[mytowerinfo[i]].maxLineNum-1&&indanger[mytowerinfo[i]]==0&&info.towerInfo[mytowerinfo[i]].strategy!=Defence){
						hisregeneration=hisregeneration-(BASE_REGENERETION_SPEED[info.towerInfo[mytowerinfo[i]].type]*LineDecay[info.towerInfo[mytowerinfo[i]].currLineNum]*RegenerationSpeedStage[info.playerInfo[myid].RegenerationSpeedLevel]*SupressPower[info.towerInfo[mytowerinfo[i]].strategy][info.towerInfo[enemy].strategy]*DefenceStage[info.playerInfo[hisid].DefenceLevel]/1.5);
					}
				}
				round=(info.towerInfo[a].resource)/(hisregeneration);
				round=round*(-1);
				if(round<min&&num<2&&((info.towerInfo[enemy].strategy==Defence&&info.towerInfo[enemy].resource<60)||info.towerInfo[enemy].strategy!=Defence)){
					if(info.round<=100&&info.towerInfo[a].owner!=myopposite(info.myID)){
						min=round;
						target=a;
					}
					if(info.round>100){
						min=round;
						target=a;
					}
				}
			}
		}
		if(min<=80){
			return target;
		}
		else{
			return -1;
		}
	}
	else{
		double mindistance=999.0;
		int ta=0;
		for(int i=0;i<noextraline.size();i++){
			if(mindistance>info.lineInfo[mytower][noextraline[i]].maxlength){
				mindistance=info.lineInfo[mytower][noextraline[i]].maxlength;
				ta=noextraline[i];
			}
		}
		return ta;
	}
}
void wethergiveup(TTowerID mytower,vector<int>&target,Info&info){
	int count=0;
	target.push_back(0);
	for(int i=0;i<info.towerNum;i++){		
		if(info.lineInfo[mytower][i].exist){
			if(info.lineInfo[mytower][i].state==Arrived||info.lineInfo[mytower][i].state==Extending){
				target.push_back(i);
				count++;
			}
		}
	}
	if(count>0){
		target[0]=1;
	}
}
void defencegiveup(TTowerID mytower,vector<int>&attack,Info&info)
{
	int count=0;
	attack.push_back(0);
	for(int i=0;i<info.towerNum;i++){
		if(info.towerInfo[i].owner!=info.myID){
			if(info.lineInfo[mytower][i].exist){
				if(info.lineInfo[mytower][i].state==Extending||info.lineInfo[mytower][i].state==Arrived){
					count++;
					attack.push_back(i);
				}
			}
		}
	}
	if(count>0)attack[0]=1;
}
void wetherconfrontation(TTowerID mytower,vector<int>&attacker,Info&info)
{
	int myid=info.myID;
	attacker.push_back(0);
	int count=0,i=0;
	for(;i<info.towerNum;i++){
		if(info.towerInfo[i].owner!=myid&&info.lineInfo[mytower][i].exist){
			if(info.lineInfo[mytower][i].state==Confrontation){
				attacker.push_back(i);
				count++;
			}
		}
	}
	attacker[0]=count;
}
void wetherattacking(TTowerID mytower,vector<int>&attacking,Info&info)
{
	int count=0;
	attacking.push_back(0);
	for(int i=0;i<info.towerNum;i++){
		if(info.towerInfo[i].owner!=info.myID){
			if(info.lineInfo[mytower][i].exist){
				if(info.lineInfo[mytower][i].state==Arrived||info.lineInfo[mytower][i].state==Extending){
					attacking.push_back(i);
					count++;
				}
			}
		}
	}
	if(count!=0){
		attacking[0]=1;
	}
}
void wetherattackingp(TTowerID mytower,vector<int>&attacking,Info&info)
{
	int count=0;
	attacking.push_back(0);
	for(int i=0;i<info.towerNum;i++){
		if(info.towerInfo[i].owner!=info.myID){
			if(info.lineInfo[mytower][i].exist){
				if(info.lineInfo[mytower][i].state==Arrived){
					attacking.push_back(i);
					count++;
				}
			}
		}
	}
	if(count!=0){
		attacking[0]=1;
	}
}
int regenerationspeedcompare(TTowerID mytower,TTowerID enemy,Info&info)//还未考虑我的塔被进攻的情况,woc这个函数和下个函数的速率计算都tm是错的，woc改吧
{
	int myid=info.myID;
	int hisid=info.towerInfo[enemy].owner;
	vector<int> mytowera;																//输入自己兵塔信息
	set<TTowerID>::iterator Mytower=info.playerInfo[myid].towers.begin();				
	for(;Mytower!=info.playerInfo[myid].towers.end();Mytower++){
		mytowera.push_back(*Mytower);													
	}
	double myregenaration=0.0;
	double hisregeneration=0.0;
	int normalline=0;
	for(int i=0;i<info.towerNum;i++){
		if(info.lineInfo[enemy][i].exist){
			if(info.lineInfo[enemy][i].state!=AfterCut){
				normalline++;
			}
		}
		if(info.towerInfo[i].owner==hisid&&info.lineInfo[i][enemy].exist&&info.lineInfo[i][enemy].state==Arrived){
			hisregeneration=hisregeneration+(BASE_REGENERETION_SPEED[info.towerInfo[i].type]*LineDecay[info.towerInfo[i].currLineNum]*RegenerationSpeedStage[info.playerInfo[hisid].RegenerationSpeedLevel]);
		}
	}
	hisregeneration=hisregeneration+(BASE_REGENERETION_SPEED[info.towerInfo[enemy].type]*RegenerationSpeedStage[info.playerInfo[hisid].RegenerationSpeedLevel]*StrategyRegeneratePower[info.towerInfo[enemy].strategy]);
	hisregeneration=hisregeneration-(normalline*BASE_REGENERETION_SPEED[info.towerInfo[enemy].type]*LineDecay[normalline]*RegenerationSpeedStage[info.playerInfo[hisid].RegenerationSpeedLevel]);
	for(int i=0;i<mytowera.size();i++){
		if(info.lineInfo[mytowera[i]][enemy].exist&&info.lineInfo[mytowera[i]][enemy].state==Arrived){
			hisregeneration=hisregeneration-(BASE_REGENERETION_SPEED[info.towerInfo[mytowera[i]].type]*LineDecay[info.towerInfo[mytowera[i]].currLineNum]*RegenerationSpeedStage[info.playerInfo[myid].RegenerationSpeedLevel]*SupressPower[info.towerInfo[mytowera[i]].strategy][info.towerInfo[enemy].strategy]*DefenceStage[info.playerInfo[hisid].DefenceLevel]/1.5);
		}
	}
	myregenaration=myregenaration+(BASE_REGENERETION_SPEED[info.towerInfo[mytower].type]*RegenerationSpeedStage[info.playerInfo[myid].RegenerationSpeedLevel]*StrategyRegeneratePower[info.towerInfo[mytower].strategy]);
	normalline=0;
	for(int i=0;i<info.towerNum;i++){
		if(info.lineInfo[mytower][i].exist){
			if(info.lineInfo[mytower][i].state!=AfterCut){
				normalline++;
			}
		}
	}
	myregenaration=myregenaration-(normalline*BASE_REGENERETION_SPEED[info.towerInfo[mytower].type]*LineDecay[normalline]*RegenerationSpeedStage[info.playerInfo[myid].RegenerationSpeedLevel]);
	vector<int> confrontator;
	vector<int>&con=confrontator;
	wetherconfrontation(mytower,con,info);
	for(int i=0;i<confrontator.size()-1;i++){
		int hisid=info.towerInfo[confrontator[i+1]].owner;
		myregenaration=myregenaration-(BASE_REGENERETION_SPEED[info.towerInfo[confrontator[i+1]].type]*LineDecay[info.towerInfo[confrontator[i+1]].currLineNum]*RegenerationSpeedStage[info.playerInfo[hisid].RegenerationSpeedLevel]*ConfrontPower[info.towerInfo[confrontator[i+1]].strategy][info.towerInfo[mytower].strategy]);
	}
	vector<int> confrontatora;
	vector<int>&cona=confrontatora;
	wetherunderattack(mytower,cona,info);
	for(int i=0;i<confrontatora.size()-1;i++){
		int hisid=info.towerInfo[confrontatora[i+1]].owner;
		myregenaration=myregenaration-(BASE_REGENERETION_SPEED[info.towerInfo[confrontatora[i+1]].type]*LineDecay[info.towerInfo[confrontatora[i+1]].currLineNum]*RegenerationSpeedStage[info.playerInfo[hisid].RegenerationSpeedLevel]*SupressPower[info.towerInfo[confrontatora[i+1]].strategy][info.towerInfo[mytower].strategy]*DefenceStage[info.playerInfo[info.myID].DefenceLevel])/(1.5);
	}
	if(hisregeneration>=0){
		return 0;
	}
	else{
		hisregeneration=hisregeneration*(-1);
		double roundme;
		double roundhe;
		if(myregenaration>=0){
			return 1;
		}
		else{
			myregenaration=myregenaration*(-1);
			roundme=info.towerInfo[mytower].resource/myregenaration;
			roundhe=info.towerInfo[enemy].resource/hisregeneration;
			if(roundme>roundhe){
				return 1;
			}
			else{
				return 0;
			}
		}
	}
}
void wetherattackcut(TTowerID enenmy,vector<int>&myatt,Info&info)//对方支援未考虑到
{
	int num=0;
	int myid=info.myID;
	int hisid=info.towerInfo[enenmy].owner;
	int keep=0;
	myatt.push_back(-1);
	vector<int> mytower;																//输入自己兵塔信息
	set<TTowerID>::iterator Mytower=info.playerInfo[myid].towers.begin();				
	for(;Mytower!=info.playerInfo[myid].towers.end();Mytower++){
		mytower.push_back(*Mytower);													
	}
	double resource=0.0;
	double minmindistance=999.0;
	for(int i=0;i<mytower.size();i++){//		计算兵线中总兵力
		if(info.lineInfo[mytower[i]][enenmy].exist){
			if(info.lineInfo[mytower[i]][enenmy].state==Arrived){
				resource=resource+(info.lineInfo[mytower[i]][enenmy].resource*(((SupressPower[info.towerInfo[mytower[i]].strategy][info.towerInfo[enenmy].strategy]-1)/2)+1)*DefenceStage[info.playerInfo[hisid].DefenceLevel]/1.5);
				myatt.push_back(mytower[i]);
				if(minmindistance>info.lineInfo[mytower[i]][enenmy].maxlength){
					keep=mytower[i];
					minmindistance=info.lineInfo[mytower[i]][enenmy].maxlength;
				}
			}
		}
	}
	resource=resource-(info.lineInfo[keep][enenmy].resource*(((SupressPower[info.towerInfo[keep].strategy][info.towerInfo[enenmy].strategy]-1)/2)+1)*DefenceStage[info.playerInfo[hisid].DefenceLevel]/1.5);
	double round=0.0;
	double attackresource=0.0,defenceresource=0.0;
	double maxlength=0.0;
	for(int i=0;i<myatt.size()-1;i++){
		if(maxlength<=info.lineInfo[myatt[i+1]][enenmy].maxlength){
			maxlength=info.lineInfo[myatt[i+1]][enenmy].maxlength;
		}
	}
	attackresource=resource;
	round=maxlength*Density/(BaseFrontSpeed*SpeedStage[info.playerInfo[myid].ExtendingSpeedLevel]);
	defenceresource=info.towerInfo[enenmy].resource;
	for(int i=0;i<round;i++){
		defenceresource=defenceresource+BASE_REGENERETION_SPEED[info.towerInfo[enenmy].type]*RegenerationSpeedStage[info.playerInfo[hisid].RegenerationSpeedLevel]*StrategyRegeneratePower[info.towerInfo[enenmy].strategy];
	}
	int linenum=0,aftercutline=0;
	for(int i=0;i<info.towerNum;i++){
		if(info.lineInfo[enenmy][i].exist){
			if(info.lineInfo[enenmy][i].state!=AfterCut){
				linenum++;
			}
			if(info.lineInfo[enenmy][i].state==AfterCut){
				aftercutline++;
			}
		}
	}
	for(int i=0;i<info.towerNum;i++){
		if(info.towerInfo[i].owner==hisid){
			if(info.lineInfo[i][enenmy].exist&&info.lineInfo[i][enenmy].state==Arrived){
				for(int j=0;j<round;j++){
					defenceresource=defenceresource+LineDecay[info.towerInfo[i].currLineNum]*BASE_REGENERETION_SPEED[info.towerInfo[i].type]*RegenerationSpeedStage[info.playerInfo[hisid].RegenerationSpeedLevel];
				}
			}
			if(info.lineInfo[i][enenmy].exist&&info.lineInfo[i][enenmy].state==Extending){
				double round2=0.0;
				round2=((1-((info.lineInfo[i][enenmy].resource)/(info.lineInfo[i][enenmy].maxlength*Density)))*info.lineInfo[i][enenmy].maxlength)/(BaseExtendSpeed*(10.0)*SpeedStage[info.playerInfo[hisid].ExtendingSpeedLevel]);
				int round3=round;
				int round4=round2;
				if(round4<round3){
					for(int j=0;j<round-round2;j++){
						defenceresource=defenceresource+LineDecay[info.towerInfo[i].currLineNum]*BASE_REGENERETION_SPEED[info.towerInfo[i].type]*RegenerationSpeedStage[info.playerInfo[hisid].RegenerationSpeedLevel];
					}
				}
			}
		}
	}
	for(int i=0;i<round;i++){
		defenceresource=defenceresource+aftercutline*BaseBackSpeed*SpeedStage[info.playerInfo[hisid].ExtendingSpeedLevel];
		defenceresource=defenceresource-linenum*LineDecay[info.towerInfo[enenmy].currLineNum]*BASE_REGENERETION_SPEED[info.towerInfo[enenmy].type]*RegenerationSpeedStage[info.playerInfo[hisid].RegenerationSpeedLevel];
	}
	if(attackresource>=defenceresource&&num<1){
		myatt[0]=keep;
	}
}
double resourcejudge(TTowerID source,TTowerID target,Info&info)
{
	double aftersource=0.0;
	aftersource=((info.towerInfo[source].resource+info.towerInfo[target].resource+info.lineInfo[source][target].resource)/2)-info.towerInfo[source].resource;
	return aftersource;
}
int myopposite(int myid)
{
	if(myid==0)return 2;
	if(myid==1)return 3;
	if(myid==2)return 0;
	if(myid==3)return 1;
	return -1;
}
void defence(TTowerID mytower,vector<int> attacker, vector<int> confrontator,vector<int> extender,Info&info)
{
	for(int i=0;i<confrontator[0];i++){
		if((info.towerInfo[confrontator[i+1]].strategy!=Attack)||(info.towerInfo[confrontator[i+1]].strategy==Attack&&info.towerInfo[mytower].resource<100)){
			info.myCommandList.addCommand(cutLine,mytower,confrontator[i+1],info.lineInfo[mytower][confrontator[i+1]].resource);
		}
	}
	if(damagecompare(mytower,attacker,info)!=-1){
		info.myCommandList.addCommand(addLine,mytower,damagecompare(mytower,attacker,info));
	}
	int count=0;
	for(int i=0;i<extender.size()-1;i++){
		if((info.lineInfo[extender[i+1]][mytower].maxlength*Density-info.lineInfo[extender[i+1]][mytower].resource)<=(BaseExtendSpeed*info.playerInfo[info.towerInfo[extender[i+1]].owner].ExtendingSpeedLevel)){
			count++;
		}
	}
	if(count>=2){
		info.myCommandList.addCommand(addLine,mytower,damagecompare(mytower,extender,info));
	}
	else{
		for(int i=0;i<extender.size()-1;i++){
			if((info.lineInfo[extender[i+1]][mytower].maxlength*Density-info.lineInfo[extender[i+1]][mytower].resource)<=(BaseExtendSpeed*info.playerInfo[info.towerInfo[extender[i+1]].owner].ExtendingSpeedLevel)){
				info.myCommandList.addCommand(addLine,mytower,extender[i+1]);
			}
		}
	}
}
int doggie(Info&info)
{
	int myid=info.myID;
	vector<int> mytower;
	set<int>::iterator it=info.playerInfo[myid].towers.begin();
	for(;it!=info.playerInfo[myid].towers.end();it++){
		mytower.push_back(*it);
	}
	int count=0;
	for(int i=0;i<mytower.size();i++){
		if(info.towerInfo[mytower[i]].strategy==Defence){
			count++;
		}
	}
	if(count==mytower.size()&&mytower.size()<3||attackbythreefkingguys(info)==1)	return 1;
	else return 0;
}
int attackbythreefkingguys(Info&info){
	int enemy[3]={0,0,0};
	int wetherattack[3]={0,0,0};
	int count=0;
	int j=0;
	for(int i=0;i<4;i++){
		if(i!=info.myID){
			enemy[j]=i;
			j++;
		}
	}
	vector<int> mytower;
	set<int>::iterator it=info.playerInfo[info.myID].towers.begin();
	for(;it!=info.playerInfo[info.myID].towers.end();it++){
		mytower.push_back(*it);
	}
	for(int i=0;i<3;i++){
		set<int>::iterator it=info.playerInfo[enemy[i]].towers.begin();
		for(;it!=info.playerInfo[enemy[i]].towers.end();++it){
			for(int j=0;j<mytower.size();j++){
				if(info.lineInfo[*it][mytower[j]].exist&&info.lineInfo[*it][mytower[j]].state!=AfterCut){
					wetherattack[i]=1;
					count++;
				}
			}
		}
	}
	if(wetherattack[0]==1&&wetherattack[1]==1&&wetherattack[2]==1&&count>=8){
		return 1;
	}
	else return 0;
}
int currentlinenum(TTowerID mytower,Info&info)
{
	int count=0;
	for(int i=0;i<info.towerNum;i++){
		if(info.lineInfo[mytower][i].exist&&info.lineInfo[mytower][i].state!=AfterCut){
			count++;
		}
	}
	return count;
}
int damagecompare(TTowerID mytower,vector<int>vvector,Info&info)
{
	double maxdamge=0.0;
	int mvp=-1;
	for(int i=0;i<vvector.size()-1;i++){
		double damage=0.0;
		damage=BASE_REGENERETION_SPEED[info.towerInfo[vvector[i+1]].type]*RegenerationSpeedStage[info.playerInfo[info.towerInfo[vvector[i+1]].owner].RegenerationSpeedLevel]*SupressPower[info.towerInfo[vvector[i+1]].strategy][info.towerInfo[mytower].strategy];
		if(maxdamge<damage){
			maxdamge=damage;
			mvp=vvector[i+1];
		}
	}
	return mvp;
}
int allconfrontator(TTowerID mytower,Info&info)
{
	int count=0;
	for(int i=0;i<info.towerNum;i++){
		if(info.lineInfo[mytower][i].exist&&(info.lineInfo[mytower][i].state==Confrontation||info.lineInfo[mytower][i].state==Attacking||info.lineInfo[mytower][i].state==Backing)){
			count++;
		}
	}
	return count;
}
//                       ::
//                      :;J7, :,                        ::;7:
//                      ,ivYi, ,                       ;LLLFS:
//                      :iv7Yi                       :7ri;j5PL
//                     ,:ivYLvr                    ,ivrrirrY2X,
//                     :;r@Wwz.7r:                :ivu@kexianli.
//                    :iL7::,:::iiirii:ii;::::,,irvF7rvvLujL7ur
//                   ri::,:,::i:iiiiiii:i:irrv177JX7rYXqZEkvv17
//                ;i:, , ::::iirrririi:i:::iiir2XXvii;L8OGJr71i
//              :,, ,,:   ,::ir@mingyi.irii:i:::j1jri7ZBOS7ivv,
//                 ,::,    ::rv77iiiriii:iii:i::,rvLq@huhao.Li
//             ,,      ,, ,:ir7ir::,:::i;ir:::i:i::rSGGYri712:
//           :::  ,v7r:: ::rrv77:, ,, ,:i7rrii:::::, ir7ri7Lri
//          ,     2OBBOi,iiir;r::        ,irriiii::,, ,iv7Luur:
//        ,,     i78MBBi,:,:::,:,  :7FSL: ,iriii:::i::,,:rLqXv::
//        :      iuMMP: :,:::,:ii;2GY7OBB0viiii:i:iii:i:::iJqL;::
//       ,     ::::i   ,,,,, ::LuBBu BBBBBErii:i:i:i:i:i:i:r77ii
//      ,       :       , ,,:::rruBZ1MBBqi, :,,,:::,::::::iiriri:
//     ,               ,,,,::::i:  @arqiao.       ,:,, ,:::ii;i7:
//    :,       rjujLYLi   ,,:::::,:::::::::,,   ,:i,:,,,,,::i:iii
//    ::      BBBBBBBBB0,    ,,::: , ,:::::: ,      ,,,, ,,:::::::
//    i,  ,  ,8BMMBBBBBBi     ,,:,,     ,,, , ,   , , , :,::ii::i::
//    :      iZMOMOMBBM2::::::::::,,,,     ,,,,,,:,,,::::i:irr:i:::,
//    i   ,,:;u0MBMOG1L:::i::::::  ,,,::,   ,,, ::::::i:i:iirii:i:i:
//    :    ,iuUuuXUkFu7i:iii:i:::, :,:,: ::::::::i:i:::::iirr7iiri::
//    :     :rk@Yizero.i:::::, ,:ii:::::::i:::::i::,::::iirrriiiri::,
//     :      5BMBBBBBBSr:,::rv2kuii:::iii::,:i:,, , ,,:,:i@petermu.,
//          , :r50EZ8MBBBBGOBBBZP7::::i::,:::::,: :,:,::i;rrririiii::
//              :jujYY7LS0ujJL7r::,::i::,::::::::::::::iirirrrrrrr:ii:
//           ,:  :@kevensun.:,:,,,::::i:i:::::,,::::::iir;ii;7v77;ii;i,
//           ,,,     ,,:,::::::i:iiiii:i::::,, ::::iiiir@xingjief.r;7:i,
//        , , ,,,:,,::::::::iiiiiiiiii:,:,:::::::::iiir;ri7vL77rrirri::
//         :,, , ::::::::i:::i:::i:i::,,,,,:,::i:i:::iir;@Secbone.ii:::
//
//--
//
//
///**