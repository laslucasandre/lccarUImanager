
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <atomic> 

#include "managerUi.h"

#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#include <SDL2/SDL.h>
#include "../lvgl/lvgl.h"
#include "../lvgl/examples/lv_examples.h"
#include "../lvgl/demos/lv_demos.h"
#include "../lv_drivers/sdl/sdl.h"

#include <stdio.h>

static std::mutex mtx;
static const lv_font_t * font_large;

static int conter = 0;
char buf[20] = "-";
static lv_obj_t * labelName;

std::atomic_bool isUiInitializedOk {false};


void managerUi::buildUi(void)
{
	std::cout << "buildUi - Start" << std::endl;
	
	/*Initialize LVGL*/
	lv_init();
	
	/*Initialize the HAL (display, input devices, tick) for LVGL*/
	hal_init();	
	
	lv_example_display();
	
	std::thread uiThread([&] {
		runUi();
	});
		
	uiThread.detach();
	
	std::cout << "buildUi - end" << std::endl;
	
}

void managerUi::runUi(void)
{
	std::cout << "runUi start" << std::endl;
	
	static int count = 0;
	
	while (1)
	{
		/* Periodically call the lv_task handler.
		 * It could be done in a timer interrupt or an OS task too.*/	
		mtx.lock();
		lv_task_handler();
		mtx.unlock();
		usleep(5 * 1000);
		if (count > 200)
		{		
			std::cout << "count: " << count << std::endl;
			count = 0;	
		}
		count++;
	}	
	
	std::cout << "runUi end" << std::endl;
	
}

void managerUi::set_lv_example_label_CAR(char *info)
{
	
	conter++;
//	lv_snprintf(buf, sizeof(buf), "%d", conter);
//	lv_snprintf(buf, sizeof(buf), "%s", info);

	printf("set_lv_example_label_CAR %d %s\n", conter, buf);
//	lv_label_set_text(labelName, buf);
//	lv_label_set_text_fmt(labelName, "Value: %d", conter);
	
	mtx.lock();
	lv_label_set_text_fmt(labelName, "%s", info);
	mtx.unlock();
	
	//lv_label_set_text_static(labelName, buf);
	

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
	//lv_disp_draw_buf_init(&disp_buf1, buf1_1, NULL, SDL_HOR_RES * 100);

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
	
void managerUi::lv_example_display(void)
{
//	/*Create an Arc*/
//	lv_obj_t * arc = lv_arc_create(lv_scr_act());
//	lv_obj_set_size(arc, 150, 150);
//	lv_arc_set_rotation(arc, 135);
//	lv_arc_set_bg_angles(arc, 0, 270);
//	lv_arc_set_value(arc, 40);
//	lv_obj_center(arc);

//	lv_obj_t * label1 = lv_label_create(lv_scr_act());
//	lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP); /*Break the long lines*/
//	lv_label_set_recolor(label1, true); /*Enable re-coloring by commands in the text*/
//	lv_label_set_text(label1,
//		"#0000ff Re-color# #ff00ff words# #ff0000 of a# label, align the lines to the center "
//		                "and wrap long text automatically.");
//	lv_obj_set_width(label1, 150); /*Set smaller width to make the lines wrap*/
//	lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
//	lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);
//
//	lv_obj_t * label2 = lv_label_create(lv_scr_act());
//	lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR); /*Circular scroll*/
//	lv_obj_set_width(label2, 150);
//	lv_label_set_text(label2, "It is a circularly scrolling text. ");
//	lv_obj_align(label2, LV_ALIGN_CENTER, 0, 40);
	
	font_large = &lv_font_montserrat_20;

	char *ptr = buf;

	labelName = lv_label_create(lv_scr_act());
	
	lv_label_set_long_mode(labelName, LV_LABEL_LONG_WRAP); /*Break the long lines*/
	lv_label_set_recolor(labelName, false); /*Enable re-coloring by commands in the text*/
	lv_label_set_text(labelName, ptr);
	
	lv_obj_set_style_text_font(labelName, font_large, 0);
	lv_obj_set_style_text_line_space(labelName, 8, 0);
	lv_obj_set_style_text_color(labelName, lv_color_hex(0xFF0000), 0);
	lv_obj_set_style_text_align(labelName, LV_TEXT_ALIGN_LEFT, 0);
	
	lv_obj_set_width(labelName, 220);
	
	lv_obj_align(labelName, LV_ALIGN_BOTTOM_RIGHT, -50, -10);	
	
	isUiInitializedOk = true;
	
}

bool managerUi::uiAvailable(void)
{
	return isUiInitializedOk;
}

