"""Convert logo PNG to LVGL-compatible RGB565 C array."""
from PIL import Image
import os

# Load and convert
img = Image.open(r"C:\Users\Usuario\Downloads\lognfx.png").convert("RGB")
w, h = img.size
print(f"Logo size: {w}x{h}")

# Resize if needed (max 200x200 to leave room for banner)
if w > 200 or h > 200:
    scale = min(200 / w, 200 / h)
    w, h = int(w * scale), int(h * scale)
    img = img.resize((w, h), Image.LANCZOS)
    print(f"Resized to: {w}x{h}")

# Convert to RGB565
pixels = list(img.getdata())
rgb565_array = []
for r, g, b in pixels:
    rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
    rgb565_array.append(rgb565)

# Generate C header file
output_path = r"C:\Users\Usuario\Documents\PlatformIO\Projects\NFX_Display\include\logo.h"

with open(output_path, "w") as f:
    f.write("// Auto-generated logo image for LVGL\n")
    f.write(f"// Original: lognfx.png, converted to {w}x{h} RGB565\n\n")
    f.write("#ifndef LOGO_H\n#define LOGO_H\n\n")
    f.write("#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n")
    f.write(f"#define LOGO_W {w}\n")
    f.write(f"#define LOGO_H {h}\n\n")

    f.write(f"static const uint8_t logo_map[{w * h * 2}] = {{\n")

    bytes_list = []
    for val in rgb565_array:
        bytes_list.append(val >> 8)     # high byte
        bytes_list.append(val & 0xFF)   # low byte

    for i in range(0, len(bytes_list), 16):
        line = ", ".join(f"0x{b:02X}" for b in bytes_list[i:i+16])
        if i + 16 < len(bytes_list):
            f.write(f"    {line},\n")
        else:
            f.write(f"    {line}\n")

    f.write("};\n\n")
    f.write("#ifdef __cplusplus\n}\n#endif\n\n")
    f.write("#endif // LOGO_H\n")

print(f"Done! Written to: {output_path}")
print(f"Array size: {len(bytes_list)} bytes ({w*h*2})")
