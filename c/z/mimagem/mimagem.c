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

typedef struct $EditBuffer                                                      // $ edit buffer
 {const struct ProtoTypes_$EditBuffer *proto;                                   // Prototypes for methods
  const XmlParse       xml;                                                     // Xml parse tree being editted
  const CairoTextImage cti;                                                     // Cairo text image that we are drawing into
  const size_t         scroll;                                                  // The number of edit lines we have scrolled down
  const size_t         fontSize;                                                // Font size to use in creating text
  const size_t         px;                                                      // Pointer pixel position in x
  const size_t         py;                                                      // Pointer pixel position in y
        size_t         pointerTag;                                              // The tag containing the pointer
        size_t         pointerPositionInTag;                                    // The character offset of the pointer in the tag containing the pointer
        size_t         pointerEditLine;                                         // The edit line containing the pointer
 } $EditBuffer;

#include <$$_prototypes.h>

static $EditBuffer drawEditBuffer_$EditBuffer_$EditBuffer                       // Draw the edit buffer and return th location of the pointer
 ($EditBuffer editBuffer)                                                       // $ edit buffer
 { i ◁ editBuffer.cti;                                                          // Cairo contest to draw in
  cr ◁ i.cr;                                                                    // Cairo contest to draw in

  cairo_set_font_size (cr, editBuffer.fontSize);                                // Cairo
  cairo_font_extents_t fontExtents;
  cairo_font_extents  (cr, &fontExtents);
  cairo_text_extents_t textExtents;

  H ◁ fontExtents.height;                                                       // Font details
  scrollPixels ◁ H * editBuffer.scroll;                                         // Number of pixels scrolled down

  page ◁ makeRectangleWH(0, 0, i.width, i.height);
  editZoneAndStatus ◁ page ▷ right(0);
  editLineNumbersAndText   ◁ editZoneAndStatus.a ▷ left(H);
  editLineNumbers          ◁ editLineNumbersAndText.a;
  editText                 ◁ editLineNumbersAndText.b;

  void drawRectangle(Rectangle r)
   {cairo_rectangle(cr, r.x, r.y, r ▷ width, r ▷ height);
   }

  cairo_set_source_rgb(cr, 1, 0.9, 0.9);                                        // Shade line numbers
  drawRectangle(editLineNumbers);
  cairo_fill(cr);

  size_t currentTagOffset = 0, currentTagNumber = -1, currentPositionInTag = 0; // Current tag and position within current tag
  double x = editText.x, y = editText.y - scrollPixels;                         // Initial text position
  cairo_move_to(cr, x, y - scrollPixels);                                       // Adjust for scrolling

  void drawTag(const XmlTag parent, const int depth)                            // Print the specified parent and its children
   {currentTagOffset = parent.node.offset;                                      // In case the pointer is located in this tag

    void startNewLine()                                                         // Move to next line
     {cairo_move_to       (cr, editLineNumbers.x, y + H);
      cairo_set_source_rgb(cr, 0, 0, 0);                                        // Color for line number
      lsprintf(n, 1024, "%lu", ++currentTagNumber);                             // Format line number
      cairo_show_text     (cr, n);                                              // Write char
      cairo_move_to       (cr, x = editText.x, y += H);
     }

    void drawChar(char c)                                                       // Draw a character
     {char s[2] = {c, 0};
      cairo_text_extents(cr, s, &textExtents);
      if (!editText ▷ containsPoint(x + textExtents.x_advance, y))
       {startNewLine();
       }

      cairo_show_text   (cr, s);                                                // Write char
      cairo_text_extents(cr, s, &textExtents);

      ++currentPositionInTag;                                                   // Cursor location
      if (editBuffer.px >= x && editBuffer.px <= x + textExtents.x_advance &&
          editBuffer.py <= y && editBuffer.py >= y-H)
       {editBuffer.pointerTag           = currentTagOffset;
        editBuffer.pointerPositionInTag = currentPositionInTag;
       }

      cairo_move_to     (cr, x += textExtents.x_advance, y += 0);               // Position ready for the next character
     }

    void drawString(char *s, size_t l)                                          // Draw a string
     {for(size_t i = 0; i < l; ++i) drawChar(s[i]);
     }

    void open()                                                                 // Add open tag or text
     {currentPositionInTag = 0;                                                 // Position in tag
      if (parent ▷ isText)                                                      // Text
       {cairo_set_source_rgb(cr, 0, 0, 0);
        drawString(parent ▷ tagString, parent ▷ tagStringLength);
        currentTagNumber++;                                                     // Count string as a tag
       }
      else if (parent ▷ empty)                                                  // Write tag with no children on the same line
       {cairo_set_source_rgb(cr, 1, 0, 0);
        makeLocalCopyOfXmlTagString(s, l, parent);
        drawChar  (XmlOpen);
        drawString(s+1, l-2);
        drawChar  (XmlClose);
        drawChar  (XmlOpen);
        drawChar  (XmlSlash);
        n ◁ parent ▷ tagName;
        drawString(n, strlen(n));
        drawChar  (XmlClose);
        currentTagNumber += 2;                                                  // Count open and close empty non text tag
       }
      else                                                                      // Opener
       {cairo_set_source_rgb(cr, 0, 0, 1);
        startNewLine();   cairo_move_to(cr, x += H * depth, y);
        drawString(parent ▷ tagString, parent ▷ tagStringLength);
       }
     }

    void close()                                                                // Add close tag unless we are on text
     {currentPositionInTag = 0;                                                 // Position in tag
      if (parent ▷ isTag)
       {if (!parent ▷ empty)
         {if (!parent ▷ stayInLine)
           {startNewLine(); cairo_move_to(cr, x += H * depth, y);
           }
          else currentTagNumber++;                                              // Count inline closing tag
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

  return editBuffer;                                                            // Return the updted edit buffer
 } // drawEditBuffer
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

  i ◁ createCairoTextImage(draw, 2000, 2000, "$1.png", "a");                    // Create image containing some text and check its digest
  i ▷ free;
 }

void test1()                                                                    //TdrawEditBuffer
 {xml  ◁ "<a><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j><B><C/><D><E/>EE<F/>1 2 3 4 5 6 7 8 9 0<G>GGG</G></D><H>HH HH</H></B><i/>i<j></j><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>";

     X ◁ parseXmlFromString(xml, 0);
  ✓ !X ▷ errors;


  void draw(CairoTextImage i)                                                   // Draw the xml into an image
   {e ◁ new $EditBuffer(cti: i, xml: X, fontSize: 100, px: 1066, py: 381, scroll: 800);

    r ◁ e ▷ drawEditBuffer;

    n ◁ X.tree ▷ nodeFromOffset(r.pointerTag);                                  // Pointer location
    ✓ n ▷ equalsString("ee");
    ✓ r.pointerPositionInTag == 1;
   }

  i ◁ createCairoTextImage(draw, 2000, 2000, "$2.png", "a");                    // Create image containing some text and check its digest
  i ▷ free;
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/xml/xml
