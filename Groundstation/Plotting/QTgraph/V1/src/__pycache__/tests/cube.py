import sys
import numpy as np
from PyQt5 import QtWidgets, QtGui, QtCore
from PyQt5.QtWidgets import QMainWindow, QApplication, QOpenGLWidget
from OpenGL.GL import *
from OpenGL.GLU import *


class CubeWidget(QOpenGLWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle('3D Cube')
        self.setFixedSize(800, 600)
        self.angle = 0  # Rotation angle

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
        glTranslatef(0.0, 0.0, -5)  # Move back along the Z-axis
        glRotatef(self.angle, 1.0, 1.0, 0.0)  # Rotate the cube

        # Draw the cube
        self.draw_cube()

        self.angle += 1  # Increment rotation angle
        self.update()  # Request a redraw

    def draw_cube(self):
        glBegin(GL_QUADS)  # Start drawing the cube
        # Front face (red)
        glColor3f(1.0, 0.0, 0.0)
        glVertex3f(-1.0, -1.0, 1.0)
        glVertex3f(1.0, -1.0, 1.0)
        glVertex3f(1.0, 1.0, 1.0)
        glVertex3f(-1.0, 1.0, 1.0)

        # Back face (green)
        glColor3f(0.0, 1.0, 0.0)
        glVertex3f(-1.0, -1.0, -1.0)
        glVertex3f(-1.0, 1.0, -1.0)
        glVertex3f(1.0, 1.0, -1.0)
        glVertex3f(1.0, -1.0, -1.0)

        # Left face (blue)
        glColor3f(0.0, 0.0, 1.0)
        glVertex3f(-1.0, -1.0, -1.0)
        glVertex3f(-1.0, -1.0, 1.0)
        glVertex3f(-1.0, 1.0, 1.0)
        glVertex3f(-1.0, 1.0, -1.0)

        # Right face (yellow)
        glColor3f(1.0, 1.0, 0.0)
        glVertex3f(1.0, -1.0, -1.0)
        glVertex3f(1.0, 1.0, -1.0)
        glVertex3f(1.0, 1.0, 1.0)
        glVertex3f(1.0, -1.0, 1.0)

        # Top face (cyan)
        glColor3f(0.0, 1.0, 1.0)
        glVertex3f(-1.0, 1.0, -1.0)
        glVertex3f(-1.0, 1.0, 1.0)
        glVertex3f(1.0, 1.0, 1.0)
        glVertex3f(1.0, 1.0, -1.0)

        # Bottom face (magenta)
        glColor3f(1.0, 0.0, 1.0)
        glVertex3f(-1.0, -1.0, -1.0)
        glVertex3f(1.0, -1.0, -1.0)
        glVertex3f(1.0, -1.0, 1.0)
        glVertex3f(-1.0, -1.0, 1.0)
        glEnd()


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.cube_widget = CubeWidget()
        self.setCentralWidget(self.cube_widget)
        self.setWindowTitle('3D Cube in PyQt')
        self.setGeometry(100, 100, 800, 600)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
