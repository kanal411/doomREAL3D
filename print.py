import pyautogui
import time

ileOkien = 1


def klik(x, y, odstep = 0.01, czas = 0.2):
    pyautogui.moveTo(x, y, duration=czas)
    time.sleep(odstep)
    pyautogui.click()

xLoad, yLoad = 1380, 653
xTick, yTick = 1407, 219
xX, yX = 1583, 164
xY, yY = 1645, 164
xSlice, ySlice = 2263, 50
xPrint, yPrint = 2415, 50
xSend, ySend = 1533, 992
xPrepare, yPrepare = 115, 51
xDevice, yDevice = 324, 47

rgbOkna = (45, 45, 49)
rgbTick = (0, 173, 66)
rgbPrint = (33, 164, 82)

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