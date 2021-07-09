#include "lvgl_app.h"
#include <stdio.h>

lv_obj_t * wave_chart;
lv_chart_series_t * wave_ser ;

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
    lv_obj_set_size(wave_chart, 640, 330);
    lv_obj_align(wave_chart, LV_ALIGN_TOP_LEFT, 50, 0);
    lv_chart_set_range(wave_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 3300);
    lv_chart_set_range(wave_chart, LV_CHART_AXIS_PRIMARY_X, 0, 640);
    lv_chart_set_axis_tick(wave_chart, LV_CHART_AXIS_PRIMARY_X, 5, 5, 5, 1, true, 20);
    lv_chart_set_axis_tick(wave_chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 5, true, 60);
    /*Do not display points on the data*/
    lv_obj_set_style_size(wave_chart, 0, LV_PART_INDICATOR);


    wave_ser = lv_chart_add_series(wave_chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);

    // uint32_t pcnt = sizeof(ecg_sample) / sizeof(ecg_sample[0]);
    // lv_chart_set_point_count(chart, pcnt);
    // lv_chart_set_ext_y_array(chart, ser, (lv_coord_t *)ecg_sample);

    lv_obj_t * slider;
    slider = lv_slider_create(lv_scr_act());
    lv_slider_set_range(slider, LV_IMG_ZOOM_NONE, LV_IMG_ZOOM_NONE * 10);
    lv_obj_add_event_cb(slider, slider_x_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_size(slider, 600, 10);
    lv_obj_align_to(slider, wave_chart, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);

    slider = lv_slider_create(lv_scr_act());
    lv_slider_set_range(slider, LV_IMG_ZOOM_NONE, LV_IMG_ZOOM_NONE * 10);
    lv_obj_add_event_cb(slider, slider_y_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_size(slider, 10, 290);
    lv_obj_align_to(slider, wave_chart, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
}

uint8_t waveStopFlg=0;
static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
        printf("clicked\r\n");
        
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");
        printf("toggled\r\n");
        if(waveStopFlg==1){
            waveStopFlg=0;
        }else{
            waveStopFlg=1;
        }
    }
}

void wave_btn(void)
{
    lv_obj_t * label;

    lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn1, LV_ALIGN_BOTTOM_LEFT, 10, -10);

    label = lv_label_create(btn1);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);

    lv_obj_t * btn2 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn2, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn2, LV_ALIGN_BOTTOM_LEFT, 120, -10);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height(btn2, LV_SIZE_CONTENT);

    label = lv_label_create(btn2);
    lv_label_set_text(label, "Run/Stop");
    lv_obj_center(label);
}