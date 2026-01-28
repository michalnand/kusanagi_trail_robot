import vpython
import numpy

class Visualise:

    def __init__(self):
        self.scene = vpython.canvas(title="IMU Viewer", width=800, height=600)

        # Flat cuboid
        self.board = vpython.box(size=vpython.vector(4, 0.3, 2), color=vpython.color.red)

    def update(self, roll, pitch, yaw):
        # reset orientation
        self.board.axis = vpython.vector(1, 0, 0)
        self.board.up   = vpython.vector(0, 1, 0)

        r = (roll)  
        p = (pitch)
        y = (yaw)

        # Yaw → Pitch → Roll (standard IMU order)
        self.board.rotate(angle=y, axis=vpython.vector(0, 1, 0))
        self.board.rotate(angle=p, axis=vpython.vector(0, 0, 1))
        self.board.rotate(angle=r, axis=vpython.vector(1, 0, 0))