# Read the input .txt file
with open("conversions/bitmap_fail.txt", "r") as file:
    data = file.read()

# Remove the '0x' prefix and any commas, then split by spaces
clean_data = data.replace("0x", "").replace(",", "").split()

# Convert the cleaned hex data to binary
byte_data = bytes([int(value, 16) for value in clean_data])

# Write the binary data to a .bin file
with open("bitmap_fail.bin", "wb") as bin_file:
    bin_file.write(byte_data)

print("File successfully converted to .bin!")
