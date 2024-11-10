import pyqtgraph as pg
from PyQt5 import QtWidgets, QtCore
from PyQt5.QtWidgets import QAction, QMessageBox, QMainWindow, QOpenGLWidget
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np
import sys
import time
from Serial_Handler import Serial_Handler

#to run: 
# pip install poetry
# poetry install
# poetry run python .\src\PyQtGraph.py

N_PLOT_POINTS = 10000
TIME_PLOT_UPDATE = 50  # in ms
TIME_3D_UPDATE = 50  # in ms

class SineWavePlot:
    def __init__(self, parent):
        self.plot_widget = pg.PlotWidget(title="Real-Time Sine Wave", titleColor="w")
        self.plot_widget.setYRange(-1, 1)
        self.plot_widget.setXRange(0, 2 * np.pi)
        self.plot_widget.setBackground("k")  # Set background to black
        self.plot_widget.showGrid(x=True, y=True)  # Show grid
        self.x_data = np.linspace(0, 2 * np.pi, 50)
        self.curve = self.plot_widget.plot(pen="y")  # Yellow line
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update)
        self.timer.start(1)  # Update every 50 ms

    def update(self):
        y_data = np.sin(self.x_data + time.time())
        self.curve.setData(self.x_data, y_data)

class Discrete_Data_Plot:
    """gets a x and ylist and plots it on a graph"""

    def __init__(self, parent, discrete_data_x, discrete_data_y, name):
        # setup
        self.plot_widget = pg.PlotWidget(title=f"Plot of {name}", titleColor="w")
        self.plot_widget.setBackground("k")  # Set background to black
        self.plot_widget.showGrid(x=True, y=True)  # Show grid
        self.curve = self.plot_widget.plot(pen="g")  # Green line for discrete values

        self.discrete_data_x = discrete_data_x
        self.discrete_data_y = discrete_data_y

        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update_data)
        self.timer.start(TIME_PLOT_UPDATE)

    def update_data(self):
        timestamp = time.time()
        try:
            self.curve.setData(
                self.discrete_data_x,
                self.discrete_data_y,
            )

        except Exception as e:
            print(e)

class Cube_Render(QOpenGLWidget):
    def __init__(
        self, cube_axis_list_x=None, cube_axis_list_y=None, cube_axis_list_z=None
    ):
        super().__init__()
        self.setWindowTitle("3D Coordinate System")
        self.setFixedSize(300, 300)  # Adjusted size for better visibility
        self.arrow_length = 3
        self.cube_axis_list_x = cube_axis_list_x
        self.cube_axis_list_y = cube_axis_list_y
        self.cube_axis_list_z = cube_axis_list_z

    def initializeGL(self):
        glClearColor(0.1, 0.1, 0.1, 1.0)  # Background color
        glEnable(GL_DEPTH_TEST)  # Enable depth testing

    def resizeGL(self, w, h):
        glViewport(0, 0, w, h)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(45, w / h, 0.1, 100.0)  # Perspective projection
        glMatrixMode(GL_MODELVIEW)

    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glLoadIdentity()

        # Move back along the Z-axis to zoom out
        glTranslatef(0.0, 0.0, -10)  # Increase the negative value to zoom out further

        x_axies = 0
        y_axies = 0
        z_axies = 0

        if self.cube_axis_list_x is not None and len(self.cube_axis_list_x):
            x_axies = self.cube_axis_list_x[-1]

        if self.cube_axis_list_y is not None and len(self.cube_axis_list_y):
            y_axies = self.cube_axis_list_y[-1]

        if self.cube_axis_list_z is not None and len(self.cube_axis_list_z):
            z_axies = self.cube_axis_list_z[-1]

        glRotatef(x_axies - 45, 1, 0, 0)
        glRotatef(y_axies, 0, 1, 0)
        glRotatef(z_axies - 45, 0, 0, 1)

        # Draw the coordinate system
        self.draw_coordinate_system()

        QtCore.QTimer.singleShot(TIME_3D_UPDATE, self.update)  # Refresh at ~60 FPS

    def draw_coordinate_system(self):
        # Draw X, Y, Z axes
        glBegin(GL_LINES)

        # X-axis (Red)
        glColor3f(1.0, 0.0, 0.0)  # Red for X-axis
        glVertex3f(0.0, 0.0, 0.0)
        glVertex3f(self.arrow_length, 0.0, 0.0)

        # Y-axis (Green)
        glColor3f(0.0, 1.0, 0.0)  # Green for Y-axis
        glVertex3f(0.0, 0.0, 0.0)
        glVertex3f(0.0, self.arrow_length, 0.0)

        # Z-axis (Blue)
        glColor3f(0.0, 0.0, 1.0)  # Blue for Z-axis
        glVertex3f(0.0, 0.0, 0.0)
        glVertex3f(0.0, 0.0, self.arrow_length + 0.5)

        glEnd()
        # Optional: Draw arrows at the ends of each axis for better visibility
        self.draw_arrow(self.arrow_length, 0.0, 0.0, "x")  # X-axis arrow
        self.draw_arrow(0.0, self.arrow_length, 0.0, "y")  # Y-axis arrow
        self.draw_arrow(0.0, 0.0, self.arrow_length + 0.5, "z")  # Z-axis arrow

    def draw_arrow(self, x, y, z, axis):
        glBegin(GL_LINES)
        if axis == "x":
            glColor3f(1.0, 0.0, 0.0)  # Red
            glVertex3f(x, y, z)
            glVertex3f(x - 0.1, y - 0.1, z)  # Arrowhead left (shortened)
            glVertex3f(x, y, z)
            glVertex3f(x - 0.1, y + 0.1, z)  # Arrowhead right (shortened)
        elif axis == "y":
            glColor3f(0.0, 1.0, 0.0)  # Green
            glVertex3f(x, y, z)
            glVertex3f(x - 0.1, y - 0.1, z)  # Arrowhead left (shortened)
            glVertex3f(x, y, z)
            glVertex3f(x + 0.1, y - 0.1, z)  # Arrowhead right (shortened)
        elif axis == "z":
            glColor3f(0.0, 0.0, 1.0)  # Blue
            glVertex3f(x, y, z)
            glVertex3f(x - 0.1, y, z - 0.1)  # Arrowhead left (shortened)
            glVertex3f(x, y, z)
            glVertex3f(x + 0.1, y, z - 0.1)  # Arrowhead right (shortened)
        glEnd()


