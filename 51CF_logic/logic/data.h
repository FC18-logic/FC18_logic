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

//@@@��FC18�������࣬���������������������ͷ�ļ���ο�ԭ����class Tentacle

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
		//@@@��FC18��ָ�����з�����������/vector���ɣ���ָ�룬���Բο�ԭ����Cell* cells
		Cell* cells;//��������ָ��
		
		TCorps totalCorps;                             //��FC18���ܵı�����
		//@@@��FC18��ָ�����б��ţ�����/vector���ɣ���ָ�룬���Բο�ԭ����Tentacle*** tentacles
		Tentacle*** tentacles;//���б��ߵ�ָ�루��������һ����ά�����ģ�ÿ�����߾���i->j��
		
		TPlayer totalPlayers;                          //��FC18�������
		//@@@��FC18��ָ��������ң�����/vector���ɣ���ָ�룬���Բο�ԭ����Player* players
		Player* players;//������ҵ�ָ��

		Map gameMap;                                   //@@@��FC18����ǰ�ĵ�ͼ

		//����дJSON�ĵ�
		Json::Value root;                              //��FC18��JSON���ڵ�
		Json::Value currentRoundJson;                  //��FC18����ǰ�غ�JSON�ĸ��ڵ㣬saveJson()��ʱ��嵽��Json�ĸ��ڵ���


		//FC15��
		int CellNum;//����
		int TentacleNum;//������
		TPlayer PlayerNum;//�����
		vector<vector<CutTentacleInfoJson>>  cutTentacleInfoJson;//��ά����洢ÿ�����ߵ��ж���Ϣ
		vector<vector<bool>> cutTentacleJson;  //��ά����ÿ�������жϱ�־
		vector<vector<bool>> cutTentacleBornJson;  // ��ά����ÿ���������ɱ�־
	};
}

#endif // !_DATA_H