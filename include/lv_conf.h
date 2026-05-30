// ========================================================
// lv_conf.h — Baseado no oficial do fabricante (ESP32-2432S028R)
// LVGL 8.3.x
// ========================================================

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

// --- Cor (16-bit RGB565) ---
#define LV_COLOR_DEPTH      16

// --- Memória ---
#define LV_MEM_SIZE         (48U * 1024U)

// --- Tick manual (lv_tick_inc no loop) ---
#define LV_TICK_CUSTOM      0

// --- Performance ---
#define LV_DISP_DEF_REFR_PERIOD     30
#define LV_INDEV_DEF_READ_PERIOD    30

// --- Features ---
#define LV_USE_ANIMATION    1
#define LV_USE_SHADOW       0
#define LV_USE_BLEND_MODES  0
#define LV_USE_OPA_SCALE    0
#define LV_USE_GRADIENT     0
#define LV_USE_GPU          0
#define LV_USE_LOG          0
#define LV_USE_PERF_MONITOR 0
#define LV_USE_MEM_MONITOR  0
#define LV_USE_GROUP        0

// --- Widgets ---
#define LV_USE_ARC          1
#define LV_USE_BAR          1
#define LV_USE_BTN          1
#define LV_USE_BTNMATRIX    1
#define LV_USE_CANVAS       1
#define LV_USE_CHECKBOX     1
#define LV_USE_DROPDOWN     1
#define LV_USE_IMG          1
#define LV_USE_LABEL        1
#define LV_USE_LINE         1
#define LV_USE_ROLLER       1
#define LV_USE_SLIDER       1
#define LV_USE_SWITCH       1
#define LV_USE_TEXTAREA     1
#define LV_USE_TABLE        1
#define LV_USE_WIN          1
#define LV_USE_KEYBOARD     1
#define LV_USE_CALENDAR     1
#define LV_USE_GRID         1
#define LV_USE_MENU         1
#define LV_USE_MSGBOX       1
#define LV_USE_SPINBOX      1
#define LV_USE_TABVIEW      1
#define LV_USE_TILEVIEW     1

// --- Extras (desligados p/ economizar flash) ---
#define LV_USE_FS_IF        0
#define LV_USE_PNG          0
#define LV_USE_GIF          0
#define LV_USE_SJPG         0
#define LV_USE_BMP          0
#define LV_USE_QRCODE       0
#define LV_USE_FREETYPE     0
#define LV_USE_TINY_TTF     0

// --- Fontes ---
#define LV_FONT_MONTSERRAT_14  1
#define LV_FONT_MONTSERRAT_22  1
#define LV_FONT_MONTSERRAT_28  1

// --- Tema ---
#define LV_USE_THEME_DEFAULT    1

#endif // LV_CONF_H
