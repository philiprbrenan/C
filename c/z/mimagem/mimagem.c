//------------------------------------------------------------------------------
// Edit Xml representing Dita.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#include <limits.h>
#include <arenaTree.c>
#include <rectangle.c>
#include <stringBuffer.c>
#include <cairoText.c>
#include <xml.c>
#include <utilities.c>

#ifndef $_included
#define $_included

//D1 Structures                                                                 // Structures describing an Arena Tree.

typedef struct $EditPosition                                                    // A position in a $ edit buffer
 {const struct ProtoTypes_$EditPosition *proto;                                 // Prototypes for methods
  size_t         character;                                                     // The number of the character at the position
  size_t         tag;                                                           // The tag containing at the position
  size_t         positionInTag;                                                 // The character offset in the tag at the position
  size_t         editLine;                                                      // The edit line containing the position
  size_t         x;                                                             // The x coordinate of the start of the position
  size_t         y;                                                             // The y coordinate of the start of the position
 } $EditPosition;

typedef struct $EditBuffer                                                      // $ edit buffer
 {const struct ProtoTypes_$EditBuffer *proto;                                   // Prototypes for methods
  XmlParse       xml;                                                           // Xml parse tree being edited
  CairoTextImage*cti;                                                           // Cairo text image that we are drawing into
  Rectangle      zone;                                                          // The rectangle in which the edit buffer will be drawn
  Rectangle      block;                                                         // A rectangle in the edit buffer that displays options in line with teh xml flowing around it so that bioth the options and the xmnl to b operated on are simultaneously visible.
  int            measureOnly;                                                   // Suppresses drawing if true.  All other operations are performed so that returned measurements of the pointer and cursor position are accurate.
  int            blockIn;                                                       // Enable the blocked out rectangle if true.
  double         lineHeight;                                                    // The distance between lines for the specified font size.
  size_t         scroll;                                                        // The number of edit lines we have scrolled down
  size_t         fontSize;                                                      // Font size to use in creating text
  size_t         px;                                                            // Pointer pixel position in x
  size_t         py;                                                            // Pointer pixel position in y
  $EditPosition  pointer;                                                       // Position of the pointer in $ edit buffer
  $EditPosition  cursor;                                                        // Position of the cursor in $ edit buffer
 } $EditBuffer;

#include <$$_prototypes.h>

