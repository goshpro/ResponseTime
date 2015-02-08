#include "ResponseTimeData.h"

#include <iostream>

using namespace ResponseTime;

//������ ���, ������� ����� ����� differense �����
inline size_t Data::findHourIndex (
	int hour //����� ����, ������� ���� (������������ ��������)
	) const {
	return (m_currentHour + hour) % ALL_HISTORY_LENGTH; //�������� �� ������� ����������, ������ �������� m_currentHour � �������� ���������� �������� ������� ������� m_aCohortResults
}

//������ �������������� ������� �� 3 * COHORT_NUMBER (�������, ����� ��� ���������� ������� ������, ��� �� ����� ����)
inline NumberOfTickets Data::fakeDiv (
	NumberOfTickets numberOfTickets //�������
	) {
	return (numberOfTickets >> 3) - (numberOfTickets >> 6); //��������� 7/64 (< 7/63)
}

//�������� ��������� ���������� ������� ��� ����������� ����
void Data::calculatePrognosis (
	int hour //����� ����, ��� �������� ���� �������� �������� ���������� ����������� �������� (����� ��� ���� ������)
	) {
	for(size_t cohortIndex = 0; cohortIndex < NUMBER_OF_COHORTS; ++cohortIndex) {
		m_aCohortResults[cohortIndex][findHourIndex (hour)] = fakeDiv (																														//������ ������� ��������������
			m_aCohortResults[cohortIndex][findHourIndex (hour - 1 * DAY)] + m_aCohortResults[cohortIndex][findHourIndex (hour - 2 * DAY)] + m_aCohortResults[cohortIndex][findHourIndex (hour - 3 * DAY)] +		//�� ��������� ��� ���,
			m_aCohortResults[cohortIndex][findHourIndex (hour - 1 * WEEK)] + m_aCohortResults[cohortIndex][findHourIndex (hour - 2 * WEEK)] + m_aCohortResults[cohortIndex][findHourIndex (hour - 3 * WEEK)] +	//��� ������
			m_aCohortResults[cohortIndex][findHourIndex (hour - 1 * MONTH)] + m_aCohortResults[cohortIndex][findHourIndex (hour - 2 * MONTH)] + m_aCohortResults[cohortIndex][findHourIndex (hour - 3 * MONTH)]	//� ��� ������
			);
	}
}

//����� ����� �� ������������� �� ����� setCohortResults.
Data::Data () : m_currentHour (0), m_responseTime(BAD_RESPONSE_TIME) {}

//���������� ���������� ���� ������ �� ��������� ��� ������.
void Data::setCohortsResults (
	NumberOfTickets const (&aCohortResults)[NUMBER_OF_COHORTS][MIN_HISTORY_LENGTH], //������ � ������������ ���� ������
	Timestamp currentTimestamp //������� �����
	) {
	m_currentHour = 0;
	m_lastTimestamp = currentTimestamp; //��������� �����
	for(size_t hour = 0; hour < MIN_HISTORY_LENGTH; ++hour) { //��������� ���������� ������� �������
		//���������� ������ ������� ���� ���������
		for(size_t cohortIndex = 0; cohortIndex < NUMBER_OF_COHORTS; ++cohortIndex) {
			m_aCohortResults[cohortIndex][hour] = aCohortResults[cohortIndex][hour];
		}
	}
	m_currentHour = findHourIndex (MIN_HISTORY_LENGTH); //� �������� �������� ���� ���������� ������ ������� ��������������� ��������
	for(int i = 0; i < PROGNOSIS_DEPTH; ++i) {	//�� ��� ��� �����
		calculatePrognosis (i);					//������������ �������
	}
}

//��������� ���� � ��������� ����
//TODO: �������� ������������
bool Data::switchHour (
	NumberOfTickets const (&aCohortHourResults)[NUMBER_OF_COHORTS], //���������� ���� ������ �� ��������� ���
	Timestamp currentTimestamp //������� �����
	) {
	if(m_lastTimestamp / SECONDS_IN_HOUR + 1 != currentTimestamp / SECONDS_IN_HOUR) {
		return false;
	}
	m_lastTimestamp = currentTimestamp; //��������� �����
	size_t hourIndex = findHourIndex (0);
	//���������� ������ ������� ���� ���������
	for(size_t cohortIndex = 0; cohortIndex < NUMBER_OF_COHORTS; ++cohortIndex) {
		m_aCohortResults[cohortIndex][hourIndex] = aCohortHourResults[cohortIndex];
	}
	for(unsigned int hour = HOURS_IN_DAY; hour <= PROGNOSIS_DEPTH; hour += HOURS_IN_DAY) {
		calculatePrognosis (hour);
	}
	m_currentHour = findHourIndex (1);
	return true;
}

//������� �� ����� ����� ������� �� ���� �������� (�������� �����)
void Data::print (
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
//TODO: �������� ������������
bool Data::calculateResponseTime (
	NumberOfTickets ticketsQueue, //������� ������� ��������
	Timestamp currentTimestamp //������� �����
	) {
	if(m_lastTimestamp / SECONDS_IN_HOUR != currentTimestamp / SECONDS_IN_HOUR) {
		m_responseTime = BAD_RESPONSE_TIME;
		return false;
	}
	NumberOfTickets numberOfTickets = 0;
	for(size_t i = 0; i < NUMBER_OF_COHORTS; ++i) {
		numberOfTickets += m_aCohortResults[i][findHourIndex (0)];
	}
	numberOfTickets = numberOfTickets * (SECONDS_IN_HOUR - currentTimestamp % SECONDS_IN_HOUR) / SECONDS_IN_HOUR;
	unsigned int hour = 1;
	while(ticketsQueue >= numberOfTickets) {
		for(size_t i = 0; i < NUMBER_OF_COHORTS; ++i) {
			numberOfTickets += m_aCohortResults[i][findHourIndex (hour)];
		}
		if(++hour == PROGNOSIS_DEPTH) {
			if(numberOfTickets == 0) {
				m_responseTime = BAD_RESPONSE_TIME;
			} else {
				m_responseTime = ticketsQueue * PROGNOSIS_DEPTH / numberOfTickets;
			}
			return true;
		}
	}
	m_responseTime = hour;
	return true;
}

//���������� ����������� ����� ��������
unsigned int Data::getResponseTime () const {
	return m_responseTime;
}

//������ ���, ������� ��� months ������� �����
size_t Data::findPastHourIndex(
	int hour, //���, �� �������� ����������� (���� ������� ����� ������������ m_currentHour)
	int months, //�� ������� ������� ����� �������
	size_t cohortIndex //����� �������
	) const {
	tm time, pastTime;
	time_t timestamp = m_lastTimestamp + (hour + END_OF_MONTH_LENGTH) * SECONDS_IN_HOUR;
	localtime_s(&time, &timestamp);
	if (time.tm_mday < FIRST_DAY_OF_MONTH[cohortIndex]) {
		--time.tm_mon;
	}
	pastTime.tm_mon -= months;
	return mktime(&time) - mktime(&pastTime);
}