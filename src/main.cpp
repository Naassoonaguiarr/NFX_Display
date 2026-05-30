#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include "logo.h"

// ========================================================
// Dimensões (landscape 320x240 — padrão oficial)
// ========================================================
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;

// ========================================================
// TFT (construtor com dimensões — padrão oficial)
// ========================================================
TFT_eSPI tft = TFT_eSPI();  // Construtor padrão TFT_eSPI

// ========================================================
// LVGL — buffer generoso (1/4 da tela — padrão oficial)
// ========================================================
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 4];
static lv_disp_drv_t disp_drv;

// ========================================================
// Logo convertida
// ========================================================
static const lv_img_dsc_t logo_dsc = {
    .header = {
        .cf = LV_IMG_CF_TRUE_COLOR,
        .always_zero = 0,
        .reserved = 0,
        .w = LOGO_W,
        .h = LOGO_H,
    },
    .data_size = LOGO_W * LOGO_H * 2,
    .data = logo_map,
};

static lv_obj_t *logo_obj = NULL;
static lv_obj_t *banner = NULL;
static lv_obj_t *banner_label = NULL;

// ========================================================
// LVGL display flush (padrão oficial)
// ========================================================
static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

// ========================================================
// Animações
// ========================================================
static void anim_opa_cb(void *var, int32_t value) {
    lv_obj_set_style_opa((lv_obj_t *)var, (lv_opa_t)value, LV_PART_MAIN);
}

static void anim_y_cb(void *var, int32_t value) {
    lv_obj_set_y((lv_obj_t *)var, value);
}

// Fim do fade → banner sobe
static void on_logo_fade_done(lv_anim_t *a) {
    lv_obj_clear_flag(banner, LV_OBJ_FLAG_HIDDEN);

    lv_anim_t slide;
    lv_anim_init(&slide);
    lv_anim_set_var(&slide, banner);
    lv_anim_set_exec_cb(&slide, anim_y_cb);
    lv_anim_set_values(&slide, 260, 190);
    lv_anim_set_time(&slide, 600);
    lv_anim_set_delay(&slide, 200);
    lv_anim_set_path_cb(&slide, lv_anim_path_ease_out);
    lv_anim_start(&slide);
}

// ========================================================
// Interface
// ========================================================
static void criar_interface() {
    lv_obj_t *bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(bg, screenWidth, screenHeight);
    lv_obj_set_style_bg_color(bg, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_border_width(bg, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(bg, 0, LV_PART_MAIN);
    lv_obj_align(bg, LV_ALIGN_CENTER, 0, 0);

    // Logo centralizada, um pouco acima
    logo_obj = lv_img_create(lv_scr_act());
    lv_img_set_src(logo_obj, &logo_dsc);
    lv_obj_align(logo_obj, LV_ALIGN_CENTER, 0, -30);
    lv_obj_set_style_opa(logo_obj, LV_OPA_TRANSP, LV_PART_MAIN);

    // Banner vermelho inferior
    banner = lv_obj_create(lv_scr_act());
    lv_obj_set_size(banner, screenWidth, 50);
    lv_obj_set_style_bg_color(banner, lv_color_hex(0xCC0000), LV_PART_MAIN);
    lv_obj_set_style_border_width(banner, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(banner, 0, LV_PART_MAIN);
    lv_obj_set_y(banner, 260);
    lv_obj_add_flag(banner, LV_OBJ_FLAG_HIDDEN);

    banner_label = lv_label_create(banner);
    lv_label_set_text(banner_label, "NFX  RELES");
    lv_obj_set_style_text_color(banner_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(banner_label, &lv_font_montserrat_22, LV_PART_MAIN);
    lv_obj_center(banner_label);
}

// ========================================================
// Boot anim
// ========================================================
static void iniciar_boot_anim() {
    lv_anim_t fade;
    lv_anim_init(&fade);
    lv_anim_set_var(&fade, logo_obj);
    lv_anim_set_exec_cb(&fade, anim_opa_cb);
    lv_anim_set_values(&fade, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_time(&fade, 1200);
    lv_anim_set_delay(&fade, 600);
    lv_anim_set_path_cb(&fade, lv_anim_path_ease_in);
    lv_anim_set_ready_cb(&fade, on_logo_fade_done);
    lv_anim_start(&fade);
}

// ========================================================
// SETUP (padrão oficial)
// ========================================================
void setup() {
    Serial.begin(115200);

    // TFT (backlight é ligado automaticamente pelo tft.begin)
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    // Teste visual rápido
    tft.fillCircle(160, 120, 40, TFT_RED);
    Serial.println("TFT OK — circulo vermelho visivel?");
    delay(2000);

    // LVGL init
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 4);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Interface + animação
    criar_interface();
    iniciar_boot_anim();

    Serial.println("NFX Display pronto.");
}

// ========================================================
// LOOP (padrão oficial — sem lv_tick_inc!)
// ========================================================
void loop() {
    lv_timer_handler();
    delay(5);
}