class GUI(QMainWindow):
    def __init__(self):
        super().__init__()
        # self.app = QtWidgets.QApplication([])

        # Create the main window and configure it
        self.setWindowTitle("Real-Time Plots")
        self.resize(1200, 2000)
        self.setStyleSheet("background-color: black; border: 5px solid black;")

        # serial interfaing
        self.serial_port = Serial_Handler(
            port="automatic", baud_rate=9600, timeout=1, format_package="f f f", buffer_max_size=N_PLOT_POINTS
        )
        timestamp = self.serial_port.BUFFER_TIMESTAMP
        axis_x = self.serial_port.BUFFER_FROMATED[0]
        axis_y = self.serial_port.BUFFER_FROMATED[1]
        axis_z = self.serial_port.BUFFER_FROMATED[2]

        # plots creation
        self.sine_wave_plot = SineWavePlot(self)
        self.serial_plotter_x = Discrete_Data_Plot(
            self, name="x_axsis", discrete_data_x=timestamp, discrete_data_y=axis_x
        )
        self.serial_plotter_y = Discrete_Data_Plot(
            self, name="y_axsis", discrete_data_x=timestamp, discrete_data_y=axis_y
        )
        self.serial_plotter_z = Discrete_Data_Plot(
            self, name="z_axsis", discrete_data_x=timestamp, discrete_data_y=axis_z
        )

        # create 3D render
        cube_widget = Cube_Render(
            cube_axis_list_x=axis_x, cube_axis_list_y=axis_y, cube_axis_list_z=axis_z
        )
        # self.setGeometry(100, 100, 800, 600)

        # text field
        self.text_field = QtWidgets.QLineEdit()
        self.text_field.setPlaceholderText("Refresh rate...")
        self.text_field.setStyleSheet("background-color: white; color: black;")
        self.text_field.setFixedSize(450, 450)

        # selector
        self.selector = QtWidgets.QComboBox()
        self.selector.addItems(["Select Option", "Option 1", "Option 2", "Option 3"])
        self.selector.setStyleSheet("background-color: white; color: black;")

        # button
        self.button = QtWidgets.QPushButton("Set refresh rate")
        self.button.clicked.connect(self.on_button_click)
        self.button.setStyleSheet("background-color: green; color: white;")

        # layouts gui
        central_widget = QtWidgets.QWidget()
        self.setCentralWidget(central_widget)
        
        grid_main = QtWidgets.QGridLayout()
        central_widget.setLayout(grid_main)

        # plots box
        plot_box = QtWidgets.QVBoxLayout()
        plot_box.addWidget(self.serial_plotter_x.plot_widget)
        plot_box.addWidget(self.serial_plotter_y.plot_widget)
        plot_box.addWidget(self.serial_plotter_z.plot_widget)

        # commands box
        command_box = QtWidgets.QVBoxLayout()
        command_box.addWidget(self.selector)
        command_box.addWidget(self.text_field)
        command_box.addWidget(self.button)

        # Add elements to the main grid layout
        grid_main.addLayout(plot_box, 0, 0)

        grid_main.addWidget(cube_widget, 0, 1)

        grid_main.addLayout(command_box, 0, 2)

        # Configure quit action in the menu bar
        quit_action = QAction("Quit", self)
        quit_action.triggered.connect(self.close)

        # Show the window
        self.show()

    def closeEvent(self, event):
        # Create the confirmation message box
        confirmation = QMessageBox(self)
        confirmation.setWindowTitle("Confirmation")
        confirmation.setText("Are you sure you want to close the application?")

        # Set custom styles
        confirmation.setStyleSheet(
            "QMessageBox { background-color: black; color: white; }"
            "QPushButton { background-color: green; color: white; padding: 5px; border-radius: 3px; }"
        )

        # Add Yes and No buttons
        yes_button = confirmation.addButton(QMessageBox.Yes)
        no_button = confirmation.addButton(QMessageBox.No)

        # Set button text to be clearly readable
        yes_button.setStyleSheet("background-color: green; color: white;")
        no_button.setStyleSheet("background-color: red; color: white;")

        # Show the message box and handle user choice
        confirmation.exec_()

        if confirmation.clickedButton() == yes_button:
            self.serial_port.IsRunning = False
            event.accept()  # Close the app
        else:
            event.ignore()  # Don't close the app

    def on_button_click(self):
        # selected_option = self.selector.currentText()
        TIME_PLOTUPDATE = self.text_field.text()
        print(TIME_PLOTUPDATE)
        # if selected_option != "Select Option":
        #     value = np.random.rand()
        #     timestamp = time.time()
        #     self.serial_plotter_x.add_discrete_value(value, timestamp)
        #     print(f"Added value: {value:.2f} at {timestamp:.2f} for {selected_option}. Text: {text_value}")


# Start the application
if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = GUI()
    sys.exit(app.exec_())
