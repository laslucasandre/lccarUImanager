#include <iostream>
#include <unistd.h>
#include <time.h>
#include <csignal>
#include <mutex>
#include <thread>

#include "../managerUi/managerUi.h"

using namespace std;

volatile std::sig_atomic_t gExitSignal = 0;

void signalHandler(int signal) {
	cout << "Interrupt signal (" << signal << ") received.\n" << endl;
	if (gExitSignal == 0) {
		gExitSignal = signal;
	}
}

int main(int argc, char *argv[])
{
	(void)argc; /*Unused*/
	(void)argv; /*Unused*/
	
	std::signal(SIGTERM, signalHandler);
	std::signal(SIGINT, signalHandler);
	
	char welcomeMessage[] = "Low Cost CAR";
	cout << welcomeMessage << endl;
	
	time_t timer;
	char buffer[26];
	struct tm* tm_info;
	
	managerUi UiObj;
	
	UiObj.buildUi();
		
	cout << "Low Cost CAR end" << endl;
	
	static int count = 0;
	
	while (1)
	{
		timer = time(NULL);
		tm_info = localtime(&timer);

		strftime(buffer, 26, "%Y-%m-%d %H:%M:%S\0", tm_info);
		puts(buffer);
//		sprintf(buffer, "%d", count);
		
		if (UiObj.uiAvailable())
		{
			cout << "set label count: " << count << endl;
			UiObj.set_lv_example_label_CAR(buffer);
		}
		else
		{
			cout << "NOT uiAvailable" << endl;
		}

		sleep(1);
		count++;
	}
	
	return gExitSignal;
}
