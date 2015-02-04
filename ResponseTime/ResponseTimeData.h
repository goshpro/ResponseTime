#include <iostream>

typedef unsigned int NumberOfTickets;
typedef int Timestamp; //предполагаю, что будет передаваться unix timestamp (который от 1 января 1970 года)

class ResponseTimeData {
public:
	static unsigned int const NUMBER_OF_COHORTS = 3; //Количество когорт

	static unsigned int const HOURS_IN_DAY = 24; //Количество промежутков, на которые разбиваются сутки. Длина промежутка соответствует точности, с которой будет вычисляться время ожидания (не представляю, зачем бы это был не 1 час, так что далее под «часом» везде будет подразумеваться именно это значение). Если сюда запихнуть число, не являющееся делителем 86400, то алгоритм будет работать хреново.
	static unsigned int const MIN_HISTORY_LENGTH = HOURS_IN_DAY * (30 + 31 + 31); //Количество часов, за которое хранится статистика по уже разобранным вопросам.
	static unsigned int const PROGNOSIS_DEPTH = HOURS_IN_DAY * 3; //Количество часов, на которое рассчитывается прогноз по разбору вопросов. Вроде бы трёх дней предостаточно… (Если сюда написать 0, то всё будет очень плохо.)
	static unsigned int const BONUS_HISTORY = HOURS_IN_DAY * 5; //Дополнительные часы, которые хранятся в истории. На всякий случай (вдруг там передвинется день нормы у когорты или типа того).
	static unsigned int const ALL_HISTORY_LENGTH = MIN_HISTORY_LENGTH + PROGNOSIS_DEPTH + BONUS_HISTORY; //Количество часов, за которое хранится статистика по разобранным вопросам. По идее, достаточно 24 * (30 + 31 + 31 + 3) = 24 * 95 — трёх с небольшим месяцев, но лучше чуть побольше на всякий случай.

	static unsigned int const DAY = 1 * HOURS_IN_DAY; //Длина в часах наименьшего промежутка, на который мы двигаемся назад, когда вычисляем ожидаемое количество ответов за конкретный час
	static unsigned int const WEEK = 7 * HOURS_IN_DAY; //Длина в часах среднего промежутка, на который мы двигаемся назад, когда вычисляем ожидаемое количество ответов за конкретный час
	static unsigned int const MONTH = 30 * HOURS_IN_DAY; //Длина в часах наибольшего промежутка, на который мы двигаемся назад, когда вычисляем ожидаемое количество ответов за конкретный час (будет заменено на функцию от lastTimestamp)
private:
	NumberOfTickets m_aCohortResults[NUMBER_OF_COHORTS][ALL_HISTORY_LENGTH]; //База данных количества разобранных вопросов за час по когортам (0 — третья; 1 — тринадцатая; 2 — двадцать третья). Блуждание по массиву будет циклическим, так что свежие данные просто будут замещать старые.
	size_t m_currentHour; //Второй индекс элемента в CohortResults, соответствующего текущему часу. К любому инкрементированию/декрементированию этого числа необходимо дописывать «% HISTORY_LENGTH». Для этого метод ниже.
	//нахожу час, который будет через differense часов
	size_t findHourIndex (
		int hour //номер часа, который ищем (относительно текущего)
		) const { 
		return (m_currentHour + hour) % ALL_HISTORY_LENGTH; //двигаюсь по массиву циклически, всегда оставляя m_currentHour в пределах допустимых значений второго индекса m_aCohortResults
	}
	Timestamp lastTimestamp; //Время, которое передавали в последний раз, когда меняли m_currentHour. Нужно для контроля того, инфа о каком именно часе передаётся.
	//замена целочисленному делению на 9 (хочется, чтобы тут получалось немного меньше, чем на самом деле)
	NumberOfTickets fakeDiv9 (
		NumberOfTickets numberOfTickets //делимое
		) {
		return (numberOfTickets >> 3) - (numberOfTickets >> 6); //получится 7/64 (< 7/63)
	}
	//вычисляю ожидаемое количество ответов для конкретного часа
	void calculatePrognosis (
		int hour //номер часа, для которого буду изменять значения количества разобранных вопросов (сразу для всех когорт)
		) {
		for(size_t i = 0; i < NUMBER_OF_COHORTS; ++i) {
			m_aCohortResults[i][findHourIndex (hour)] = fakeDiv9 (																														//считаю среднее арифметическое
				m_aCohortResults[i][findHourIndex (hour - 1 * DAY)] + m_aCohortResults[i][findHourIndex (hour - 2 * DAY)] + m_aCohortResults[i][findHourIndex (hour - 3 * DAY)] +		//за последние три дня,
				m_aCohortResults[i][findHourIndex (hour - 1 * WEEK)] + m_aCohortResults[i][findHourIndex (hour - 2 * WEEK)] + m_aCohortResults[i][findHourIndex (hour - 3 * WEEK)] +	//три недели
				m_aCohortResults[i][findHourIndex (hour - 1 * MONTH)] + m_aCohortResults[i][findHourIndex (hour - 2 * MONTH)] + m_aCohortResults[i][findHourIndex (hour - 3 * MONTH)]	//и три месяца
				);
		}
	}
	unsigned int m_responseTime; //последнее рассчитанное время ожидания в часах
public:
//	ResponseTimeData () : m_currentHour (0) {} //ваще-то пофиг, какой час будет изначальным

