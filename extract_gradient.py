from PIL import Image
import matplotlib.pyplot as plt

# Step 1: Read the gradient image
image_path = ''  # Replace with the path to your gradient image
img = Image.open(image_path)

# Step 2: Get the dimensions of the image
width, height = img.size
print(f"Image dimensions: {width}x{height}")

# Step 3: Select a row of pixels to extract the color gradient (assuming the gradient is horizontal)
y = height // 2  # Select the row of pixels in the middle of the image
print(f"Extracting pixels from row: {y}")

# Extract the color values from each pixel in the selected row
gradient_colors = [img.getpixel((x, y)) for x in range(width)]

# Step 4: Remove duplicate color values
gradient_colors = list(dict.fromkeys(gradient_colors))

# Step 5: Print the list of RGB color values
print("List of RGB color values:")
for color in gradient_colors:
    print(color)

# Step 6: Display the color gradient
plt.imshow([gradient_colors], aspect="auto")
plt.axis("off")
plt.title("Extracted Color Gradient")
plt.show