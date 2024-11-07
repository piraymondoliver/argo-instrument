#from os import defpath
#import io
import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import re
import serial as ser
#import sys
#import threading
import time
#from pynput import keyboard
from serial.serialutil import Timeout

# Declare and initialize local variables and constants
hashval = 0

#font = {'family': 'serif',
#        'color':  'slateblue',
#        'weight': 'normal',
#        'size': 16,
#        }

image_row_labels = ["Row 1", "Row 2", "Row 3", "Row 4", "Row 5", "Row 6", "Row 7", "Row 8"]
image_col_labels = ["Col 1", "Col 2", "Col 3", "Col 4", "Col 5", "Col 6", "Col 7", "Col 8"]

def heatmap(data, row_labels, col_labels, ax=None,
            cbar_kw={}, cbarlabel="", **kwargs):
    """
    Create a heatmap from a numpy array and two lists of labels.

    Parameters
    ----------
    data
        A 2D numpy array of shape (N, M).
    row_labels
        A list or array of length N with the labels for the rows.
    col_labels
        A list or array of length M with the labels for the columns.
    ax
        A `matplotlib.axes.Axes` instance to which the heatmap is plotted.  If
        not provided, use current axes or create a new one.  Optional.
    cbar_kw
        A dictionary with arguments to `matplotlib.Figure.colorbar`.  Optional.
    cbarlabel
        The label for the colorbar.  Optional.
    **kwargs
        All other arguments are forwarded to `imshow`.
    """

    if not ax:
        ax = plt.gca()

    # Plot the heatmap
    im = ax.imshow(data, **kwargs)

    # Create colorbar
    cbar = ax.figure.colorbar(im, ax=ax, **cbar_kw)
    cbar.ax.set_ylabel(cbarlabel, rotation=-90, va="bottom")

    # We want to show all ticks...
    ax.set_xticks(np.arange(data.shape[1]))
    ax.set_yticks(np.arange(data.shape[0]))
    # ... and label them with the respective list entries.
    ax.set_xticklabels(col_labels)
    ax.set_yticklabels(row_labels)

    # Let the horizontal axes labeling appear on top.
    ax.tick_params(top=True, bottom=False,
                   labeltop=True, labelbottom=False)

    # Rotate the tick labels and set their alignment.
    plt.setp(ax.get_xticklabels(), rotation=-30, ha="right",
             rotation_mode="anchor")

    # Turn spines off and create white grid.
    ax.spines[:].set_visible(False)

    ax.set_xticks(np.arange(data.shape[1]+1)-.5, minor=True)
    ax.set_yticks(np.arange(data.shape[0]+1)-.5, minor=True)
    ax.grid(which="minor", color="w", linestyle='-', linewidth=3)
    ax.tick_params(which="minor", bottom=False, left=False)

    return im, cbar

def annotate_heatmap(im, data=None, valfmt="{x:.2f}",
                     textcolors=("black", "white"),
                     threshold=None, **textkw):
    """
    A function to annotate a heatmap.

    Parameters
    ----------
    im
        The AxesImage to be labeled.
    data
        Data used to annotate.  If None, the image's data is used.  Optional.
    valfmt
        The format of the annotations inside the heatmap.  This should either
        use the string format method, e.g. "$ {x:.2f}", or be a
        `matplotlib.ticker.Formatter`.  Optional.
    textcolors
        A pair of colors.  The first is used for values below a threshold,
        the second for those above.  Optional.
    threshold
        Value in data units according to which the colors from textcolors are
        applied.  If None (the default) uses the middle of the colormap as
        separation.  Optional.
    **kwargs
        All other arguments are forwarded to each call to `text` used to create
        the text labels.
    """

    if not isinstance(data, (list, np.ndarray)):
        data = im.get_array()

    # Normalize the threshold to the images color range.
    if threshold is not None:
        threshold = im.norm(threshold)
    else:
        threshold = im.norm(data.max())/2.

    # Set default alignment to center, but allow it to be
    # overwritten by textkw.
    kw = dict(horizontalalignment="center",
              verticalalignment="center")
    kw.update(textkw)

    # Get the formatter in case a string is supplied
    if isinstance(valfmt, str):
        valfmt = mpl.ticker.StrMethodFormatter(valfmt)

    # Loop over the data and create a `Text` for each "pixel".
    # Change the text's color depending on the data.
    texts = []
    for i in range(data.shape[0]):
        for j in range(data.shape[1]):
            kw.update(color=textcolors[int(im.norm(data[i, j]) < threshold)])
            text = im.axes.text(j, i, valfmt(data[i, j], None), **kw, fontsize='small', fontweight='bold')
            texts.append(text)

    return texts

