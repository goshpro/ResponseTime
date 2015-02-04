#include "ResponseTimeData.h"

#include <ctime>

int main () {
	srand (static_cast<unsigned int>(time (nullptr)));
	ResponseTimeData data;
	std::cout << "Test\n" << 0123 << "\n\n";
	NumberOfTickets results[3][data.MIN_HISTORY_LENGTH];
	for(size_t i = 0; i < data.MIN_HISTORY_LENGTH; ++i) {
		for(unsigned int j = 0; j < data.NUMBER_OF_COHORTS; ++j) {
			results[j][i] = 100;
		}
	}
	data.setCohortResults (123, results[0], results[1], results[2], data.MIN_HISTORY_LENGTH);
	for(size_t i = 0; i < 100; ++i) {
		data.switchHour (3600 * (i + 1), 100 + rand () % 100, 200 + rand () % 100, 300 + rand () % 100);
	}
	data.print (-20, data.PROGNOSIS_DEPTH - 1);
	NumberOfTickets ticketsQueue = 100000;
	data.calculateResponseTime (ticketsQueue);
	std::cout << "\nQueue: " << ticketsQueue << " ticket(s)\nResponse time: " << data.getResponseTime () << " hour(s)\n\n";
	system ("pause");
	return 0;
}