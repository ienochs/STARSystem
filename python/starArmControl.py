"""
    Star Arm Control

    This code takes commands from the STAR Labview VI and directs the movement
    of a xArm 6 robotic arm with direct drive linear motor (UFactory). Errors
    and error locations are returned to Labview.

    Created 15 September 2023
    By Ian Enochs

    https://github.com/ienochs/STARSystem/blob/main/python/starArmControl.py
"""

# Import applicable libraries
import traceback
import time

# Import relevant xarm stuff
try:
    from xarm.tools import utils
    from xarm import version
    from xarm.wrapper import XArmAPI
except:
    print('Import unsuccessful')
else:
    print('Import successful')

# Define the function pprint
def pprint(*args, **kwargs):
    try:
        stack_tuple = traceback.extract_stack(limit=2)[0]
        print('[{}][{}] {}'.format(time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time())), stack_tuple[1], ' '.join(map(str, args))))
    except:
        print(*args, **kwargs)
pprint('xArm-Python-SDK Version:{}'.format(version.__version__))

# Set up the xarm
arm = XArmAPI('XXX.XXX.X.XXX', baud_checkset=False)# Set the arm address, NOTE: NEED TO EDIT
arm.clean_warn()# Cleans warnings
arm.clean_error()# Cleans errors
arm.motion_enable(True)# Enable motion
arm.set_mode(0)# Set to position control mode
arm.set_state(0)# Set to sport state, whatever that means
time.sleep(1)# Suspend execution for 1 second
params = {'speed': 200, 'acc': 1000, 'angle_speed': 20, 'angle_acc': 500, 'events': {}, 'variables': {}, 'callback_in_thread': True, 'quit': False}# Define movement parameters

# Define position array
posArray =[[0, 0, 0, 0, 0, 0],
           [350.0, -100.0, 110.0, 180.0, 0.0, 0.0],# pos1
           [450.0, -100.0, 110.0, 180.0, 0.0, 0.0],# pos2
           [550.0, -100.0, 110.0, 180.0, 0.0, 0.0],# pos3
           [650.0, -100.0, 110.0, 180.0, 0.0, 0.0],# pos4
           [650.0, 100.0, 110.0, 180.0, 0.0, 0.0],# pos5
           [550.0, 100.0, 110.0, 180.0, 0.0, 0.0],# pos6
           [450.0, 100.0, 110.0, 180.0, 0.0, 0.0],# pos7
           [350.0, 100.0, 110.0, 180.0, 0.0, 0.0],# pos8
           [350.0, 200.0, 110.0, 180.0, 0.0, 0.0],# pos9
           [450.0, 200.0, 110.0, 180.0, 0.0, 0.0],# pos10
           [550.0, 200.0, 110.0, 180.0, 0.0, 0.0],# pos11
           [650.0, 200.0, 110.0, 180.0, 0.0, 0.0],# pos12
           [650.0, 300.0, 110.0, 180.0, 0.0, 0.0],# pos13
           [550.0, 300.0, 110.0, 180.0, 0.0, 0.0],# pos14
           [450.0, 300.0, 110.0, 180.0, 0.0, 0.0],# pos15
           [350.0, 300.0, 110.0, 180.0, 0.0, 0.0],# pos16
           [-350.0, -100.0, 110.0, 180.0, 0.0, 180.0],# pos17
           [-450.0, -100.0, 110.0, 180.0, 0.0, 180.0],# pos18
           [-550.0, -100.0, 110.0, 180.0, 0.0, 180.0],# pos19
           [-650.0, -100.0, 110.0, 180.0, 0.0, 180.0],# pos20
           [-650.0, 100.0, 110.0, 180.0, 0.0, 180.0],# pos21
           [-550.0, 100.0, 110.0, 180.0, 0.0, 180.0],# pos22
           [-450.0, 100.0, 110.0, 180.0, 0.0, 180.0],# pos23
           [-350.0, 100.0, 110.0, 180.0, 0.0, 180.0],# pos24
           [-350.0, 200.0, 110.0, 180.0, 0.0, 180.0],# pos25
           [-450.0, 200.0, 110.0, 180.0, 0.0, 180.0],# pos26
           [-550.0, 200.0, 110.0, 180.0, 0.0, 180.0],# pos27
           [-650.0, 200.0, 110.0, 180.0, 0.0, 180.0],# pos28
           [-650.0, 300.0, 110.0, 180.0, 0.0, 180.0],# pos29
           [-550.0, 300.0, 110.0, 180.0, 0.0, 180.0],# pos30
           [-450.0, 300.0, 110.0, 180.0, 0.0, 180.0],# pos31
           [-350.0, 300.0, 110.0, 180.0, 0.0, 180.0],# pos32
           [350.0, -100.0, 110.0, 180.0, 0.0, 0.0],# pos33
           [450.0, -100.0, 110.0, 180.0, 0.0, 0.0],# pos34
           [550.0, -100.0, 110.0, 180.0, 0.0, 0.0],# pos35
           [650.0, -100.0, 110.0, 180.0, 0.0, 0.0],# pos36
           [650.0, 100.0, 110.0, 180.0, 0.0, 0.0],# pos37
           [550.0, 100.0, 110.0, 180.0, 0.0, 0.0],# pos38
           [450.0, 100.0, 110.0, 180.0, 0.0, 0.0],# pos39
           [350.0, 100.0, 110.0, 180.0, 0.0, 0.0],# pos40
           [350.0, 200.0, 110.0, 180.0, 0.0, 0.0],# pos41
           [450.0, 200.0, 110.0, 180.0, 0.0, 0.0],# pos42
           [550.0, 200.0, 110.0, 180.0, 0.0, 0.0],# pos43
           [650.0, 200.0, 110.0, 180.0, 0.0, 0.0],# pos44
           [650.0, 300.0, 110.0, 180.0, 0.0, 0.0],# pos45
           [550.0, 300.0, 110.0, 180.0, 0.0, 0.0],# pos46
           [450.0, 300.0, 110.0, 180.0, 0.0, 0.0],# pos47
           [350.0, 300.0, 110.0, 180.0, 0.0, 0.0]# pos48
           ]

