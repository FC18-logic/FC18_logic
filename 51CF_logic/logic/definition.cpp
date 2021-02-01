#include "definition.h"
#include <cmath>
#include <algorithm>
TPoint operator-(const TPoint& p1, const TPoint& p2)
{
	TPoint ret;
	ret.m_x = p1.m_x - p2.m_x;
	ret.m_y = p1.m_y - p2.m_y;
	return ret;
}

TLength getDistance(const TPoint& p1, const TPoint& p2)
{
	TPoint distance = p1 - p2;
	return sqrt(distance.m_x*distance.m_x + distance.m_y*distance.m_y);
}

int generateRanInt(int start, int end) {
	return rand() % (end - start + 1) + start;
}

std::ostream& operator<<(std::ostream& os, const CommandList& cl)
{
	//FC15�ɴ���
	return os;
}


/***********************************************************************************************
*������ :��FC18��getDist��ȡ���뺯������
*������������ : ����FC18��������������֮��ľ���
*�������� : p1<const TPoint&>--��1���㣬p2<const TPOint&>--��2����
*��������ֵ : <TPoint>����
*���� : ������
***********************************************************************************************/
TDist getDist(const TPoint& p1, const TPoint& p2) {
	TPoint dp = p1 - p2;
	int dx = dp.m_x;
	int dy = dp.m_y;
	return max(abs(dx), abs(dy));
}


/***********************************************************************************************
*������ :��FC18��getDist��ȡ���뺯������
*������������ : ͨ���ļ����뵱ǰ��Ϸ�ĵ�ͼ���ݡ�����������ݣ�����ʼ��������飬д����غ�����
				Json
*�������� : inMap<ifstream&> �����ļ�������enableOutput<bool> �Ƿ����������Ϣ��true--����
			false--������
*��������ֵ : false--�����ͼʧ�ܣ�true--�����ͼ�ɹ�
*���� : ������
***********************************************************************************************/
TDist getDist(const int p1_x, const int p1_y, const int p2_x, const int p2_y) {
	int dx = p1_x - p2_x;
	int dy = p1_y - p2_y;
	return max(abs(dx), abs(dy));
}