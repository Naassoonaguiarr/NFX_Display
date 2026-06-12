"""Convert logo PNG to LVGL-compatible RGB565 C array.
Generates data in ESP32 native byte order (little-endian per pixel)
for use with LV_COLOR_16_SWAP=0 and LovyanGFX pushPixels swap=true."""

from PIL import Image
import os

# Load and convert
img = Image.open(r"C:\Users\Usuario\Downloads\lognfx.png").convert("RGBA")
w, h = img.size
print(f"Logo size: {w}x{h}")

# Resize to fit nicely on 320x240 (leave room for banner)
max_w, max_h = 180, 170
scale = min(max_w / w, max_h / h)
w, h = int(w * scale), int(h * scale)
img = img.resize((w, h), Image.LANCZOS)
print(f"Resized to: {w}x{h}")

# Separate alpha channel
img_rgb = img.convert("RGB")
alpha = img.split()[3]  # alpha channel

output_path = r"C:\Users\Usuario\Documents\PlatformIO\Projects\NFX_Display\include\logo.h"

with open(output_path, "w") as f:
    f.write("// Logo image for LVGL — RGB565 with alpha map\n")
    f.write(f"// Converted from lognfx.png: {w}x{h}\n\n")
    f.write("#ifndef LOGO_IMG_H\n#define LOGO_IMG_H\n\n")
    f.write("#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n")
    f.write(f"#define LOGO_W {w}\n")
    f.write(f"#define LOGO_H {h}\n\n")

    # RGB565 pixel data (little-endian per pixel for ESP32)
    pixels = list(img_rgb.getdata())
    alpha_pixels = list(alpha.getdata())

    rgb565_bytes = []
    for (r, g, b), a in zip(pixels, alpha_pixels):
        if a < 128:  # transparent → use chroma key (magenta)
            rgb565 = 0xF81F  # magenta as transparency marker
        else:
            rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

        # Little-endian: low byte first (ESP32 native)
        rgb565_bytes.append(rgb565 & 0xFF)    # low byte
        rgb565_bytes.append(rgb565 >> 8)      # high byte

    f.write(f"static const uint8_t logo_map[{len(rgb565_bytes)}] = {{\n")
    for i in range(0, len(rgb565_bytes), 16):
        line = ", ".join(f"0x{b:02X}" for b in rgb565_bytes[i:i+16])
        if i + 16 < len(rgb565_bytes):
            f.write(f"    {line},\n")
        else:
            f.write(f"    {line}\n")
    f.write("};\n\n")
    f.write("#ifdef __cplusplus\n}\n#endif\n\n")
    f.write("#endif // LOGO_IMG_H\n")

print(f"Done! {w}x{h}, {len(rgb565_bytes)} bytes")
print(f"Written to: {output_path}")
