import pyqtgraph as pg
from pyqtgraph.Qt import QtWidgets, QtCore  # Import QtWidgets for QApplication
import numpy as np
import sys
import time

# Create the application object
app = QtWidgets.QApplication([])  # Use QApplication

# Create a window to hold the plot
win = pg.GraphicsLayoutWidget(show=True, title="Real-Time Sine Wave Plot")
win.resize(800, 600)

# Add a plot to the window
plot = win.addPlot(title="Real-Time Sine Wave")

# Set axis limits
plot.setYRange(-1, 1)
plot.setXRange(0, 2 * np.pi)

# Initialize curve for updating data
curve = plot.plot(pen='y')  # yellow line

# Generate initial data
x_data = np.linspace(0, 5 * np.pi, 100)

# This function will update the plot with new data
def update():
    # Update the y values based on the sine wave, using current time as phase shift
    y_data = np.sin(x_data + time.time())
    
    # Update the curve with new data
    curve.setData(x_data, y_data)

# Set up a timer to call the update function periodically (every 50 ms)
timer = QtCore.QTimer()
timer.timeout.connect(update)
timer.start(5)  # 50 ms

# Start the Qt event loop
if __name__ == '__main__':
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtWidgets.QApplication.instance().exec_()  # Use QApplication for the event loop