# Check for errors
def errorCheck(errorCheckCode):
    if errorCheckCode != 0:# If errorCheckCode is not equal to zero, quit and return error
        params['quit'] = True
        return("error")
    if errorCheckCode == 0:# If errorCheckCode is not equal to zero, quit and return error
        return("success")

def startup():
    pos = "startup"
#    arm.set_linear_track_enable(enable=True)# Enable the linear track
#    arm.set_linear_track_back_origin(speed=10, wait=True)# Set the linear track speed
    if arm.error_code == 0 and not params['quit']:# If everything good to proceed
        code = posSafe(0)# Send posSafe the linear track position of 0
    return(errorCheck(code)+"_"+str(pos)+"_1")# Check for error code and send status to the Labview VI with the position and movement step

def posSafe(trackPos):
    j6_angle = arm.get_servo_angle(6)[1]
    j1_angle = arm.get_servo_angle(1)[1]
    time.sleep(.1)
    if arm.error_code == 0 and not params['quit']:
        code = arm.set_servo_angle(6,(0 - j6_angle), speed=60, relative=True, wait=True)# Set end effector to normal position  i.e. 0, using minimal movement
        if code != 0: return code
        code = arm.set_servo_angle(angle=[j1_angle,0,0,0,-90,0], speed=40, wait=True)# Set servo angles to position for J1 rotation
        if code != 0: return code
        code = arm.set_servo_angle(1, -90, speed=40, wait=True)# Set servo J1 to safe position for linear track transit
#        if code != 0: return code# If there's an error return it
#        code = arm.set_linear_track_pos(trackPos, speed=20, wait=True, auto_enable=True)# Move the linear track to position 0
    return(code)

def normalMove(pos):
    if arm.error_code == 0 and not params['quit']:  # If everything good to proceed
        code = arm.set_position(z=100, speed=params['speed'], mvacc=params['acc'], relative=True, wait=True)# Move the arm down 100 mm
        if code != 0: return (errorCheck(code)+"_pos"+str(pos)+"_1")# If there's an error code check and return it to the Labview VI with the position and movement step
        code = arm.set_position(*posArray[pos], speed=params['speed'], mvacc=params['acc'], wait=True)# Move the arm linearly to the desired coordinates
        if code != 0: return (errorCheck(code)+"_pos"+str(pos)+"_2")# If there's an error code check and return it to the Labview VI with the position and movement step
        code = arm.set_position(z=-100, speed=params['speed'], mvacc=params['acc'], relative=True, wait=True)# Move the arm down 100 mm
    return(errorCheck(code)+"_pos"+str(pos)+"_3")# Check for error code and send status to the Labview VI with the position and movement step

def pos1():
    pos = 1# Name the position
    if arm.error_code == 0 and not params['quit']:  # If everything good to proceed
        code = posSafe(0)# Send posSafe the linear track position of 0
        if code != 0: return (errorCheck(code)+"_pos"+str(pos)+"_1")# If there's an error code check and return it to the Labview VI with the position and movement step
        code = arm.set_servo_angle(1, 0, speed=40, wait=True)# Set servo 1 to 0 deg
        if code != 0: return (errorCheck(code)+"_pos"+str(pos)+"_2")# If there's an error code check and return it to the Labview VI with the position and movement step
        code = arm.set_servo_angle(5, 0, speed=70, wait=False)# Set servo 5 to 0 deg
        if code != 0: return (errorCheck(code)+"_pos"+str(pos)+"_3")# If there's an error code check and return it to the Labview VI with the position and movement step
        code = arm.set_position(*posArray[pos], speed=params['speed'], mvacc=params['acc'], wait=True)# Move the arm linearly to the desired coordinates
        if code != 0: return (errorCheck(code)+"_pos"+str(pos)+"_4")# If there's an error code check and return it to the Labview VI with the position and movement step
        code = arm.set_position(z=-100, speed=params['speed'], mvacc=params['acc'], relative=True, wait=True)# Move the arm down 100 mm
    return(errorCheck(code)+"_pos"+str(pos)+"_5")# Check for error code and send status to the Labview VI with the position and movement step

