import serial
import time
import pygame
import sys

# first attempt
# https://projecthub.arduino.cc/ansh2919/serial-communication-between-python-and-arduino-663756

pygame.init()
pygame.joystick.init()

# Screen setup
screenSize = [800,500]
font_size = 30
FPS = 20

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
joyposx = 400
joyposy = 250
# create joystick indicator 
joyindicator = pygame.Rect(joyposx, joyposy, 10, 10)
joyindback = pygame.Rect(joyposx-50, joyposy-50, 100, 100)

col = "royalblue"
joysticks = []


port = 'COM3'
baudrate = 115200
arduino = serial.Serial(port, baudrate, timeout=.1)
time.sleep(1)
num = [[],[]]

def send(input1,input2):
    arduino.write(bytes(input1, 'utf-8'))
    time.sleep(0.05)
    arduino.write(bytes(input2, 'utf-8'))
    time.sleep(0.05)
    
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
        draw_text("Controller Type: " + str(joystick.get_name()), font, pygame.Color("azure"), 10, 35)
        draw_text("Num of Axes: " + str(joystick.get_numaxes()), font, pygame.Color("azure"), 10, 60)

    # joystick input
    for joystick in joysticks:
        drive = joystick.get_axis(1)
        steer = joystick.get_axis(0)

    #event handler
    for event in pygame.event.get():
        if event.type == pygame.JOYDEVICEADDED:
            joy = pygame.joystick.Joystick(event.device_index)
            joysticks.append(joy)
        if event.type == pygame.QUIT:
            run = False
    print(str(-1*drive) + " " + str(steer))
    send(str(-1*drive), str(steer))
    pygame.display.flip()

           
pygame.quit()
sys.exit()    
'''
drive = input("Enter first number: ") # Taking input from user 
steer = input("Enter second number: ")
send(drive, steer) 
'''
