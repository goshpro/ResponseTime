/*
#include "ResponseTimeData.h"

bool ResponseTimeData::setCohortResults ( //���������� ���������� ����������� ������� �� ��������� ��� ������. ����� false, ���� ���������� ������ ��� ������ ����������� ��� ���������� ���������� ����� (��� �� ��������, ���� ��������� ���� �������)
	Timestamp currentTimestamp, //������� �����
	size_t cohortIndex, //����� ������� (0, 1 ��� 2)
	NumberOfTickets * aCohortResults, //������ � ������������
	size_t aCohortResultsLength //����� ������������� �������
	) {
	lastTimestamp = currentTimestamp; //��������� �����
	for(size_t i = 0; i < aCohortResultsLength; ++i) { //��������� ���� ���������� ������
		m_aCohortResults[cohortIndex][findHour (i)] = aCohortResults[i]; //���������� ��������������� ������� ������ ���������
	}
	m_currentHour = findHour (aCohortResultsLength); //� �������� �������� ���� ���������� ������ ������� ��������������� ��������
	return aCohortResultsLength >= HISTORY_LENGTH - BONUS_HISTORY - PROGNOSIS_DEPTH;
}
*/