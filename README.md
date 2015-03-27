#summary README
#labels Phase-Deploy

## Overview ##

The gdipp project is the relaunch of the suspended gdi++.dll  project. Like the InkStone project, we found the source code of gdi++.dll project is too complex to manage, and decide to start over a new code base. The goal of gdipp project is to continue the development of gdi++, expand its functionality, improve its performance, provide better compatibility for the current and future Windows versions (potentially at the cost of removing compatibility for old Windows), create detailed and formal documentation, as well as tools to help using it.

Under the installed directory, the following files are contained (depend on the chosen load mode, some files may not be installed). Here is some brief descriptions of each:

  * gdimm\_xx.dll: The main component of the gdipp project, comparative of gdi++.dll. Most of the font management, text rendering and API simulating are taken place here. gdimm means gdi-- for emphasizing its design idea of simplicity comparing to gdi++.dll.

  * gdipp\_common\_xx.dll: supports libraries for the other components. An outstanding component is the setting manager.

  * gdipp\_demo\_xx.exe: a demo that illustrates and debugs the gdimm.dll. It loads the gdimm.dll and uses the gdimm text out functions to render benchmark text. If no gdimm.dll is found in the current directory, it uses the original Windows APIs.

  * gdipp\_hook\_xx.exe: invokes **SetWindowHookEx** to inject gdimm.dll to processes at start and invokes **UnhookWindowsHookEx** to eject gdimm.dll at termination. It serves as subprocess of gdipp Service. It terminates immediately if being manually executed. 32-bit application only affects 32-bit processes, and so does the 64-bit version.

  * gdipp\_loader\_xx.exe: accepts an executable file path as argument and runs it, comparative of gdi++.exe. If the process is successfully spawned, inject gdimm.dll to the running process, and the new process will render text with gdimm.dll instantly.

  * gdipp\_svc\_xx.exe: installed as a Windows service. It spawns and maintains gdipp Hook to accomplish gdimm.dll injection. Once stopped, gdipp Hook will be terminated and gdimm.dll will be automatically ejected from all processes (text will be rendered by Windows default). It can be set to start with system via services.msc. The display name of the service is "gdipp Service".

  * COPYING and COPYING.LESSER: license of gdipp. Read them before you use, modify or redistribute gdipp.

