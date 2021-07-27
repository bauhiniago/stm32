#include "../../lv_examples.h"
#if LV_USE_IMG && LV_BUILD_EXAMPLES

lv_obj_t * fox_img;

void Wireless_Welcome(void)
{
    fox_img = lv_img_create(lv_scr_act());
    lv_obj_align(fox_img, LV_ALIGN_CENTER, 0, -20);
    lv_obj_set_size(fox_img, 198, 199);

    lv_obj_t * img2 = lv_img_create(lv_scr_act());
    LV_IMG_DECLARE(title);
    lv_img_set_src(img2, &title);
    lv_obj_align(img2, LV_ALIGN_BOTTOM_MID, 0, -66);
    lv_obj_set_size(img2, 352, 66);


}

#endif
