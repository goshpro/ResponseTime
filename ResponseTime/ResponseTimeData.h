#include <iostream>

typedef unsigned int NumberOfTickets;
typedef int Timestamp; //�����������, ��� ����� ������������ unix timestamp (������� �� 1 ������ 1970 ����)

class ResponseTimeData {
public:
	static unsigned int const NUMBER_OF_COHORTS = 3; //���������� ������

	static unsigned int const HOURS_IN_DAY = 24; //���������� �����������, �� ������� ����������� �����. ����� ���������� ������������� ��������, � ������� ����� ����������� ����� �������� (�� �����������, ����� �� ��� ��� �� 1 ���, ��� ��� ����� ��� ������ ����� ����� ��������������� ������ ��� ��������). ���� ���� ��������� �����, �� ���������� ��������� 86400, �� �������� ����� �������� �������.
	static unsigned int const MIN_HISTORY_LENGTH = HOURS_IN_DAY * (30 + 31 + 31); //���������� �����, �� ������� �������� ���������� �� ��� ����������� ��������.
	static unsigned int const PROGNOSIS_DEPTH = HOURS_IN_DAY * 3; //���������� �����, �� ������� �������������� ������� �� ������� ��������. ����� �� ��� ���� ������������� (���� ���� �������� 0, �� �� ����� ����� �����.)
	static unsigned int const BONUS_HISTORY = HOURS_IN_DAY * 5; //�������������� ����, ������� �������� � �������. �� ������ ������ (����� ��� ������������ ���� ����� � ������� ��� ���� ����).
	static unsigned int const ALL_HISTORY_LENGTH = MIN_HISTORY_LENGTH + PROGNOSIS_DEPTH + BONUS_HISTORY; //���������� �����, �� ������� �������� ���������� �� ����������� ��������. �� ����, ���������� 24 * (30 + 31 + 31 + 3) = 24 * 95 � ��� � ��������� �������, �� ����� ���� �������� �� ������ ������.

	static unsigned int const DAY = 1 * HOURS_IN_DAY; //����� � ����� ����������� ����������, �� ������� �� ��������� �����, ����� ��������� ��������� ���������� ������� �� ���������� ���
	static unsigned int const WEEK = 7 * HOURS_IN_DAY; //����� � ����� �������� ����������, �� ������� �� ��������� �����, ����� ��������� ��������� ���������� ������� �� ���������� ���
	static unsigned int const MONTH = 30 * HOURS_IN_DAY; //����� � ����� ����������� ����������, �� ������� �� ��������� �����, ����� ��������� ��������� ���������� ������� �� ���������� ��� (����� �������� �� ������� �� lastTimestamp)
private:
	NumberOfTickets m_aCohortResults[NUMBER_OF_COHORTS][ALL_HISTORY_LENGTH]; //���� ������ ���������� ����������� �������� �� ��� �� �������� (0 � ������; 1 � �����������; 2 � �������� ������). ��������� �� ������� ����� �����������, ��� ��� ������ ������ ������ ����� �������� ������.
	size_t m_currentHour; //������ ������ �������� � CohortResults, ���������������� �������� ����. � ������ �����������������/����������������� ����� ����� ���������� ���������� �% HISTORY_LENGTH�. ��� ����� ����� ����.
	//������ ���, ������� ����� ����� differense �����
	size_t findHourIndex (
		int hour //����� ����, ������� ���� (������������ ��������)
		) const { 
		return (m_currentHour + hour) % ALL_HISTORY_LENGTH; //�������� �� ������� ����������, ������ �������� m_currentHour � �������� ���������� �������� ������� ������� m_aCohortResults
	}
	Timestamp lastTimestamp; //�����, ������� ���������� � ��������� ���, ����� ������ m_currentHour. ����� ��� �������� ����, ���� � ����� ������ ���� ���������.
	//������ �������������� ������� �� 9 (�������, ����� ��� ���������� ������� ������, ��� �� ����� ����)
	NumberOfTickets fakeDiv9 (
		NumberOfTickets numberOfTickets //�������
		) {
		return (numberOfTickets >> 3) - (numberOfTickets >> 6); //��������� 7/64 (< 7/63)
	}
	//�������� ��������� ���������� ������� ��� ����������� ����
	void calculatePrognosis (
		int hour //����� ����, ��� �������� ���� �������� �������� ���������� ����������� �������� (����� ��� ���� ������)
		) {
		for(size_t i = 0; i < NUMBER_OF_COHORTS; ++i) {
			m_aCohortResults[i][findHourIndex (hour)] = fakeDiv9 (																														//������ ������� ��������������
				m_aCohortResults[i][findHourIndex (hour - 1 * DAY)] + m_aCohortResults[i][findHourIndex (hour - 2 * DAY)] + m_aCohortResults[i][findHourIndex (hour - 3 * DAY)] +		//�� ��������� ��� ���,
				m_aCohortResults[i][findHourIndex (hour - 1 * WEEK)] + m_aCohortResults[i][findHourIndex (hour - 2 * WEEK)] + m_aCohortResults[i][findHourIndex (hour - 3 * WEEK)] +	//��� ������
				m_aCohortResults[i][findHourIndex (hour - 1 * MONTH)] + m_aCohortResults[i][findHourIndex (hour - 2 * MONTH)] + m_aCohortResults[i][findHourIndex (hour - 3 * MONTH)]	//� ��� ������
				);
		}
	}
	unsigned int m_responseTime; //��������� ������������ ����� �������� � �����
public:
//	ResponseTimeData () : m_currentHour (0) {} //����-�� �����, ����� ��� ����� �����������

