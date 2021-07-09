#include "../../lv_examples.h"
#include <stdio.h>
#include "SHT30.h"
#if LV_USE_BAR && LV_BUILD_EXAMPLES


/**
 * A temperature meter example
 */
lv_obj_t * temp_bar;
lv_obj_t * humi_bar;
lv_obj_t * temp_label;
lv_obj_t * humi_label;
void SHT30_bar(void)
{
    static lv_style_t style_indic;

    lv_style_init(&style_indic);
    lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_color(&style_indic, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_dir(&style_indic, LV_GRAD_DIR_VER);

    temp_bar = lv_bar_create(lv_scr_act());
    lv_obj_add_style(temp_bar, &style_indic, LV_PART_INDICATOR);
    lv_obj_set_size(temp_bar, 20, 200);
    lv_obj_align(temp_bar, LV_ALIGN_LEFT_MID, 20, -50);
    lv_bar_set_range(temp_bar, 0, 40);

    humi_bar = lv_bar_create(lv_scr_act());
    lv_obj_add_style(humi_bar, &style_indic, LV_PART_INDICATOR);
    lv_obj_set_size(humi_bar, 20, 200);
    lv_obj_align(humi_bar, LV_ALIGN_LEFT_MID, 60, -50);
    lv_bar_set_range(humi_bar, 0, 100);


    temp_label = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(temp_label, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(temp_label, true);                      /*Enable re-coloring by commands in the text*/
    lv_obj_set_width(temp_label, 60);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(temp_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(temp_label, LV_ALIGN_LEFT_MID, 0, 60);

    humi_label = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(humi_label, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(humi_label, true);                      /*Enable re-coloring by commands in the text*/
    lv_obj_set_width(humi_label, 60);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(humi_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(humi_label, LV_ALIGN_LEFT_MID, 60, 60);
}


#endif