def pos2():
    return(normalMove(2))
def pos3():
    return(normalMove(3))
def pos3():
    return(normalMove(3))
def pos4():
    return(normalMove(4))
def pos5():
    return(normalMove(5))
def pos6():
    return(normalMove(6))
def pos7():
    return(normalMove(7))
def pos8():
    return(normalMove(8))
def pos9():
    return(normalMove(9))
def pos10():
    return(normalMove(10))
def pos11():
    return(normalMove(11))
def pos12():
    return(normalMove(12))
def pos13():
    return(normalMove(13))
def pos14():
    return(normalMove(14))
def pos15():
    return(normalMove(15))
def pos16():
    return(normalMove(16))

def pos17():
    pos = 17# Name the position
    if arm.error_code == 0 and not params['quit']:  # If everything good to proceed
        code = posSafe(0)# Send posSafe the linear track position of 0
        if code != 0: return (errorCheck(code)+"_pos"+str(pos)+"_1")# If there's an error code check and return it to the Labview VI with the position and movement step
        code = arm.set_servo_angle(1, -180, speed=40, wait=True)# Set servo J1 to safe position for linear track transit
        if code != 0: return (errorCheck(code)+"_pos"+str(pos)+"_2")
        code = arm.set_servo_angle(5, 0, speed=70, wait=False)# Set servo 5 to 0 deg
        if code != 0: return (errorCheck(code)+"_pos"+str(pos)+"_3")# If there's an error code check and return it to the Labview VI with the position and movement step
        code = arm.set_position(*posArray[pos], speed=params['speed'], mvacc=params['acc'], wait=True)# Move the arm linearly to the desired coordinates
        if code != 0: return (errorCheck(code)+"_pos"+str(pos)+"_4")# If there's an error code check and return it to the Labview VI with the position and movement step
        code = arm.set_position(z=-100, speed=params['speed'], mvacc=params['acc'], relative=True, wait=True)# Move the arm down 100 mm
    return(errorCheck(code)+"_pos"+str(pos)+"_5")# Check for error code and send status to the Labview VI with the position and movement step

def pos18():
    return(normalMove(18))
def pos19():
    return(normalMove(19))
def pos20():
    return(normalMove(20))
def pos21():
    return(normalMove(21))
def pos22():
    return(normalMove(22))
def pos23():
    return(normalMove(23))
def pos24():
    return(normalMove(24))
def pos25():
    return(normalMove(25))
def pos26():
    return(normalMove(26))
def pos27():
    return(normalMove(27))
def pos28():
    return(normalMove(28))
def pos29():
    return(normalMove(29))
def pos30():
    return(normalMove(30))
def pos31():
    return(normalMove(31))
def pos32():
    return(normalMove(32))

def pos33():
    pos = 1# Name the position
    if arm.error_code == 0 and not params['quit']:  # If everything good to proceed
        code = posSafe(700)# Send posSafe the linear track position of 700
        if code != 0: return (errorCheck(code)+"_pos"+str(pos)+"_1")# If there's an error code check and return it to the Labview VI with the position and movement step
        code = arm.set_servo_angle(1, 0, speed=40, wait=True)# Set servo 1 to 0 deg
        if code != 0: return (errorCheck(code)+"_pos"+str(pos)+"_2")# If there's an error code check and return it to the Labview VI with the position and movement step
        code = arm.set_servo_angle(5, 0, speed=70, wait=False)# Set servo 5 to 0 deg
        if code != 0: return (errorCheck(code)+"_pos"+str(pos)+"_3")# If there's an error code check and return it to the Labview VI with the position and movement step
        code = arm.set_position(*posArray[pos], speed=params['speed'], mvacc=params['acc'], wait=True)# Move the arm linearly to the desired coordinates
        if code != 0: return (errorCheck(code)+"_pos"+str(pos)+"_4")# If there's an error code check and return it to the Labview VI with the position and movement step
        code = arm.set_position(z=-100, speed=params['speed'], mvacc=params['acc'], relative=True, wait=True)# Move the arm down 100 mm
    return(errorCheck(code)+"_pos"+str(pos)+"_5")# Check for error code and send status to the Labview VI with the position and movement step

def pos34():
    return(normalMove(34))
def pos35():
    return(normalMove(35))
def pos36():
    return(normalMove(36))
def pos37():
    return(normalMove(37))
def pos38():
    return(normalMove(38))
def pos39():
    return(normalMove(39))
def pos40():
    return(normalMove(40))
def pos41():
    return(normalMove(41))
def pos42():
    return(normalMove(42))
def pos43():
    return(normalMove(43))
def pos44():
    return(normalMove(44))
def pos45():
    return(normalMove(45))
def pos46():
    return(normalMove(46))
def pos47():
    return(normalMove(47))
def pos48():
    return(normalMove(48))
