#!/usr/bin/python
import cv2 as cv
import logging
import time
import serial
import glob
import sys


width, height = 320, 240
ser_y = 120
ser_x = 1500
v1_prev = 0
v2_prev = 1500


def start():
    with open("log.log", 'w'):
        pass
    logging.basicConfig(
        filename="log.log",
        format="%(filename)s[LINE:%(lineno)d]# %(levelname)-8s %(message)s",
        level=logging.DEBUG)
    logging.debug("Start")


def camera(id=1, x=640, y=480):
    logging.debug("Initialize camera")
    try:
        cap = cv.VideoCapture(id)
    except Exception:
        logging.error("Wrong id")
        raise Exception("Wrong id")
    cap.set(3, x)
    cap.set(4, y)

    return cap


def connect(port=None, baud=9600):
    ser = None
    if not port:
        ports = glob.glob('/dev/ttyACM*') + glob.glob('/dev/ttyUSB*')
        for p in ports:
            try:
                ser = serial.Serial(p, baud)
            except Exception:
                pass
            else:
                break
    else:
        try:
            ser = serial.Serial(port, baud)
        except Exception:
            logging.error("Wrong port")
            raise Exception("Wrong port!")

    time.sleep(1)
    if ser:
        logging.debug("Connect to port")
    else:
        logging.error("Serial port not found!")
        raise Exception("Serial port not found!")

    return ser


def move(ser, cent, center, lv=10, z=0):
    global ser_y, ser_x, v1_prev, v2_prev
    logging.debug("Move")
    if not(ser):
        logging.error("Connection with serial port is wrong!")
        raise Exception("Wrong connection with serial port")
    kp_1 = 0.0625
    kd_1 = 0.125
    v1 = ((cent[1]-center[1])/2) * kp_1
    ser_y += v1
    d1 = v1 - v1_prev
    v1_prev = v1
    ser_y += d1 * kd_1
    if ser_y > 255:
        ser_y = 255
    if ser_y < 0:
        ser_y = 0

    kp_2 = 5
    kd_2 = 0.5
    v2 = ((cent[0]-center[0])/0.49) * kp_2
    ser_x += v2
    d2 = v2 - v2_prev
    v2_prev = v2
    ser_x += d2 * kd_2
    if ser_x < 1200:
        ser_x = 1200
    if ser_x > 1720:
        ser_x = 1720
    tmp = (ser_x-1200)*255 / 520
    ser.write([int(tmp), int(ser_y), int(z)])
    print(ser_x, ser_y, z)


def main(cap, ser):
    center = (int(width/2), int(height/2))
    cascade = cv.CascadeClassifier(
        "cascades/haarcascade_frontalface_default.xml")
    smile = cv.CascadeClassifier("cascades/haarcascade_smile.xml")
    logging.debug("Starting loop")
    if not(cap):
        logging.error("Connection with camera not found!")
        raise Exception("Wrong connection with camera")
    if not(ser):
        logging.error("Connection with serial port not found!")
        raise Exception("Wrong connection with serial port")
    try:
        while True:
            ser.flush()
            ser.flushInput()
            ser.flushOutput()
            ok, img = cap.read()
            if not ok:
                logging.error("Camera not found!")
                raise Exception("Camera not found!")
            gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
            rects = cascade.detectMultiScale(gray, scaleFactor=1.05,
                                             minNeighbors=4,
                                             minSize=(40, 40),
                                             flags=cv.CASCADE_SCALE_IMAGE)
            # out = cv.cvtColor(gray, cv.COLOR_GRAY2BGR)
            cent = ()

            smiles = smile.detectMultiScale(gray, scaleFactor=1.7,
                                            minNeighbors=22,
                                            minSize=(10, 10),
                                            flags=cv.cv.CV_HAAR_SCALE_IMAGE)

            for x, y, w, h in rects:
                cent = (int(x+w/2), int(y+h/2))
                cv.rectangle(img, (x, y), (x+w, y+h), (0, 0, 255), 2)
                cv.circle(img, cent, 3, (100, 50, 255))
                if len(smiles):
                    tz = 1
                else:
                    tz = 0
                move(ser, cent, center, lv=40, z=tz)

            for x, y, w, h in smiles:
                cv.rectangle(img, (x, y), (x+w, y+h), (255, 0, 0), 1)

            cv.imshow("face", img)

            if cv.waitKey(30) > 0:
                ser.close()
                break
    except KeyboardInterrupt:
        ser.close()
        sys.exit(0)


if __name__ == "__main__":
    start()
    ser = connect()
    cap = camera(1, width, height)
    main(cap, ser)
