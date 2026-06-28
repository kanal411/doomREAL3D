import pyautogui
import time

ileOkien = 1 # don't change this


def klik(x, y, odstep = 0.01, czas = 0.2):
    pyautogui.moveTo(x, y, duration=czas)
    time.sleep(odstep)
    pyautogui.click()

# change these if you are not using 1440p monitor
xLoad, yLoad = 1380, 653 # (you don't have to change this one unless you have really weak PC) x and y coordinates of a window that pops up when the stl is loading in bambu studio
xSlice, ySlice = 2263, 50 # x and y coordinates of the "slice plate" button
xPrint, yPrint = 2415, 50 # x and y coordinates of the "print plate" button
xSend, ySend = 1533, 992 # x and y coordinates of the "send" button
xPrepare, yPrepare = 115, 51 # x and y coordinates of the "prepare" tab
xDevice, yDevice = 324, 47 # x and y coordinates of the "device" tab

# you probably don't have to change those, change only if something doesn't work
rgbOkna = (45, 45, 49) # (you don't have to change this one unless you have really weak PC) rgb values of a window that pops up when the stl is loading in bambu studio
rgbPrint = (33, 164, 82) # rgb values of the "print plate" button when it is enabled (green)

def color_close(c1, c2, tolerance=10):
    return all(abs(a - b) <= tolerance for a, b in zip(c1, c2))

time.sleep(1)

pyautogui.hotkey('win', 'tab')

time.sleep(2)

for i in range(ileOkien):
    pyautogui.press('right')
    time.sleep(2)

pyautogui.press('enter')

time.sleep(1)

pyautogui.hotkey('ctrl', 'i')

time.sleep(1.5)

pyautogui.write("mapa.stl")

time.sleep(1.5)

pyautogui.press('enter')

time.sleep(2)

while color_close(pyautogui.pixel(xLoad, yLoad), rgbOkna, 0):
    time.sleep(1)

time.sleep(3)

klik(xSlice, ySlice)

while not color_close(pyautogui.pixel(xPrint, yPrint), rgbPrint, 15):
    time.sleep(1)

time.sleep(1)

klik(xPrint, yPrint)

time.sleep(0.5)

while not color_close(pyautogui.pixel(xDevice, yDevice), rgbPrint, 15):
    time.sleep(1)

time.sleep(1)

klik(xPrepare, yPrepare)

pyautogui.press('del')

time.sleep(0.3)

pyautogui.hotkey('win', 'tab')

time.sleep(2)

for i in range(ileOkien):
    pyautogui.press('right')
    time.sleep(2)

pyautogui.press('enter')