	//���������� ���������� ����������� ������� �� ��������� ��� ������. ����� false, ���� ���������� ������ ��� ������ ����������� ��� ���������� ���������� ����� (��� �� ��������, ���� ��������� ���� �������).
	bool setCohortResults (
		Timestamp currentTimestamp, //������� �����
//		size_t cohortIndex, //����� ������� (0, 1 ��� 2)
		NumberOfTickets * aThirdCohortResults, //������ � ������������ ������� �������
		NumberOfTickets * aThirteenthCohortResults, //������ � ������������ ����������� �������
		NumberOfTickets * aTwentyThirdCohortResults, //������ � ������������ �������� ������� �������
		size_t aCohortResultsLength //����� ���������� �������� (����������, ������ ���� ����������)
		) {
		lastTimestamp = currentTimestamp; //��������� �����
		for(size_t i = 0; i < aCohortResultsLength; ++i) { //��������� ���������� ������� �������
			size_t hourIndex = findHourIndex (i);
			//���������� ������ ������� ���� ���������
			m_aCohortResults[0][hourIndex] = aThirdCohortResults[i];
			m_aCohortResults[1][hourIndex] = aThirteenthCohortResults[i];
			m_aCohortResults[2][hourIndex] = aTwentyThirdCohortResults[i];
		}
		m_currentHour = findHourIndex (aCohortResultsLength); //� �������� �������� ���� ���������� ������ ������� ��������������� ��������
		for(int i = 0; i < PROGNOSIS_DEPTH; ++i) {	//�� ��� ��� �����
			calculatePrognosis (i);					//������������ �������
		}
		return aCohortResultsLength >= MIN_HISTORY_LENGTH;
	}
	//������� �� ����� ����� ������� �� ���� �������� (�������� �����)
	void print (
//		size_t cohortIndex, //����� ������� (0, 1 ��� 2)
		int firstHour,	//�� ����� ����
		int lastHour	//�� ����� (����� ������� � ������� ���)
		) const {
		if(firstHour <= lastHour) { //������� ���-�� ������ ����� ������� ����������
			std::cout << "Third cohort results:\n";
			for(int i = firstHour; i <= lastHour; ++i) {
				std::cout << m_aCohortResults[0][findHourIndex (i)] << "\t";
			}
			std::cout << "\n\nThirteenth cohort results:\n";
			for(int i = firstHour; i <= lastHour; ++i) {
				std::cout << m_aCohortResults[1][findHourIndex (i)] << "\t";
			}
			std::cout << "\n\nTwenty third cohort results:\n";
			for(int i = firstHour; i <= lastHour; ++i) {
				std::cout << m_aCohortResults[2][findHourIndex (i)] << "\t";
			}
			std::cout << "\n";
		}
	}
	//��������� ����� ��������, �������� ������ ������� ��������, � ���������� ��� � m_responseTime
	void calculateResponseTime (
		NumberOfTickets ticketsQueue //������� ������� ��������
		) {
		unsigned int hour = 0;
		NumberOfTickets numberOfTickets = 0;
		while(ticketsQueue >= numberOfTickets) {
			for(unsigned int i = 0; i < NUMBER_OF_COHORTS; ++i) {
				numberOfTickets += m_aCohortResults[i][findHourIndex (hour)];
			}
			if(++hour == PROGNOSIS_DEPTH) {
				if(numberOfTickets == 0) {
					m_responseTime = 666;
				} else {
					m_responseTime = ticketsQueue * PROGNOSIS_DEPTH / numberOfTickets;
				}
				return;
			}
		}
		m_responseTime = hour;
	}
	//���������� ���������� ����� ��������
	unsigned int getResponseTime () const {
		return m_responseTime;
	}
};