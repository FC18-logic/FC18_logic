//#pragma once
#ifndef _DATA_H
#define _DATA_H
/*
**  Data�ṹ��
**  �������ݵĴ洢�͹���
**  ���ⲿ�洢�����ֻ��洢 ID �� Data �е���
*/
class Cell;
class Tentacle;
class Player;


#include "map.h"
#include "../json/json.h"

struct CutTentacleInfoJson
{
	int cutID = -1;
	double frontResource = -1;
};

namespace DATA//���ռ�
{
	struct Data
	{
		Cell* cells;//��������ָ��
		int CellNum;//����

		Tentacle*** tentacles;//���б��ߵ�ָ�루��������һ����λ�����ģ�ÿ�����߾���i->j��
		int TentacleNum;//������

		Player* players;//������ҵ�ָ��
		int PlayerNum;//�����

		Json::Value root;//JSON���ڵ�
		Json::Value currentRoundJson;//��ǰ�غ�JSON�ĸ��ڵ�
		vector<vector<CutTentacleInfoJson>>  cutTentacleInfoJson;//��ά����洢ÿ�����ߵ��ж���Ϣ
		vector<vector<bool>> cutTentacleJson;  //��ά����ÿ�������жϱ�־
		vector<vector<bool>> cutTentacleBornJson;  // ��ά����ÿ���������ɱ�־

		Map gameMap;//��ǰ�ĵ�ͼ
	};
}

#endif // !_DATA_H