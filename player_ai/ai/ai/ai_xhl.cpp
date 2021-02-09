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
void player_ai(Info& info)
{
	//���޽�����
	//cxy
	vector<int> MyTower;
	vector<int> MyCorps;
	srand(time(NULL));
	for (int i = 0; i < info.towerInfo.size(); i++)
		if (info.towerInfo[i].ownerID == info.myID)MyTower.push_back(i);
	for (int i = 0; i < info.corpsInfo.size(); i++)
		if (info.corpsInfo[i].owner == info.myID)MyCorps.push_back(i);
	for (int i = 0; i < MyTower.size(); i++)
		if (info.towerInfo[MyTower[i]].pdtType != -1)
		{
			if (MyTower.size() <= 9)info.myCommandList.addCommand(towerCommand, { TProduct, MyTower[i], PExtender });
			else info.myCommandList.addCommand(towerCommand, { TProduct, MyTower[i], PUpgrade });
		}
	for (int i = 0; i < MyCorps.size(); i++)
		if (info.corpsInfo[MyCorps[i]].movePoint > 0)
		{
			int nowx = info.corpsInfo[MyCorps[i]].pos.m_x, nowy = info.corpsInfo[MyCorps[i]].pos.m_y;
			if ((*info.gameMapInfo)[nowy][nowx].owner == info.myID && (*info.gameMapInfo)[nowy][nowx].type != TRTower)
			{
				info.myCommandList.addCommand(corpsCommand, { CBuild, MyCorps[i] });
			}
			else
			{
				info.myCommandList.addCommand(corpsCommand, { CMove, MyCorps[i], rand() % 4 });
			}
		}
}