##################################
#    Start intial program run    #   
##################################

# Get the timestamp of the Python program intialization
fname_timestamp = time.strftime("%Y-%m-%d_%H.%M.%S")
print(fname_timestamp) # Print it to console

# Create a new logfile under the name of the current timestamp
logfile = open(fname_timestamp + "_ARGO_log" + '.txt', "x")
logfile.close()

# Create a new logfile under the name of the current timestamp
csvfile = open(fname_timestamp + "_ARGO_templog" + '.csv', "x")
csvfile.close()

# Open the COM port on the ARGO
argo = ser.Serial(port='COM9', baudrate=9600, timeout=15)

# Wait 4 seconds
time.sleep(4)

# Send a stimulus to the ARGO
argo.write(str.encode("\n"))

# Go into the main loop
while True:
    # Wait 2 seconds
    time.sleep(4)

    # Get the serial data from the ARGO
    bytesToRead = argo.inWaiting()
    serial_data = argo.read(bytesToRead).decode('utf-8')
    time.sleep(1)
    print(serial_data)

    # Clean the serial data of extra newlines that are empty
    serial_data_buffer = serial_data.split("\n")
    non_empty_lines = [line for line in serial_data_buffer if line.strip() != ""]
    serial_data_without_empty_lines = ""
    for line in non_empty_lines:
        serial_data_without_empty_lines += (line + "\n")

    # Print the serial data for testing and verification
    print(serial_data_without_empty_lines)

    # Append the new serial data to the logfile
    logfile = open(fname_timestamp + "_ARGO_log" + '.txt', "a")
    logfile.write(serial_data_without_empty_lines)
    logfile.close()

    cleaned_line = ""
    line_buffer = str.splitlines(serial_data_without_empty_lines)

    for line in line_buffer:
        # Retrieve the temperature of the IR sensor
        if (line.startswith("^^ ")):
            # Remove the first three chars from the line ("^^ " in the case of an IR sensor thermistor temperature header)
            cleaned_line = re.sub('[^\d.]', '', line)
            #print(cleaned_line)
            IR_sensor_temp = float(cleaned_line)

            # Append the IR sensor thermistor temp to the logfile
            logfile = open(fname_timestamp + "_ARGO_log" + '.txt', "a")
            IR_sensor_temp_str = ("IR Sensor Thermistor Temperature: " + str(IR_sensor_temp) + " *C\n")
            logfile.write(IR_sensor_temp_str)
            logfile.close()
            
            # Wait 1 sec
            time.sleep(1)

        # Retrieve and parse the timestamp data
        if (line.startswith("$$ ")):
            # Remove the first three chars from the line ("$$ " in the case of an Arduino millis timestamp header)
            cleaned_line = re.sub('[^\d.]', '', line)

            # Get the timestamp data
            millis_timestamp = int(cleaned_line)

            # Append the millis timestamp to the logfile
            logfile = open(fname_timestamp + "_ARGO_log" + '.txt', "a")
            millis_confirmation = (str(millis_timestamp) + " milliseconds have elapsed since the ARGO has booted (Arduino-side)\n")
            logfile.write(millis_confirmation)
            logfile.close()

            # Wait 1 sec
            time.sleep(1)

        # Clear the cleaned_line buffer
        cleaned_line = ""

        # Parse the image data and generate an image
        if (line.startswith("!! ")):
            # Remove the first three chars from the line ("!! " in the case of an IR Image header)
            cleaned_line = re.sub('(?m)[!@#$ \n\0\r]', '', line)

            # Get the floats from the cleaned line and reshape into an 8x8 array
            # Test
            print(cleaned_line)
            
            image_float_array = [float(i) for i in cleaned_line.split(",")]

            float_array_1x64 = np.array(image_float_array)
            final_float_array = np.array(image_float_array) 
            #print(final_float_array)
            final_float_array = np.reshape(final_float_array, (8,8))
            #print(final_float_array)

            # Get max and min temp values from the array
            max_val = np.amax(final_float_array)
            min_val = np.amin(final_float_array)
            max_val_str = str(print(max_val))
            min_val_str = str(print(min_val))
            max_val_int = float(np.amax(final_float_array))
            min_val_int = float(np.amin(final_float_array))
            max_min_temps = str(print("TMax: " + str(max_val_int) + "*C    Tmin: " + str(min_val_int) + "*C"))
            print(max_min_temps)

            # Plot the data
            plt_name = ("IR Camera Target Heatmap: millis = " + str(millis_timestamp) + ", TMax = " + str(max_val_int) + "*C, Tmin = " + str(min_val_int) + "*C")
            fig, ax = plt.subplots()
            im, cbar = heatmap(final_float_array, image_row_labels, image_col_labels, ax=ax,
                            cmap="plasma", cbarlabel="Temperature [*C]", interpolation="catrom")
            texts = annotate_heatmap(im, valfmt="{x:.1f}")
            ax.set_title(plt_name)
            fig.tight_layout()    
            plt.savefig( (str(fname_timestamp) + "_ARGO_image" + str(millis_timestamp) + ".png"), dpi=100)
            plt.show()  
            plt.close(fig)

            # Wait 500 ms
            time.sleep(0.5)

            # Append the temp data to the csv file
            csvfile = open(fname_timestamp + "_ARGO_templog" + '.csv', "a")
            csvfile.write(str(cleaned_line) + "\n")
            csvfile.close()

        # Retreive and parse the hash data
        if (line.startswith("## ")):
            # Remove the first three chars from the line ("## " in the case of an IR Image hash header)
            cleaned_line = re.sub('[^\d.]', '', line)
            print(float(cleaned_line))
      
            # Hash the floats that were transmitted
            hashval = 0.0
            i = 0
            for i in range(len(float_array_1x64)):
                hashval += np.single(float_array_1x64[i])
               
            # Wait 500 ms
            time.sleep(0.5)
            
            # Get the timestamp data
            image_hash = float(cleaned_line)
            print(hashval)
            if (abs(image_hash - hashval) < 1):
                # Append a confirmation that the received image hashed correctly to the logfile
                logfile = open(fname_timestamp + "_ARGO_log" + '.txt', "a")
                hash_confirmation = ("The image was succesfully retrieved\nTransmitted Hash: " + str(image_hash) + "    Checksum: " + str(hashval) + "\n\n\n")
                logfile.write(hash_confirmation)
                logfile.close()

            # Wait 500 ms
            time.sleep(0.5)

            # Calculate the average temp
            #t_avg = float(np.mean(final_float_array))
            #area = 0.011 # target area in m^2

            #Calculate the emmisivity for the 1st temperature trial at 36*C
            #if (t_avg < 52.5):
            #    t_36_degC = 36.0

            #Calculate the emmisivity for the 3rd temperature trial at 69*C
            #elif (t_avg < 73.0):
            #    t_69_degC = 69.0

            #Calculate the emmisivity for the 3rd temperature trial at 77*C
            #else:
            #    t_77_degC = 77.0
    
    # Wait 500 ms
    time.sleep(0.5)

    # Clear the cleaned_line buffer
    cleaned_line = ""

    #logfile = open(fname_timestamp + "_ARGO_log" + '.txt', "r")
    #print(logfile.read())
    #logfile.close()

    # Get user input
    print("(Press any key in the Python Console to Enter a Command and Generate or Refresh the Main Menu)\n")
    x = input()
    argo.write(str.encode(x))
    time.sleep(0.1)
