/*
#include "ResponseTimeData.h"

bool ResponseTimeData::setCohortResults ( //Записываем результаты определённой когорты за последние три месяца. Вернём false, если переданный массив был меньше необходимой для корректных вычислений длины (это не критично, если передаётся лишь кусочек)
	Timestamp currentTimestamp, //текущее время
	size_t cohortIndex, //номер когорты (0, 1 или 2)
	NumberOfTickets * aCohortResults, //массив с результатами
	size_t aCohortResultsLength //длина передаваемого массива
	) {
	lastTimestamp = currentTimestamp; //сохраняем время
	for(size_t i = 0; i < aCohortResultsLength; ++i) { //пробегаем весь переданный массив
		m_aCohortResults[cohortIndex][findHour (i)] = aCohortResults[i]; //записываем соответствующей когорте нужный результат
	}
	m_currentHour = findHour (aCohortResultsLength); //в качестве текущего часа запоминаем индекс первого неиспользуемого элемента
	return aCohortResultsLength >= HISTORY_LENGTH - BONUS_HISTORY - PROGNOSIS_DEPTH;
}
*/