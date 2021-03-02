#include "ai.h"
#include "definition.h"
#include "user_toolbox.h"
#include <iostream>
#include <vector>
//���ai����д��player_ai�У��˺���ÿ�غ�����1�Σ�����ÿ�غϻ�ȡ��ҵ�����
//��Ϸͨ��Info��ʵ�θ����player_ai����������Ϣ�����������������š���ͼ�Ȳ�����Ϣ
//���ͨ��info.myCommandList.addCommmand(<��������>,<�����б�:����1,����2...>)�������
//ÿ�غ�������಻����50����myCommandList��addCommand�����Ѿ��Դ������ƣ�����50���������Զ�����
//�������޸Ĳ�����ʹ����󴫻���Ϸ��myCommandlist���г���50�������Ϸ���Զ��ж���ҳ���

struct Memo {
	int myID = -1;
	int myTowerCount = 0;
	int myBattleCount = 0;
	int myConstructCount = 0;
	int commandCount = 0;
};

Memo MemoInitial(Info& info) {
	Memo memo;
	memo.myID = info.myID;
	for (int i = 0; i < info.corpsInfo.size(); ++i) {
		if (info.corpsInfo[i].owner == memo.myID && info.corpsInfo[i].exist) {
			if (info.corpsInfo[i].type == Battle) { memo.myBattleCount++; }
			else { memo.myConstructCount++; }
		}
	}
	for (int i = 0; i < info.towerInfo.size(); ++i) {
		if (info.towerInfo[i].ownerID == memo.myID && info.towerInfo[i].exist) {
			memo.myTowerCount++;
		}
	}
	return memo;
}

bool isMyTower(TowerInfo& tower, int myID) {
	return tower.ownerID == myID && tower.exist;
}

bool isValidTerritory(int x, int y) {
	return x >= 0 && x <= 14 && y >= 0 && y <= 14;
}

bool isMyTerritory(int x, int y, Info& info) {
	if (!isValidTerritory(x, y)) return false;
	return ((*info.gameMapInfo)[y][x].owner == info.myID);
}

std::vector<CorpsInfo> GetCorp(int x, int y, Info& info) {
	vector<CorpsInfo> ret;
	if (!isValidTerritory(x, y)) return ret;
	for (int i = 0; i < (*info.gameMapInfo)[y][x].corps.size(); ++i) {
		ret.push_back(info.corpsInfo[(*info.gameMapInfo)[y][x].corps.at(i)]);
	}
	return ret;
}

TowerInfo GetTower(int x, int y, Info& info) {
	TowerInfo tower;
	tower.exist = false;
	if (!isValidTerritory(x, y)) return tower;
	if ((*info.gameMapInfo)[y][x].TowerIndex == NOTOWER) return tower;
	return info.towerInfo[(*info.gameMapInfo)[y][x].TowerIndex];
}

bool CanBuildTowerAt(int x, int y, Info& info, Memo& memo) {
	if (memo.myTowerCount >= 10 || memo.commandCount >= 50) return false; // �Ѿ��ﵽ�����������ޡ�
	if (!isMyTerritory(x, y, info)) return false; // �����ҵ���ز��ܽ��졣
	if (GetTower(x, y, info).exist) return false; // ����Ѿ������˲��ܽ��졣
	return true;
}

bool TowerNeedRepair(TowerInfo& tower, Info& info) {
	if (tower.healthPoint <= 0.67 * (tower.level * 20 + 80)) { // ������������С�ڵȼ���Ѫ��2/3ʱ����Ҫ�޸�
		return true;
	}
	return false;
}

TCorpsID FindTowerTarget(TowerInfo& tower, Info& info) { // Ѱ��tower��Χ�ڹ�����Χ�ڣ����ڵз��ı��ţ���������š���������ڣ��򷵻�-1
	CorpsInfo corp;
	for (int ix = tower.position.m_x - 2 > 0 ? tower.position.m_x - 2 : 0; ix <= tower.position.m_x + 2 && ix < 15; ++ix) {
		for (int iy = tower.position.m_y - 2 > 0 ? tower.position.m_y - 2 : 0; iy <= tower.position.m_y + 2 && iy < 15; ++iy) {
			if (GetTower(ix, iy, info).exist) continue; // ��������Ԫ���Ѿ������ˣ���ô�Ͳ��ܹ��������Ԫ���ϵı��š�
			for (int i = 0; i < (*info.gameMapInfo)[iy][ix].corps.size(); ++i) {
				corp = GetCorp(ix, iy, info)[i];
				if (corp.exist && corp.owner != info.myID) {
					return corp.ID;
				}
			}
		}
	}
	return -1;
}

bool isMyExtender(CorpsInfo& corp, Info& info) {
	return corp.owner == info.myID && corp.exist && corp.type == Construct && corp.m_BuildType == Extender;
}

bool isMyBuilder(CorpsInfo& corp, Info& info) {
	return corp.owner == info.myID && corp.exist && corp.type == Construct && corp.m_BuildType == Builder;
}

TCorpsID FindBattlerTarget(CorpsInfo& corp, Info& info) {
	// TODO
	return -1;
}

