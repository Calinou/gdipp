The task of gdimm.dll is to replace the original text rendering function in Windows with its own implementation. Ideally, every single character should be in its new look, new style, new shape. It is not difficult to accomplish the function replacement, but it is not easy to generate the exactly same result as the original in all cases.

It is a sophisticated task. The result of the text out functions are not only affected by the input parameters, but also properties of the device context (DC), set prior to the function call. Such properties include but not limited to foreground/background colors, bitmap bits, text alignment and coordination system transformations. All properties must be carefully taken care of to correctly draw the text. If the DC is applied to a 90-degree rotation, the function call may seem exactly the same as others, until explicitly look into the coordination transformations.

Fortunately, most of the properties can be obtained from API function calls, granting the possibility of re-creating the whole text system by our own. However, it cannot be as efficient as Windows:

  * Reason 1: Windows has data that we have not. In order to render text, we need to manipulate the DC bitmap. The DC bitmap bits are not directly accessible to user-space programs. While for Windows, text rendering might be just memory manipulation, for user-space programs we need to retrieve the bitmap bits of the corresponding area first, change right bits, and write the whole bitmap back. The difference in efficiency comes from text with transparent background, that the rendering performance is a function of bits set in the text glyph for Windows, but constant for us, no matter how few strokes the character has.

  * Reason 2: Windows is closed-source. We do not know how does Windows do the scaling and rotation internally. We do not know how does Windows map the logical fonts to physical fonts. There are number of attributes that are vaguely documentation for their purposes. It is understandable that the documentation is for users, not for re-implementers.

Based on the reasons above, we choose not to implement every function call, but redirect those especially sophisticated and rare cases to the original API for better compatibility. Currently, the following cases are not implemented:
  * The font is created with escapement (rotation).
  * The DC has a transformation.
  * The DC has a modified coordination system.