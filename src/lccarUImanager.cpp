#include <iostream>
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
	
	
	return 0;
}