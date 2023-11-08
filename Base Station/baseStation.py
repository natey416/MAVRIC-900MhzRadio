import time
import serial
import serial.tools.list_ports
import pygame
import sys
from pySerialTransfer import pySerialTransfer as txfer

# THIS CODE IS A NIGHTMARE
# I won't be commenting this due to it not being used in our final version
# to look at how it actually works, it's much better to look at 
# baseStationSimple.py
# That code is a lot better to look at.

# first attempt
# https://projecthub.arduino.cc/ansh2919/serial-communication-between-python-and-arduino-663756

print("Searching for COM Ports")
ports = list(serial.tools.list_ports.comports())
print("found ports: ")
for p in ports:
    print(p.device)

if len(ports) > 1:
    input = input("Which COM port do you want to use? (NUM ONLY) ")
    port = 'COM' + input
else:
    port = ports[0].device

pygame.init()
pygame.joystick.init()

# Screen setup
screenSize = [400,250]
font_size = 15
FPS = 30

# create game window
screen = pygame.display.set_mode((screenSize[0],screenSize[1]))
pygame.display.set_caption("Joystick")
font = pygame.font.SysFont("Futura", font_size)

# draw text helper function
def draw_text(text, font, text_col, x, y):
    img = font.render(text, True, text_col)
    screen.blit(img, (x,y))

# initialize some variables and start clock
clock = pygame.time.Clock()
drive = 0
steer = 0
joyposx = screenSize[0]/2
joyposy = screenSize[1]/2
# create joystick indicator 
joyindicator = pygame.Rect(joyposx, joyposy, 10, 10)
joyindback = pygame.Rect(joyposx-50, joyposy-50, 100, 100)

col = "royalblue"
joysticks = []
axis = [1,0]
deadzone = 0.05

class struct(object):
    drive = float
    steer = float
    

if __name__=='__main__':
    try:
        data = struct
        print("Attempting to connect to: " + str(port))
        link = txfer.SerialTransfer(port)
        link.open()
        print("Link established")
        time.sleep(1)

        run = True
        while run:
            clock.tick(FPS)

            screen.fill(pygame.Color("black"))
            pygame.draw.rect(screen, pygame.Color("white"), joyindback)
            joyindicator.topleft = (joyposx + 50*steer - 5, joyposy + 50*drive - 5)
            pygame.draw.rect(screen, pygame.Color(col), joyindicator)

            #joystick info
            draw_text("Controllers: " + str(pygame.joystick.get_count()), font, pygame.Color("azure"), 10, 10)
            for joystick in joysticks:
                draw_text("Controller Type: " + str(joystick.get_name()), font, pygame.Color("azure"), 10, 10+font_size)
                draw_text("Num of Axes: " + str(joystick.get_numaxes()), font, pygame.Color("azure"), 10, 10+2*font_size)

            # joystick input
            for joystick in joysticks:
                    if abs(joystick.get_axis(1)) > deadzone:
                        drive = joystick.get_axis(1)
                    else:
                        drive = 0
                    if abs(joystick.get_axis(0)) > deadzone:
                        steer = joystick.get_axis(0)
                    else:
                        steer = 0

            #event handler
            for event in pygame.event.get():
                if event.type == pygame.JOYDEVICEADDED:
                    joy = pygame.joystick.Joystick(event.device_index)
                    joysticks.append(joy)
                if event.type == pygame.QUIT:
                    run = False
                    raise KeyboardInterrupt

            sendSize = 0
            data.drive = drive
            data.steer = steer

            sendSize = link.tx_obj(data.drive, start_pos=sendSize)
            sendSize = link.tx_obj(data.steer, start_pos=sendSize)
            link.send(sendSize)
            pygame.display.flip()
    except KeyboardInterrupt:
        pygame.quit()
        link.close()
        print("Goodbye!")
        sys.exit()    
'''
drive = input("Enter first number: ") # Taking input from user 
steer = input("Enter second number: ")
send(drive, steer) 
'''
