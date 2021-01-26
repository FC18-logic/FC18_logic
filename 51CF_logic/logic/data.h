//#pragma once
#ifndef _DATA_H
#define _DATA_H
/*
**  Data�ṹ��
**  �������ݵĴ洢�͹���
**  ���ⲿ�洢�����ֻ��洢 ID �� Data �е���
*/
//��������FC15������Ӧ���ò�����
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Cell;//ԭ����ϸ����
class Tentacle;//ԭ���Ĵ�����
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//@@@��FC18���������࣬���������������������ͷ�ļ���ο�ԭ����class Cell

//��FC18�������࣬���������������������ͷ�ļ���ο�ԭ����class Tentacle
class Crops;//��FC18��������
class Tower;//��FC18����������
class Player;//��FC18�������


#include "map.h"
#include "../json/json.h"

struct CutTentacleInfoJson
{
	int cutID = -1;
	double frontResource = -1;
};

namespace DATA                                         //@@@��FC18�����ռ�
{
	struct Data                                        //@@@��FC18��Data�ṹ�壬�������ڴ��н�������������ݹ����洢
	{
		TTower totalTowers;                            //��FC18���ܵķ�������
		//��FC18��ָ�����з�����������/vector���ɣ���ָ�룬���Բο�ԭ����Cell* cells
		vector<class Tower> myTowers;                  //��FC18�����з�����������
		
		
		TCorps totalCorps;                             //��FC18���ܵı�����
		vector<Crops> myCorps;                         //��FC18�����б��Ű�����Ų�
		Army corps;                                    //��FC18��ÿ������ı��ű�
		
		
		TPlayer totalPlayers;                          //��FC18�������
		//@@@��FC18��ָ��������ң�����/vector���ɣ���ָ�룬���Բο�ԭ����Player* players
		Player* players;//������ҵ�ָ��

		Map gameMap;                                   //@@@��FC18����ǰ�ĵ�ͼ
		TRound totalRounds;                            //��FC18����ǰ�Ļغ���

		//����дJSON�ĵ�
		Json::Value commandJsonRoot;                   //��FC18�����лغ������Json���ڵ�
		Json::Value infoJsonRoot;                      //��FC18�����лغ���ҡ��������ͱ���Json�ĸ��ڵ�
		Json::Value mapInfoJsonRoot;                   //��FC18�����лغϵ�ͼ��Ϣ��Json���ڵ�
		Json::Value currentRoundCommandJson;           //��FC18����ǰ�غ������Json���ڵ�
		Json::Value currentRoundPlayerJson;            //��FC18����ǰ�غ���ҵ�Json���ڵ�
		Json::Value currentRoundTowerJson;             //��FC18����ǰ�غϷ�������Json���ڵ�
		Json::Value currentRoundCorpsJson;             //��FC18����ǰ�غϱ��ŵ�Json���ڵ�
		Json::Value currentRoundMapJson;               //��FC18����ǰ�غϵ�ͼ��Json���ڵ�



		//FC15��
		int CellNum;//����
		int TentacleNum;//������
		TPlayer PlayerNum;//�����
		Cell* cells;//��������ָ��
		vector<vector<CutTentacleInfoJson>>  cutTentacleInfoJson;//��ά����洢ÿ�����ߵ��ж���Ϣ
		vector<vector<bool>> cutTentacleJson;  //��ά����ÿ�������жϱ�־
		vector<vector<bool>> cutTentacleBornJson;  // ��ά����ÿ���������ɱ�־
		Tentacle*** tentacles;//��FC15�����б��ߵ�ָ�루��������һ����ά�����ģ�ÿ�����߾���i->j��
		Json::Value root;                              //JSON���ڵ�
		Json::Value currentRoundJson;                  //��ǰ�غ�JSON�ĸ��ڵ㣬saveJson()��ʱ��嵽��Json�ĸ��ڵ���
	};
}

#endif // !_DATA_H