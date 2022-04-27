/**
 * @file managerUi.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/

#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <atomic> 

#include "managerUi.h"

#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#include <SDL2/SDL.h>
//#include "../lvgl/lvgl.h"
#include "../lvgl/examples/lv_examples.h"
#include "../lvgl/demos/lv_demos.h"
#include "../lv_drivers/sdl/sdl.h"

#include <stdio.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static std::mutex mtx;

static std::atomic_bool isUiInitializedOk {false};
static std::atomic_bool uiRunningPermition {false};

static lv_obj_t * lbData;
static lv_obj_t * lbTime;

static lv_style_t style_top;
static lv_style_t style_center;
static lv_style_t style_down;

static lv_obj_t * clock_label;
static lv_obj_t * date_label;

static lv_obj_t * tv;

/* Fonts */

static const lv_font_t * font_top;
static const lv_font_t * font_center;

static const lv_font_t * font_down;

const char* DAY[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
const char* MONTH[] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };


/**********************
 *  PUBLIC METHODS
 **********************/

bool managerUi::uiAvailable(void)
{
	return isUiInitializedOk;
}

void managerUi::uiFinish(bool status)
{
	uiRunningPermition = status;
}

void managerUi::lv_set_datetime(char *dateString, char *timeString)
{
	static int conter = 0;
	
	mtx.lock();
	lv_label_set_text_fmt(lbData, "%s", dateString);
	lv_label_set_text_fmt(lbTime, "%s", timeString);
	mtx.unlock();
	
	LV_LOG_USER("cnt: %d [%s] [%s]\n", conter++, dateString, timeString);	
	
}

void managerUi::buildUi(void)
{

	LV_LOG_USER("Start");
	
	/*Initialize LVGL*/
	lv_init();
	
	/*Initialize the HAL (display, input devices, tick) for LVGL*/
	hal_init();	
	
	uiFinish(true);
	
	lv_create_initial_page();
	
	std::thread uiThread([&] {
		runUi();
	});
		
	uiThread.detach();
	
	LV_LOG_USER("end");
}

/**********************
 *  PRIVATE METHODS
 **********************/


void managerUi::runUi(void)
{
	LV_LOG_USER("Start");
	
	static int count = 0;
	
	while (uiRunningPermition)
	{
		/* Periodically call the lv_task handler.
		 * It could be done in a timer interrupt or an OS task too.*/	
		mtx.lock();
		lv_task_handler();
		mtx.unlock();
		usleep(5 * 1000);
		if (count > 200)
		{		
			LV_LOG_USER("Ui Ticked");
			count = 0;	
		}
		count++;
	}	
	
	LV_LOG_USER("End");
	
}

void managerUi::hal_init(void)
{
	/* Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
	sdl_init();

	/*Create a display buffer*/
	static lv_disp_draw_buf_t disp_buf1;
	static lv_color_t buf1_1[SDL_HOR_RES * 100];
	static lv_color_t buf1_2[SDL_HOR_RES * 100];
	lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, SDL_HOR_RES * 100);	

	/*Create a display*/
	static lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv); /*Basic initialization*/
	disp_drv.draw_buf = &disp_buf1;
	disp_drv.flush_cb = sdl_display_flush;
	disp_drv.hor_res = SDL_HOR_RES;
	disp_drv.ver_res = SDL_VER_RES;
	disp_drv.antialiasing = 1;

	lv_disp_t * disp = lv_disp_drv_register(&disp_drv);

	lv_theme_t * th = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
	lv_disp_set_theme(disp, th);

	lv_group_t * g = lv_group_create();
	lv_group_set_default(g);

	/* Add the mouse as input device
	 * Use the 'mouse' driver which reads the PC's mouse*/
	static lv_indev_drv_t indev_drv_1;
	lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
	indev_drv_1.type = LV_INDEV_TYPE_POINTER;

	/*This function will be called periodically (by the library) to get the mouse position and state*/
	indev_drv_1.read_cb = sdl_mouse_read;
	lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);

	static lv_indev_drv_t indev_drv_2;
	lv_indev_drv_init(&indev_drv_2); /*Basic initialization*/
	indev_drv_2.type = LV_INDEV_TYPE_KEYPAD;
	indev_drv_2.read_cb = sdl_keyboard_read;
	lv_indev_t *kb_indev = lv_indev_drv_register(&indev_drv_2);
	lv_indev_set_group(kb_indev, g);

	static lv_indev_drv_t indev_drv_3;
	lv_indev_drv_init(&indev_drv_3); /*Basic initialization*/
	indev_drv_3.type = LV_INDEV_TYPE_ENCODER;
	indev_drv_3.read_cb = sdl_mousewheel_read;
	lv_indev_t * enc_indev = lv_indev_drv_register(&indev_drv_3);
	lv_indev_set_group(enc_indev, g);

	/*Set a cursor for the mouse*/
	LV_IMG_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
	lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
	lv_img_set_src(cursor_obj, &mouse_cursor_icon); /*Set the image source*/
	lv_indev_set_cursor(mouse_indev, cursor_obj); /*Connect the image  object to the driver*/
}
	
