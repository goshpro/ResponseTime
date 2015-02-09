#include "SupportResponseTime.h"

#include <iostream>

int main () {
	srand (static_cast<unsigned int>(time (nullptr)));
	SupportResponseTime::Data data;
	time_t currentTimestamp = 123456789;
	std::cout << "Test\n" << 0123 << "\n\n";
	SupportResponseTime::NumberOfTickets results[SupportResponseTime::NUMBER_OF_COHORTS][SupportResponseTime::MIN_HISTORY_LENGTH];
	for(size_t hour = 0; hour < SupportResponseTime::MIN_HISTORY_LENGTH; ++hour) {
		for(size_t cohortIndex = 0; cohortIndex < SupportResponseTime::NUMBER_OF_COHORTS; ++cohortIndex) {
			results[cohortIndex][hour] = 100 * (cohortIndex + 1);
		}
	}
	data.setCohortsResults (results, currentTimestamp);
	for(size_t i = 0; i < 10; ++i) {
		currentTimestamp += SupportResponseTime::SECONDS_IN_HOUR;
		SupportResponseTime::NumberOfTickets numberOfTickets[3] = { 100 + rand () % 100, 200 + rand () % 100, 300 + rand () % 100 };
		data.switchHour (numberOfTickets, currentTimestamp);
	}
	data.print (0, SupportResponseTime::PROGNOSIS_DEPTH - 1);
	SupportResponseTime::NumberOfTickets ticketsQueue = 100000;
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