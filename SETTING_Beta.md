#summary Description of the latest beta version (0.9.1) setting file
#labels Phase-Deploy

gdipp improves user experience by allowing users to customize settings. All user settings are written in gdipp\_setting.xml, which is shared by both 32-bit and 64-bit components. A default setting file is installed along in the gdipp directory. Changed settings are applied when gdimm is loaded into the new process. When editing manually, be aware of the XML structure. gdipp\_setting.xml is saved in UTF-8 encoding.

The root node of gdipp\_setting.xml is `<gdipp>`. Its child nodes are `<gdimm>`, `<demo>` and `<exclude>`. `<gdimm>` node contains the selector nodes of gdimm settings. `<demo>` node contains settings for gdipp Demo. `<exclude>` node contains excluded process names and font names, which is used by both gdimm and gdipp Service.

The `<version>` node at the begnning of gdipp\_setting.xml identifies the version of the setting file, and does not affect behavior of any gdipp component. If the versions of gdipp components differs from this version numbers, please obtain the installer and reinstall gdipp.


## gdimm selector nodes ##

gdimm selector nodes applies contained settings to selected targets. Each selector node has various number of child setting nodes and attributes. When setting **S** of object **O** is requested, gdipp visits all selector nodes. If the current node **N** _selects_ **O**, gdipp checks if **N** contains **S**. If yes, use the setting value. If no, continue to the next node, until all nodes are visited. If **O** is not selected by any node or the setting file is missing, gdipp applies the internal corresponding default values to **S**.

Node selection is based on the constraint of the optional node attributes. **N** _selects_ **O** only if all supplied attributes of **N** match the property of **O**. More attributes mean more constraint. If no attribute is supplied for **N**, any object will be selected by **N**. Currently there are two types of selector nodes. `<process>` nodes select processes and `<font>` nodes select font faces. `<process>` nodes have higher priority than `<font>` nodes.

