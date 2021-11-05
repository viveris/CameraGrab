![Camera Grab logo](img/CameraGrab.png)

# Camera Grab

## Description

The CameraGrab allows to take picture from camera based on OpenCV library.
This application compile with Visual Studio and run only on Windows.

## Dependencies

### Application generation

This application is compiled with Microsoft Visual Studio Community 2019.

It's depends on OpenCV version 4.5.3 available at:

https://sourceforge.net/projects/opencvlibrary/files/4.5.3/opencv-4.5.3-vc14_vc15.exe/download

### Clone and project init

```bash
git clone git@github.com:viveris/CameraGrab.git
```

## Compilation

Open *CameraGrab.sln* (Visual Studio Solution).

Select target (Debug or Release).

Compile Solution (CTRL + B)

Output is located to :
 * *<solution_dir>*/x64/Release (for Release target).
 * *<solution_dir>*/x64/Debug (for Release target).

## Execution

Copy *opencv_world453.dll* (for Release) or *opencv_world453d.dll* (for Debug) on same level than CameraGrab.exe

## Manual

```
CameraGrab.exe /h
CameraGrab.exe <option> <parameter>
options :
        [/h|/help]: Display this help.
        [/d|/device]: Index of capture device.
        [/o|/output file]: File to save camera picture.
        [/f|/focus focus]: Set focus value (0 to 255; -1 for autofocus).
        [/F|/format <bmp|jpg|png>] : Specify output format.
        [/l|/list-device]: List availables devices.
        [/W|/width]: Width of capture device.
        [/H|/height]: Height of capture device.
        [/v|/version]: Display application version.
```