static $EditBuffer drawEditBuffer_$EditBuffer_$EditBuffer                       // Draw the edit buffer and return the location of the pointer and cursor
 ($EditBuffer * editBuffer)                                                     // $ edit buffer
 {i    ◀ editBuffer->cti;                                                       // Image description
  cr   ◁ i->cr;                                                                 // Cairo context to draw in
  draw ◁ !editBuffer->measureOnly;                                              // Draw unless we only want to measure
  closestSoFar ◀ ULONG_MAX;                                                     // Shortest distance so far to pointer
  lineNumberGutterText ◁ 8;                                                     // Pixels Gutter between line numbers and text

  paleColours ◁ makeColourPale();                                               // Background colours for each tag by depth with text getting the same colour as its parent.
  lastBackGroundColourDrawn ◁ paleColours.p2;                                   // Keep track of the last character background colour used so that we can match the line numbers

  void                openTagFillColor() {i ▶ rgb(0,   0,   0.4);}              // Color for opening tag
  void               closeTagFillColor() {i ▶ rgb(0,   0,   0.4);}              // Color for closing tag
  void                   textFillColor() {i ▶ rgb(0,   0,   0  );}              // Color for text
  void             lineNumberFillColor() {i ▶ rgb(0.3, 0.3, 0.3);}              // Color for line numbers
  void lineNumberGutterTextFillColor  () {i ▶ rgb(0.3, 0.3, 0.3);}              // Fill   color for the gutter between the line numbers and the text being editted.
  void lineNumberGutterTextStrokeColor() {i ▶ rgb(1,   1,   1  );}              // Stroke color for the gutter between the line numbers and the text being editted.
  void   blockedOutAreaBackgroundColor() {i ▶ rgb(1,   1,   1  );}              // Fill color for the background of the blocked out area

  void       openFont() {i ▶ font(CairoTextFont_sansBold);    openTagFillColor();} // Font  for opening tag
  void      closeFont() {i ▶ font(CairoTextFont_sans);       closeTagFillColor();} // Font  for closing tag
  void       textFont() {i ▶ font(CairoTextFont_serif);          textFillColor();} // Font  for text
  void lineNumberFont() {i ▶ font(CairoTextFont_sansMono); lineNumberFillColor();} // Font  for line numbers

  i ▶ fontSize(editBuffer->fontSize);                                           // Cairo font
  editBuffer->lineHeight = i->fontHeight;                                       // Record line height
  scrollPixels ◁ i->fontHeight * editBuffer->scroll;                            // Number of pixels scrolled down

  size_t getLineNumberWidth()                                                   // Width of line numbers
   {N ◁ 8ul; n ◁ editBuffer->xml ▷ count;                                       // Maximum width we will allow
    char z[2] = {'0', 0};                                                       // A string of one zero
    a ◁ i ▶ textAdvance(z);                                                     // Width of string of one zero
    t ◀ 1ul;                                                                    // Power of ten
    for(size_t i = 1; i <= N; ++i)                                              // Find power of ten neede to allow all tag numbers
     {t *= 10ul;                                                                // Next power
      if (t > n) return i * a;                                                  // Big enough
     }
    return a * N;                                                               // Use default in extremis
   }

  lineNumberWidth     ◁ getLineNumberWidth();                                   // Width of line numbers
  editLineNumbersText ◁ editBuffer->zone      ▷ left(lineNumberWidth);          // Split the drawing area into line numbers and text
  editLineNumbers     ◁ editLineNumbersText.a;                                  // Line numbers
  editGutterAndText   ◁ editLineNumbersText.b ▷ left(lineNumberGutterText);     // Split the drawing area into line numbers and text
  editGutter          ◁ editGutterAndText.a;                                    // Gutter just before text
  editText            ◁ editGutterAndText.b;                                    // Text

  size_t currentTagOffset = 0, currentTagNumber =  0, currentPositionInTag = 0, // Current tag and position within current tag
         currentChar      = 0, currentEditLine  =  1;                           // Current character number counted over all tags drawn, current edit line

  double x = editText.x, y = editText.y - scrollPixels - i->fontHeight;         // Initial text position allowing for the new line operation which always occurs first.

  void drawTagOrText(XmlTag parent, int depth)                                  // Print the specified parent and its children
   {currentTagOffset = parent.node.offset;                                      // In case the pointer is located in this tag

    t ◁ parent ▷ isText; pcN ◁ paleColours.N;
    backgroundColour  ◁ paleColours.p[(abs(depth - (t ? 1 : 0))) % pcN];        // Choose the back ground colour for this depth and tag
    backgroundColour1 ◁ paleColours.p[(abs(depth - (t ? 2 : 1))) % pcN];        // Background colour for previous layer

    void startNewLine(int indent)                                               // Move to next line and indent if requested
     {++currentEditLine;                                                        // Edit line in the edit buffer drawing zone
      dx ◁ (indent ? i->fontHeight * depth : 0);                                // Requested indentation

      if (draw)                                                                 // Finish current line
       {i ▶ save;
        r ◁ makeRectangleWH(x, y, editText.X - x, i->fontHeight);               // Rectangle for line number
        i ▶ rectangle(r, lastBackGroundColourDrawn);                            // Background for line number
        i ▶ restore;
       }

      y += i->fontHeight;                                                       // Y coordinate of top of new line

      if (draw)
       {lsprintf(n, 1024, "%lu", currentTagNumber);                             // Format line number
        i ▶ save;

        b ◁ lastBackGroundColourDrawn;                                          // Background colour of line number
        i ▶ colour(b);
        cairo_rectangle     (cr, editLineNumbers.x, y,                          // Background for line number
                                 lineNumberWidth,   i->fontHeight);
        if (indent)
         {cairo_rectangle   (cr, editText.x, y, dx, i->fontHeight);             // Indentation for text
         }
        cairo_fill          (cr);

        lineNumberFont      ();                                                 // Text of line number
        a  ◁ i ▶ textAdvance(n);                                                // Width of this line number
        tx ◁ editLineNumbers.x + lineNumberWidth - a;
        i ▶ text            (tx, y + i->fontAscent, n);
        i ▶ restore;
       }
      i ▶ move(x = editText.x + dx, y);                                         // Position for first character of new line
     }

    void drawChar(char c, int openClose)                                        // Draw a character at the current (x,y) position and advance the currrent position to the end of the character drawn. A gradient background is drawn for the first/last letters of a tag (0: no gradient, 1: opening gradient, 2: closing gradient)
     {char s[2] = {c, 0};                                                       // Character to be drawn as a string

      width ◁ i ▶ textAdvance(s);                                               // Measure character

      void drawBackGroundForChar()                                              // Draw the background for the current character
       {b ◁ backgroundColour;
        B ◁ backgroundColour1;
        w ◁ width; h ◁ i->fontHeight;

        i ▶ save;
        if (openClose)                                                          // Gradient background
         {cairo_pattern_t *g = cairo_pattern_create_linear(x, y, x + w, y);     // Create gradient
          if (openClose == XmlOpen)                                             // Open
           {cairo_pattern_add_color_stop_rgb(g, 0, B.r, B.g, B.b);
            cairo_pattern_add_color_stop_rgb(g, 1, b.r, b.g, b.b);
            memcpy((void *)&lastBackGroundColourDrawn, &b, sizeof(b));
           }
          else                                                                  // Close
           {cairo_pattern_add_color_stop_rgb(g, 0, b.r, b.g, b.b);
            cairo_pattern_add_color_stop_rgb(g, 1, B.r, B.g, B.b);
            memcpy((void *)&lastBackGroundColourDrawn, &B, sizeof(B));
           }
          cairo_set_source (cr, g);
         }
        else                                                                    // Solid
         {cairo_set_source_rgb(cr, b.r, b.g, b.b);
          memcpy((void *)&lastBackGroundColourDrawn, &b, sizeof(b));
         }
        cairo_rectangle(cr, x, y, w, h);                                        // Draw background
        cairo_fill     (cr);
        i ▶ restore;
       }

      charR ◁ makeRectangleWH(x, y, width, i->fontHeight);                      // Rectangle occupied by current character

      if (editBuffer->blockIn && editBuffer->block ▷ containsACorner(charR))    // Skip over blocked out area
       {b ◁ lastBackGroundColourDrawn;                                          // Background colour of line number
        i ▶ save;
        r ◁ makeRectangleWH(x, y, editBuffer->block.X - x, i->fontHeight);      // Rectangle for line number
        i ▶ rectangle(r, b);
        i ▶ restore;
        x = editBuffer->block.X;                                                // Restart after the blocked out area
       }

      if (x + width >= editText.X) startNewLine(0);                             // Start a new line if the draw would be off the end of the line

      if (draw && editText ▷ containsACorner(charR))                            // Draw character or symbol if drawing and we are in the drawing area
       {drawBackGroundForChar();                                                // Background
        i ▶ text(x, y, s);
       }

      ++currentPositionInTag; ++currentChar;                                    // Pointer and cursor location
      if (editBuffer->py <= y + i->fontHeight && editBuffer->py >= y)           // Line containing pointer
       {d ◁ fabs(editBuffer->px - x - width / 2);                               // Distance from the center of the current character to pointer.
        if (d < closestSoFar)                                                   // Best distance so far
         {closestSoFar = d;
          editBuffer->pointer.tag           = currentTagOffset;
          editBuffer->pointer.positionInTag = currentPositionInTag;
          editBuffer->pointer.editLine      = currentEditLine;
          editBuffer->pointer.character     = currentChar;
          editBuffer->pointer.x             = x;
          editBuffer->pointer.y             = y;
         }
       }

      if (editBuffer->cursor.character   == currentChar)                        // Cursor location
       {editBuffer->cursor.tag            = currentTagOffset;
        editBuffer->cursor.positionInTag  = currentPositionInTag;
        editBuffer->cursor.editLine       = currentEditLine;
        editBuffer->cursor.x              = x;
        editBuffer->cursor.y              = y;
       }

      cairo_move_to(cr, x += width, y += 0);                                    // Position ready for the next character
     } // drawChar

    void drawString(char *s, size_t l, int openClose)                           // Draw a string
     {for(size_t i = 0; i < l; ++i)
       {const int d = (i == 0     && openClose == XmlOpen ) ||
                      (i == l - 1 && openClose == XmlClose) ? openClose : 0;
        drawChar(s[i], d);
       }
     }

    void open()                                                                 // Add open tag or text
     {currentPositionInTag = 0;                                                 // Position in tag
      currentTagNumber++;                                                       // Count string as a tag

      if (parent ▷ isText)                                                      // Text
       {textFont();
        drawString(parent ▷ tagString, parent ▷ tagStringLength, 0);
       }
      else if (parent ▷ empty)                                                  // Write tag with no children on the same line
       {makeLocalCopyOfXmlTagString(s, l, parent);
        openFont();
//      drawChar  (XmlOpen);
        drawString(s, l, XmlOpen);
//      drawChar  (XmlClose);
        currentTagNumber++;                                                     // Close tag
        closeFont();
//      drawChar  (XmlSlash);
        n ◁ parent ▷ tagName;
        drawString(n, strlen(n), XmlClose);
//      drawChar  (XmlClose);
       }
      else                                                                      // Opener
       {openFont();
        startNewLine(1);
//      drawChar  (XmlOpen);
        drawString(parent ▷ tagString, parent ▷ tagStringLength, XmlOpen);
//      drawChar  (XmlClose);
       }
     } // open

    void close()                                                                // Add close tag unless we are on text
     {currentPositionInTag = 0;                                                 // Position in tag
      if (parent ▷ isTag)
       {if (!parent ▷ empty)
         {currentTagNumber++;                                                   // Close tag
          if (!parent ▷ stayInLine)
           {startNewLine(1);
           }
          closeFont();
//        drawChar  (XmlSlash);
          drawString(XmltagName, strlen(parent ▷ tagName), XmlClose);
//        drawChar  (XmlClose);
         }
       }
     } // close

    open(); Xmlfe(child, parent) drawTagOrText(child, depth+1); close();        // Draw this level and its children
   }

  root    ◁ editBuffer->xml ▷ root;
  rootTag ◁ root ▷ first;
  drawTagOrText(rootTag, 0);                                                    // Start at the root tag

  if (1)                                                                        // Draw gutter between line numbers and text
   {g ◁ editGutter;
    lineNumberGutterTextFillColor();
    i ▶ rectangleWH(g.x, g.y, g ▷ width,   g ▷ height);
    lineNumberGutterTextStrokeColor();
    i ▶ rectangleWH(g.x, g.y, g ▷ width/2, g ▷ height);
   }

  if (editBuffer->blockIn)                                                      // Clear the blocked out area
   {blockedOutAreaBackgroundColor();
    b ◁ editBuffer->block;
    i ▶ rectangleWH(b.x, b.y, b ▷ width, b ▷ height);
   }

  return *editBuffer;                                                           // Return the updated edit buffer
 } // drawEditBuffer

