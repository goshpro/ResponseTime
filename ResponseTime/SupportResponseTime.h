#include <ctime>

namespace SupportResponseTime {
	typedef unsigned int NumberOfTickets;

	size_t const NUMBER_OF_COHORTS = 3; //���������� ������.
	unsigned int const FIRST_DAY_OF_MONTH[3] = { 1, 11, 20 }; //����� � ����� ������� ���������� ��������� �����.

	unsigned int const HOURS_IN_DAY = 24; //���������� �����������, �� ������� ����������� �����. ����� ���������� ������������� ��������, � ������� ����� ����������� ����� �������� (�� �����������, ����� �� ��� ��� �� 1 ���, ��� ��� ����� ��� ������ ����� ����� ��������������� ������ ��� ��������). ���� ���� ��������� �����, �� ���������� ��������� 86400, �� �������� ����� �������� �������.
	unsigned int const MIN_HISTORY_LENGTH = (30 + 31 + 31) * HOURS_IN_DAY; //���������� �����, �� ������� �������� ���������� �� ��� ����������� ��������.
	unsigned int const PROGNOSIS_DEPTH = 3 * HOURS_IN_DAY; //���������� �����, �� ������� �������������� ������� �� ������� ��������. ����� �� ��� ���� ������������� (���� ���� �������� 0, �� �� ����� ����� �����. � ����� �� �� ������ ���� ������, ��� WEEK.)
	unsigned int const BONUS_HISTORY = 0; //� �� ����, ����� ���.
	unsigned int const ALL_HISTORY_LENGTH = MIN_HISTORY_LENGTH + PROGNOSIS_DEPTH + BONUS_HISTORY; //���������� �����, �� ������� �������� ���������� �� ����������� ��������. �� ����, ���������� 24 * (30 + 31 + 31 + 3) = 24 * 95 � ��� � ��������� �������, �� ����� ���� �������� �� ������ ������.

	unsigned int const SECONDS_IN_HOUR = 86400 / HOURS_IN_DAY; //���������� ������ � ����� ����.

	unsigned int const DAY = 1 * HOURS_IN_DAY; //����� � ����� ����������� ����������, �� ������� �� ��������� �����, ����� ��������� ��������� ���������� ������� �� ���������� ���.
	unsigned int const WEEK = 7 * HOURS_IN_DAY; //����� � ����� �������� ����������, �� ������� �� ��������� �����, ����� ��������� ��������� ���������� ������� �� ���������� ���.

	unsigned int const END_OF_MONTH_LENGTH = 13 * HOURS_IN_DAY / 2; //����� � ����� ������ ������ � �������, ����� ������ �������� ��������.

	unsigned int const BAD_RESPONSE_TIME = 666; //����� ������, ������� ���������������, ����� ���-�� ����� �� ���.

	class Data {
	public:
		//�����������
		Data ();
		//���������� ���������� ���� ������ �� ��������� ��� ������.
		void setCohortsResults (NumberOfTickets const (&aCohortResults)[NUMBER_OF_COHORTS][MIN_HISTORY_LENGTH], time_t currentTimestamp);
		//��������� ���� � ��������� ����
		bool switchHour (NumberOfTickets const (&aCohortHourResults)[NUMBER_OF_COHORTS], time_t currentTimestamp);
		//��������� ����� ��������. ���������� false, ���� ���������� ����� �� ������������� �������� ���� � ���������� �������.
		bool calculateResponseTime (NumberOfTickets ticketsQueue, time_t currentTimestamp);
		//���������� ����������� ����� ����� ��������
		unsigned int getResponseTime () const;
		//������� �� ����� ����� ������� �� ���� �������� (�������� �����)
		void print(int firstHour, int lastHour) const;
	private:
		NumberOfTickets m_aCohortResults[NUMBER_OF_COHORTS][ALL_HISTORY_LENGTH]; //���� ������ ���������� ����������� �������� �� ��� �� �������� (0 � ������; 1 � �����������; 2 � �������� ������). ��������� �� ������� ����� �����������, ��� ��� ������ ������ ������ ����� �������� ������.
		time_t m_lastTimestamp; //�����, ������� ���������� � ��������� ���, ����� ������ m_currentHour. ����� ��� �������� ����, ���� � ����� ������ ���� ���������.
		unsigned int m_responseTime; //��������� ������������ ����� �������� � �����
		size_t m_currentHour; //������ ������ �������� � CohortResults, ���������������� �������� ����. � ������ �����������������/����������������� ����� ����� ���������� ���������� �% HISTORY_LENGTH�. ��� ����� ����� ����.

		size_t findHourIndex (int hour) const; //������ ���, ������� ����� ����� differense �����
		NumberOfTickets fakeDiv (NumberOfTickets numberOfTickets); //������ �������������� ������� �� 3 * COHORT_NUMBER (�������, ����� ��� ���������� ������� ������, ��� �� ����� ����)
		void calculatePrognosis (int hour); //�������� ��������� ���������� ������� ��� ����������� ����
		size_t findPastHourIndex (int hour, int months, size_t cohortIndex) const; //������ ���, ������� ��� months ������� �����
	};
}