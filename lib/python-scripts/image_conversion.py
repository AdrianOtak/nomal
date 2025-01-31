from PIL import Image

# Load the image
img = Image.open("images/fail.jpg")

# Resize to match display resolution (480x800)
img = img.resize((800, 480))

# Convert to RGB for color processing
img = img.convert("RGB")

# Create a new image for output
output_img = Image.new("RGB", img.size)

# Define thresholds for color classification
black_threshold = 100

# Function to save bitmap as PNG
def save_bitmap_as_png(bitmap, width, height, filename):
    img = Image.new("1", (width, height))  # Create a new image with 1-bit mode
    pixels = img.load()

    byte_index = 0
    for y in range(height):
        for x in range(width):
            bit = (bitmap[byte_index] >> (7 - (x % 8))) & 1
            pixels[x, y] = bit
            if x % 8 == 7:
                byte_index += 1
    img.save(filename)

# Error diffusion coefficients
def clamp(value):
    """Clamp value between 0 and 255."""
    return max(0, min(255, value))

pixels = img.load()
output_pixels = output_img.load()

for y in range(img.height):
    for x in range(img.width):
        old_pixel = pixels[x, y]
        r, g, b = old_pixel

        # Determine the closest color (Black or White only)
        if r < black_threshold and g < black_threshold and b < black_threshold:
            new_pixel = (0, 0, 0)  # Black
        else:
            new_pixel = (255, 255, 255)  # White

        # Set the new pixel value
        output_pixels[x, y] = new_pixel

        # Calculate quantization error
        quant_error = (
            r - new_pixel[0],
            g - new_pixel[1],
            b - new_pixel[2],
        )

        # Spread the error to neighboring pixels
        if x < img.width - 1:
            pixels[x + 1, y] = tuple(
                clamp(round(pixels[x + 1, y][i] + quant_error[i] * 7 / 16)) for i in range(3)
            )
        if y < img.height - 1:
            if x > 0:
                pixels[x - 1, y + 1] = tuple(
                    clamp(round(pixels[x - 1, y + 1][i] + quant_error[i] * 3 / 16))
                    for i in range(3)
                )
            pixels[x, y + 1] = tuple(
                clamp(round(pixels[x, y + 1][i] + quant_error[i] * 5 / 16))
                for i in range(3)
            )
            if x < img.width - 1:
                pixels[x + 1, y + 1] = tuple(
                    clamp(round(pixels[x + 1, y + 1][i] + quant_error[i] * 1 / 16))
                    for i in range(3)
                )

# Initialize the bitmap for black/white
bw_bitmap = []

# Create the black/white bitmap with error diffusion
for y in range(img.height):
    bw_byte = 0

    for x in range(img.width):
        old_pixel = pixels[x, y]
        r, g, b = old_pixel

        # Determine the closest color for black/white
        if r < black_threshold and g < black_threshold and b < black_threshold:
            # Black pixel
            bw_pixel = 0  # Black in black/white
        else:
            # White pixel
            bw_pixel = 1  # White in black/white

        # Set the pixel value for the current byte
        bw_byte |= (bw_pixel << (7 - x % 8))

        # After processing 8 pixels, save the byte and reset
        if x % 8 == 7:
            bw_bitmap.append(bw_byte)
            bw_byte = 0

    # Handle remaining bits for widths not divisible by 8
    if img.width % 8 != 0:
        bw_bitmap.append(bw_byte)

# Save bitmap as header file for black/white
def save_bitmap_array(name, bitmap):
    bitmap_array = f"const unsigned char {name}[] PROGMEM = {{\n"
    for i, byte in enumerate(bitmap):
        bitmap_array += f"0x{byte:02X}, "
        if (i + 1) % 16 == 0:
            bitmap_array += "\n"
    bitmap_array += "\n};"

    with open(f"include/{name}.h", "w") as f:
        f.write(bitmap_array)

# Save the bitmap for black/white
save_bitmap_array("bitmap_fail", bw_bitmap)