static void maintainCursorPosition_$EditBuffer_$EditBuffer                      // Set the scroll amount of the altered edit buffer so that the position on the screen of the line containing the cursor is as close as possible to the position in the base edit buffer when the altered buffer is drawn in the place of the base buffer despite the altered buffer having been zoomed or having its width changed relative to the base buffer.  Both buffers should have been drawn before this operations (with measureOnly=1 if no drawing is required) so that the current position of the line containing the cursor is known in both buffers at the start of this operation. After this operation the altered buffer can be drawn in the area originally occupied by the base buffer while minimizing the amount the user must move their line of sight to track the cursor position.
 ($EditBuffer * base,                                                           // Base    $ edit buffer
  $EditBuffer * altered)                                                        // Altered $ edit buffer
 {b ◁ base->   cursor.y;                                                        // Location of cursor line of base $ edit buffer on display in pixels
  a ◁ altered->cursor.y;                                                        // Location of cursor line in altered $ edit buffer on display in pixels

  s ◁ (a - b) / altered->lineHeight + altered->scroll;                          // Amount we should scroll to minimize the apparent movement of the tag containing the cursor when we change font size or change the edit buffer width

  altered->scroll = nearbyint(s);
//altered->scroll = floor(s);
//altered->scroll = ceil(s);
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()
 {xml  ◁ "<a><b><c/><d><e/>ee<f/>ff<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>";
     x ◁ parseXmlFromString(xml, 0);
  ✓ !x ▷ errors;

  void draw(CairoTextImage * i)
   {i ▶ fontSize (40);
    line ◀ 0ul;

    void drawXml(const XmlTag tag, int start, int depth)
     {makeLocalCopyOfXmlTagString(t, l, tag);
      x ◁ i->fontHeight * depth;
      y ◁ i->fontHeight * line++;
      i ▶ text(x, y, t);
      if(0)start=start;
     }

    x ▷ scan(drawXml);
   }

  i ◀ makeCairoTextImage(draw, 2000, 2000, "$0.png", "deb8");
  i ▷ free;
 }

void test1()                                                                    //TdrawEditBuffer //TmaintainCursorPosition
 {xml  ◁ "<a><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j><B><C/><D><E/>EE<F/>1 2 3 4 5 6 7 8 9 0<G>GGG</G></D><H>HH HH</H></B><i/>i<j></j><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>";

     X ◁ parseXmlFromString(xml, 0);
  ✓ !X ▷ errors;

  void draw(CairoTextImage * const i)                                           // Draw the xml into an image
   {page ◁ makeRectangleWH(0, 0, i->width, i->height);
    wScroll ◁ 4; fontSize ◁ 100;                                                // Scroll amount in wide mode, font size of text in image

    ww ◁ page ▷ right(0);                                                       // Measure in wide mode to find the location of the pointer expected to be the middle G in GGG
    we ◀ new $EditBuffer(cti: i, xml: X, fontSize: fontSize, px: 715, py: 894, scroll: wScroll, zone: ww.a);
    wr ◀ we ▷ drawEditBuffer;
         i  ▶ saveAsPng("$1_wide.png", "a"); i ▶ clearWhite;

    wn ◁ X.tree ▷ offset(wr.pointer.tag);                                       // Pointer location in wide version
    ✓ wn ▷ equalsString("GGG");
    ✓ wr.pointer.positionInTag ==  2;
    ✓ wr.pointer.character     == 81;
    ✓ wr.pointer.editLine      == 12;

    nw ◁ page ▷ left(i->width * 4 / 8);                                         // Measure in narrow mode to find position of cursor as set by pointer in previous image
    ne ◀ new $EditBuffer(cti: i, xml: X, fontSize: fontSize * 9.0 / 8, cursor: wr.pointer, zone: nw.a);
    nr ◀ ne ▷ drawEditBuffer;
    i  ▶ saveAsPng("$1_narrow.png", "a"); i ▶ clearWhite;

    nn ◁ X.tree ▷ offset(nr.cursor.tag);                                        // Cursor location in narrow mode
    ✓ nn ▷ equalsString("GGG");
    ✓ nr.cursor.positionInTag == wr.pointer.positionInTag;
    ✓ nr.cursor.character     == wr.pointer.character;
    ✓ nr.cursor.editLine      == 14;

    wr.cursor = wr.pointer;                                                     // Simulate a click - the cursor position is set to match the pointer position
    wr ▷ maintainCursorPosition(&nr);                                           // Position the narrow display so that GGG is in much the same screen position as the wide display

    nr.measureOnly = 0;                                                         // Request draw of the edit buffer
    nR ◁ nr ▷ drawEditBuffer;                                                   // Draw scrolled edit buffer
//  i  ▷ save("$1_narrowScrolled.png", "8d73");

    nN ◁ X.tree ▷ offset(nR.cursor.tag);                                        // Cursor location in narrow mode
    ✓ nN ▷ equalsString("GGG");
    ✓ nR.cursor.positionInTag == wr.pointer.positionInTag;
    ✓ nR.cursor.character     == wr.pointer.character;
    ✓ nR.cursor.editLine      == nr.cursor.editLine;
   }

  i ◀ makeCairoTextImage(draw, 2000, 2000, "$1.png", "a");                      // Create image containing some text and check its digest
  i ▷ free;
 }

void test2()                                                                    // Block out an area
 {xml  ◁ "<a><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j><B><C/><D><E/>EE<F/>1 2 3 4 5 6 7 8 9 0<G>GGG</G></D><H>HH HH</H></B><i/>i<j></j><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>";

     X ◁ parseXmlFromString(xml, 0);
  ✓ !X ▷ errors;

  void draw(CairoTextImage * i)                                                 // Draw the xml into an image
   {page ◁ makeRectangleWH(0, 0, i->width, i->height);
    fontSize ◁ 100;                                                             // Font size of text in image

    b ◁ makeRectangleWH(500, 500, 1000, 1000);                                  // Block out this area
    e ◀ new $EditBuffer(cti: i, xml: X, fontSize: fontSize, zone: page, block: b, blockIn: 1);
    e ▷ drawEditBuffer;
   }

  i ◀ makeCairoTextImage(draw, 2000, 2000, "$2.png", "a");                      // Create image containing some text and check its digest
  i ▷ free;
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/xml/xml
