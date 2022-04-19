#pragma once

#include<thread>

class managerUi {
public:	
	void buildUi(void);
	void set_lv_example_label_CAR(void);
	
	bool uiAvailable(void);
private:
	static void hal_init(void);
	void runUi(void);
	std::thread thRunUi;
	
	static void set_angle(void);

	static void lv_example_display(void);
		
};
