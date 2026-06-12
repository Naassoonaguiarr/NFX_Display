// ========================================================
// NFX DISPLAY — Boot Animation com LovyanGFX + LVGL
// ESP32-2432S028R, 320x240, ILI9341 (ou ST7789)
// ========================================================

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lvgl.h>
#include "logo.h"

// ========================================================
// CONFIG DO DISPLAY (LovyanGFX — comprovadamente funcional)
// ========================================================
#ifdef CYD_ST7789
  #define CYD_PANEL lgfx::Panel_ST7789
#else
  #define CYD_PANEL lgfx::Panel_ILI9341
#endif

class LGFX : public lgfx::LGFX_Device {
    CYD_PANEL               _panel_instance;
    lgfx::Bus_SPI           _bus_instance;
    lgfx::Light_PWM         _light_instance;
    lgfx::Touch_XPT2046     _touch_instance;

public:
    LGFX() {
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host   = SPI2_HOST;
            cfg.spi_mode   = 0;
            cfg.freq_write = 40000000;
            cfg.freq_read  = 20000000;
            cfg.spi_3wire  = true;
            cfg.use_lock   = true;
            cfg.dma_channel = SPI_DMA_CH_AUTO;
            cfg.pin_sclk   = 14;
            cfg.pin_mosi   = 13;
            cfg.pin_miso   = 12;
            cfg.pin_dc     = 2;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs   = 15;
            cfg.pin_rst  = -1;
            cfg.panel_width  = 240;
            cfg.panel_height = 320;
            cfg.offset_rotation = 3;
            cfg.rgb_order  = true;    // CYD precisa BGR (circulo vermelho = amarelo sem isso)
            cfg.invert     = false;
#ifdef CYD_ST7789
            cfg.invert     = true;
#endif
            _panel_instance.config(cfg);
        }
        {
            auto cfg = _light_instance.config();
            cfg.pin_bl = 21;
            cfg.freq   = 12000;
            cfg.pwm_channel = 7;
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }
        {
            auto cfg = _touch_instance.config();
            cfg.pin_cs   = 33;
            cfg.pin_int  = 36;
            cfg.pin_mosi = 32;
            cfg.pin_miso = 39;
            cfg.pin_sclk = 25;
            cfg.spi_host = VSPI_HOST;
            cfg.freq = 1000000;
            cfg.bus_shared = false;
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }
        setPanel(&_panel_instance);
    }
};

LGFX tft;

// ========================================================
// LVGL
// ========================================================
static const uint16_t scrWidth  = 320;
static const uint16_t scrHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[scrWidth * 40];  // 40 linhas = buffer seguro
static lv_disp_drv_t disp_drv;

// Logo
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
static lv_obj_t *banner    = NULL;
static lv_obj_t *banner_label = NULL;

// ========================================================
// LVGL Flush (LovyanGFX)
// ========================================================
static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushPixels((uint16_t *)color_p, w * h, true);  // swap bytes p/ SPI MSB-first
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
    lv_obj_set_size(bg, scrWidth, scrHeight);
    lv_obj_set_style_bg_color(bg, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_border_width(bg, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(bg, 0, LV_PART_MAIN);
    lv_obj_align(bg, LV_ALIGN_CENTER, 0, 0);

    // Logo — visível IMEDIATAMENTE (sem animação, pra testar)
    logo_obj = lv_img_create(lv_scr_act());
    lv_img_set_src(logo_obj, &logo_dsc);
    lv_obj_align(logo_obj, LV_ALIGN_CENTER, 0, -25);
    // SEM fade — opacidade total direto

    // Banner vermelho — visível IMEDIATAMENTE
    banner = lv_obj_create(lv_scr_act());
    lv_obj_set_size(banner, scrWidth, 50);
    lv_obj_set_style_bg_color(banner, lv_color_hex(0xCC0000), LV_PART_MAIN);
    lv_obj_set_style_border_width(banner, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(banner, 0, LV_PART_MAIN);
    lv_obj_align(banner, LV_ALIGN_BOTTOM_MID, 0, 0);

    banner_label = lv_label_create(banner);
    lv_label_set_text(banner_label, "NFX  RELES");
    lv_obj_set_style_text_color(banner_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(banner_label, &lv_font_montserrat_22, LV_PART_MAIN);
    lv_obj_center(banner_label);
}

static void iniciar_boot_anim() {
    // Animação desligada por enquanto — exibe tudo direto
    Serial.println("Interface estatica exibida. Logo + banner visiveis?");
}

// ========================================================
// SETUP
// ========================================================
void setup() {
    Serial.begin(115200);

    tft.init();
    tft.setBrightness(255);
    tft.setRotation(1);          // Landscape 320x240
    tft.fillScreen(TFT_BLACK);

    // Teste rápido — circulo vermelho (1.5s)
    tft.fillCircle(160, 120, 40, TFT_RED);
    delay(1500);

    // LVGL
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, scrWidth * 40);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = scrWidth;
    disp_drv.ver_res = scrHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    criar_interface();
    iniciar_boot_anim();

    Serial.println("NFX Display pronto!");
}

// ========================================================
// LOOP
// ========================================================
void loop() {
    lv_tick_inc(5);              // avança o tick do LVGL
    lv_timer_handler();
    delay(5);
}
