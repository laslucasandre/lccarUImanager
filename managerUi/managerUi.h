#pragma once

#include<thread>
#include<mutex>

#include "../lvgl/lvgl.h"

class managerUi {
public:	
	void buildUi(void);
	
	void runUi(void);
	void lv_set_datetime(char *dateString, char *timeString);
	
	bool uiAvailable(void);
	
	void uiFinish(bool status);
	
private:
	static void hal_init(void);
	
	
	static void set_angle(void);

	static void lv_create_datetime(lv_obj_t * parent);
	
	static void lv_create_initial_page(void);
		
};
