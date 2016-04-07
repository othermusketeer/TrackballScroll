TrackballScroll
===============
**Allow scrolling with a trackball without scroll wheel by using a low level mouse hook.**

Changes the behaviour of both X-Buttons (typically buttons 3 and 4) to
- When the left XButton is held, the cursor is frozen and any vertical movement of the trackball is translated to Scroll Wheel movements.
- The right XButton acts as a middle mouse button.

###### Credits
- Original code by Martin Seelge, https://github.com/Seelge/TrackballScroll/

###### Requirements
- A trackball or mouse with X-Buttons
- A Microsoft Windows operating system

This software has been tested with a *Logitech Marble Mouse*(tm) and *7*.

###### Download the latest release
Original version: https://github.com/Seelge/TrackballScroll/releases/latest
This version: https://github.com/Othermusketeer/TrackballScroll/releases/latest

###### Run the program
- Execute `TrackballScroll.exe`, no installation is necessary. A console window is displayed while the program is running, close it to disable the mouse hook.
- When using this program with a driver software that allows customization of the button behavior, make sure to set the X-Buttons to default behavior. E.g. with a *Logitech Trackman Marble*, make sure to set the buttons 3 and 4 to `default` button behaviour and not `back`/`Universal Scroll`.
- The lines scrolled per wheel event are determined by the *Microsoft Windows* mouse wheel settings.

###### Compile the source code
- Please at least look at the original author's code before using my edited version
- Clone the repository from the github page or download the latest source code archive
- Open the solution with *Microsoft Visual Studio 2013*. If you are using a different version, e.g. *Visual C++ Express*, create a new solution and add the `TrackballScropp.cpp` file.
- Change the build type to `Release` and `x64`. The program also works in both 64 and 32 bit programs.
- Build the solution

###### Changelog
- 2016-04-07	Changed XButton functions. XButton1 activates scroll wheel emulation, and XButton2 emulates middle click

###### Roadmap
- I may add a tray icon (likely)
- While I have no need for it, I will consider adding the ability to edit a config file to change how the program functions.