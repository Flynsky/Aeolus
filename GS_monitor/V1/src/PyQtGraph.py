import pyqtgraph as pg
from pyqtgraph.Qt import QtWidgets, QtCore
import numpy as np
import sys
import time

class SineWavePlot:
    def __init__(self, parent):
        self.plot_widget = parent.addPlot(title="Real-Time Sine Wave")
        self.plot_widget.setYRange(-1, 1)
        self.plot_widget.setXRange(0, 2 * np.pi)
        self.x_data = np.linspace(0, 2 * np.pi, 100)
        self.curve = self.plot_widget.plot(pen='y')  # yellow line
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update)
        self.timer.start(50)  # Update every 50 ms

    def update(self):
        y_data = np.sin(self.x_data + time.time())
        self.curve.setData(self.x_data, y_data)

class DiscreteValuePlot:
    def __init__(self, parent):
        self.plot_widget = parent.addPlot(title="Discrete Values")
        self.curve = self.plot_widget.plot(pen='g')  # green line for discrete values
        self.discrete_x_data = []
        self.discrete_y_data = []
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.generate_data)
        self.timer.start(1000)  # Generate data every second

    def generate_data(self):
        timestamp = time.time()
        value = np.random.rand()  # Simulated discrete value
        self.add_discrete_value(value, timestamp)

    def add_discrete_value(self, value, timestamp):
        self.discrete_x_data.append(timestamp)
        self.discrete_y_data.append(value)
        self.curve.setData(self.discrete_x_data, self.discrete_y_data)

class MainWindow:
    def __init__(self):
        self.app = QtWidgets.QApplication([])

        # Create a window to hold the plots and controls
        self.win = pg.GraphicsLayoutWidget(show=True, title="Real-Time Sine Wave and Discrete Values Plot")
        self.win.resize(800, 600)

        # Create plot objects
        self.sine_wave_plot = SineWavePlot(self.win)
        self.discrete_value_plot = DiscreteValuePlot(self.win)

        # Create layout for controls
        self.control_layout = QtWidgets.QVBoxLayout()

        # Add dropdown selector
        self.selector = QtWidgets.QComboBox()
        self.selector.addItems(["Select Option", "Option 1", "Option 2", "Option 3"])
        self.control_layout.addWidget(self.selector)

        # Add button to trigger action
        self.button = QtWidgets.QPushButton("Add Discrete Value")
        self.button.clicked.connect(self.on_button_click)
        self.control_layout.addWidget(self.button)

        # Add controls to the window
        self.win.setLayout(self.control_layout)

    def on_button_click(self):
        selected_option = self.selector.currentText()
        if selected_option != "Select Option":
            value = np.random.rand()  # Simulate a random value for demonstration
            timestamp = time.time()
            self.discrete_value_plot.add_discrete_value(value, timestamp)
            print(f"Added value: {value} at {timestamp} for {selected_option}")

# Start the application
if __name__ == '__main__':
    window = MainWindow()
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtWidgets.QApplication.instance().exec_()
