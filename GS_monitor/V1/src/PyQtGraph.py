import pyqtgraph as pg
from pyqtgraph.Qt import QtWidgets, QtCore
import numpy as np
import sys
import time
from Serial_manager import Serial

class SineWavePlot:
    def __init__(self, parent):
        self.plot_widget = pg.PlotWidget(title="Real-Time Sine Wave", titleColor='w')
        self.plot_widget.setYRange(-1, 1)
        self.plot_widget.setXRange(0, 2 * np.pi)
        self.plot_widget.setBackground('k')  # Set background to black
        self.plot_widget.showGrid(x=True, y=True)  # Show grid
        self.x_data = np.linspace(0, 2 * np.pi, 100)
        self.curve = self.plot_widget.plot(pen='y')  # Yellow line
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update)
        self.timer.start(50)  # Update every 50 ms

    def update(self):
        y_data = np.sin(self.x_data + time.time())
        self.curve.setData(self.x_data, y_data)

class DiscreteValuePlot:
    def __init__(self, parent):
        self.plot_widget = pg.PlotWidget(title="Discrete Values", titleColor='w')
        self.plot_widget.setBackground('k')  # Set background to black
        self.plot_widget.showGrid(x=True, y=True)  # Show grid
        self.curve = self.plot_widget.plot(pen='g')  # Green line for discrete values
        self.discrete_x_data = []
        self.discrete_y_data = []

        self.Serial_Port = Serial()
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.generate_data)
        self.timer.start(10)  # Redout data every second

    def generate_data(self):
        timestamp = time.time()
        try:
            value = list(self.Serial_Port.read_buffer())[0][0]
            print(F'plot:{value}')
            self.add_discrete_value(value, timestamp)
        except Exception as e:
            pass

    def add_discrete_value(self, value, timestamp):
        self.discrete_x_data.append(timestamp)
        self.discrete_y_data.append(value)
        self.curve.setData(self.discrete_x_data, self.discrete_y_data)

class MainWindow:
    def __init__(self):
        self.app = QtWidgets.QApplication([])

        # Create a window to hold the plots and controls
        self.win = QtWidgets.QWidget()
        self.win.setWindowTitle("Real-Time Plots")
        self.win.resize(800, 600)
        self.win.setStyleSheet("background-color: black; border: 5px solid black;")
        # self.win.setWindowFlags(QtCore.Qt.FramelessWindowHint) #no frame
        # Alternatively, if you want a window with a black frame:

        # Create plot objects
        self.sine_wave_plot = SineWavePlot(self.win)
        self.discrete_value_plot = DiscreteValuePlot(self.win)

        # Create a text field
        self.text_field = QtWidgets.QLineEdit()
        self.text_field.setPlaceholderText("Enter some text...")
        self.text_field.setStyleSheet("background-color: white; color: black;")  # Style for text field
       
        # Create control elements
        self.selector = QtWidgets.QComboBox()
        self.selector.addItems(["Select Option", "Option 1", "Option 2", "Option 3"])
        self.selector.setStyleSheet("background-color: white; color: black;")  # Style for dropdown

        # Add button to trigger action
        self.button = QtWidgets.QPushButton("Add Discrete Value")
        self.button.clicked.connect(self.on_button_click)
        self.button.setStyleSheet("background-color: green; color: white;")  # Style for button

        # Create layout
        self.grid_layout = QtWidgets.QGridLayout()
        self.win.setLayout(self.grid_layout)

        # Add elements to the grid layout
        self.grid_layout.addWidget(self.sine_wave_plot.plot_widget, 0, 0)
        self.grid_layout.addWidget(self.discrete_value_plot.plot_widget, 1, 0)
        self.grid_layout.addWidget(self.text_field, 0, 2)  # Text field in row 2
        self.grid_layout.addWidget(self.selector, 1, 2)  # Dropdown in row 3
        self.grid_layout.addWidget(self.button, 2, 2)     # Button in row 4

        # Show the window
        self.win.show()

    def on_button_click(self):
        selected_option = self.selector.currentText()
        text_value = self.text_field.text()  # Get text from the text field
        if selected_option != "Select Option":
            value = np.random.rand()  # Simulate a random value for demonstration
            timestamp = time.time()
            self.discrete_value_plot.add_discrete_value(value, timestamp)
            print(f"Added value: {value:.2f} at {timestamp:.2f} for {selected_option}. Text: {text_value}")

# Start the application
if __name__ == '__main__':
    window = MainWindow()
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtWidgets.QApplication.instance().exec_()
