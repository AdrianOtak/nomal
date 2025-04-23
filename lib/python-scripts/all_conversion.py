import os
from PIL import Image

# Define paths
images_folder = "images/"
conversions_folder = "conversions/"

# Ensure the conversions folder exists
os.makedirs(conversions_folder, exist_ok=True)

# Define thresholds for color classification
black_threshold = 100

# Function to apply Floyd-Steinberg error diffusion and convert the image to black/white
def floydSteinbergDither(pixels, width, height):
    for y in range(height):
        for x in range(width):
            old_pixel = pixels[x, y]
            r, g, b = old_pixel

            # Determine the closest color (Black or White only)
            if r < black_threshold and g < black_threshold and b < black_threshold:
                new_pixel = (0, 0, 0)  # Black
            else:
                new_pixel = (255, 255, 255)  # White

            # Set the new pixel value
            pixels[x, y] = new_pixel

            # Calculate quantization error
            quant_error = (
                r - new_pixel[0],
                g - new_pixel[1],
                b - new_pixel[2],
            )

            # Spread the error to neighboring pixels
            if x < width - 1:
                pixels[x + 1, y] = tuple(
                    clamp(round(pixels[x + 1, y][i] + quant_error[i] * 7 / 16)) for i in range(3)
                )
            if y < height - 1:
                if x > 0:
                    pixels[x - 1, y + 1] = tuple(
                        clamp(round(pixels[x - 1, y + 1][i] + quant_error[i] * 3 / 16))
                        for i in range(3)
                    )
                pixels[x, y + 1] = tuple(
                    clamp(round(pixels[x, y + 1][i] + quant_error[i] * 5 / 16))
                    for i in range(3)
                )
                if x < width - 1:
                    pixels[x + 1, y + 1] = tuple(
                        clamp(round(pixels[x + 1, y + 1][i] + quant_error[i] * 1 / 16))
                        for i in range(3)
                    )

# Function to clamp value between 0 and 255
def clamp(value):
    """Clamp value between 0 and 255."""
    return max(0, min(255, value))

# Function to save bitmap as .bin file
def save_bitmap_as_bin(bw_bitmap, filename):
    with open(filename, "wb") as bin_file:
        for byte in bw_bitmap:
            bin_file.write(byte.to_bytes(1, 'little'))

# Iterate through each PNG file in the images folder
for filename in os.listdir(images_folder):
    if filename.endswith(".JPG"):
        # Open and resize the image
        img = Image.open(os.path.join(images_folder, filename))
        img = img.resize((800, 480))  # Resize to match display resolution
        img = img.convert("RGB")  # Convert to RGB for processing

        # Process the image pixels for black/white
        pixels = img.load()
        floydSteinbergDither(pixels, img.width, img.height)

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

        # Save the bitmap as .bin in the conversions folder
        bin_filename = os.path.join(conversions_folder, f"{filename[:-4]}.bin")
        save_bitmap_as_bin(bw_bitmap, bin_filename)
        print(f"Converted {filename} to {bin_filename}")

print("All jpg files have been converted and saved as .bin files.")
print("Converted files should be located in:", os.path.abspath(conversions_folder))