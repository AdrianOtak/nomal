import os
from PIL import Image

# Ordnerpfade
images_folder = "images/"
conversions_folder = "conversions/"
os.makedirs(conversions_folder, exist_ok=True)

# 7-Farbpalette (Waveshare 7.3" ACeP, 4 Bit = 2 Pixel pro Byte)
COLOR_MAP = {
    0x0: (0, 0, 0),         # Black
    0x1: (255, 255, 255),   # White
    0x2: (0, 255, 0),       # Green
    0x3: (0, 0, 255),       # Blue
    0x4: (255, 0, 0),       # Red
    0x5: (255, 255, 0),     # Yellow
    0x6: (255, 165, 0),     # Orange
}

# Hilfsfunktionen
def clamp(value):
    return max(0, min(255, value))

def closest_color(r, g, b):
    min_dist = float('inf')
    best_index = 0
    for index, (cr, cg, cb) in COLOR_MAP.items():
        dist = (r - cr) ** 2 + (g - cg) ** 2 + (b - cb) ** 2
        if dist < min_dist:
            min_dist = dist
            best_index = index
    return best_index

# Floyd–Steinberg Dithering anwenden
def apply_dithering(img):
    pixels = img.load()
    width, height = img.size

    for y in range(height):
        for x in range(width):
            old_pixel = pixels[x, y]
            r_old, g_old, b_old = old_pixel

            index = closest_color(r_old, g_old, b_old)
            r_new, g_new, b_new = COLOR_MAP[index]
            pixels[x, y] = (r_new, g_new, b_new)

            # Fehler berechnen
            err = (
                r_old - r_new,
                g_old - g_new,
                b_old - b_new
            )

            # Fehlerverteilung auf Nachbarpixel
            for dx, dy, factor in [(1, 0, 7 / 16), (-1, 1, 3 / 16), (0, 1, 5 / 16), (1, 1, 1 / 16)]:
                nx, ny = x + dx, y + dy
                if 0 <= nx < width and 0 <= ny < height:
                    pr, pg, pb = pixels[nx, ny]
                    pixels[nx, ny] = (
                        clamp(int(pr + err[0] * factor)),
                        clamp(int(pg + err[1] * factor)),
                        clamp(int(pb + err[2] * factor))
                    )

# Konvertierung starten
for filename in os.listdir(images_folder):
    if filename.lower().endswith((".jpg", ".jpeg", ".png")):
        image_path = os.path.join(images_folder, filename)
        img = Image.open(image_path).convert("RGB").resize((800, 480))

        # Floyd-Steinberg Dithering auf 7-Farbpalette
        apply_dithering(img)
        pixels = img.load()

        # Ausgabe-Byte-Array (2 Pixel pro Byte)
        output_bytes = []
        for y in range(img.height):
            for x in range(0, img.width, 2):
                color1 = closest_color(*pixels[x, y])
                color2 = closest_color(*pixels[x + 1, y])
                byte = (color1 << 4) | color2
                output_bytes.append(byte)

        # Speichern
        out_filename = os.path.join(conversions_folder, filename.rsplit(".", 1)[0] + ".bin")
        with open(out_filename, "wb") as f:
            f.write(bytearray(output_bytes))

        print(f"Converted {filename} -> {out_filename} with dithering.")

print("Alle Bilder wurden erfolgreich mit Dithering konvertiert.")
