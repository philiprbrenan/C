//------------------------------------------------------------------------------
// Edit Xml representing Dita.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
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
  size_t         positionInTag;                                                 // The character offset of the pointer in the tag at the position
  size_t         editLine;                                                      // The edit line containing the position
 } $EditPosition;

typedef struct $EditBuffer                                                      // $ edit buffer
 {const struct ProtoTypes_$EditBuffer *proto;                                   // Prototypes for methods
  XmlParse       xml;                                                           // Xml parse tree being editted
  CairoTextImage cti;                                                           // Cairo text image that we are drawing into
  Rectangle      zone;                                                          // The rectangle in which the edit buffer will be drawn
  int            measureOnly;                                                   // Suppresses drawing if true.  All other operations are performed so that returned measurements of the pointer and cursor position are accurate.
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
 ($EditBuffer editBuffer)                                                       // $ edit buffer
 {i    ◁ editBuffer.cti;                                                        // Image description
  cr   ◁ i.cr;                                                                  // Cairo context to draw in
  draw ◁ !editBuffer.measureOnly;                                               // Draw unless we only want to measure

  cairo_set_font_size (cr, editBuffer.fontSize);                                // Cairo
  cairo_font_extents_t fontExtents;
  cairo_font_extents  (cr, &fontExtents);
  cairo_text_extents_t textExtents;

  H ◁ fontExtents.height;                                                       // Font details
  editBuffer.lineHeight = H;                                                    // Record line height
  scrollPixels ◁ H * editBuffer.scroll;                                         // Number of pixels scrolled down

  editLineNumbersAndText   ◁ editBuffer.zone ▷ left(H);                         // Split the drawing area into line numbers and text
  editLineNumbers          ◁ editLineNumbersAndText.a;                          // Line numbers
  editText                 ◁ editLineNumbersAndText.b;                          // Text

  void drawRectangle(Rectangle r)
   {if (draw) cairo_rectangle(cr, r.x, r.y, r ▷ width, r ▷ height);
   }

  cairo_set_source_rgb(cr, 1, 0.9, 0.9);                                        // Shade line numbers
  drawRectangle(editLineNumbers);
  cairo_fill(cr);

  size_t currentTagOffset = 0, currentTagNumber = -1, currentPositionInTag = 0, // Current tag and position within current tag
         currentChar      = 0, currentEditLine  =  1;                           // Current character number counted over all tags drawn, current edit line
  double x = editText.x, y = editText.y - scrollPixels;                         // Initial text position
  cairo_move_to(cr,   x, y);

  void drawTag(const XmlTag parent, const int depth)                            // Print the specified parent and its children
   {currentTagOffset = parent.node.offset;                                      // In case the pointer is located in this tag

    void startNewLine()                                                         // Move to next line
     {++currentEditLine;                                                        // Edit line in the edit buffer drawing zone
      cairo_move_to       (cr, editLineNumbers.x, y + H);
      cairo_set_source_rgb(cr, 0, 0, 0);                                        // Color for line number
      if (draw)
       {lsprintf(n, 1024, "%lu", currentTagNumber);                             // Format line number
        cairo_show_text   (cr, n);                                              // Write char
       }
      cairo_move_to       (cr, x = editText.x, y += H);
     }

    void drawChar(char c)                                                       // Draw a character
     {char s[2] = {c, 0};

      cairo_text_extents(cr, s, &textExtents);
      if (x >= editText.X) startNewLine();                                      // The draw would be off the end of the line
      if (draw) cairo_show_text(cr, s);                                         // Write char
      cairo_text_extents       (cr, s, &textExtents);

      ++currentPositionInTag; ++currentChar;                                    // Pointer and cursor location
      if (editBuffer.px >= x && editBuffer.px <= x + textExtents.x_advance &&   // Pointer location
          editBuffer.py <= y && editBuffer.py >= y - H)
       {editBuffer.pointer.tag           = currentTagOffset;
        editBuffer.pointer.positionInTag = currentPositionInTag;
        editBuffer.pointer.editLine      = currentEditLine;
        editBuffer.pointer.character     = currentChar;
       }

      if (editBuffer.cursor.character   == currentChar)                         // Cursor location
       {editBuffer.cursor.tag            = currentTagOffset;
        editBuffer.cursor.positionInTag  = currentPositionInTag;
        editBuffer.cursor.editLine       = currentEditLine;
       }

      cairo_move_to(cr, x += textExtents.x_advance, y += 0);                    // Position ready for the next character
     }

    void drawString(char *s, size_t l)                                          // Draw a string
     {for(size_t i = 0; i < l; ++i) drawChar(s[i]);
     }

    void open()                                                                 // Add open tag or text
     {currentPositionInTag = 0;                                                 // Position in tag
      if (parent ▷ isText)                                                      // Text
       {currentTagNumber++;                                                     // Count string as a tag
        cairo_set_source_rgb(cr, 0, 0, 0);
        drawString(parent ▷ tagString, parent ▷ tagStringLength);
       }
      else if (parent ▷ empty)                                                  // Write tag with no children on the same line
       {currentTagNumber++;                                                     // Open tag
        cairo_set_source_rgb(cr, 1, 0, 0);
        makeLocalCopyOfXmlTagString(s, l, parent);
        drawChar  (XmlOpen);
        drawString(s+1, l-2);
        drawChar  (XmlClose);
        currentTagNumber++;                                                     // Close tag
        drawChar  (XmlOpen);
        drawChar  (XmlSlash);
        n ◁ parent ▷ tagName;
        drawString(n, strlen(n));
        drawChar  (XmlClose);
       }
      else                                                                      // Opener
       {currentTagNumber++;                                                     // Open tag
        cairo_set_source_rgb(cr, 0, 0, 1);
        startNewLine();   cairo_move_to(cr, x += H * depth, y);
        drawString(parent ▷ tagString, parent ▷ tagStringLength);
       }
     }

    void close()                                                                // Add close tag unless we are on text
     {currentPositionInTag = 0;                                                 // Position in tag
      if (parent ▷ isTag)
       {if (!parent ▷ empty)
         {currentTagNumber++;                                                   // Close tag
          if (!parent ▷ stayInLine)
           {startNewLine(); cairo_move_to(cr, x += H * depth, y);
           }
          cairo_set_source_rgb(cr, 0, 0, 1);
          drawChar  (XmlOpen);
          drawChar  (XmlSlash);
          drawString(XmltagName, strlen(parent ▷ tagName));
          drawChar  (XmlClose);
         }
       }
     }

    open(); Xmlfe(child, parent) drawTag(child, depth+1); close();              // Draw this level and its children
   }

  drawTag(editBuffer.xml ▷ root, -1);                                           // Start at the root

  return editBuffer;                                                            // Return the updated edit buffer
 } // drawEditBuffer

static void maintainCursorPosition_$EditBuffer_$EditBuffer                      // Set the scroll amount of the altered edit buffer so that the position on the screen of the line containing the cursor is as close as possible to the position in the base edit buffer when the altered buffer is drawn in the place of the base buffer despite the altered buffer having been zoomed or having its width changed relative to the base buffer.  Both buffers should have been drawn before this operations (with measureOnly=1 if no drawing is required) so that the current position of the line containing the cursor is known in both buffers at the start of this operation. After this operation the altered buffer can be drawn in the area originally occupied by the base buffer while minimizing the amount the user must move their line of sight to track the cursor position.
 ($EditBuffer   base,                                                           // Base    $ edit buffer
  $EditBuffer * altered)                                                        // Altered $ edit buffer
 {b ◁ (base   . cursor.editLine - base   . scroll) * base   . lineHeight;       // Location of cursor line of base $ edit buffer on display in pixels
  a ◁ (altered->cursor.editLine - altered->scroll) * altered->lineHeight;       // Location of cursor lkne in altered $ edit buffer on display in pixels
  s ◁ (a - b) / altered->lineHeight + altered->scroll;                          // Amount we should scroll to minimize the apparent movement of the tag containing the cursor when we change font size or change the edit buffer width
  altered->scroll = nearbyint(s);
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()
 {xml  ◁ "<a><b><c/><d><e/>ee<f/>ff<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>";
     x ◁ parseXmlFromString(xml, 0);
  ✓ !x ▷ errors;

  void draw(CairoTextImage i)
   {cr ◀ i.cr;
    cairo_set_font_size (cr, 40);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_font_extents_t fontExtents;
    cairo_font_extents  (cr, &fontExtents);

    line ◀ 0ul;

    void drawXml(const XmlTag tag, int start, int depth)
     {makeLocalCopyOfXmlTagString(t, l, tag);
      cairo_move_to     (cr, fontExtents.max_x_advance * depth,
                    line++ * fontExtents.height);
      cairo_show_text   (cr, t);
      if(0)start=start;
     }

    x ▷ scan(drawXml);
   }

  i ◁ makeCairoTextImage(draw, 2000, 2000, "$1.png", "49ad");
  i ▷ free;
 }

void test1()                                                                    //TdrawEditBuffer //TmaintainCursorPosition
 {xml  ◁ "<a><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j><B><C/><D><E/>EE<F/>1 2 3 4 5 6 7 8 9 0<G>GGG</G></D><H>HH HH</H></B><i/>i<j></j><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>";

     X ◁ parseXmlFromString(xml, 0);
  ✓ !X ▷ errors;

  void draw(CairoTextImage i)                                                   // Draw the xml into an image
   {page ◁ makeRectangleWH(0, 0, i.width, i.height);
    wScroll ◁ 4; fontSize ◁ 100;                                                // Scroll amount in wide mode, font size of text in image

    ww ◁ page ▷ right(0);                                                       // Measure in wide mode to find the location of the pointer expected to be the middle G in GGG
    we ◁ new $EditBuffer(cti: i, xml: X, fontSize: fontSize, px: 810, py: 1185, scroll: wScroll, zone: ww.a);
    wr ◀ we ▷ drawEditBuffer;
         i  ▷ save("$1_wide.png", "e2bfbf"); i ▷ clearWhite;

    wn ◁ X.tree ▷ nodeFromOffset(wr.pointer.tag);                                // Pointer location in wide version
    ✓ wn ▷ equalsString("GGG");
    ✓ wr.pointer.positionInTag ==  2;
    ✓ wr.pointer.character     == 149;
    ✓ wr.pointer.editLine      == 14;

    nw ◁ page ▷ left(i.width * 4 / 8);                                          // Measure in narrow mode to find position of cursor as set by pointer in previous image
    ne ◀ new $EditBuffer(cti: i, xml: X, fontSize: fontSize * 12 / 8, cursor: wr.pointer, zone: nw.a);
    nr ◀ ne ▷ drawEditBuffer;
         i  ▷ save("$1_narrow.png", "b5be"); i ▷ clearWhite;

    nn ◁ X.tree ▷ nodeFromOffset(nr.cursor.tag);                                // Cursor location in narrow mode
    ✓ nn ▷ equalsString("GGG");
    ✓ nr.cursor.positionInTag ==  wr.pointer.positionInTag;
    ✓ nr.cursor.character     ==  wr.pointer.character;
//  ✓ nr.cursor.editLine      == 18;                                            // Font size unchanged
    ✓ nr.cursor.editLine      == 27;                                            // New font size

    wr.cursor = wr.pointer;                                                     // Simulate a click - the cursor position is set to match the pointer position
    wr ▷ maintainCursorPosition(&nr);                                           // Position the narrow display so that GGG is in much the same screen position as the wide display

    nr.measureOnly = 0;                                                         // Request draw of the edit buffer
    nR ◁ nr ▷ drawEditBuffer;                                                   // Draw scrolled edit buffer
         i  ▷ save("$1_narrowScrolled.png", "6de8");

    nN ◁ X.tree ▷ nodeFromOffset(nR.cursor.tag);                                // Cursor location in narrow mode
    ✓ nN ▷ equalsString("GGG");
    ✓ nR.cursor.positionInTag ==  wr.pointer.positionInTag;
    ✓ nR.cursor.character     ==  wr.pointer.character;
    ✓ nR.cursor.editLine      == 27;                                            // In this particular case changing the font size did not change the edit line number of the tag containing the cursor.
   }

  i ◁ makeCairoTextImage(draw, 2000, 2000, "$2.png", "6de8");                   // Create image containing some text and check its digest
  i ▷ free;
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/xml/xml
