# Slamy's Motion Test Suite

## What's this?

This is software related to testing motion clarity of displays.

## Why?

It's 2020 and monitor manufactures struggle to engineer a TFT display which comes even close to the motion clarity the CRTs in the past were able to produce.
There exists a whole community around this topic and seems to be centered around the [Blur Busters](https://blurbusters.com/).
There even exists a test which can be executed in the browser called the [UFO Test](https://www.testufo.com/).

Sadly though this test seems to have problems being used under Linux.
I've performed tests on my current Debian testing distribution using GTX960 with proprietary Nvidia drivers. Firefox seems to be unable to archive more than 60 FPS while Google Chrome is capable of doing so but suffers from tearing.

I really wanted to remove the Browser from the chain as I believe a native application directly attached to an OpenGL context might perform better.

## Disclaimer

This software is delivered as is. The author can't be held responsible for damages caused by the execution.

## How to build

	mkdir build
	cd build
	cmake ..

### How to build with eclipse sibling project

	mkdir ../motionTestSuite_build
	cd ../motionTestSuite_build
	cmake -DCMAKE_ECLIPSE_GENERATE_LINKED_RESOURCES=OFF -G "Eclipse CDT4 - Unix Makefiles" ../motionTestSuite

## How to use

    Allowed options:
      --help                produce help message
      --novsync             Disables VSync
      --msdelay arg (=0)    Force delay between each frame (for VRR tests)
      --display arg (=0)    Select display to use
      --strobe              Activates strobe cross talk test
      --full                Full screen mode

For pursuit camera test you probably want to use

    ./motionTestSuite --vsync --full

### Keyboard Shortcuts

* ESC: to exit the application
* F1: Pursuit Camera
* F2: Strobe Cross Talk
* F3: Vertical Scrolling Text
* F4: Convergence Test Grid (for CRTs)
* F5: Sharpness Test Grid (for CRTs)
* F6: Linear Contrast Stripes
* F7: Contrast Stripes in 32 steps
* F8: Motion Picture Response Time
* F9: Solid Color
    * 1: Red Screen
    * 2: Green Screen
    * 3: Blue Screen
    * 4: White Screen
    * 5: Black Screen
* Q: Increase scrolled pixels per second
* A: Decrease scrolled pixels per second
* J: Toggle Jitter Visualisation Graph

## Known bugs

### Consistent frame drop after a few seconds in full screen mode.

This is caused by disabling the desktop compositor to increase performance.
Most Linux full screen applications do this and I can't solve this.
The Compositor is reenabled after exiting the application.

The "OUT OF SYNC" analysis is suppressed during the first 3 seconds because of this.

## Troubleshooting

### Frame Stutter in Multi-Monitor setup

Having multiple monitors attached might cause problems with the frame rate.
I've experimented with a 144 Hz and a 60 Hz monitor in operation at the same time.
But Xorg or the Nvidia driver are causing extreme stutter. This can be fixed by having the same refresh rate on all monitors.

### Extreme Frame Stutter in Windowed mode

The Compositor must be active for the Windowed mode to work.

For KDE just press Shift+Alt+F12 to toggle Compositor mode.
The visuals of the desktop should alter immediatly as windows should be
casting shadows when the Compositor is active.

If the stutter remains, the VSync settings might be off:
* Start Compositor from the Launcher to show its settings
* Configure VSync as "Automatic"

It should be fine now.

### Frame Stutter in Windowed mode is not resolvable

Please use the full screen mode.
Only have **one** monitor active during testing.

Having the full screen mode active should indicate to the driver that FLIP mode shall be used.
At least with Nvidia drivers this should now do it.

### Frame Stutter when moving the mouse cursor between Windows

This is why we can't have nice things...
It's especially bad when I move my cursor in and out a Firefox window.
Keep your cursor always on the the MotionTestSuite during testing.

### API analysis using NVIDA driver

You can check if the test is executed with the correct framerate by activating API analysis.

* Execute nvidia-settings
* Go to X Screen 0 -> OpenGL Settings
* Check "Enable Graphics API Visual Indicator"
* Start the application.
* Look at the upper left of the screen
* VSYNC must be active/green

### I want to use the official Blur Busters UFO as a moving target

The `testpattern.png` file can be replaced with a custom graphic.
To use the Blur Busters TestUFO execute these lines on the shell:

	wget https://www.testufo.com/images-test/ghosting/bbufo-tinytext-markers.png
	convert bbufo-tinytext-markers.png testpattern.bmp

Please keep in mind that this application might not be suitable for official testing means
as it might not behave exactly the same as https://www.testufo.com/ would.
Results from the motionTestSuite and https://www.testufo.com/ must **never** be compared against each other.

### I have motion sickness now

You are not supposed to look at the test for more than a few seconds

