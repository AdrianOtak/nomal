# konvertiere_bin_zu_header.py
with open("bild1.bin", "rb") as f:
    data = f.read()

with open("bild1.h", "w") as out:
    out.write("const unsigned char bild1[] PROGMEM = {\n")
    for i in range(0, len(data), 12):
        chunk = data[i:i+12]
        out.write("  " + ", ".join(f"0x{b:02X}" for b in chunk) + ",\n")
    out.write("};\n")
    out.write(f"const unsigned int bild1_len = {len(data)};\n")
