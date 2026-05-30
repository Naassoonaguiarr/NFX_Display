// ========================================================
// User_Setup.h — TFT_eSPI para ESP32-2432S028 (CYD 2.8")
// ========================================================

#ifndef USER_SETUP_LOADED
#define USER_SETUP_LOADED

// --- Driver do Display ---
#define ILI9341_DRIVER
// #define ST7789_DRIVER       // Descomente se sua placa usa ST7789

// --- Dimensões ---
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

// --- Pinos TFT (HSPI) ---
#define TFT_MISO  12
#define TFT_MOSI  13
#define TFT_SCLK  14
#define TFT_CS    15
#define TFT_DC    2
#define TFT_RST  -1            // Conectado ao RST do ESP32

// --- Backlight ---
#define TFT_BL    21
#define TFT_BACKLIGHT_ON  1

// --- SPI ---
#define SPI_FREQUENCY         40000000
#define SPI_READ_FREQUENCY    20000000
#define SPI_TOUCH_FREQUENCY    2500000

// --- Touch (XPT2046) ---
#define TOUCH_CS  33

// --- Fontes (desabilita as que não precisa para economizar flash) ---
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

// --- SPIFFS (para carregar imagens do filesystem se quiser) ---
#define SPIFFS_IMAGE_MAX_SIZE  (200 * 200 * 2)

#endif // USER_SETUP_LOADED
