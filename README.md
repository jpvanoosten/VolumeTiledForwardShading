# Table of Contents

 * [Introduction](#introduction)
 * [Controls](#controls)
   * [Camera](#camera)
   * [XBox 360, XBox One](#xbox-360-xbox-one-controller)
     * [Buttons](#buttons)
     * [Axes](#axes) 
 * [Requirements](#requirements)
 * [Compiling](#compiling)
 * [Installation](#installation)
 * [Configuration](#configuration)
 * [Troubleshooting](#troubleshooting)
 * [Known Issues](#known-issues)
 * [FAQ](#faq)
 * [Maintainers](#maintainers)

# Introduction

This project demonstrates the following rendering techniques.

 * Forward Rendering
 * Tiled Forward Rendering (Forward+)
 * Volume Tiled Forward Rendering
 * Volume Tiled Forward Rendering with BVH Optimization

To compile and run the game, see the COMPILING section below.
To install and run the game, see the INSTALLATION section below.
 
# Controls

The demo uses the following keyboard keys:

 * [ESC]: Exits the application.
 * [Alt+F4]: Exits the application.

 * [Q]: Pan the camera down (along the camera's local Y axis).
 * [E]: Pan the camera up (along the camera's local Y axis).
 * [W]: Pan the camera forward (along the camera's local Z axis).
 * [A]: Pan the camera left (along the camera's local X axis).
 * [S]: Pan the camera backward (along the camera's local Z axis).
 * [D]: Pan the camera right (along the camera's local X axis).
 * [F]: Moves the camera (Focus) to the currently selected light.
 * [0]: Move the camera to the world origin.
 * [R]: Resets the camera to the position specified in the configuration file.
 * [SHIFT]: Hold shift while pressing one of the other movement keys to move the camera faster.

 * [M]: Toggle Muilti-Sample Anti Aliasing (MSAA)
 * [L]: Toggle rendering of debug light volumes.
 * [V]: Toggle V-sync (turn this on if you experience screen tearing).
 * [SPACE]: Toggle animation of the lights in the scene.

 * [1]: Render using Forward Rendering.
 * [2]: Render using Tiled Forward Shading (Forward+)
 * [3]: Render using Volume Tiled Forward Shading
 * [4]: Render using Volume Tiled Forward Shading with BVH
 
 * [Ctrl+1]: Toggle Statistics UI
 * [Ctrl+2]: Toggle Profiler UI
 * [Ctrl+3]: Toggle Generate Lights UI
 * [Ctrl+4]: Toggle Light Editor UI
 * [Ctrl+5]: Toggle Options UI

Tiled Forward Rendering

 * [F1]: Toggle display of the tile heatmap.
 
Volume Tiled Forward Rendering

 * [F1]: Toggle display of volume tile heatmap.
 * [F2]: Toggle display of debug volume tiles.
 * [Shift+F]: Toggle the update of the volume tiles. Makes it possible to observe the volume tiles while the debug volume tiles are rendered.

## Camera

The camera for this demo uses a First-Person Shoter style controller. To rotate the view, click and drag on the screen using the left-mouse button. Use the W, A, S, D, to move the camera forward, left, backward, and right (respectively). Use the Q, and E keys to move the camera up and down in the camera's local space. Hold the Shift key to move the camera faster. Left-mouse clicking and dragging on a UI window will move that window instead of rotating the camera.

## XBox 360, XBox One Controller

The demo supports XInput controllers (XBox 360, XBox One, or any similar input device).

### Buttons 

[LS] - Left analog stick. (Also a button).
[RS] - Right analog stick (Also a button).
[RB] - Right bumper.
[LB] - Left bumper.
[LT] - Left trigger.
[RT] - Right trigger.

 * [LS]: Toggle faster translation.
 * [RS]: Toggle raster rotation.
 * [LT]: Pan the camera down.
 * [RT]: Pan the camera up.

### Axes

 * [LS]: Pan camera.
 * [RS]: Rotate camera.
 
# Compiling

This project comes with solution files for Visual Studio 2017.
The Visual Studio 2017 solution file can be found in the ./vs_2017/ folder.

The project has a dependency on the DirectX 12 SDK.
The DirectX 12 SDK is installed with the Windows Game SDK that must be selected when installing Visual Studio 2017.

# Requirements

This is a Windows project built and tested on Windows 10 with DirectX 12. 
The demo has a dependency on Windows 10 (64-bit) and requires a GPU that supports DirectX 12.
 
The project was built using Visual Studio 2017 (msvc141) and require the Microsoft Visual Studio 2017 C++ Runtime to be installed before running the demo. The installer for the MSVC141 runtime can be found in the ./vs_2017/ folder. Install the vc_redist.x64.exe to run the demo. Installing the Visual Studio 2017 C++ redistributable is only required when the Visual Studio development environment is not installed.

# Installation

Besides DirectX 12 and the Visual Studio C++ runtime requirements described in the REQUIREMENTS section, there are no further dependencies required to run the demo.

The demo uses a configuration file in order to run. The default configuration file will be used if no configuration file is specified when running the demo. The configuration file defines the size of the render window, which model file to load, the default position of the camera, as well as other configuration settings. Configuration files can be found in the ./Game/Conf/ folder. Configuration files have the .3dgep extension.

The configuration files can be used to automatically run the demo by executing the RegisterFileType_Win10_Rel_x64.bat file to register the Windows 10 64-bit version of the demo as the defualt handler for the .3dgep file extension. The file handelers can be removed from the registry by running the UnregisterFileType_Win10_Rel_x64.bat file.

After registering the file handler, the demo can be run with a particular scene by executing the appropriately named configuration file in the ./Game/Conf/  folder.


# Configuration

The executable accepts only a single command-line argument which is used to specify all of the configuration settings to run the demo with a particular scene file. The easiest way to create a new configuration file is to copy an existing one and modify the existing settings.

Do not delete the ./Conf/DefaultConfiguration.3dgep file as this one is used if no configuration file is specified on the command-line when running the programing (for example, if the executable is launched by double-clicking on the .exe file in Windows Explorer).

The configuration files are XML documents that contain the following information:
 
 * WindowWidth (int)           : The width of the window's client area in pixels.
 * WindowHeight (int)          : The height of the window's client area in pixels.
 * FullScreen (int)            : Currently unsupported. Set to 0 for windows mode, or 1 for fullscreen mode.
 * SceneFileName (string)      : The path to the model file to load. This path is expressed relative to the configuration file.
 * SceneScaleFactor (float)    : A scale factor to apply to the scene after loading.
 * CameraPosition (float3)     : A 3-component vector representing the initial 3D position of the camera.
 * CameraRotation (float4)     : A rotation quaternion representing the initial 3D rotation of the camera. No rotation is (0, 0, 0, 1)
 * NormalCameraSpeed (float)   : The speed of the camera.
 * FastCameraSpeed (float)     : The speed of the camera while the [SHIFT] key is pressed.
 * CameraPivotDistance (float) : The initial distance that the cameras rotation pivot point is placed in front of the camera.
 * PointLights (Array)         : The point lights array should only be configured using the on-screen GUIs.
 * SpotLights (Array)          : The spot lights array should only be configured using the on-screen GUIs.
 * DirectionalLights (Array)   : The directional lights array should only be configured using the on-screen GUIs.
 * LightsMinBounds (float3)    : The minimum bounds to generate lights. 
 * LightsMaxBounds (float3)    : The maximum bounds to generate lights. 
 * MinSpotAngle (float)        : The minimum spot light angle (in degrees) to use when generating spot lights.
 * MaxSpotAngle (float)        : The maximum spot light angle (in degrees) to use when generating spot lights. 
 * MinRange (float)            : The minimum range of generated light sources.
 * MaxRange (float)            : The maximum rnage of generated light sources.
 * NumPointLights (int)        : The number of point lights to randomly generate.
 * NumSpotLights (int)         : The number of spot lights to randomly generate.
 * NumDirectionalLights (int)  : The number of directional lights to randomly generate.
 
# Troubleshooting

This section describes troubleshooting tips if the demo does not run.

 * If the demo does not run make sure you have read the Requirements and Installation sections of this document.
 * If you still have problems running the demo then leave a comment on the website and I will try to answer your comment.

Website for more information: https://www.3dgep.com/volume-tiled-forward-shading/

# Known Issues

 * There is still a minor issue with the Volume Tiled Forward Shading with BVH that some lights are missing. I think this has something to do with the BVH traversal. The error does not occur when not using the BVH optimization.
 
# FAQ

 * 

# Contributors

This project is maintained by

 * [Jeremiah van Oosten](https://github.com/jpvanoosten) @jpvanoosten
