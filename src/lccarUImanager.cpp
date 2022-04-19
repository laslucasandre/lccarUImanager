#include <iostream>
#include <unistd.h>
#include "../managerUi/managerUi.h"

using namespace std;

int main(int argc, char *argv[])
{
	(void)argc; /*Unused*/
	(void)argv; /*Unused*/
	
	char welcomeMessage[] = "Low Cost CAR";
	cout << welcomeMessage << endl;
	
	managerUi UiObj;
	
	UiObj.buildUi();
	
	cout << "endd" << endl;
	
	static int count = 0;
	
	while (1)
	{
		
		if (UiObj.uiAvailable())
		{
			cout << "set label count: " << count << endl;
			UiObj.set_lv_example_label_CAR();
		}
		else
		{
			cout << "NOT uiAvailable" << endl;
		}
		sleep(1);
		count++;
	}
	
	return 0;
}
