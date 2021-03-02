#include "ai.h"
#include "definition.h"
#include "user_toolbox.h"
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <queue>
#include <time.h>
//���ai����д��player_ai�У��˺���ÿ�غ�����1�Σ�����ÿ�غϻ�ȡ��ҵ�����
//��Ϸͨ��Info��ʵ�θ����player_ai����������Ϣ�����������������š���ͼ�Ȳ�����Ϣ
//���ͨ��info.myCommandList.addCommmand(<��������>,<�����б�:����1,����2...>)�������
//ÿ�غ�������಻����50����myCommandList��addCommand�����Ѿ��Դ������ƣ�����50���������Զ�����
//�������޸Ĳ�����ʹ����󴫻���Ϸ��myCommandlist���г���50�������Ϸ���Զ��ж���ҳ���

struct myCorpsinfo {

	TPoint despos;
	TPoint	pos;		//��������
	int		level;		//���ŵȼ�
	TCorpsID		ID;	//����ID
	THealthPoint	HealthPoint;	//����ֵ
	TBuildPoint		BuildPoint;		//�Ͷ���
	TPlayerID		owner;			//�������ID
	corpsType       type;           //��������
	TMovePoint      movePoint;      //�ж���
	battleCorpsType		m_BattleType;	//ս������
	constructCorpsType	m_BuildType;	//�������
};
struct myCorpsinfo convertinfo(CorpsInfo corp) {
	struct myCorpsinfo mycorp;
	mycorp.pos = corp.pos;
	mycorp.ID = corp.ID;
	mycorp.HealthPoint = corp.HealthPoint;
	mycorp.BuildPoint = corp.BuildPoint;
	mycorp.owner = corp.owner;
	mycorp.type = corp.type;
	mycorp.movePoint = corp.movePoint;
	mycorp.m_BattleType = corp.m_BattleType;
	mycorp.m_BuildType = corp.m_BuildType;
	mycorp.despos.m_x = INF;
	mycorp.despos.m_y = INF;
	return mycorp;
}
struct mymapBlock                                 //��FC18����ͼ������
{
	terrainType type;                           //��FC18���ؿ����ͣ���ӦterrainTypeö����
	vector<int> occupyPoint;                    //��FC18������ҵ�ռ������ֵ����Ϊ������-1
	int owner;                                  //��FC18�����������ţ�-1Ϊ����TRANSITION��-2Ϊ����PUBLIC
	int TowerIndex;								//@@@��FC18��λ�ڸõ�Ԫ��������±꣬��Ӧdata���myTowers
	int isdes;									//�Ƿ���Ŀ�ĵأ������ظ����ñ���������ʮ�غϺ�������
	bool iscons;								//�Ƿ��Ѿ������˻򵽴���
	int movePoint;								//�ƶ��Ѷ�
	TPoint pos;
};
struct tmpMap {
	struct tmpMap* father;
	struct tmpMap* son;
	int point;
};
class myculture {
public:
	myculture() {}
	void updatemyculture(Info& info) {
		for (int i = 0; i < info.corpsInfo.size(); i++) {
			if (info.corpsInfo[i].owner == info.myID) {
				if (info.corpsInfo[i].type == 0) {
					myBattlecorps.push_back(convertinfo(info.corpsInfo[i]));
				}
				else {
					myConstructcorps.push_back(convertinfo(info.corpsInfo[i]));
				}
			}
		}
		for (int i = 0; i < info.towerInfo.size(); i++) {
			if (info.towerInfo[i].ownerID == info.myID) {
				mytower.push_back(info.towerInfo[i]);
			}
		}
	}
	vector<myCorpsinfo> myBattlecorps;
	vector<myCorpsinfo> myConstructcorps;
	vector<TowerInfo> mytower;
};
TPoint hqpos;
int horizdir;
int vertdir;

void updatehq(Info& info) {
	for (int i = 0; i < info.towerInfo.size(); i++) {
		if (info.towerInfo[i].ownerID == info.myID) {
			hqpos = info.towerInfo[i].position;
		}
	}
}

//��ȡĿ��λ��
TPoint getdestiny(TPoint curpos, int mid_x, int mid_y, Info info) {
	bool newp = 0;
	for (int j = 0; j < 30; j++) {
		for (int i = 0; i < info.totalTowers; i++) {
			if (getDist(info.towerInfo[i].position, curpos) < 2) {
				newp = 1;
				break;
			}
		}
		if (newp == 1) {
			newp = 0;
			srand(j);
			curpos.m_x += (rand() % 2) * (horizdir == 3 ? 1 : -1);
			curpos.m_y += (rand() % 2) * (vertdir == 0 ? 1 : -1);
			if (curpos.m_x > 14 || curpos.m_y > 14 || curpos.m_x < 0 || curpos.m_y < 0) {
				curpos.m_x = (horizdir == 3 ? 0 : 14);
				curpos.m_y = (vertdir == 0 ? 0 : 14);
			}
		}
	}
	return curpos;

}

