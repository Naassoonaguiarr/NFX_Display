// ========================================================
// TESTE COM LovyanGFX — config EXATA do repo AthenasArch
// ESP32-2432S028R, ILI9341, SPI2_HOST, backlight PWM
// ========================================================

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// --- Configuração EXATA do lgfx_ESP32_2432S028.h funcional ---
class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_ILI9341 _panel;
    lgfx::Bus_SPI    _bus;
    lgfx::Light_PWM  _light;
    lgfx::Touch_XPT2046 _touch;

public:
    LGFX() {
        // ---- SPI bus (display no HSPI = SPI2_HOST) ----
        {
            auto cfg = _bus.config();
            cfg.spi_host   = SPI2_HOST;   // HSPI
            cfg.spi_mode   = 0;
            cfg.freq_write = 40000000;    // 40 MHz
            cfg.freq_read  = 16000000;
            cfg.spi_3wire  = false;
            cfg.use_lock   = true;
            cfg.dma_channel = 1;
            cfg.pin_sclk   = 14;
            cfg.pin_mosi   = 13;
            cfg.pin_miso   = 12;
            cfg.pin_dc     = 2;
            _bus.config(cfg);
            _panel.setBus(&_bus);
        }

        // ---- Display ILI9341 ----
        {
            auto cfg = _panel.config();
            cfg.pin_cs    = 15;
            cfg.pin_rst   = -1;
            cfg.pin_busy  = -1;
            cfg.memory_width  = 240;
            cfg.memory_height = 320;
            cfg.panel_width   = 240;
            cfg.panel_height  = 320;
            cfg.offset_x  = 0;
            cfg.offset_y  = 0;
            cfg.offset_rotation = 0;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits  = 1;
            cfg.readable    = true;
            cfg.invert      = false;
            cfg.rgb_order   = false;  // RGB (false = RGB, true = BGR)
            cfg.dlen_16bit  = false;
            cfg.bus_shared  = false;
            _panel.config(cfg);
        }

        // ---- Backlight PWM (GPIO 21) ----
        {
            auto cfg = _light.config();
            cfg.pin_bl = 21;
            cfg.invert = false;
            cfg.freq   = 44100;        // 44.1 kHz PWM
            cfg.pwm_channel = 7;
            _light.config(cfg);
        }

        // ---- Touch XPT2046 (VSPI) ----
        {
            auto cfg = _touch.config();
            cfg.x_min      = 300;
            cfg.x_max      = 3900;
            cfg.y_min      = 200;
            cfg.y_max      = 3700;
            cfg.pin_int    = 36;
            cfg.bus_shared = false;
            cfg.offset_rotation = 6;
            cfg.spi_host   = VSPI_HOST;
            cfg.freq       = 1000000;  // 1 MHz
            cfg.pin_sclk   = 25;
            cfg.pin_mosi   = 32;
            cfg.pin_miso   = 39;
            cfg.pin_cs     = 33;
            _touch.config(cfg);
        }

        setPanel(&_panel);
    }
};

LGFX tft;

// ========================================================
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n=== TESTE LovyanGFX ESP32-2432S028R ===");

    tft.init();
    tft.setBrightness(255);
    tft.setRotation(1);          // Landscape 320x240

    Serial.print("Largura: ");  Serial.println(tft.width());
    Serial.print("Altura: ");   Serial.println(tft.height());
    Serial.println("Iniciando teste de cores...");
}

void loop() {
    Serial.println("VERMELHO");
    tft.fillScreen(TFT_RED);
    delay(1500);

    Serial.println("VERDE");
    tft.fillScreen(TFT_GREEN);
    delay(1500);

    Serial.println("AZUL");
    tft.fillScreen(TFT_BLUE);
    delay(1500);

    Serial.println("BRANCO");
    tft.fillScreen(TFT_WHITE);
    delay(1500);

    Serial.println("PRETO + circulo + texto");
    tft.fillScreen(TFT_BLACK);
    tft.fillCircle(160, 120, 60, TFT_RED);
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString("NFX RELES", 160, 100, &fonts::FreeSansBold24pt7b);
    tft.drawCentreString("LovyanGFX OK!", 160, 150, &fonts::FreeSans12pt7b);
    delay(3000);
}
