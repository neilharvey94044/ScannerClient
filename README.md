

# Cross-Platform Uniden Scanner Client

![ScannerClient](ScannerClient.png "ScannerClient")


ScannerClient is a very simply client that provides visual scanner status and audio streamed over IP.  This has enabled me to leave my SDS200 in the cold garage connected to a discone antenna on the roof while I listen anywhere inside my warm house.

ScannerClient makes no attempt to "program" the scanner, it is a simple program for listening with limited control functionality.

A short demo video is provided (ScannerClientDemo.mp4).

Internal design has the following major threads of execution:
1. Real Time Streaming Protocol (RTSP) management.
2. Real-time Transport Protocol (RTP) management.
3. Status update using Uniden PSI command.
3. Status update using Uniden PSI command.
4. Audio using rtaudio.
5. GUI using wxWidgets.
6. Upsampling of audio using libsamplerate.  The scanner exposes an audio stream that is 8000 samples per second.  This has been fine on Windows and Ubuntu, however, OSX does not support it, which necessitated upsampling to 44,100 sps.  This provided improved audio on my computers.
6. Upsampling of audio using libsamplerate.  The scanner exposes an audio stream that is 8000 samples per second.  This has been fine on Windows and Ubuntu, however, OSX does not support it, which necessitated upsampling to 44,100 sps.  This provided improved audio on my computers.

The executable name is `sc.exe` on Windows and `sc` on Linux and OSX.

Written with C++20.

License is GPL2.0+.

# Some Background on Buttons
- **Settings** - update the IP address of your scanner first under General settings.
- **Start** - causes scanner client to connect to the scanner.
- **Stop** - disconnects.
- **System** - toggles hold on the current System.
- **Department** - toggles hold on the current Department.
- **Channel** - toggles hold on the current Channel.
- **Bump** - causes scanner to resume scanning.
- **Avoid** - just like the avoid button on the scanner.
- **Reboot** - will reboot your scanner.  Use this when the scanner gets hinky and ScannerClient can't connect to it.  I've also run into problems with my router after weeks or months of uninterrupted use and needed to reboot it too.  I've also run into problems with my router after weeks or months of uninterrupted use and needed to reboot it too.
- **Weather Scan** - scans the weather frequencies.  Use the System button to resume scanning normally.
- **Update Clock** - updates the scanners clock with your computer's system date/time.
- **CTRL Key** - same as the Bump button.
- **SPACE key** - same as the Channel button.




# Caveats
1. Currently built and run on Windows 10, Ubuntu 22, and OSX 10.15. All components used by ScannerClient were selected because they run on all three platforms.
2. Requires an IP link to the scanner.  This can be wired or wifi.
3. Only tested with the Uniden SDS200.  Tested both cable connection between scanner and router,  and using a wifi dongle connected to the router.  In both cases the computer was connected to the router over wifi.
4.  Testing has been limited.  I'm one person.
5.  Many capabilities and functions of the scanner have not been implemented.
6.  If you get the "Advanced" settings confused, delete the sc.cfg file, restart and, restart and re-enter your IP address.  The defaults will be restored.
7.  Windows security will stop the program the first time you run it.  When prompted, authorize the program to run and Windows will not repeat.  I will likely resolve this eventually.
8. If you've got the network and IP address correct, but ScannerClient is not connecting, try rebooting the scanner.  Often the audio service on the scanner will stop working because it didn't receive a valid termination of the RTSP session.
9.  Does not show all modes of the scanner, I implemented what I was immediately interested in.  More can be done over time.

# Ideas for Enhancements
1. Application volume control and mute.
2. Search, discovery, close call, etc.
3. Ability to enable/disable service types.
4. Ability to select a specific audio device.  Defaults to the default audio device for now.
5. Ability to enter quick keys.
6. Direct frequency entry.

# Build Steps For Windows