	//Записываем результаты определённой когорты за последние три месяца. Вернём false, если переданный массив был меньше необходимой для корректных вычислений длины (это не критично, если передаётся лишь кусочек).
	bool setCohortResults (
		Timestamp currentTimestamp, //текущее время
//		size_t cohortIndex, //номер когорты (0, 1 или 2)
		NumberOfTickets * aThirdCohortResults, //массив с результатами третьей когорты
		NumberOfTickets * aThirteenthCohortResults, //массив с результатами тринадцатой когорты
		NumberOfTickets * aTwentyThirdCohortResults, //массив с результатами двадцать третьей когорты
		size_t aCohortResultsLength //длина переданных массивов (разумеется, должна быть одинаковой)
		) {
		lastTimestamp = currentTimestamp; //сохраняем время
		for(size_t i = 0; i < aCohortResultsLength; ++i) { //пробегаем переданные массивы целиком
			size_t hourIndex = findHourIndex (i);
			//записываем каждой когорте свой результат
			m_aCohortResults[0][hourIndex] = aThirdCohortResults[i];
			m_aCohortResults[1][hourIndex] = aThirteenthCohortResults[i];
			m_aCohortResults[2][hourIndex] = aTwentyThirdCohortResults[i];
		}
		m_currentHour = findHourIndex (aCohortResultsLength); //в качестве текущего часа запоминаем индекс первого неиспользуемого элемента
		for(int i = 0; i < PROGNOSIS_DEPTH; ++i) {	//на три дня вперёд
			calculatePrognosis (i);					//рассчитываем прогноз
		}
		return aCohortResultsLength >= MIN_HISTORY_LENGTH;
	}
	//выводим на экран кусок истории по всем когортам (тестовый метод)
	void print (
//		size_t cohortIndex, //номер когорты (0, 1 или 2)
		int firstHour,	//от этого часа
		int lastHour	//до этого (точка отсчёта — текущий час)
		) const {
		if(firstHour <= lastHour) { //выводим что-то только когда границы адекватные
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
	//оцениваем время ожидания, эмулируя разбор очереди вопросов, и записываем его в m_responseTime
	void calculateResponseTime (
		NumberOfTickets ticketsQueue //текущая очередь вопросов
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
	//возвращаем сохранённое время ожидания
	unsigned int getResponseTime () const {
		return m_responseTime;
	}
};