The most commonly used attribute is **name**, which limits the process name and font face name the node applies to. This attribute is written in [C++ TR1 Regular Expressions](http://msdn.microsoft.com/en-us/library/bb982727.aspx). For more information about regular expression, see http://en.wikipedia.org/wiki/Regular_expression. Below is the complete reference of node attributes available in gdipp.

| **Node type** | **Attribute name** | **Possible values** | **Description** |
|:--------------|:-------------------|:--------------------|:----------------|
| process, font | name | _Any regular expression_ | Target process name and font face name. |
| font | bold | 0, 1 | The boldness of the target font face. When value is 0, bold font faces do not use the setting items in the node. When value is 1, it restricts non-bold font faces (normal, semibold) from applying the node. The boldness of the value has nothing to do with the internal weight of the physical font file, but rather the displayed logical font. |
| font | italic | 0, 1 | Select/Deselect italic font face in the same manner as **bold**. |
| font | max\_height | _Positive integer_ | Similar to gdi++ **MaxHeight**. The maximum point size (what you see in the Windows font selection dialog) of the target font face. The main purpose of this attribute is for compatibility of cross-platform programs (e.g. GTK). |

Font face name usually consists of two parts: font family name and font style name, connecting with a space. Font family name is what you see in the **Font** column of the font dialog. Font style name is what you see in the **Font Style** column of the font dialog. One font family may contain multiple styles, and may map to different physical font files. Typically, there are four font styles: (empty for Regular style), Bold, Italic, Bold Italic. Therefore, a bold italic Segoe UI font face name is **Segoe UI Bold Italic**. For more information about font, see http://en.wikipedia.org/wiki/Font.

In order to override the default setting values, `<process>...</process>` and `<font>...</font>` shall be placed after the last one of each type of nodes. These two nodes match all objects that are not selected by any of the nodes above. They come with the default setting file and it is advised not to remove them.


### Examples ###

Suppose the gdimm requests **auto\_hinting**, **embedded\_bitmap** and **embolden** settings. The following clip is found:

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

<font>
  <auto_hinting>1</auto_hinting>
  <embolden>2.0</embolden>
</font>
```

In this example, if the current process is notepad.exe, and font face name is Segoe UI Bold, the final value of both **auto\_hinting** and **embedded\_bitmap** are 0, since the first `<process>` node matches the process name. The final value of **embolden** is 1.0, since the first `<font>` node matches the font face name. If the current process name is changed to textpad.exe, **embedded\_bitmap** becomes 1, because only the second `<process>` node matches the process name. **auto\_hinting** becomes 1, which comes from the last fail-safe `<font>` node.

Suppose the following setting is found:

```
<font name="Segoe UI.*" bold="1" italic="0">
  ...
</font>

<font name="Tahoma.*" italic="1">
  ...
</font>

<font name="Consolas.*">
  ...
</font>
```

The first `<font>` node matches any font face with face name starting with Segoe UI. It also restricts the selected face to be bold and non-italic. The second node matches italic Tahoma font faces, no matter if bold or not. The third node matches all Consolas font faces disregarding font style.


## gdimm setting items ##

Below is a table of the descriptions of available `<gdimm>` setting items. For yes/no values, 1 means yes and 0 means no and. Availability indicates which renderers employ such item: FreeType (FT), GetGlyphOutline (GGO), DirectWrite (DW), WIC (WIC).

| **Setting name** | **Possible values** | **Default value** | **Availability** | **Description** |
|:-----------------|:--------------------|:------------------|:-----------------|:----------------|
| auto\_hinting | 0, 1, 2 | 1 | FT | Controls the auto-hinting mode of the FreeType renderer. Auto-hinting is a special hinting mode of FreeType. Check http://freetype.sourceforge.net/autohinting/index.html for details. Value 0 completely disables auto-hinting. Value 1 instructs FreeType to use TrueType bytecode interpreter preceding to auto-hinting. Value 2 forces FreeType to use auto-hinting all the time. |
| embedded\_bitmap | 0, 1 | 0 | FT | Enable/disable use of embedded bitmap in the font file. If enable, the glyph bitmaps are shown as-is. |
| embolden | _Any integer_ | 0 | FT, GGO | Adjust the weight of all text. 0 makes no change, positive integers embolden the text, while negative integers affect the opposite way. Additional computation is require for any non-zero value. |
| freetype/cache\_max\_xxx |  |  | FT | Settings in the **freetype** node are parameters passing to initialize FreeType libraries. These parameters are used only at the startup stage of each process, therefore setting them in `<font>` nodes has no effect. **cache\_max\_xxx** are parameters for FreeType cache subsystem (reference http://freetype.sourceforge.net/freetype2/docs/reference/ft2-cache_subsystem.html#FTC_Manager_New). |
| freetype/lcd\_filter | 0, 1, 2, 16 | 1 | FT | Parameter that affects subpixel rendering (reference: http://freetype.sourceforge.net/freetype2/docs/reference/ft2-lcd_filtering.html#FT_LcdFilter). |
| gamma/xxx | _Non-negative decimal_ | 1.0 | All | Correct colors by the given [gamma](http://en.wikipedia.org/wiki/Gamma_correction) value. The red, green and blue channels are corrected separately. For WIC renderer, the result gamma is the average of the three values. Color is not corrected if the gamma value is 1.0, which is also the default for all channels. Gamma correction is applied universally, and cannot be bypassed. |
| hinting | 0, 1, 2, 3 | 1 | All | Strength of [hinting](http://en.wikipedia.org/wiki/Font_hinting). Hinting makes characters clear to read, at cost of transformed shapes. Value 0 disables hinting, which makes the edges of characters blur, but the glyph shapes are maintained. Other values increases the strength of hinting accordingly. Regarding the DirectWrite series renderers, this setting is parsed differently, and the maximum antialiasing is reached when value is 1. |
| kerning | 0, 1 | 0 | FT | Whether or not use [kern](http://en.wikipedia.org/wiki/Kerning) pairs to adjust inter-character gap. According to [MSDN](http://msdn.microsoft.com/en-us/library/dd370916%28VS.85%29.aspx), DirectWrite always does kerning. |
| render\_mode | 0, 1, 2 |  | All | **mono**, **gray** and **subpixel** determine how gdipp chooses the monochrome, gray-level and subpixel modes when rendering glyphs. For each item, value 0 disables one render mode, and value 2 forces gdipp to use such mode. When set to 1, gdipp automatically chooses the most appropriate one according to DC color depth. Similar to the selector nodes, these items are checked in order of **mono** -> **gray** -> **subpixel**. Therefore, if multiple items are set to 2, the firstly checked setting is applied. |
| pixel\_geometry | 0, 1 | 0 | All | The geometric arrangement of the primary colors for subpixel render mode. Value 0 means RGB. Value 1 means BGR. |
| aliased\_text | 0, 1 | 0 | All | The font whose quality is NONANTIALIASED\_QUALITY is ignored by gdipp if set to 0. Otherwise, it is rendered according to the DC color depth. |
| renderer | 0, 10, 20, 30, 31 | 10 | All | Which renderer is used to render fonts. Each main renderer type reserves 10 values for itself and 9 possible variations. ClearType is used when set to 0 (no rendering). FreeType renderer is used with value 10. The GetGlyphOutline API renders fonts when set to 20. DirectWrite and WIC renderers are chosen when set to 30 and 31, respectively. |
| shadow/offset\_xxx | _Any integer_ | 1 | FT, GGO, DW | Settings in the **shadow** node controls the effect of text shadow. **offset\_xxx** settings indicate the horizontal offset and vertical offset of the shadow. Positive horizontal offset moves shadow right. Positive vertical offset move shadow down. |
| shadow/alpha | 0 - 255 | 0 | FT, GGO, DW | The transparency of the text shadow. 0 disables the shadow, and 255 makes the shadow totally opaque. |


## gdipp Demo setting items ##

Below is a table of the descriptions of each `<demo>` setting item.

| **Setting name** | **Possible values** | **Default value** | **Description** |
|:-----------------|:--------------------|:------------------|:----------------|
| count | _Non-negative integer_ | 5000 | Number of times to render text. |
| threads | _Positive integer_ | 2 | Number of threaded painter windows. |
| random\_text | 0, 1 | 0 | Render random text. Each character is randomly generated. Most of the characters will be not readable. It is guaranteed that none of the characters is control character. |
| font | _Available font name_ |  | Name of the fonts that are used to render the text. Regular expression is not used. You can specify multiple **font** settings to use multiple fonts, which are randomly selected for each render. If **random\_text = 0**, the name of the font in each render is chosen as the content of the text. |


## gdipp exclusion list ##

The following kinds of node are under the the `<exclude>` node. To exclude certain fonts, set their **renderer = 0** in `<gdimm>`.

| **Node name** | **Description** |
|:--------------|:----------------|
| process | gdipp excludes these processes from injecting or rendering. The process name is matched against regular expression. To specify multiple excluded processes, use multiple nodes. |

For stability, several system processes are excluded in the default setting file. In case of severe fault caused by gdipp, user should have chance to login and disable it.