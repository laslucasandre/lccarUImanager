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
	struct tm* tm_info;
	char dateString[20];
	char timeString[20];
	
	
	managerUi UiObj;
	
	UiObj.buildUi();
		
	static int count = 0;
	
	while (gExitSignal == 0)
	{
		timer = time(NULL);
		tm_info = localtime(&timer);

		strftime(dateString, 20, "%d/%m/%Y\0", tm_info);
		strftime(timeString, 20, "%H:%M:%S\0", tm_info);
		
		if (UiObj.uiAvailable())
		{
			UiObj.lv_set_datetime(dateString, timeString);
		}
		else
		{
			cout << "NOT uiAvailable" << endl;
		}

		sleep(1);
		count++;
	}
	
	if (gExitSignal != 0) 
	{
		UiObj.uiFinish(false);
	}
	
	cout << "Low Cost CAR end: gExitSignal = " << gExitSignal << endl;
	
	return gExitSignal;
}
