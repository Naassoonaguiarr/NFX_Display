// ========================================================
// TESTE LovyanGFX — usando config AUTOMÁTICA do CYD
// ========================================================

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// --- Config CYD: ILI9341 OU ST7789 (alterna via build_flags) ---
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
        // --- Bus SPI (display no HSPI) ---
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host   = SPI2_HOST;       // HSPI
            cfg.spi_mode   = 0;
            cfg.freq_write = 40000000;
            cfg.freq_read  = 20000000;
            cfg.spi_3wire  = true;            // 3-wire SPI (só escreve)
            cfg.use_lock   = true;
            cfg.dma_channel = SPI_DMA_CH_AUTO;
            cfg.pin_sclk   = 14;
            cfg.pin_mosi   = 13;
            cfg.pin_miso   = 12;
            cfg.pin_dc     = 2;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        // --- Panel ---
        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs   = 15;
            cfg.pin_rst  = -1;
            cfg.panel_width  = 240;
            cfg.panel_height = 320;
            cfg.offset_rotation = 3;          // Landscape
#ifdef CYD_ST7789
            cfg.rgb_order   = true;           // BGR
            cfg.invert      = true;
#endif
            _panel_instance.config(cfg);
        }

        // --- Backlight (GPIO 21, PWM) ---
        {
            auto cfg = _light_instance.config();
            cfg.pin_bl = 21;
            cfg.freq   = 12000;               // 12kHz PWM
            cfg.pwm_channel = 7;
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        // --- Touch (VSPI separado) ---
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
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n=== TESTE CYD LovyanGFX ===");

    tft.init();
    tft.setBrightness(255);
    tft.setRotation(1);

    Serial.print("Width:  "); Serial.println(tft.width());
    Serial.print("Height: "); Serial.println(tft.height());

    // Se chegou aqui, display funcionou!
    tft.fillScreen(TFT_RED);
    Serial.println("VERMELHO — ve a tela?");
    delay(2000);
}

void loop() {
    static int cor = 0;
    static const uint16_t cores[] = {
        TFT_RED, TFT_GREEN, TFT_BLUE, TFT_WHITE, TFT_BLACK
    };
    static const char* nomes[] = {
        "VERMELHO", "VERDE", "AZUL", "BRANCO", "PRETO"
    };

    tft.fillScreen(cores[cor]);
    Serial.println(nomes[cor]);

    if (cor == 4) {  // tela preta = desenha algo
        tft.fillCircle(160, 120, 60, TFT_RED);
        tft.setTextColor(TFT_WHITE);
        tft.drawCentreString("NFX RELES", 160, 110, &fonts::Font4);
        Serial.println("  Circulo + texto desenhado");
        delay(3000);
    } else {
        delay(1500);
    }

    cor = (cor + 1) % 5;
}