In the **Script** directory, we also provide batch scripts to users who prefer to install gdipp in various load modes manually. To use these scripts, extract all files from the msi file with the [provided script](http://code.google.com/p/gdipp/downloads/detail?name=msi_extract.bat). Place the extracted directory to the desired path. Go to the **Script** directory and execute the script.
  * Create\_Services.bat: creates gdipp 32-bit and 64-bit services, regardless of what the bitness of the Windows is.
  * Delete\_Services.bat: stops and deletes the gdipp 32-bit and 64-bit services.
  * Start\_Service\_xx.bat: starts the _xx_-bit gdipp Service. _xx_-bit service only renders _xx_-bit processes.
  * Stop\_Service\_xx.bat: stops the _xx_-bit gdipp Service.
  * Registry\_Enable\_xx.bat: enables the **registry load mode** for gdipp in _xx_-bit Windows, e.g. use Registry\_Enable\_64.bat only if you are running 64-bit Windows. For more information, read the next section.
  * Registry\_Disable\_xx.bat: undoes the registry changes by Registry\_Enable\_xx.bat.

Detailed description of gdipp\_setting.xml is placed in SETTING for latest [stable version](http://code.google.com/p/gdipp/wiki/SETTING_Stable) and [beta version](http://code.google.com/p/gdipp/wiki/SETTING_Beta).

**Please review [ChangeLog](http://code.google.com/p/gdipp/wiki/ChangeLog) and SETTING before installing a new version.**


## Registry Load Mode ##

Registry load mode is an alternative way to enable gdipp's system-wide text rendering. The hook mechanism is implemented internally in the Windows, grants the DLL injection more complete and less footprint. However, it is not the recommended way of using gdipp with the following reasons:

  * The registry load mode is designed to support injection of simple and isolated DLL. As the scale of gdipp grows, this mode is less and less suited, and destined to be abadoned completely. As the registry load mode is widely used in gdi++, please compare the functionality of gdipp and gdi++.

  * The injection mechanism of the default gdipp Service (SetWindowsHookEx) is also shipped by Microsoft. From the name one can tell it is more suited for system-wide hooking. The services works better and better as gdipp advances.

  * gdipp Service provides better compatibility and controllability. As described below, related registry paths are inconsistent between Windows versions. Errors can be introduced when deploying gdipp in different systems. While gdipp Service can be started and stopped (in GUI management tool) thoroughly at any time, it may be required to reboot Windows to do the same for the registry load mode.

  * Microsoft declared that the related registry values may be changed or deprecated in the future.

If you insist on using the registry load mode, here is information of how to enable/disable it. The following registry values need to be modified to enable the registry load mode.
  * In 32-bit Windows, all the values are in **HKEY\_LOCAL\_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Windows**.
  * In 64-bit Windows, all the values for the 64-bit gdimm.dll are in **HKEY\_LOCAL\_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Windows**.
  * In 64-bit Windows, all the values for the 32-bit gdimm.dll are in **HKEY\_LOCAL\_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Windows NT\CurrentVersion\Windows**.

The following table lists the values:

| **Name** | **Type** | **Default value** | **Description** |
|:---------|:---------|:------------------|:----------------|
| AppInit\_DLLs | REG\_SZ | (Empty) | The absolute path the corresponding gdimm.dll. |
| LoadAppInit\_DLLs | REG\_DWORD | 0 | This value only presents in Windows versions later than Windows XP. It must be set to 1 to activate the DLL loading functionality. |

**Before enabling the registry load mode for gdipp, make sure the absolute path of gdipp directory is added to the %PATH% environment variable.** If you are not sure what does this mean, install with the gdipp installer without choosing the service component. It will add the environment variable automatically.

The registry load mode and gdipp Service shall not be used simultaneously.

To disable the registry load mode, revert all the modified registry to the default values.

For more information, please review http://msdn.microsoft.com/en-us/library/dd744762%28VS.85%29.aspx and http://support.microsoft.com/kb/197571.


## Better than gdi++ ##

gdipp is superior than gdi++ in various aspects:
  * Seamless support for 32/64-bit Windows. The main reason that gdi++ does support 64-bit Windows is the [Detours](http://research.microsoft.com/en-us/projects/detours/) hooking library, which is free only for its 32-bit version. gdimm instead uses the open-source [EasyHook](http://easyhook.codeplex.com/) library, and support 64-bit Windows from scratch.
  * DirectWrite support.
  * Vertical text in programs like PowerPoint are correctly rendered. There is no color glitch in PowerPoint presentation.
  * Subtitles in various media players (SPlayer，KMPlayer，PotPlayer ...) are rendered without problem, no matter which video renderer is selected. On the contrary, subtitles are displayed only with the Overlay Mixer in gdi++ case.
  * Aero window title can be rendered correctly (by default rendered with ClearTyep).
  * No glitch after changing the wallpaper in Windows 7 when Aero is enabled.
  * Rotated text are displayed correctly.
  * Numerous compatibility enhancement (e.g. the control character in the Date/Time window in Windows 7).
  * So on and so forth...


## Relationship with ClearType ##

There is no need to disable ClearType when you are using gdipp. After gdipp takes its place in the processes, all text and characters are **only** rendered by gdipp. Since all characters are rendered once, no performance overhead is introduced by enabling both font renderer. In fact, ClearType is highly recommended to be enabled, because in cases that gdipp cannot process some text  (e.g. one character in a sentence is not presented in any of the installed fonts), it will forward the rendering task to ClearType.


## Credits ##

gdimm uses FreeType to manage fonts and glyphs. Related documentation is placed in the **FreeType** directory. Check http://freetype.sourceforge.net/ for details.

gdimm uses EasyHook to handle API hooking. Related documentation is placed in the **EasyHook** directory. Check http://easyhook.codeplex.com/ for details.

gdimm uses pugixml to process XML in setting.xml. http://code.google.com/p/pugixml/ is the website.

The installer is generated by Windows Installer XML toolset. Visit http://wix.sourceforge.net/ for their website.


## Thank you ##

Please visit our website for documentation, bug feedback and feature request.

gdipp project

http://code.google.com/p/gdipp/