#include "SupportResponseTime.h"

#include <iostream>

using namespace SupportResponseTime;

//нахожу час, который будет через differense часов
inline size_t Data::findHourIndex (
	int hour //номер часа, который ищем (относительно текущего)
	) const {
	return (m_currentHour + hour) % ALL_HISTORY_LENGTH; //двигаюсь по массиву циклически, всегда оставляя m_currentHour в пределах допустимых значений второго индекса m_aCohortResults
}

//замена целочисленному делению на 3 * COHORT_NUMBER (хочется, чтобы тут получалось немного меньше, чем на самом деле)
inline NumberOfTickets Data::fakeDiv (
	NumberOfTickets numberOfTickets //делимое
	) {
	return (numberOfTickets >> 3) - (numberOfTickets >> 6); //получится 7/64 (< 7/63)
}

//вычисляю ожидаемое количество ответов для конкретного часа
void Data::calculatePrognosis (
	int hour //номер часа, для которого буду изменять значения количества разобранных вопросов (сразу для всех когорт)
	) {
	for(size_t cohortIndex = 0; cohortIndex < NUMBER_OF_COHORTS; ++cohortIndex) {
		m_aCohortResults[cohortIndex][findHourIndex (hour)] = fakeDiv (					//считаю среднее арифметическое
			m_aCohortResults[cohortIndex][findHourIndex (hour - 1 * DAY)] +
			m_aCohortResults[cohortIndex][findHourIndex (hour - 2 * DAY)] +				//за последние три дня,
			m_aCohortResults[cohortIndex][findHourIndex (hour - 3 * DAY)] +
			m_aCohortResults[cohortIndex][findHourIndex (hour - 1 * WEEK)] +
			m_aCohortResults[cohortIndex][findHourIndex (hour - 2 * WEEK)] +			//три недели
			m_aCohortResults[cohortIndex][findHourIndex (hour - 3 * WEEK)] +
			m_aCohortResults[cohortIndex][findPastHourIndex(hour, 1, cohortIndex)] +
			m_aCohortResults[cohortIndex][findPastHourIndex(hour, 2, cohortIndex)] +	//и три месяца
			m_aCohortResults[cohortIndex][findPastHourIndex(hour, 3, cohortIndex)]
			);
	}
}

//Чтобы вдруг не переполнилось во время setCohortResults.
Data::Data () : m_currentHour (0), m_responseTime(BAD_RESPONSE_TIME) {}

//Записываем результаты всех когорт за последние три месяца.
void Data::setCohortsResults (
	NumberOfTickets const (&aCohortResults)[NUMBER_OF_COHORTS][MIN_HISTORY_LENGTH], //массив с результатами всех когорт
	time_t currentTimestamp //текущее время
	) {
	m_currentHour = 0;
	m_lastTimestamp = currentTimestamp; //сохраняем время
	for(size_t hour = 0; hour < MIN_HISTORY_LENGTH; ++hour) { //пробегаем переданные массивы целиком
		//записываем каждой когорте свой результат
		for(size_t cohortIndex = 0; cohortIndex < NUMBER_OF_COHORTS; ++cohortIndex) {
			m_aCohortResults[cohortIndex][hour] = aCohortResults[cohortIndex][hour];
		}
	}
	m_currentHour = findHourIndex (MIN_HISTORY_LENGTH); //в качестве текущего часа запоминаем индекс первого неиспользуемого элемента
	for (size_t hour = 0; hour < PROGNOSIS_DEPTH; ++hour) {	//на три дня вперёд
		calculatePrognosis (hour);					//рассчитываем прогноз
	}
}

