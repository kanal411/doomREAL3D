# doomREAL3D
Simple turn-based shooter that instead of displaying gameplay on a monitor, prints it on a 3D printer.

## Requirements
- Windows
- Visual Studio 2022 (or any C++17-compatible compiler)
### Automatic printing requirements (optional)
- Python 3
- pyautogui
- Bambu Studio connected to your printer

## Setup
1. Build 3dDoom.cpp with Visual Studio 2022 (C++17, x64).
2. (Only for automatic printing) Place print.py in the same folder as the compiled 3dDoom.exe.
3. (Only for automatic printing) If your primary monitor is not 1440p: Edit print.py and change all the variables according to comments.
4. Run 3dDoom.exe.

## How to play
Each turn you enter instructions in the form of <distance> <direction> <shoot> (for example: "5 north no" or "10 west yes"). Your goal is to eliminate all 4 enemies. If they get too close, you lose. The map is a 255×255 grid with simple stairs, an elevated platform, and four pillars.

## Automatic printing
This is experimental function, it will simulate mouse and keyboard input, so close any important programs before running it. It is vulnerable to changes in Bambu Studio UI, and needs adjustments in print.py for every resolutions except 1440p.

## AI usage
AI was used for debugging and help with generating STL from c++.