void player_ai(Info& info)
{
	int tasknum = 0;//��¼ִ���˶���������
	myculture cul;
	cul.updatemyculture(info);
	/*if (info.totalRounds == 1) {
		for (int i = 0; i < info.gameMapInfo->size(); i++) {
			for (int j = 0; j < (*info.gameMapInfo)[0].size(); j++) {
				myMap[i][j].isdes = 0;
				myMap[i][j].iscons = 0;
			}
		}
		myMap[hqpos.m_y][hqpos.m_x].isdes = 1;	//�ѱ�ռ�ݻ���ΪĿ��
	}*/
	if (info.myID == 1)
	{
		hqpos.m_x = 0;
		hqpos.m_y = 0;
	}
	else if (info.myID == 2)
	{
		hqpos.m_x = 14;
		hqpos.m_y = 0;
	}
	else if (info.myID == 3)
	{
		hqpos.m_x = 14;
		hqpos.m_y = 14;
	}
	else
	{
		hqpos.m_x = 0;
		hqpos.m_y = 14;
	}
	const int height = 15;
	const int width = 15;
	if (info.totalRounds == 1) {
		if (hqpos.m_x < width / 2)
			horizdir = 3;		//����
		else
			horizdir = 2;
		if (hqpos.m_y < height / 2)
			vertdir = 0;        //����
		else
			vertdir = 1;
	}
	//����ʮ�������쵽ʮ����
	if (cul.mytower.size() + cul.myConstructcorps.size() < 10) {
		for (unsigned int i = 0; i < cul.mytower.size() && i < 10 - cul.mytower.size() - cul.myConstructcorps.size(); ) {
			if (cul.mytower[i].pdtType == -1)
			{
				info.myCommandList.addCommand((commandType)1, vector<int>{0, cul.mytower[i].ID, 4});
				i++;
			}
		}
	}
	for (unsigned int i = 0; i < cul.myConstructcorps.size(); i++) {
		if ((*info.gameMapInfo)[cul.myConstructcorps[i].pos.m_y][cul.myConstructcorps[i].pos.m_x].TowerIndex == -1)
			info.myCommandList.addCommand((commandType)0, vector<int>{7, cul.myConstructcorps[i].ID});
		else {
			TPoint target = getdestiny(cul.myConstructcorps[i].pos, 8, 8, info);
			enum corpsMoveDir dir;
			if (rand() % 2 == 0) {
				if (target.m_x - cul.myConstructcorps[i].pos.m_x > 0) 
					dir = CRight;
				else
					dir = CLeft;
			}
			else {
				if (target.m_y - cul.myConstructcorps[i].pos.m_y > 0) 
					dir = CUp;
				else
					dir = CDown;
			}
			info.myCommandList.addCommand((commandType)0, vector<int>{0, cul.myConstructcorps[i].ID, dir});
		}
	}
	for (unsigned int i = 0; i < cul.mytower.size(); i++) {
		if (cul.mytower[i].pdtType == -1) {
			info.myCommandList.addCommand((commandType)1, vector<int>{0, cul.mytower[i].ID, rand()%3});
		}
	}
	for (unsigned int i = 0; i < cul.myBattlecorps.size(); i++) {
		int RAND = rand() % 2;
		if ( RAND == 0) {
		//�����ƶ�
			TPoint target = getdestiny(cul.myBattlecorps[i].pos, 8, 8, info);
			corpsMoveDir dir;
			if (rand() % 2 == 0) {
				if (target.m_x - cul.myBattlecorps[i].pos.m_x > 0)
					dir = CRight;
				else
					dir = CLeft;
			}
			else {
				if (target.m_y - cul.myBattlecorps[i].pos.m_y > 0)
					dir = CUp;
				else
					dir = CDown;
			}
			info.myCommandList.addCommand(corpsCommand, vector<int>{CMove, cul.myBattlecorps[i].ID, dir});
		}
		else if (RAND == 1) {
			//����פ��
			if ((*info.gameMapInfo)[cul.myBattlecorps[i].pos.m_y][cul.myBattlecorps[i].pos.m_x].TowerIndex != -1)
				info.myCommandList.addCommand(corpsCommand, vector<int>{CStationTower, cul.myBattlecorps[i].ID, info.towerInfo[(*info.gameMapInfo)[cul.myBattlecorps[i].pos.m_y][cul.myBattlecorps[i].pos.m_x].TowerIndex].ID});
		}
	}
	for (unsigned int i = 0; i < cul.mytower.size(); i++) {
		//������
		for (unsigned int j = 0; j < info.totalCorps; j++) {
			if (info.corpsInfo[j].owner != info.myID)
				if (getDist(info.corpsInfo[j].pos, cul.mytower[i].position) < 3)
					info.myCommandList.addCommand(towerCommand, { CAttackCorps, cul.mytower[i].ID, info.corpsInfo[j].ID });
		}
	}
	for (unsigned int i = 0; i < cul.myBattlecorps.size(); i++) {
		//���Ź���
		for (unsigned int j = 0; j < info.totalCorps; j++) {
			if (info.corpsInfo[j].owner != info.myID)
				if (getDist(info.corpsInfo[j].pos, cul.myBattlecorps[i].pos) < 2)
					info.myCommandList.addCommand(corpsCommand, { CAttackCorps, cul.myBattlecorps[i].ID, info.corpsInfo[j].ID });
		}
	}

}