## Prequesite Software Installation (all free)
### Install Visual Studio 2022 Community Edition. 
During installation with the 'Visual Studio Installer' make sure you select at least the C++ Desktop development category or you'll be unable to compile the project due to the missing C++ tools.
### Install cmake.  
https://cmake.org/install/
### Install and build wxWidgets for static release (without DLL).  
Download https://github.com/wxWidgets/wxWidgets/releases/download/v3.2.1/wxWidgets-3.2.1.zip and unzip it to somewhere such as `~/repos/wxWidgets-3.2.2.1` where "~" is your home directory.  
Navigate to ~/repos/wxWidgets-3.2.2.1\build\msw (or wherever you extracted) and open wx_vc17.sln.  
Choose Release and x64 for the build configuration and "Build Solution". All should compile successfully and you can close the project.
### install git
https://git-scm.com/download/win


### Obtain ScannerClient Source Code
```bash
cd ~
mkdir repos
cd repos
git clone https://github.com/neilharvey94044/ScannerClient.git
cd ScannerClient
```

### Generate ScannerClient:
```bash
cmake -DCMAKE_BUILD_TYPE=Release -S . -B ./build
```
### Build ScannerClient:
```bash
cmake --build ./build --config Release
```

### To Install - just copies to a directory in your program files does not add to Start Menu
```bash
cmake --install ./build
```
Note: do all the above from the top level ScannerClient directory.

# Build Steps for Linux
## Basic build support
```bash
$ sudo apt-get install git build-essential automake cmake
```
## Base dependencies
```bash
sudo apt-get install libgtk-3-dev libpulse-dev
```
## Build and install wxWidgets
First download wxWidgets source to `~/repos/wxWidgets-3.2.2.1` or similar, change the following to match.
```bash
cd ~
mkdir wx_install
cd ~/repos/wxWidgets-3.2.2.1
./config --prefix ~/wx_install --disable-shared --enable-monolithic --enable-propgrid
make -j4
make install
```
## WX_CONFIG Environment Variable
Set an environment variable named `WX_CONFIG` to the wx-config executable where you built wxWidgets, in the case above that would be  `~/wx_install/bin/wx-config`.  
This is the default used by the CMakeLists.txt for ScannerClient.
```bash
export WX_CONFIG=~/wx_install/bin/wx-config
```
## Generate and build ScannerClient
```bash
cd ~
mkdir repos
cd repos
git clone https://github.com/neilharvey94044/ScannerClient.git
cd ScannerClient
mkdir build
cmake -S . -B ./build -DCMAKE_CONFIG_TYPE=Release
cmake --build ./build --config Release
```
Look for the executable in ~/repos/ScannerClient/build/Release, you can run from here or move to where you like.


# Build Steps for Apple

## Install xcode
If you have an older but capable mac you may need an older version of xcode that you won't easily find using brew or the App Store.  Try https://xcodereleases.com which seems to have links to every version.

## Install CMake and Git
Install cmake version 3.25 minimum, earlier versions have bugs that fail to build wxWidgets with the required options. Brew may be a good option for installing or get directly from Kitware.
## Install wxWidgets
Follow the same instructions for Linux.

## WX_CONFIG Environment Variable
Follow the Linux instructions.

## Generate and build ScannerClient
Follow instructions for Linux, however, look for the executable in ~/repos/ScannerClient/build/sc.app/Contents/MacOS


## RFCs and References Used In Development

#### Real-Time Streaming Protocol
https://www.rfc-editor.org/rfc/rfc7826#page-25
https://www.w3.org/2008/WebVideo/Fragments/wiki/UA_Server_RTSP_Communication
https://www.cs.columbia.edu/~hgs/rtsp/draft/draft-ietf-mmusic-rtsp-03.html#SECTION00020000000000000000

#### Real-Time Transport Protocol
https://www.rfc-editor.org/rfc/rfc3550#page-4

#### G.711 Codec
https://en.wikipedia.org/wiki/G.711

#### ITU Software Tools Library on Github
https://github.com/openitu/STL


 