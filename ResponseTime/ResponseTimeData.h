#include <ctime>

namespace ResponseTime {
	typedef unsigned int NumberOfTickets;
	typedef time_t Timestamp; //�����������, ��� ����� ������������ unix timestamp (������� �� 1 ������ 1970 ����).

	unsigned int const NUMBER_OF_COHORTS = 3; //���������� ������.
	unsigned int const FIRST_DAY_OF_MONTH[3] = { 1, 11, 20 }; //����� � ����� ������� ���������� ��������� �����.

	unsigned int const HOURS_IN_DAY = 24; //���������� �����������, �� ������� ����������� �����. ����� ���������� ������������� ��������, � ������� ����� ����������� ����� �������� (�� �����������, ����� �� ��� ��� �� 1 ���, ��� ��� ����� ��� ������ ����� ����� ��������������� ������ ��� ��������). ���� ���� ��������� �����, �� ���������� ��������� 86400, �� �������� ����� �������� �������.
	unsigned int const MIN_HISTORY_LENGTH = (30 + 31 + 31) * HOURS_IN_DAY; //���������� �����, �� ������� �������� ���������� �� ��� ����������� ��������.
	unsigned int const PROGNOSIS_DEPTH = 3 * HOURS_IN_DAY; //���������� �����, �� ������� �������������� ������� �� ������� ��������. ����� �� ��� ���� ������������� (���� ���� �������� 0, �� �� ����� ����� �����.)
	unsigned int const BONUS_HISTORY = 0; //� �� ����, ����� ���.
	unsigned int const ALL_HISTORY_LENGTH = MIN_HISTORY_LENGTH + PROGNOSIS_DEPTH + BONUS_HISTORY; //���������� �����, �� ������� �������� ���������� �� ����������� ��������. �� ����, ���������� 24 * (30 + 31 + 31 + 3) = 24 * 95 � ��� � ��������� �������, �� ����� ���� �������� �� ������ ������.

	unsigned int const SECONDS_IN_HOUR = 86400 / HOURS_IN_DAY; //���������� ������ � ����� ����.

	unsigned int const DAY = 1 * HOURS_IN_DAY; //����� � ����� ����������� ����������, �� ������� �� ��������� �����, ����� ��������� ��������� ���������� ������� �� ���������� ���.
	unsigned int const WEEK = 7 * HOURS_IN_DAY; //����� � ����� �������� ����������, �� ������� �� ��������� �����, ����� ��������� ��������� ���������� ������� �� ���������� ���.
	unsigned int const MONTH = 30 * HOURS_IN_DAY; //����� � ����� ����������� ����������, �� ������� �� ��������� �����, ����� ��������� ��������� ���������� ������� �� ���������� ��� (����� �������� �� ������� �� lastTimestamp).

	unsigned int const END_OF_MONTH_LENGTH = 13 * HOURS_IN_DAY / 2; //����� � ����� ������ ������ � �������, ����� ������ �������� ��������.

	class Data {
	public:
		Data (); //�����������
		void setCohortsResults (NumberOfTickets const (&aCohortResults)[NUMBER_OF_COHORTS][MIN_HISTORY_LENGTH], Timestamp currentTimestamp); //���������� ���������� ���� ������ �� ��������� ��� ������.
		bool switchHour (NumberOfTickets const (&aCohortHourResults)[NUMBER_OF_COHORTS], Timestamp currentTimestamp); //��������� ���� � ��������� ����
		void print (int firstHour, int lastHour) const; //������� �� ����� ����� ������� �� ���� �������� (�������� �����)
		void calculateResponseTime (NumberOfTickets ticketsQueue, Timestamp currentTimestamp); //��������� ����� ��������, �������� ������ ������� ��������, � ���������� ��� � m_responseTime
		unsigned int getResponseTime () const; //���������� ���������� ����� ��������
	private:
		NumberOfTickets m_aCohortResults[NUMBER_OF_COHORTS][ALL_HISTORY_LENGTH]; //���� ������ ���������� ����������� �������� �� ��� �� �������� (0 � ������; 1 � �����������; 2 � �������� ������). ��������� �� ������� ����� �����������, ��� ��� ������ ������ ������ ����� �������� ������.
		Timestamp m_lastTimestamp; //�����, ������� ���������� � ��������� ���, ����� ������ m_currentHour. ����� ��� �������� ����, ���� � ����� ������ ���� ���������.
		size_t m_currentHour; //������ ������ �������� � CohortResults, ���������������� �������� ����. � ������ �����������������/����������������� ����� ����� ���������� ���������� �% HISTORY_LENGTH�. ��� ����� ����� ����.

		size_t findHourIndex (int hour) const; //������ ���, ������� ����� ����� differense �����
		NumberOfTickets fakeDiv9 (NumberOfTickets numberOfTickets); //������ �������������� ������� �� 9 (�������, ����� ��� ���������� ������� ������, ��� �� ����� ����)
		void calculatePrognosis (int hour); //�������� ��������� ���������� ������� ��� ����������� ����
		unsigned int m_responseTime; //��������� ������������ ����� �������� � �����
	};
}