void managerUi::lv_create_datetime(lv_obj_t * parent)
{
	
//	lv_obj_t * pDown = lv_obj_create(parent);
//	lv_obj_set_height(pDown, LV_SIZE_CONTENT);

	lbData = lv_label_create(parent);
	
//	lv_obj_add_style(lbData, &style_down, 0);
	
	lv_label_set_long_mode(lbData, LV_LABEL_LONG_WRAP); /*Break the long lines*/
	lv_label_set_recolor(lbData, false); /*Enable re-coloring by commands in the text*/
	lv_label_set_text(lbData, "-");
	
	lv_obj_set_style_text_font(lbData, font_down, 0);
	lv_obj_set_style_text_line_space(lbData, 8, 0);
	lv_obj_set_style_text_color(lbData, lv_color_hex(0xFFFFFF), 0);
	lv_obj_set_style_text_align(lbData, LV_TEXT_ALIGN_LEFT, 0);
	
	lv_obj_set_width(lbData, 120);
	lv_obj_set_height(lbData, 22);
	
	lv_obj_align(lbData, LV_ALIGN_TOP_LEFT, 0, -10);	
	
	lbTime = lv_label_create(parent);
	
//	lv_obj_add_style(lbTime, &style_down, 0);
	
	lv_label_set_long_mode(lbTime, LV_LABEL_LONG_WRAP); /*Break the long lines*/
	lv_label_set_recolor(lbTime, false); /*Enable re-coloring by commands in the text*/
	lv_label_set_text(lbTime, "-");
	
	lv_obj_set_style_text_font(lbTime, font_down, 0);
	lv_obj_set_style_text_line_space(lbTime, 8, 0);
	lv_obj_set_style_text_color(lbTime, lv_color_hex(0xFFFFFF), 0);
	lv_obj_set_style_text_align(lbTime, LV_TEXT_ALIGN_LEFT, 0);
	
	lv_obj_set_width(lbTime, 90);
	lv_obj_set_height(lbTime, 22);
	
	lv_obj_align(lbTime, LV_ALIGN_TOP_RIGHT, 0, -10);	
	
//	LV_IMG_DECLARE(img_demo_widgets_avatar);
//	lv_obj_t * avatar2 = lv_img_create(parent);
//	lv_img_set_src(avatar2, &img_demo_widgets_avatar);	
//	lv_obj_align(avatar2, LV_ALIGN_CENTER, 0, 0);	
//	lv_img_set_zoom(avatar2, 64);	
	
	LV_IMG_DECLARE(parktelltaleimagealpha100);
	lv_obj_t * avatar = lv_img_create(parent);
	lv_img_set_src(avatar, &parktelltaleimagealpha100);
	lv_obj_align(avatar, LV_ALIGN_CENTER, 0, 0);		
	lv_img_set_zoom(avatar, 75);

	
}

void managerUi::lv_create_initial_page(void)
{
	
	font_top = &lv_font_montserrat_20;
	font_center = &lv_font_montserrat_24;
	font_down = &lv_font_montserrat_20;
	
	lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, font_center);
	
	lv_style_init(&style_top);
//	lv_style_set_text_opa(&style_top, LV_OPA_50);
	lv_style_init(&style_center);
	lv_style_init(&style_down);
	
//	lv_obj_set_style_text_font(lv_scr_act(), font_center, 0);

	lv_style_set_bg_color(&style_down, lv_color_hex(0x000000));
//	lv_style_set_border_color(&style_down, lv_palette_lighten(LV_PALETTE_BLUE, 3));
//	lv_style_set_border_width(&style_down, 3);
//	lv_style_set_radius(&style_down, 5);

	lv_obj_t * panelDown = lv_obj_create(lv_scr_act());
	lv_obj_set_height(panelDown, 35);
	lv_obj_set_width(panelDown, 480);
	lv_obj_align(panelDown, LV_ALIGN_BOTTOM_LEFT, 0, 0);
	lv_obj_add_style(panelDown, &style_down, 0);

	lv_create_datetime(panelDown);
	

//	lv_img_set_zoom(avatar,128);
	

	
//	lv_obj_set_size(avatar, 30, 30);
	
	
	isUiInitializedOk = true;
}