void player_ai(Info& info)
{
	Memo memo = MemoInitial(info); // ����ʵʱ�жϵ�ǰ�������Ƿ�ﵽ���ޣ����������Ƿ�ﵽ����

	// ����
	for (int i = 0; i < info.corpsInfo.size(); ++i) {
		// �������ţ��ҵ����ҵĿ�����
		if (isMyExtender(info.corpsInfo[i], info)) {
			//�ڿ����ߵ���Χ�������½����ĵз�����ǰ���½�����
			int x = info.corpsInfo[i].pos.m_x;
			int y = info.corpsInfo[i].pos.m_y;
			if (CanBuildTowerAt(x, y, info, memo)) {
				info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[i].ID }); // �ʣ��˴���info.corpsInfo[i].ID�ǲ���ʼ�յ���i��
				memo.myTowerCount++;
				memo.myConstructCount--;
				memo.commandCount += 1;
			}
			else if (CanBuildTowerAt(x + 1, y, info, memo)) {
				info.myCommandList.addCommand(corpsCommand, { CMove, info.corpsInfo[i].ID, CRight });
				info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[i].ID });
				memo.myTowerCount++;
				memo.myConstructCount--;
				memo.commandCount += 2;
			}
			else if (CanBuildTowerAt(x - 1, y, info, memo)) {
				info.myCommandList.addCommand(corpsCommand, { CMove, info.corpsInfo[i].ID, CLeft });
				info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[i].ID });
				memo.myTowerCount++;
				memo.myConstructCount--;
				memo.commandCount += 2;
			}
			else if (CanBuildTowerAt(x, y + 1, info, memo)) {
				info.myCommandList.addCommand(corpsCommand, { CMove, info.corpsInfo[i].ID, CDown });
				info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[i].ID });
				memo.myTowerCount++;
				memo.myConstructCount--;
				memo.commandCount += 2;
			}
			else if (CanBuildTowerAt(x, y - 1, info, memo)) {
				info.myCommandList.addCommand(corpsCommand, { CMove, info.corpsInfo[i].ID, CUp });
				info.myCommandList.addCommand(corpsCommand, { CBuild, info.corpsInfo[i].ID });
				memo.myTowerCount++;
				memo.myConstructCount--;
				memo.commandCount += 2;
			}
		}
	}

	// ���Ĳ���
	TowerInfo tower;
	for (int x = 0; x < 15; ++x) {
		for (int y = 0; y < 15; ++y) { // ˫��ѭ��������ͼ
			tower = GetTower(x, y, info);
			//��������������������ҵ�����������Ҫά��
			if (isMyTower(tower, memo.myID) && TowerNeedRepair(tower, info)) { 
				//�������ҿ��Ƿ��Ѿ����ҵ�Builder����ά���������
				for (int j = 0; j < GetCorp(x, y, info).size(); ++j) {
					CorpsInfo corp = GetCorp(x, y, info)[j];
					if (isMyBuilder(corp, info)) {
						info.myCommandList.addCommand(corpsCommand, { CRepair, corp.ID });
						memo.myConstructCount--;
						memo.commandCount++;
						break;
					}
				}
				// ����builder
				info.myCommandList.addCommand(towerCommand, { TProduct, tower.ID, PBuilder });
				memo.commandCount++;
				break;
			}
			//�����������������ҵ�����������Ҫά��
			else if (isMyTower(tower, memo.myID)) {
				// ��ô���������Χ���Թ��ҽ�����
				if (CanBuildTowerAt(x + 1, y, info, memo)
					|| CanBuildTowerAt(x, y + 1, info, memo)
					|| CanBuildTowerAt(x - 1, y, info, memo)
					|| CanBuildTowerAt(x, y - 1, info, memo)) { // С���⣺��ʵ��Ӧ���������ж��������������޵ģ��������ⲻ��
					info.myCommandList.addCommand(towerCommand, { TProduct, tower.ID, PExtender }); // ���½������ߣ��������������½���ɣ��������һ���غ�ֱ���ƶ����õ�Ԫ���½�����
					memo.commandCount++;
					// memo.myConstructCount++; �����ûд�ã���Ϊ�����Ǽ�һ������֮��ÿ�ζ����½������ţ�����memo����ŵ������������ã������ⲻ��
				}
				// ����ҵ���Χ�����Խ��������е��˱���
				else if (int id_tmp = FindTowerTarget(tower, info) != -1) {
					info.myCommandList.addCommand(towerCommand, { TAttackCorps, tower.ID, id_tmp }); //�򹥻��������
					memo.commandCount++;
				}
				// ��������£�����һ�û��������߼�
				else if (tower.level < 8) {
					info.myCommandList.addCommand(towerCommand, { TProduct, tower.ID, PUpgrade }); // �������������
					memo.commandCount++;
				}
				else {
					info.myCommandList.addCommand(towerCommand, { TProduct, tower.ID, PWarrior }); // ������������������սʿ�ɡ�
					memo.commandCount++;
				}
			}
		}
	}

	

}