//Добавляем инфу о прошедшем часе. Возвращаем false, когда передали некошерное время (в этом случае ничего не изменяем).
bool Data::switchHour (
	NumberOfTickets const (&aCohortHourResults)[NUMBER_OF_COHORTS], //результаты всех когорт за прошедший час
	time_t currentTimestamp //текущее время
	) {
	if(m_lastTimestamp / SECONDS_IN_HOUR + 1 != currentTimestamp / SECONDS_IN_HOUR) { //Переданный timestamp должен быть в следующем часе.
		return false;
	}
	m_lastTimestamp = currentTimestamp; //сохраняем время
	size_t hourIndex = findHourIndex (0);
	//записываем каждой когорте свой результат
	for(size_t cohortIndex = 0; cohortIndex < NUMBER_OF_COHORTS; ++cohortIndex) {
		m_aCohortResults[cohortIndex][hourIndex] = aCohortHourResults[cohortIndex];
	}
	//До конца промежутка прогноза пересчитываем/считаем ожидаемое количество ответов с шагом в 1 день (то бишь ровно для тех часов, на которые новый час мог оказать влияние).
	for(unsigned int hour = HOURS_IN_DAY; hour <= PROGNOSIS_DEPTH; hour += HOURS_IN_DAY) {
		calculatePrognosis (hour);
	}
	calculatePrognosis (PROGNOSIS_DEPTH); //А это на случай, если PROGNOSIS_DEPTH не делится на HOURS_IN_DAY. Таким образом мы оставляем актуальной длину прогноза.
	m_currentHour = findHourIndex (1); //Говорим, что прошёл час.
	return true;
}

//Оцениваем время ожидания, эмулируя разбор очереди вопросов, и записываем его в m_responseTime. Если пришёл плохой timestamp — возвращаем false и ничего не меняем.
bool Data::calculateResponseTime (
	NumberOfTickets ticketsQueue, //текущая очередь вопросов
	time_t currentTimestamp //текущее время
	) {
	if(m_lastTimestamp / SECONDS_IN_HOUR != currentTimestamp / SECONDS_IN_HOUR) { //Должны находиться в том же часу.
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

//возвращаем сохранённое время ожидания
unsigned int Data::getResponseTime () const {
	return m_responseTime;
}

//нахожу час, который был months месяцев назад
size_t Data::findPastHourIndex(
	int hour, //час, от которого отсчитываем (сюда передаём сдвиг относительно m_currentHour)
	int months, //на сколько месяцев назад листаем
	size_t cohortIndex //номер когорты
	) const {
	tm time, pastTime;
	time_t timestamp = m_lastTimestamp + (hour + END_OF_MONTH_LENGTH) * SECONDS_IN_HOUR;
	localtime_s(&time, &timestamp);
	if (static_cast<unsigned int>(time.tm_mday) < FIRST_DAY_OF_MONTH[cohortIndex]) {
		--time.tm_mon;
		if (time.tm_mon < 0) {
			time.tm_mon += 12;
			--time.tm_year;
		}
	}
	pastTime = time;
	pastTime.tm_mon -= months;
	while (pastTime.tm_mon < 0) {
		pastTime.tm_mon += 12;
		--pastTime.tm_year;
	}
	return findHourIndex((mktime(&pastTime) - mktime(&time)) / SECONDS_IN_HOUR);
}

//выводим на экран кусок истории по всем когортам (тестовый метод)
void Data::print(
	int firstHour,	//от этого часа
	int lastHour	//до этого (точка отсчёта — текущий час)
	) const {
	if (firstHour <= lastHour) { //выводим что-то только когда границы адекватные
		std::cout << "Third cohort results:\n";
		for (int i = firstHour; i <= lastHour; ++i) {
			std::cout << m_aCohortResults[0][findHourIndex(i)] << "\t";
		}
		std::cout << "\n\nThirteenth cohort results:\n";
		for (int i = firstHour; i <= lastHour; ++i) {
			std::cout << m_aCohortResults[1][findHourIndex(i)] << "\t";
		}
		std::cout << "\n\nTwenty third cohort results:\n";
		for (int i = firstHour; i <= lastHour; ++i) {
			std::cout << m_aCohortResults[2][findHourIndex(i)] << "\t";
		}
		std::cout << "\n";
	}
}