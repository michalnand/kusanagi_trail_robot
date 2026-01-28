import vpython as vp
import numpy as np

class Visualise:
    def __init__(self):
        self.scene = vp.canvas(title="IMU Viewer", width=800, height=600)

        # Board
        self.board = vp.box(
            size=vp.vector(4, 0.3, 2),
            color=vp.color.red
        )

        # Gimbals (yaw → pitch → roll)
        self.yaw_ring = vp.ring(
            radius=3.0,
            thickness=0.05,
            axis=vp.vector(0, 1, 0),
            color=vp.color.yellow
        )

        self.pitch_ring = vp.ring(
            radius=2.5,
            thickness=0.05,
            axis=vp.vector(0, 0, 1),
            color=vp.color.green
        )

        self.roll_ring = vp.ring(
            radius=2.0,
            thickness=0.05,
            axis=vp.vector(1, 0, 0),
            color=vp.color.cyan
        )

    def reset(self):
        for obj in [self.yaw_ring, self.pitch_ring, self.roll_ring, self.board]:
            obj.axis = vp.vector(1, 0, 0)
            obj.up   = vp.vector(0, 1, 0)

    def update(self, roll, pitch, yaw):
        self.reset()

        r = roll
        p = pitch
        y = yaw

        # --- YAW ---
        for obj in [self.yaw_ring, self.pitch_ring, self.roll_ring, self.board]:
            obj.rotate(angle=y, axis=vp.vector(0, 1, 0))

        # --- PITCH ---
        for obj in [self.pitch_ring, self.roll_ring, self.board]:
            obj.rotate(angle=p, axis=vp.vector(0, 0, 1))

        # --- ROLL ---
        for obj in [self.roll_ring, self.board]:
            obj.rotate(angle=r, axis=vp.vector(1, 0, 0))
