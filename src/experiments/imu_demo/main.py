import serial_json

import visualise

import numpy




def quat_mul(q1, q2):
    w1, x1, y1, z1 = q1
    w2, x2, y2, z2 = q2
    return numpy.array([
        w1*w2 - x1*x2 - y1*y2 - z1*z2,
        w1*x2 + x1*w2 + y1*z2 - z1*y2,
        w1*y2 - x1*z2 + y1*w2 + z1*x2,
        w1*z2 + x1*y2 - y1*x2 + z1*w2
    ])

def quat_normalize(q):
    return q / numpy.linalg.norm(q)

def quat_conj(q):
    return numpy.array([q[0], -q[1], -q[2], -q[3]])

def rotate_vector(q, v):
    qv = numpy.hstack([0.0, v])
    return quat_mul(quat_mul(q, qv), quat_conj(q))[1:]

def quat_to_euler(q):
    w, x, y, z = q
    roll  = numpy.arctan2(2*(w*x + y*z), 1 - 2*(x*x + y*y))
    pitch = numpy.arcsin(2*(w*y - z*x))
    yaw   = numpy.arctan2(2*(w*z + x*y), 1 - 2*(y*y + z*z))
    return roll, pitch, yaw





def imu_fusion(ax, ay, az, gx, gy, gz, dt, alpha):
    """
    ax, ay, az : accelerometer [g]
    gx, gy, gz : gyro [rad/s]
    returns roll, pitch, yaw [rad]
    """
    # ---- persistent orientation ----
    if not hasattr(imu_fusion, "q"):
        imu_fusion.q = numpy.array([1.0, 0.0, 0.0, 0.0])

    q = imu_fusion.q

    # ---- normalize accelerometer ----
    acc = numpy.array([ax, ay, az])
    acc_norm = numpy.linalg.norm(acc)
    if acc_norm == 0:
        return 0.0, 0.0, 0.0
    acc /= acc_norm

    # ---- gyro integration ----
    omega = numpy.array([gx, gy, gz])
    omega_quat = numpy.hstack([0.0, omega])

    q_dot = 0.5 * quat_mul(q, omega_quat)
    q_gyro = q + q_dot * dt
    q_gyro = quat_normalize(q_gyro)

    # ---- gravity correction (roll & pitch only) ----
    g_world = numpy.array([0.0, 0.0, 1.0])
    g_body_est = rotate_vector(quat_conj(q_gyro), g_world)

    error = numpy.cross(acc, g_body_est)

    omega_corr = omega + (1 - alpha) * error

    omega_corr_quat = numpy.hstack([0.0, omega_corr])
    q_dot_corr = 0.5 * quat_mul(q, omega_corr_quat)
    q = q + q_dot_corr * dt
    q = quat_normalize(q)

    imu_fusion.q = q

    return quat_to_euler(q)


"""
    ax,ay,az : accelerometer [g]
    gx,gy,gz : gyro [rad/s] 
"""
def madgwick_imu(ax, ay, az, gx, gy, gz, dt=0.01, beta=0.1):
    # quaternion state
    if not hasattr(madgwick_imu, "q"):
        madgwick_imu.q = numpy.array([1.0, 0.0, 0.0, 0.0])

    q = madgwick_imu.q
    w, x, y, z = q

    # normalize acc
    acc = numpy.array([ax, ay, az])
    if numpy.linalg.norm(acc) == 0:
        return quat_to_euler(q)
    
    acc /= numpy.linalg.norm(acc)
    
    ax, ay, az = acc

    # gradient descent
    f = numpy.array([
        2*(x*z - w*y) - ax,
        2*(w*x + y*z) - ay,
        2*(0.5 - x*x - y*y) - az
    ])

    # loss
    J = numpy.array([
        [-2*y,  2*z, -2*w, 2*x],
        [ 2*x,  2*w,  2*z, 2*y],
        [ 0,   -4*x, -4*y, 0 ]
    ])

    step = J.T @ f
    step /= numpy.linalg.norm(step)

    # gyro quaternion
    omega_q = numpy.array([0.0, gx, gy, gz])
    q_dot = 0.5 * quat_mul(q, omega_q) - beta * step

    q += q_dot * dt
    q = quat_normalize(q)

    madgwick_imu.q = q

    return quat_to_euler(q)


if __name__ == "__main__":


    # Example loop
    roll = pitch = yaw = 0
  
    port_name   = "/dev/tty.usbserial-10"
    json_loader = serial_json.SerialJson(port_name)

    visualiser = visualise.Visualise()

    roll = 0
    pitch = 0
    yaw = 0
    
    while True:
        parsed, success, dt = json_loader.get_json()

        if success:
            

            acc  = numpy.array(parsed["acc"])
            gyro = numpy.array(parsed["gyro"])

            # convert mg to g
            acc = acc*0.001

            # convert mdps to rad/s
            gyro = gyro*numpy.pi/180.0

            ax, ay, az = acc[0], acc[1], acc[2]
            gx, gy, gz = gyro[0], gyro[1], gyro[2]

            #roll  = numpy.arctan2(ay, az)
            #pitch = numpy.arctan2(-ax, numpy.sqrt(ay**2 + az**2))

            
            roll, pitch, yaw = madgwick_imu(ax, ay, az, gx, gy, gz, dt=dt, beta=0.1)

            #roll, pitch, yaw = imu_fusion(ax, ay, az, gx, gy, gz, dt=dt, alpha=0.2)

            
            print(acc)
            print(gyro)
            print(roll, pitch, yaw)
            print("\n\n")

            
            #visualiser.update(roll, -pitch, yaw)
            visualiser.update(-pitch, -roll, yaw)

    print("done")