# import matplotlib.pyplot as plt

# # Read the data from the text file
# with open('./input/island.txt', 'r') as file:
#     lines = file.readlines()

# # Extract the number of rows and dimensions
# num_rows, dimensions = map(int, lines[0].split())

# # Extract the data points
# data = [tuple(map(float, line.split())) for line in lines[1:]]

# # Separate the data into x and y components
# x, y = zip(*data)

# # Plot the data
# plt.figure(figsize=(10, 6))
# plt.scatter(x, y, color='black', alpha = 0.5)

# # Add titles and labels
# plt.title('Scatter Plot of Island.txt')
# plt.xlabel('First Dimension')
# plt.ylabel('Second Dimension')

# # Display the plot
# plt.show()

# Read data from a3-1000.txt and write the first two dimensions to a2-1000.txt
input_file = './input/a3-1000.txt'
output_file = './input/a2-1000.txt'

# Read the data from the input file
with open(input_file, 'r') as file:
    lines = file.readlines()

# Extract the number of rows and dimensions
num_rows, dimensions = map(int, lines[0].split())

# Write the output file with the new dimensions
with open(output_file, 'w') as file:
    # Write the header line with the updated dimensions
    file.write(f"{num_rows} 2\n")
    # Process and write each line with only the first two dimensions
    for line in lines[1:]:
        first_two_dimensions = ' '.join(line.split()[:2])
        file.write(first_two_dimensions + '\n')

print(f"Data with first two dimensions has been written to {output_file}")
