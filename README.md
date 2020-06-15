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

The project contains a hard copy of the test ufo from blur busters. It's not my intention to cause copyright infringement but have a similar image to test with.
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

Start the program. It will enter full screen mode and show multiple instances of the Test UFO scrolling to the right.
This allows investigation into motion clarity and strobe cross talk.

### Keyboard Shortcuts

* ESC: to exit the application
* F1: Pursuit Camera
* F2: Strobe Cross Talk
* F3: Vertical Scrolling Text
* F4: Convergence Test Grid
* F5: Contrast Stripes
* F6: Motion Picture Response Time
* 1: Red Screen
* 2: Green Screen
* 3: Blue Screen
* 4: White Screen
* 5: Black Screen


## Troubleshooting

### Multiple Monitor setup

Having multiple monitors attached might cause problems with the frame rate. I've experimented with a 144 Hz and a 60 Hz monitor in operation at the same time. Even so the application was displayed on the 144 Hz monitor the frame rate was reduced to 60 Hz. Please consider this.

### API analysis using NVIDA driver

You can check if the test is executed with the correct framerate by activating API analysis.

* Execute nvidia-settings
* Go to X Screen 0 -> OpenGL Settings
* Check "Enable Graphics API Visual Indicator"
* Start the application.
* Look at the upper left of the screen
* Expected is something like:

	GL 120 FPS
	FLIP - VSYNC ON

If FLIP is BLIT something is wrong.
And VSYNC must be active.

### Micro Stutter

I currently have this issue sometimes but I don't know why this happens...

### I have motion sickness now

You are not supposed to look at the test for more than a few seconds
