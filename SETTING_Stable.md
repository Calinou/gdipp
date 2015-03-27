#summary Description of the latest stable version (0.7.6) setting file
#labels Phase-Deploy

gdipp improves user experience by allowing users to tweak settings. All user settings are in gdipp\_setting.xml, which is shared by both 32-bit and 64-bit components. A default setting file is installed in the gdipp directory. In case of any setting item not presented in gdipp\_setting.xml, or gdipp\_setting.xml is missing, gdipp uses the default values in place of the missing one. Changed settings are applied when gdimm is loaded into the new process. When editing manually, be aware of the XML structure. gdipp\_setting.xml is saved in UTF-8 encoding.

The root node of gdipp\_setting.xml is `<gdipp>`. Its child nodes are `<gdimm>`, `<service>` and `<exclude>`. `<gdimm>` node contains settings for gdimm. `<service>` node contains settings for gdipp Service. `<exclude>` node contains excluded process names and font names, which is used by both gdimm and gdipp Service.


---


Two types of child nodes can be included in `<gdimm>`. `<process>` nodes indicate separate settings for specified processes. `<font>` nodes indicate separate settings for specified font faces. The specified name of the processes and fonts comes from the "name" attribute of each node. This attribute support [C++ TR1 Regular Expressions](http://msdn.microsoft.com/en-us/library/bb982727.aspx). For more information about regular expression, see http://en.wikipedia.org/wiki/Regular_expression. The name for either type is matched top-down (the first available setting value is used). If one setting is not available in the first node, the subsequent nodes are queried, until all nodes are visited. If the setting is still not available, the default value is used. `<process>` nodes have higher priority than `<font>` nodes.

Font face name usually consists of two parts: font family name and font style name, connecting with a space. Font family name is what you see in the "Font" column of the font dialog. Font style name is what you see in the "Font Style" column of the font dialog. One font family may contain multiple styles, and may map to different physical font files. Typically, there are four font styles: (empty for Regular style), Bold, Italic, Bold Italic. Therefore, a bold italic Segoe UI font face name is "Segoe UI Bold Italic". For more information about font, see http://en.wikipedia.org/wiki/Font.

For example, suppose the gdimm requests **auto\_hinting**, **embedded\_bitmap** and **embolden** settings. The following clip is found:

```
<process name="note.*">
  <auto_hinting>0</auto_hinting>
  <embedded_bitmap>0</embedded_bitmap>
</process>

<process name=".*pad\.exe">
  <embedded_bitmap>1</embedded_bitmap>
</process>

<font name="Segoe UI.*">
  <embedded_bitmap>0</embedded_bitmap>
  <embolden>1.0</embolden>
</font>

<font name=".*">
  <auto_hinting>1</auto_hinting>
  <embolden>2.0</embolden>
</font>
```

In this example, if the current process is notepad.exe, and font face name is "Segoe UI Bold", the final value of both **auto\_hinting** and **embedded\_bitmap** are 0, since the first `<process>` node matches the process name. The final value of **embolden** is 1.0, since the first `<font>` node matches the font face name. If the current process name is changed to "textpad.exe", **embedded\_bitmap** becomes 1, because only the second `<process>` node matches the process name. **auto\_hinting** becomes 1, which comes from the last fail-safe `<font>` node.

The `<version>` node at the begnning of gdipp\_setting.xml identifies the version of the setting file, and does not affect behavior of any gdipp component. If the versions of gdipp components differs from this version numbers, please obtain the installer and reinstall gdipp.

Below is a table of the descriptions of each `<gdimm>` setting item. For yes/no values, 1 means yes and 0 means no and.

| **Setting name** | **Possible values** | **Default value** | **Description** |
|:-----------------|:--------------------|:------------------|:----------------|
| auto\_hinting | 0, 1 | 0 | Enable/disable auto hinting in the FreeType loader mode. Auto hinting is a special hinting mode of FreeType. Check http://freetype.sourceforge.net/autohinting/index.html for details. This value is only effective if **freetype\_loader** is 1, and always enabled if **light\_mode** is 1. |
| embedded\_bitmap | 0, 1 | 0 | Enable/disable use of embedded bitmap in the font file. If enable, the glyph bitmaps are shown as-is. |
| embolden | _Any integer_ | 0 | Adjust the weight of all text. 0 makes no change, positive integers embolden the text, while negative integers affect the opposite way. Additional computation is require for any non-zero value. |
| freetype/cache\_max\_xxx |  |  | Settings in the **freetype** node are parameters passing to initialize FreeType libraries. These parameters are used only at the startup stage of each process, therefore setting them in `<font>` nodes has no effect. **cache\_max\_xxx** are parameters for FreeType cache subsystem (reference http://freetype.sourceforge.net/freetype2/docs/reference/ft2-cache_subsystem.html#FTC_Manager_New). Note that the default cache parameters are pretty conservative. You are free to increase these numbers base on your system configurations. |
| freetype/lcd\_filter | 0, 1, 2, 16 | 1 | Parameter that affects subpixel rendering (reference: http://freetype.sourceforge.net/freetype2/docs/reference/ft2-lcd_filtering.html#FT_LcdFilter). |
| gamma/xxx | _Non-negative decimal_ | 1.0 | Correct colors by the given [gamma](http://en.wikipedia.org/wiki/Gamma_correction) value. For gray-level rendering mode (**subpixel\_render** is 0), only the value in the `<gray>` node is used, and all gray levels are corrected. For subpixel rendering mode (**subpixel\_render** is 1), red, green and blue channels are corrected separately. Color is not corrected if the gamma value is 1.0, which is also the default for all channels. Gamma correction is applied universally, and cannot be bypassed. |
| hinting | 0, 1 | 1 | Enable/disable [hinting](http://en.wikipedia.org/wiki/Font_hinting). Hinting makes characters clear to read, at cost of transformed shapes. Disabling hinting make the edges of characters blur, but the glyph shapes are unchanged. |
| light\_mode | 0, 1 | 1 | Enable/disable light mode when loading glyphs in FreeType loader mode. According to FreeType documentation, "Many generated glyphs are more fuzzy but better resemble its original shape. A bit like rendering on Mac OS X". **light\_mode = 1** forces **auto\_hinting** to 1. |
| max\_height | _Non-negative integer_ | 72 | Equivalent to gdi++ **MaxHeight**. gdimm.dll renders all text when it is 0. For any positive integer, gdimm.dll renders the characters whose height is no more than it, and leaves the rest to Windows. The unit of the height is the same as what you see in the font selection dialog. The main purpose of this item is for compatibility, like [StarDict](http://stardict.sourceforge.net/). It is not recommended to set to 0. |
| render\_mono | 0, 1 | 0 | The monochrome text are rendered by Windows when it is 0, otherwise by gdimm.dll. Monochrome text are rare to see, except some disabled menu items. ClearType renders monochrome glyphs much better than FreeType. |
| render\_non\_aa | 0, 1 | 0 | The text with font whose quality is NONANTIALIASED\_QUALITY is rendered as monochrome if set to 0, otherwise, rendered as anti-aliased. Setting this value to 1 forces some monochrome text to be rendered beautifully, but risks of causing compatibility issue. |
| renderer | 0, 1, 2 | 2 | Which renderer is used to render fonts. ClearType is used when set to **0** (no rendering). The GetGlyphOutline API renders fonts when set to **1**. FreeType becomes the renderer when set to **2**. Value 0 is used to exclude certain fonts. The GetGlyphOutline mode has the best compatibility, and the FreeType mode supports more effects. |
| shadow/offset\_xxx | _Any integer_ | 1 | Settings in the **shadow** node controls the effect of text shadow. **offset\_xxx** settings indicate the horizontal offset and vertical offset of the shadow. Positive horizontal offset moves shadow right. Positive vertical offset move shadow down. |
| shadow/alpha | 0 - 255 | 8 | The transparency of the text shadow. 0 disables the shadow, and 255 makes the shadow totally opaque. |
| subpixel\_render | 0, 1 | 1 | Value 0 renders text with gray levels, otherwise uses subpixel rendering. Generally subpixel rendering is more suitable for LCD monitors. |
| zero\_alpha | 0, 1 |1 | When set to 0, the alpha channel of the bitmap in DC is untouched when rendering, otherwise gdimm.dll zeros the alpha channel. This option is used to fix compatiblity issue of some Qt applications. This setting might become obsolete in the future. |


---


Below is a table of the descriptions of each `<demo>` setting item.

| **Setting name** | **Possible values** | **Default value** | **Description** |
|:-----------------|:--------------------|:------------------|:----------------|
| count | _Non-negative integer_ | 5000 | Number of times to render text. |
| font | Available font name |  | Name of the fonts that are used to render the text. Regular expression is not used. You can specify multiple **font** settings to use multiple fonts, which are randomly selected for each render. If **random\_text = 0**, the name of the font in each render is chosen as the content of the text. |
| random\_text | 0, 1 | 0 | Render random text. Each character is randomly generated. Most of the characters will be not readable. It is guaranteed that none of the characters is control character. |


---


Below is a table of the descriptions of each `<service>` setting item.

| **Setting name** | **Possible values** | **Default value** | **Description** |
|:-----------------|:--------------------|:------------------|:----------------|
| poll\_interval | _Non-negative decimal_ | 1 | Interval to query Windows if there is new process created, in seconds. Smaller values increase responsiveness at the cost of more CPU consumption. Note that Windows may consider small values invalid and cripples gdipp Service. |


---


The following kinds of node are under the the `<exclude>` node. To exclude certain fonts, set their **renderer = 0** in `<gdimm>`.

| **Node name** | **Description** |
|:--------------|:----------------|
| process | gdipp excludes these processes from injecting or rendering. The process name is matched against regular expression. To specify multiple excluded processes, use multiple nodes. |

For stability, the following system processes are excluded by default:
  * conhost.exe
  * csrss.exe
  * dllhost.exe
  * dwm.exe
  * lsass.exe
  * lsm.exe
  * services.exe
  * smss.exe
  * svchost.exe
  * taskhost.exe
  * userinit.exe
  * wininit.exe
  * winlogon.exe