#include "lvgl_app.h"
#include <stdio.h>


lv_obj_t * label1;
void counter_label(void)
{
    label1 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    
    lv_label_set_text(label1, "counter");
    lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);


    // lv_obj_t * label2 = lv_label_create(lv_scr_act());
    // lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
    // lv_obj_set_width(label2, 150);
    // lv_label_set_text(label2, "It is a circularly scrolling text.");
    // lv_obj_align(label2, LV_ALIGN_CENTER, 0, 40);
}

lv_obj_t * wave_chart;
lv_chart_series_t * wave_ser ;
lv_obj_t * wave_slider_x;
lv_obj_t * wave_slider_y;
static void slider_x_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    int32_t v = lv_slider_get_value(obj);
    lv_chart_set_zoom_x(wave_chart, v);
}
static void slider_y_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    int32_t v = lv_slider_get_value(obj);
    lv_chart_set_zoom_y(wave_chart, v);
}
void wave_chart_init(void)
{
    /*Create a chart*/
    wave_chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(wave_chart, 380, 330);
    lv_obj_align(wave_chart, LV_ALIGN_TOP_LEFT, 50, 0);
    lv_chart_set_range(wave_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 3300);
    lv_chart_set_range(wave_chart, LV_CHART_AXIS_PRIMARY_X, 0, 380);
    lv_chart_set_axis_tick(wave_chart, LV_CHART_AXIS_PRIMARY_X, 5, 5, 5, 1, true, 20);
    lv_chart_set_axis_tick(wave_chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 5, true, 60);
    /*Do not display points on the data*/
    lv_obj_set_style_size(wave_chart, 0, LV_PART_INDICATOR);


    wave_ser = lv_chart_add_series(wave_chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);

    // uint32_t pcnt = sizeof(ecg_sample) / sizeof(ecg_sample[0]);
    // lv_chart_set_point_count(chart, pcnt);
    // lv_chart_set_ext_y_array(chart, ser, (lv_coord_t *)ecg_sample);

    
    wave_slider_x = lv_slider_create(lv_scr_act());
    lv_slider_set_range(wave_slider_x, LV_IMG_ZOOM_NONE, LV_IMG_ZOOM_NONE * 10);
    lv_obj_add_event_cb(wave_slider_x, slider_x_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_size(wave_slider_x, 380, 10);
    lv_obj_align_to(wave_slider_x, wave_chart, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);

    wave_slider_y = lv_slider_create(lv_scr_act());
    lv_slider_set_range(wave_slider_y, LV_IMG_ZOOM_NONE, LV_IMG_ZOOM_NONE * 10);
    lv_obj_add_event_cb(wave_slider_y, slider_y_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_size(wave_slider_y, 10, 290);
    lv_obj_align_to(wave_slider_y, wave_chart, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
}

uint8_t waveStopFlg=0;
uint8_t waveAutoFlg=0;
static void wave_draw_stop(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_VALUE_CHANGED){
        //printf("STOP/RUN\r\n");
        if(waveStopFlg==1){
            waveStopFlg=0;
        }else{
            waveStopFlg=1;
        }
    }
}

static void wave_zoom_stop(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED){
        printf("RESET\r\n");
        lv_slider_set_value(wave_slider_x, 0, LV_ANIM_ON);
        lv_slider_set_value(wave_slider_y, 0, LV_ANIM_ON);
        lv_chart_set_zoom_x(wave_chart,  256);
        lv_chart_set_zoom_y(wave_chart, 256);
    }
}

static void wave_auto_mode(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
   if(code == LV_EVENT_VALUE_CHANGED){
        if(waveAutoFlg==1){
            waveAutoFlg=0;
        }else{
            waveAutoFlg=1;
        }
    }
}

void wave_btn(void)
{
    lv_obj_t * label;

    lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn1, wave_zoom_stop, LV_EVENT_ALL, NULL);
    lv_obj_align(btn1, LV_ALIGN_TOP_LEFT, 20, 400);

    label = lv_label_create(btn1);
    lv_label_set_text(label, "RESET");
    lv_obj_center(label);

    lv_obj_t * btn2 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn2, wave_draw_stop, LV_EVENT_ALL, NULL);
    lv_obj_align(btn2, LV_ALIGN_TOP_LEFT, 120, 400);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height(btn2, LV_SIZE_CONTENT);

    label = lv_label_create(btn2);
    lv_label_set_text(label, "Run/Stop");
    lv_obj_center(label);

    lv_obj_t * btn3 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn3, wave_auto_mode, LV_EVENT_ALL, NULL);
    lv_obj_align(btn3, LV_ALIGN_TOP_LEFT, 250, 400);
    lv_obj_add_flag(btn3, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height(btn3, LV_SIZE_CONTENT);

    label = lv_label_create(btn3);
    lv_label_set_text(label, "Auto");
    lv_obj_center(label); 
}
uint8_t distortion_mode=0;
static void distortion_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
        char list[5][32]={
            "No distortion",
            "Top distortion",
            "Bottom distortion",
            "Two-way distortion",
            "Crossover distortion",
        };
        for (uint8_t i = 0; i < 5; i++)
        {
            if(strstr(buf,list[i])){
                distortion_mode=i;
            }
        
        }
        printf("mode:%d\r\n",distortion_mode);
    }
}

void distortion_list(void)
{
    /*Create a normal drop down list*/
    lv_obj_t * dd = lv_dropdown_create(lv_scr_act());
    lv_dropdown_set_options(dd, "No distortion\n"
                                "Top distortion\n"
                                "Bottom distortion\n"
                                "Two-way distortion\n"
                                "Crossover distortion"
                                );
    lv_obj_align(dd, LV_ALIGN_TOP_LEFT, 20, 460);\
    lv_obj_set_width(dd,220);
    lv_obj_add_event_cb(dd, distortion_handler, LV_EVENT_ALL, NULL);
}
