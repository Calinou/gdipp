#summary What's new in gdipp
#labels Phase-Deploy

Minor bug fixes are omitted in this log.

The version number is in <font color='red'>red</font>**if the setting file setting.xml is changed. You should review the [HowToUpgrade](http://code.google.com/p/gdipp/wiki/HowToUpgrade) document first and carefully transfer the old settings to the new setting.xml rather than simply overwriting the new setting.xml.**Do not use an old setting file in a new gdipp version.**Critical settings may not be recognized.**

**0.9.1**
  * Global:
    * <font color='red'>Officially use LGPL as the license of gdipp from now on.</font>
    * Fix bug that unnamed selector nodes in the setting file are ignored.
    * **max\_height** refers to the font point size now.

  * gdimm:
    * Replace ScriptPlace().
    * Fix bug that text shadow is misplaced.
    * Support Windows XP (DirectWrite series renders will not be available).

  * gdipp Service:
    * Fix problem that rendering is not unavailable if Windows is not logged in automatically (Thanks to the help of [pepe.haskell@gmail.com]).
    * Support rendering under **UAC** and **Run As Administrator** mode (Thanks to the help of [pepe.haskell@gmail.com]).
    * Support rendering when multiple users are logged in simultaneously.

  * Installer:
    * Remove support of registry load mode.
    * New feature selection dialog.
    * Add license dialog.


<font color='red'>0.9.0</font>*** Global:
    * Update to FreeType 2.4.2.
    * Update to pugixml 0.9.
    * Fix the [R6030](https://code.google.com/p/gdipp/source/detail?r=6030) "CRT not initialized" runtime error by moving gdipp\_common\_xx.dll to Delay Loaded DLLs.**

  * gdimm:
    * Completely new internal implementation: more API hooks, hierarchical cache subsystem and high-performance painting algorithms.
    * Use **cache\_max\_faces** and **cache\_max\_sizes** instead of **cache\_max\_bytes** as cache limit token.
    * Support the **max\_height** optional parameter in setting file.

  * gdipp Demo:
    * Paint text in separate multi-threaded windows. The thread number is configured in the setting file.

  * gdipp Hook:
    * Change hook type to WH\_CBT.

  * gdipp Service:
    * Respawn gdipp Hook when unexpectedly terminated.

  * Installer:
    * Add manual installation batch script.
    * Display load mode descriptions in scrollable text control.


<font color='red'>0.8.2</font>*** gdimm:
    * Support font linking scaling factor.
    * Support color specified as a palette entry.
    * Fix bug that shadow has no offset.
    * Fix bug that fails font linking when control characters exist.
    * Fix bug of cache node reference count when multiple font linking is taken.
    * Better text extent when it is opaque background.**

  * gdipp Demo and Preview:
    * Explicitly load setting when gdimm.dll cannot be loaded.

  * gdipp Service:
    * Use SetWindowsHookEx instead of enumeration and WMI to hook processes. Therefore gdipp Service no longer consumes any CPU resource.
    * The **poll\_interval** service setting is deprecated because of the change.
    * gdipp Enumerator is renamed to gdipp Hook.

  * Installer
    * Make symbolic link of gdipp Common and setting file instead of writing %PATH% environment variable. This should minimize the "missing gdipp\_common\_xx.dll" errors.
    * Update registry installation script.
    * Fix bug in installer script.


<font color='red'>0.8.1</font>*** Global:
    ***<font color='red'>Multiple setting items are changed. Please review <a href='http://code.google.com/p/gdipp/wiki/SETTING_Beta'>SETTING_Beta</a> for the new layout.</font>
    * Enable SSE2 instead of SSE on compiling.

  * gdimm:
    * Rewrite DirectWrite renderer.
    * Add WIC renderer.
    * Rewrite drawing system and significantly improve performance.
    * Add kerning support.
    * Improve font manager thus minimizing crashes in multi-threaded applications (e.g. Windows Explorer).
    * Fix historic compatibility issue of Qt applications and desktop glitch when changing wallpaper.
    * Fix bug that certain Chinese fonts have wrong advance distance by adding FT\_LOAD\_IGNORE\_GLOBAL\_ADVANCE\_WIDTH flag again.
    * Fix GDI object leak cause by DLL injection and ejection.

  * gdipp Demo:
    * Add Tools menu and a number of functions.

  * gdipp Enumerator:
    * Inject gdimm to all processes in the same session as the active user.
    * Eject gdimm from all processes in the same session as the active user.

  * gdipp Service:
    * Only inject gdimm to processes in the same session as the active user.


<font color='red'>0.8.0</font>*** Global:
    * Move development platform to Visual Studio 2010.
    * The "renderer" setting item has new values as DirectWrite is introduced.
    * Abandon the setting item "light\_mode". The new integer-based "auto\_hinting" and "hinting" are used instead.
    * Increase cache size and disable shadow in the default setting.**

  * gdimm:
    * Support DirectWrite. Since the minimum system requirement of DirectWrite is Windows Vista, gdipp is no longer available in Windows XP.
    * Rewrite the GDI drawing function, which is more efficient.
    * New component "gdipp Enumerator", which enumerates processes and injects gdimm. Can be used stand-alone.
    * Reduce overhead by switching to the single-instance model for the text out objects.
    * More compatible OS/2 table retrieval function.
    * Fix bug that renders incorrect glyphs if the font face is other than the first face in a TTC file.
    * Ignore ExtTextOut call if within open path bracket (BeginPath, EndPath, AbortPath). This is a fix for Google Earth.
    * Add batch files for manual registry installation.


<font color='red'>0.7.6</font>*** Global:
    * Default zero\_alpha to 1.**

  * gdimm:
    * Use reference count to manage cached glyph. Better compatibility in multi-threaded environments.
    * Redrawing for injection only affects active windows of the target process.
    * Decrease embolden weight for artifical bold font faces.
    * Fix bug that gdipp crashes process when point size is very small.
    * Various code changes and optimizations.


<font color='red'>0.7.5</font>*** Global:
    * Rename**setting.xml**to**gdipp\_setting.xml**.
    * The "embolden" setting is now integer based.**1.0**in previous versions is equivalent to**64**now.
    * More comprehensive minidump.**

  * gdimm:
    * Optimize rendering performance by using glyph bitmap cache.
    * Optimize performance of the setting cache.
    * Better italic style detection.
    * Emulate GDI that only have 3 classes of bold weight.
    * Better redraw mechanism after gdimm is loaded and before unloaded.

  * gdipp Demo
    * Better COM initialization mechanism.

  * gdipp Preview
    * Add prototype of gdipp Preview.


<font color='red'>0.7.4</font>*** Global:
    * Create minidump under the "crash\_dump" subdirectory in the gdipp directory when process is crashed because of gdipp.
    * Create as separate dll.
    * Add `<version>` in setting.
    * Add rule for VirtualBox.exe in default setting.xml.
    * Remove `<font>` in `<exclude>`. Use `<renderer>`0`</renderer>` instead.
    * Default value of `<alpha>` in `<shadow>` is changed to 8.
    * Add gdipp Demo settings to the default setting.xml.
    * Default value of `<count>` in `<demo>` is changed to 5000.
    * Rename `<rand_text>` to `<random_text>`.**

  * gdimm:
    * Fix bug that the bold style is not working if the font has no separate bold file.
    * Redraw process foreground window when unloading.
    * Optimize rendering performance.

  * Demo:
    * Use [WTL](http://wtl.sourceforge.net/) as the UI library.


<font color='red'>0.7.3</font>*** Global:
    * Optimize regular expression performance.
    * Case-insensitive regular expression.
    * Add settings for gdipp Demo.**

  * gdimm:
    * Fix memory leak for GDI objects.
    * Better way to deal with control characters in FreeType mode.
    * More compatible algorithm to calculate glyph rect.

  * Demo:
    * Significantly improvement with random text and effects.


**0.7.2**
  * Global:
    * Move documentation online.

  * gdimm:
    * Change font manager to more compatible multi-threading mechanism. gdipp works in Windows XP again.
    * New structure of the render engine with significant performance gain.

  * Service:
    * Simplify the process creation monitor, making it consume less CPU.


**0.7.1**
  * Global:
    * Add FreeType cache settings.

  * gdimm:
    * Add interlock on FTC\_ImageCache\_LookupScaler.
    * Fix bug that certain characters cannot be rendered when freetype\_loader=0.
    * Brand new font manager that fix the crash problems in 0.7.0.
    * Double-checking gdimm unloading subroutine.
    * Fix bug that crashes gdimm when negative distances are specified in lpDx.

  * Service:
    * Spawn thread to inject.

(Due to the new font manager and multi-threading implementation, 0.7.1 may not work in Windows XP. We will use a more compatible implementation in 0.7.2, which supports Windows XP.)


**0.7.0**
  * Global:
    * Enable SSE for 32-bit build.
    * Windows XP should render much better now.

  * gdimm:
    * Multi-threaded rendering.
    * Fix bug that inverts the hinting setting when freetype\_loader=0.
    * Fix bug that renders text on different baseline in certain processes (e.g. IE8).
    * Fix bug which causes Windows Installer fail to start the second time.

  * Loader:
    * Fix a bug that causes it fail to work.

  * Service:
    * Fix bug that service cannot restart if there is lingering gdimm.dll in living processes.


**0.6.1**
  * gdimm:
    * Change back to monitor service termination in ExtTextOutW.
    * Fix bug that crash gdimm when setting.xml is missing or in bad structure.

  * Installer:
    * Install service with "demand" start type.
    * Fix wrong path in installer for registry load mode.
    * Fix bug that the new setting.xml does not overwrite the old one.


**0.6.0**
  * Global:
    * Match font and process settings with regular expression.

  * gdimm:
    * Fix bug that causes font linking fails.
    * Fix bug that crashes gdimm if freetype\_loader=0.
    * Fix bug that gdimm does not render in when not in Aero.
    * Start a monitor thread in gdimm for active unloading.
    * Monitor service termination via event.
    * Add setting cache.
    * Use the face name instead of family name to match font settings.
    * Use the face name instead of family name to identify font in font manager.
    * Support oblique style (with default slant angle of 20 degrees).

  * gdipp Service:
    * Add poll interval option.
    * Initial injection (with internal process name list).
    * gdipp Service no loader belongs to UIGroup.

  * Installer:
    * Fix bugs related to 64-bit platforms in installer.
    * Backup setting file when installing.
    * Quiet uninstall when the registry load mode is selected.


**0.5.3**
  * gdimm:
    * Add option of gamma correction.
    * Add option of rendering non-AA font.
    * Add option of zeroing the alpha channel.

  * Installer:
    * Fix bug of service installing in the installer script.


**0.5.2**
  * gdimm:
    * Exclude bitmap with 16 bpp.

  * gdipp Loader and Service:
    * Inject 32-bit and 64-bit process separately.

  * Installer:
    * Install 32-bit and 64-bit services separately.
    * Merge all languages into one msi.


**0.5.1**
  * gdimm:
    * Draw shadow.
    * Process individual setting.
    * Optimize setting algorithms.
    * Fix bug that fonts with embedded bitmap cannot be rendered when hinting is disabled.
    * gdimm checks process exclusion.

  * gdipp Loader:
    * Fix bug in 0.5 that it loads itself infinitely if no argument is fed.

  * Installer:
    * Install EasyHook to System folder for better compatiblity.
    * Start gdipp Service earlier so that explorer can be rendered after Windows starts.
    * setting.xml will not be overwritten when installing, and will not be delete after uninstallation.
    * Remove prompt to user to reboot after installation, if the Registry load mode was chosen.


**0.5.0**
  * gdimm:
    * Chrome inside sandbox can be rendered now.
    * More stable algorithm for loading font linking registry values.
    * Font link match direction is changed back to "first -> last".
    * Fix bug which causes memory leaking.
    * Fix bug about font stream.
    * Force redrawing host process after being injected by gdipp Service.
    * Correct calculation of character width, thus the text in the Aero theme Window titlebars can be rendered.
    * Get glyph index by calling GetGlyphIndices, better compatibility.
    * Add setting for embedded bitmap.
    * Compile FreeType into gdimm.dll.

  * gdipp Service:
    * Checks if the target process has already loaded gdimm.dll.

  * gdipp Demo and gdipp Loader:
    * Remove dependency for Visual C++ 2008 Runtime.

  * Installer:
    * Distribute gdipp with installer, made by Windows Install XML.