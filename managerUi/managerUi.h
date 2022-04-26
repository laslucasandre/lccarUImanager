#pragma once

#include<thread>
#include<mutex>

class managerUi {
public:	
	void buildUi(void);
	
	void runUi(void);
	void set_lv_example_label_CAR(char *info);
	
	bool uiAvailable(void);
private:
	static void hal_init(void);
	
	
	static void set_angle(void);

	static void lv_example_display(void);
		
};
