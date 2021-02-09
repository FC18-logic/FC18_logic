#include "ai.h"
#include "definition.h"
#include "user_toolbox.h"
#include <iostream>
#include <vector>
#include <ctime>
//���ai����д��player_ai�У��˺���ÿ�غ�����1�Σ�����ÿ�غϻ�ȡ��ҵ�����
//��Ϸͨ��Info��ʵ�θ����player_ai����������Ϣ�����������������š���ͼ�Ȳ�����Ϣ
//���ͨ��info.myCommandList.addCommmand(<��������>,<�����б�:����1,����2...>)�������
//ÿ�غ�������಻����50����myCommandList��addCommand�����Ѿ��Դ������ƣ�����50���������Զ�����
//�������޸Ĳ�����ʹ����󴫻���Ϸ��myCommandlist���г���50�������Ϸ���Զ��ж���ҳ���
int canfight(Info& info, int q, int x, int y, int myID) {//�ж��ܷ񹥴򲢷��صз�����ID
	if (!(x >= 0 && x <= 14 && y >= 0 && y <= 14)) return -1;
	vector<int> corps = (*info.gameMapInfo)[y][x].corps;//���û�����ñ������vector��ʲô�����أ�
	for (int i = 0; i < corps.size(); i++) {
		if (info.corpsInfo[corps[i]].owner != myID) return info.corpsInfo[corps[i]].ID;//��λ����������ҵı���
	}
	return -1;
}
void player_ai(Info& info)
{
	int myID = info.myID;
	int command_num = 0;										//������
	int Tower_num = info.totalTowers;								//��FC18���ܵķ���������
	int Corps_num = info.totalCorps;								//��FC18���ܵı��Ÿ���
	int my_Tower_num = info.playerInfo[myID - 1].tower.size();	//���ʸ��������������Ŀ
	int my_Corps_num = info.playerInfo[myID - 1].corps.size();	//���ʸ�������б��ŵ���Ŀ														
	int my_Buider_num = 0;										// �ҵķ���������
	int my_Extender_num = 0;
	int my_Warrior_num = 0;
	int my_Archer_num = 0;
	int my_Cavalry_num = 0;
	set<int> my_Tower = info.playerInfo[myID - 1].tower;         //�ҵķ����������
	set<int> my_Corps = info.playerInfo[myID - 1].corps;		//�ҵı��������
	vector<int> my_Buider;
	vector<int> my_Extender;
	vector<int> my_Warrior;
	vector<int> my_Archer;
	vector<int> my_Cavalry;
	set<int>::iterator tower_it;							//���ڷ����ҵ����ĵ�����
	set<int>::iterator corps_it;							//���ڷ����ҵı��ŵĵ�����
	vector<TPoint> my_land;                                 //�������ҷ�����������
	for (corps_it = my_Corps.begin(); corps_it != my_Corps.end(); corps_it++) {//�����ҵı��ţ��ҳ����ǵ�����
		int i = *corps_it;
		if (info.corpsInfo[i].exist) {
			if (info.corpsInfo[i].type == Construct) {
				if (info.corpsInfo[i].m_BuildType == Builder) {
					my_Buider_num++;
					my_Buider.push_back(i);
				}
				if (info.corpsInfo[i].m_BuildType == Extender) {
					my_Extender_num++;
					my_Extender.push_back(i);
				}
			}
			if (info.corpsInfo[i].type == Battle) {
				if (info.corpsInfo[i].m_BattleType == Warrior) {
					my_Warrior_num++;
					my_Warrior.push_back(i);
				}
				if (info.corpsInfo[i].m_BattleType == Archer) {
					my_Archer_num++;
					my_Archer.push_back(i);
				}
				if (info.corpsInfo[i].m_BattleType == Cavalry) {
					my_Cavalry_num++;
					my_Cavalry.push_back(i);
				}
			}
		}
	}
	//�ҵ������Լ���ص�����
	for (int x = 0; x < 15; x++) {
		for (int y = 0; y < 15; y++) {
			if ((*info.gameMapInfo)[y][x].owner == myID) {
				TPoint q;
				q.m_x = x;
				q.m_y = y;
				my_land.push_back(q);
			}
		}
	}
	//��Extender������
	for (int j = 0; j < my_Extender_num; j++) {
		int q = my_Extender[j];
		int E_x = info.corpsInfo[q].pos.m_x;
		int E_y = info.corpsInfo[q].pos.m_y;
		if (info.playerInfo[myID - 1].tower.size() < 10 && command_num < 50) {
			for (int k = 0; k < my_land.size(); k++) {
				int x = my_land[k].m_x;
				int y = my_land[k].m_y;
				if ((*info.gameMapInfo)[y][x].TowerIndex == -1) {//��������û����

					//�Ҿ���Ӧ�ü�һ�����ر���ÿ�ƶ�һ�������ĵ��ж����ĺ���
					//�����������������ƶ�һ�����
					if (x == E_x && y == E_y) {
						info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[q].ID });
						command_num++;
						my_Extender_num--;
						my_Tower_num++;
					}
					if (x == E_x + 1 && y == E_y) {
						info.myCommandList.addCommand(corpsCommand, { CMove, info.corpsInfo[q].ID, CRight });
						if (info.corpsInfo[q].movePoint >= 1) {
							info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[q].ID });
							command_num = command_num + 2;
						}
						else command_num++;
						my_Extender_num--;
						my_Tower_num++;
					}
					if (x == E_x - 1 && y == E_y) {
						info.myCommandList.addCommand(corpsCommand, { CMove, info.corpsInfo[q].ID, CLeft });
						if (info.corpsInfo[q].movePoint >= 1) {
							info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[q].ID });
							command_num = command_num + 2;
						}
						else command_num++;
						my_Extender_num--;
						my_Tower_num++;
					}
					if (x == E_x && y == E_y + 1) {//��û̫���������任���������ҵĹ�ϵ���п���д��
						info.myCommandList.addCommand(corpsCommand, { CMove, info.corpsInfo[q].ID, CDown });
						if (info.corpsInfo[q].movePoint >= 1) {
							info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[q].ID });
							command_num = command_num + 2;
						}
						else command_num++;
						my_Extender_num--;
						my_Tower_num++;
					}
					if (x == E_x && y == E_y - 1) {
						info.myCommandList.addCommand(corpsCommand, { CMove, info.corpsInfo[q].ID, CUp });
						if (info.corpsInfo[q].movePoint >= 1) {
							info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[q].ID });
							command_num = command_num + 2;
						}
						else command_num++;
						my_Extender_num--;
						my_Tower_num++;
					}

				}
			}
		}
		else break;
	}
	//��Buider������
	for (int j = 0; j < my_Buider_num; j++) {
		if (command_num > 50) break;
		int q = my_Buider[j];
		int x = info.corpsInfo[q].pos.m_x;
		int y = info.corpsInfo[q].pos.m_y;
		if ((*info.gameMapInfo)[y][x].TowerIndex != -1) {
			int TI = (*info.gameMapInfo)[y][x].TowerIndex;
			if (info.towerInfo[TI].healthPoint < 0.33 * (info.towerInfo[TI].level * 20 + 80)) {
				info.myCommandList.addCommand(corpsCommand, { CRepair, info.corpsInfo[q].ID });
				command_num++;
			}
		}
		else if ((*info.gameMapInfo)[y][x].type == TRPlain) {  //����Ϊƽԭ
			info.myCommandList.addCommand(corpsCommand, { CChangeTerrain,info.corpsInfo[q].ID, TRForest });
			command_num++;
		}
	}
	//����ս����������
	for (int j = 0; j < my_Warrior_num; j++) {
		if (command_num > 50) break;
		int q = my_Warrior[j];//���Լ��Ĺ�����Χ����Ѱ������ҵı���
		int x = info.corpsInfo[q].pos.m_x;
		int y = info.corpsInfo[q].pos.m_y;
		for (int dx = -TBattleRange[0]; dx <= TBattleRange[0]; dx++) {
			for (int dy = -TBattleRange[0]; dy <= TBattleRange[0]; dy++) {
				int p = canfight(info, q, x + dx, y + dy, myID);
				if (p != -1) {
					info.myCommandList.addCommand(corpsCommand, { CAttackCorps,info.corpsInfo[q].ID, p });
					command_num++;
				}
			}
		}
	}
	for (int j = 0; j < my_Archer_num; j++) {
		if (command_num > 50) break;
		int q = my_Archer[j];//���Լ��Ĺ�����Χ����Ѱ������ҵı���
		int x = info.corpsInfo[q].pos.m_x;
		int y = info.corpsInfo[q].pos.m_y;
		for (int dx = -TBattleRange[1]; dx <= TBattleRange[1]; dx++) {
			for (int dy = -TBattleRange[1]; dy <= TBattleRange[1]; dy++) {
				int p = canfight(info, q, x + dx, y + dy, myID);
				if (p != -1) {
					info.myCommandList.addCommand(corpsCommand, { CAttackCorps,info.corpsInfo[q].ID, p });
					command_num++;
				}
			}
		}
	}
	for (int j = 0; j < my_Cavalry_num; j++) {
		if (command_num > 50) break;
		int q = my_Cavalry[j];//���Լ��Ĺ�����Χ����Ѱ������ҵı���
		int x = info.corpsInfo[q].pos.m_x;
		int y = info.corpsInfo[q].pos.m_y;
		for (int dx = -TBattleRange[2]; dx <= TBattleRange[2]; dx++) {
			for (int dy = -TBattleRange[2]; dy <= TBattleRange[2]; dy++) {
				int p = canfight(info, q, x + dx, y + dy, myID);
				if (p != -1) {
					info.myCommandList.addCommand(corpsCommand, { CAttackCorps,info.corpsInfo[q].ID, p });
					command_num++;
				}
			}
		}
	}
	//������
	for (tower_it = my_Tower.begin(); tower_it != my_Tower.end(); tower_it++) {
		if (command_num > 50) break;
		int i = *tower_it;
		int x = info.towerInfo[i].position.m_x;
		int y = info.towerInfo[i].position.m_y;
		int flag = 0;//����ÿ����ֻ�����һ��������Ա��һ��
		if (/*info.towerInfo[i].pdtType == -1*/1) {
			if (info.towerInfo[i].healthPoint < 0.67 * (info.towerInfo[i].level * 20 + 80)) {//�������ֵ����������������
				info.myCommandList.addCommand(towerCommand, { TProduct, info.towerInfo[i].ID, PBuilder });
				command_num++;
				flag = 1;
			}
			if (!flag && my_Tower_num < 2) {//���������������������
				info.myCommandList.addCommand(towerCommand, { TProduct, info.towerInfo[i].ID, PExtender });
				command_num++;
				flag = 1;
			}
			if (!flag && my_Warrior_num < 2) {//���սʿ����������սʿ
				info.myCommandList.addCommand(towerCommand, { TProduct, info.towerInfo[i].ID, PWarrior });
				command_num++;
				flag = 1;
			}
			if (!flag && my_Archer_num < 2) {//սʿ���˵��ǹ����ֲ���
				info.myCommandList.addCommand(towerCommand, { TProduct, info.towerInfo[i].ID, PArcher });
				command_num++;
				flag = 1;
			}
			if (!flag && my_Cavalry_num < 2) {//ֻ�з�ʦ����
				info.myCommandList.addCommand(towerCommand, { TProduct, info.towerInfo[i].ID, PCavalry });
				command_num++;
				flag = 1;
			}
		}
		if (!flag) {//ʵ��û����������
			if (info.towerInfo[i].level < 8) {
				info.myCommandList.addCommand(towerCommand, { TProduct, info.towerInfo[i].ID, PUpgrade });
			}
		}
		for (int dx = -2; dx <= 2; dx++) {//����з������ڹ�����Χ�ڣ��򹥴�
			for (int dy = -2; dy <= 2; dy++) {
				int p = canfight(info, i, x + dx, y + dy, myID);
				if (p != -1) {
					info.myCommandList.addCommand(towerCommand, { CAttackCorps,info.towerInfo[i].ID, p });
					command_num++;
					flag = 1;
				}
			}
		}
	}

}
