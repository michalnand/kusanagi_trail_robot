import robot_hw_endpoint
import time
import numpy

if __name__ == "__main__":

    client = robot_hw_endpoint.HWClient()

    '''
    while True:
        for adr in range(255):
            response = client.ping(adr)

            if response.receive_status and response.crc_status and response.ping_status:
                print("got response from ", adr)
    '''

    valid = 0
    error = 0
    device_address = 100

    num_samples = 1000

    time_all = []

    time_start = time.time()
    for n in range(num_samples):
        ta = time.time()
        response = client.ping(device_address)
        tb = time.time()

        time_all.append(tb - ta)
        if response.receive_status and response.crc_status and response.ping_status:
            valid+= 1
        else:
            error+= 1
    time_stop = time.time()

    tp = num_samples/(time_stop - time_start)


    print("valid ", valid)
    print("error ", error)
    print("\n\n\n")

    time_all = numpy.array(time_all)    

    print("ping time ")
    print("mean_time        = ", round(time_all.mean(), 5))
    print("std_time         = ", round(time_all.std(), 5))
    print("68% percentile   = ", round(numpy.percentile(time_all, 68), 5))
    print("95% percentile   = ", round(numpy.percentile(time_all, 95), 5))
    print("99.7% percentile = ", round(numpy.percentile(time_all, 99.7), 5))
    print("\n\n\n")

    tp_all = numpy.array(1.0/time_all)    
    print("packets per second ")
    print("mean             = ", round(tp_all.mean(), 5))
    print("std              = ", round(tp_all.std(), 5))
    #print("68% percentile   = ", round(numpy.percentile(tp_all, 100 - 68), 5))
    #print("95% percentile   = ", round(numpy.percentile(tp_all, 100 - 95), 5))
    #print("99.7% percentile = ", round(numpy.percentile(tp_all, 100 - 99.7), 5))
    print("\n\n\n")
