#include "ResponseTimeData.h"

#include <iostream>

int main () {
	srand (static_cast<unsigned int>(time (nullptr)));
	ResponseTime::Data data;
	ResponseTime::Timestamp currentTimestamp = 123456;
	std::cout << "Test\n" << 0123 << "\n\n";
	ResponseTime::NumberOfTickets results[ResponseTime::NUMBER_OF_COHORTS][ResponseTime::MIN_HISTORY_LENGTH];
	for(size_t hour = 0; hour < ResponseTime::MIN_HISTORY_LENGTH; ++hour) {
		for(unsigned int cohortIndex = 0; cohortIndex < ResponseTime::NUMBER_OF_COHORTS; ++cohortIndex) {
			results[cohortIndex][hour] = 100 * (cohortIndex + 1);
		}
	}
	data.setCohortsResults (results, currentTimestamp);
	for(size_t i = 0; i < 100; ++i) {
		currentTimestamp += ResponseTime::SECONDS_IN_HOUR;
		ResponseTime::NumberOfTickets numberOfTickets[3] = { 100 + rand () % 100, 200 + rand () % 100, 300 + rand () % 100 };
		data.switchHour (numberOfTickets, currentTimestamp);
	}
	data.print (-20, ResponseTime::PROGNOSIS_DEPTH - 1);
	ResponseTime::NumberOfTickets ticketsQueue = 100000;
	data.calculateResponseTime (ticketsQueue, currentTimestamp);
	std::cout << "\nQueue: " << ticketsQueue << " ticket(s)\nResponse time: " << data.getResponseTime () << " hour(s)\n\n";
	int a = 0, b = 0;
	if(a = b) {
		std::cout << "true\n\n";
	} else {
		std::cout << "false\n\n";
	}
	system ("pause");
	return 0;
}