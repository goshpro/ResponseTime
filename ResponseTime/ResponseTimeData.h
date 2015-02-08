#include <ctime>

namespace ResponseTime {
	typedef unsigned int NumberOfTickets;
	typedef time_t Timestamp; //Предполагаю, что будет передаваться unix timestamp (который от 1 января 1970 года).

	unsigned int const NUMBER_OF_COHORTS = 3; //Количество когорт.
	unsigned int const FIRST_DAY_OF_MONTH[3] = { 1, 11, 20 }; //Когда у какой когорты начинается расчётный месяц.

	unsigned int const HOURS_IN_DAY = 24; //Количество промежутков, на которые разбиваются сутки. Длина промежутка соответствует точности, с которой будет вычисляться время ожидания (не представляю, зачем бы это был не 1 час, так что далее под «часом» везде будет подразумеваться именно это значение). Если сюда запихнуть число, не являющееся делителем 86400, то алгоритм будет работать хреново.
	unsigned int const MIN_HISTORY_LENGTH = (30 + 31 + 31) * HOURS_IN_DAY; //Количество часов, за которое хранится статистика по уже разобранным вопросам.
	unsigned int const PROGNOSIS_DEPTH = 3 * HOURS_IN_DAY; //Количество часов, на которое рассчитывается прогноз по разбору вопросов. Вроде бы трёх дней предостаточно… (Если сюда написать 0, то всё будет очень плохо.)
	unsigned int const BONUS_HISTORY = 0; //Я не знаю, зачем это.
	unsigned int const ALL_HISTORY_LENGTH = MIN_HISTORY_LENGTH + PROGNOSIS_DEPTH + BONUS_HISTORY; //Количество часов, за которое хранится статистика по разобранным вопросам. По идее, достаточно 24 * (30 + 31 + 31 + 3) = 24 * 95 — трёх с небольшим месяцев, но лучше чуть побольше на всякий случай.

	unsigned int const SECONDS_IN_HOUR = 86400 / HOURS_IN_DAY; //Количество секунд в одном часе.

	unsigned int const DAY = 1 * HOURS_IN_DAY; //Длина в часах наименьшего промежутка, на который мы двигаемся назад, когда вычисляем ожидаемое количество ответов за конкретный час.
	unsigned int const WEEK = 7 * HOURS_IN_DAY; //Длина в часах среднего промежутка, на который мы двигаемся назад, когда вычисляем ожидаемое количество ответов за конкретный час.
	unsigned int const MONTH = 30 * HOURS_IN_DAY; //Длина в часах наибольшего промежутка, на который мы двигаемся назад, когда вычисляем ожидаемое количество ответов за конкретный час (будет заменено на функцию от lastTimestamp).

	unsigned int const END_OF_MONTH_LENGTH = 13 * HOURS_IN_DAY / 2; //Длина в часах «конца месяца» — периода, когда Агенты начинают херачить.

	class Data {
	public:
		Data (); //конструктор
		void setCohortsResults (NumberOfTickets const (&aCohortResults)[NUMBER_OF_COHORTS][MIN_HISTORY_LENGTH], Timestamp currentTimestamp); //Записываем результаты всех когорт за последние три месяца.
		bool switchHour (NumberOfTickets const (&aCohortHourResults)[NUMBER_OF_COHORTS], Timestamp currentTimestamp); //добавляем инфу о прошедшем часе
		void print (int firstHour, int lastHour) const; //выводим на экран кусок истории по всем когортам (тестовый метод)
		void calculateResponseTime (NumberOfTickets ticketsQueue, Timestamp currentTimestamp); //оцениваем время ожидания, эмулируя разбор очереди вопросов, и записываем его в m_responseTime
		unsigned int getResponseTime () const; //возвращаем сохранённое время ожидания
	private:
		NumberOfTickets m_aCohortResults[NUMBER_OF_COHORTS][ALL_HISTORY_LENGTH]; //База данных количества разобранных вопросов за час по когортам (0 — третья; 1 — тринадцатая; 2 — двадцать третья). Блуждание по массиву будет циклическим, так что свежие данные просто будут замещать старые.
		Timestamp m_lastTimestamp; //Время, которое передавали в последний раз, когда меняли m_currentHour. Нужно для контроля того, инфа о каком именно часе передаётся.
		size_t m_currentHour; //Второй индекс элемента в CohortResults, соответствующего текущему часу. К любому инкрементированию/декрементированию этого числа необходимо дописывать «% HISTORY_LENGTH». Для этого метод ниже.

		size_t findHourIndex (int hour) const; //нахожу час, который будет через differense часов
		NumberOfTickets fakeDiv9 (NumberOfTickets numberOfTickets); //замена целочисленному делению на 9 (хочется, чтобы тут получалось немного меньше, чем на самом деле)
		void calculatePrognosis (int hour); //вычисляю ожидаемое количество ответов для конкретного часа
		unsigned int m_responseTime; //последнее рассчитанное время ожидания в часах
	};
}