# Dependency #

gdipp uses the following dependency libraries:
  * **FreeType**: http://freetype.sourceforge.net/download.html
  * **EasyHook**: http://easyhook.codeplex.com/releases/
  * **pugixml**: http://code.google.com/p/pugixml/downloads/list
  * **WTL**: http://wtl.sourceforge.net/
  * **DirectX SDK**: http://msdn.microsoft.com/en-us/directx/

You will need all of them before building gdipp. It is recommended to always obtain the latest version of each library. For EasyHook, **Binaries with Examples** is necessary to build gdipp, while **Complete Source Code** is necessary to debug gdipp with hooking related issues.

## FreeType ##

Download and extract the archive. We provide a FreeType project file in the **freetype** directory with necessary modifications (e.g. x64 configuration). Carefully copy the content of the extracted **freetype-_x.y.z_** directory to the existing **freetype** directory without overwriting our **freetype.vcproj**. _x.y.z_ refers to the major and minor version number of the FreeType build you download.

## EasyHook ##

Download and extract the **Binaries with Eaxamples** archive. Place the extracted root directory in the gdipp root directory and rename it to **EasyHook**.

## pugixml ##
Download and extract the archive. pugixml does not come with Visual Studio project file, therefore we make one for you. Copy the content of the extracted **<pugixml-_x.y_>** directory to the existing **pugixml** directory.

## WTL ##

Download the extract the archive. Add the path of its include directory to the "Include files" VC++ Directories. The option can be found under the "Projects and Solutions" branch in the Options window.

## DirectX SDK ##

The project files look for the DirectX SDK in the same directory as the solution file. You can either install the SDK in the solution directory, or make a symbolic link.

# Build #

gdipp is configured to be built with Microsoft Visual Studio. The minimum required version is Visual Studio 2008 SP1 (which supports [&lt;regex&gt;](http://msdn.microsoft.com/en-us/library/bb982382%28v=VS.100%29.aspx)). Open the solution with Visual Studio, and select the desired configuration. Everything should go easy, and the output files are placed in the corresponding directories in the gdipp root directory.

Hooking for Chrome is enabled by supplying the "GDIPP\_INJECT\_SANDBOX" preprocessor directive when building gdimm. Removing it disables such support. It is enabled by default.

You may build gdipp with Cygwin or MinGW. However, modification to the source code is needed.

# Debug #

gdimm.dll cannot be run directly. In order to debug gdimm, set **gdipp Demo** as the StartUp Project. Before your first run, remember to copy **EasyHook`_`_xx_.dll** to the system directories.

**To debug a gdimm problem exists in all cases**, run gdipp Demo in debug mode, and set breakpoint in the hooked ExtTextOutW. If the problem only occurs in certain text, uncomment `debug_text` and set it to the problematic text.

**To debug a gdimm problem exists only in certain process**, run gdipp Loader in debug mode with appropriate command arguments. Attach to the launched process and set breakpoint. You can also start gdipp Service, inject the debug gdimm to the target process, wait until the assertion fail window to pop out. At that time, attach the target process, and click "Retry" in the assertion fail window.

In order to debug hooking related issues, obtain the EasyHook **Complete Source Code** archive. Build the debug version, and substitute the release version .lib and .dll files.

# Installer #

gdipp builds the installer with [Windows Installer XML](http://wix.sourceforge.net/) toolset. Obtain WiX by following http://wix.sourceforge.net/releases/. Simply use the latest installer, and do not forget to install the **Visual Studio 2008 Integration** component. http://www.tramontana.co.hu/wix/index.php is a complete WiX tutorial to start with.

To embed zh-cn locale into the en-us installer, the [EmbedTransform](http://www.tramontana.co.hu/wix/download.php?file=samples/embedtransform.zip&type=application/zip) tool is required. Download and place is in the WiX bin directory before compiling the WiX project. http://www.tramontana.co.hu/wix/lesson9.php talks about locale embedding in WiX.