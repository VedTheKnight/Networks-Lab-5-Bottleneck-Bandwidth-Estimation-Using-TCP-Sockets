import matplotlib.pyplot as plt
from scipy import stats
import numpy as np

# Initialize an empty list to store the values
values = []

# Read values from 'output.txt'
with open('output.txt', 'r') as f:
    for line in f:
        # Check if the line contains the correct output format
        if "P/(t1-t2)" in line:
            try:
                # Extract the value and convert it to float
                value = float(line.split("=")[1].strip()[:-5])
                values.append(value)
            except ValueError as e:
                print(f"Could not convert line to float: {line}, error: {e}")

# Debugging: Print out the values read from the file
if len(values) == 0:
    print("No values were read from the file.")
# else:
#     print(f"Values read from the file: {values}")

# Check for empty values before plotting
if values:
    # Create a histogram with an appropriate bin size
    counts, bins, patches = plt.hist(values, bins=range(0, 5000, 10), edgecolor='black')  # Adjusted bin size based on your data range
    plt.title('Histogram of Link Speed Estimates')
    plt.xlabel('Estimated Link Speed (mbps)')  # Updated to kbps
    plt.ylabel('Frequency')
    plt.grid(True)
    # plt.xlim(0, 50)  # Adjust x-axis limits based on expected values
    max_count = np.max(counts)

    # Find all bins with the maximum count
    max_bin_indices = np.where(counts == max_count)[0]

    # Print all bins with the maximum values
    for index in max_bin_indices:
        print(f"One of the bins with the maximum count is between {bins[index]} and {bins[index+1]}.")
    unique_values, counts = np.unique(values, return_counts=True)
    max_value_count = np.max(counts)

    # Get all values with the highest frequency (mode)
    modes = unique_values[counts == max_value_count]
    print(f"The mode(s) of the values are {modes} with a count of {max_value_count}.")


    # Save the plot as a PNG file
    plt.savefig('blahh.png')  # Specify the filename
    plt.close()  # Close the plot to free up resources
else:
    print("No valid data to plot.")
