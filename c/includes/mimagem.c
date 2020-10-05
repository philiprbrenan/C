#line 1 "/home/phil/c/z/mimagem/mimagem.c"
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

#ifndef Mimagem_included
#define Mimagem_included

//D1 Structures                                                                 // Structures describing an Arena Tree.

typedef struct MimagemEditPosition                                                    // A position in a Mimagem edit buffer
 {const struct ProtoTypes_MimagemEditPosition *proto;                                 // Prototypes for methods
  size_t         character;                                                     // The number of the character at the position
  size_t         tag;                                                           // The tag containing at the position
  size_t         positionInTag;                                                 // The character offset of the pointer in the tag at the position
  size_t         editLine;                                                      // The edit line containing the position
 } MimagemEditPosition;

typedef struct MimagemEditBuffer                                                      // Mimagem edit buffer
 {const struct ProtoTypes_MimagemEditBuffer *proto;                                   // Prototypes for methods
  XmlParse       xml;                                                           // Xml parse tree being editted
  CairoTextImage cti;                                                           // Cairo text image that we are drawing into
  Rectangle      zone;                                                          // The rectangle in which the edit buffer will be drawn
  int            measureOnly;                                                   // Suppresses drawing if true.  All other operations are performed so that returned measurements of the pointer and cursor position are accurate.
  double         lineHeight;                                                    // The distance between lines for the specified font size.
  size_t         scroll;                                                        // The number of edit lines we have scrolled down
  size_t         fontSize;                                                      // Font size to use in creating text
  size_t         px;                                                            // Pointer pixel position in x
  size_t         py;                                                            // Pointer pixel position in y
  MimagemEditPosition  pointer;                                                       // Position of the pointer in Mimagem edit buffer
  MimagemEditPosition  cursor;                                                        // Position of the cursor in Mimagem edit buffer
 } MimagemEditBuffer;

#include <mimagem_prototypes.h>

static MimagemEditBuffer drawEditBuffer_MimagemEditBuffer_MimagemEditBuffer                       // Draw the edit buffer and return the location of the pointer and cursor
 (MimagemEditBuffer editBuffer)                                                       // Mimagem edit buffer
 {const typeof(editBuffer.cti) i = editBuffer.cti;                                                        // Image description
  const typeof(i.cr) cr = i.cr;                                                                  // Cairo context to draw in
  const typeof(!editBuffer.measureOnly) draw = !editBuffer.measureOnly;                                               // Draw unless we only want to measure

  const typeof(makeColourPale()) paleColours = makeColourPale();                                               // Backgrfound colours for each tag by depth with text getting the same colour as its parent.

  cairo_set_font_size (cr, editBuffer.fontSize);                                // Cairo
  cairo_font_extents_t fontExtents;
  cairo_font_extents  (cr, &fontExtents);
  cairo_text_extents_t textExtents;

//const typeof(fontExtents.height) H = fontExtents.height;                                                       // Font details
  const typeof(editBuffer.fontSize) H = editBuffer.fontSize;                                                      // Font details
  editBuffer.lineHeight = H;                                                    // Record line height
  const typeof(H * editBuffer.scroll) scrollPixels = H * editBuffer.scroll;                                         // Number of pixels scrolled down

  const typeof(editBuffer.zone.proto->left(editBuffer.zone, H * 3 / 2)) editLineNumbersAndText = editBuffer.zone.proto->left(editBuffer.zone, H * 3 / 2);                 // Split the drawing area into line numbers and text
  const typeof(editLineNumbersAndText.a) editLineNumbers = editLineNumbersAndText.a;                          // Line numbers
  const typeof(editLineNumbersAndText.b) editText = editLineNumbersAndText.b;                          // Text

  void drawRectangle(Rectangle r)
   {if (draw) cairo_rectangle(cr, r.x, r.y, r.proto->width(r), r.proto->height(r));
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

    const typeof(paleColours.p[(depth - (parent.proto->isText(parent) ? 1 : 0)) % paleColours.N]) backgroundColour = paleColours.p[(depth - (parent.proto->isText(parent) ? 1 : 0)) % paleColours.N]; // Choose the back ground colour for this depth and tag

    void        openColor() {cairo_set_source_rgb(cr, 0,   0,   0.4);}          // Color for opening tag
    void       closeColor() {cairo_set_source_rgb(cr, 0,   0,   0.4);}          // Color for closing tag
    void        textColor() {cairo_set_source_rgb(cr, 0,   0,   0  );}          // Color for text
    void  lineNumberColor() {cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);}          // Color for line numbers

    void         openFont() {i.proto->setFont(i, i.sansBold);       openColor();}       // Font  for opening tag
    void        closeFont() {i.proto->setFont(i, i.sans);          closeColor();}       // Font  for closing tag
    void         textFont() {i.proto->setFont(i, i.serif);          textColor();}       // Font  for text
    void   lineNumberFont() {i.proto->setFont(i, i.sansMono); lineNumberColor();}       // Font  for line numbers

    void startNewLine()                                                         // Move to next line
     {++currentEditLine;                                                        // Edit line in the edit buffer drawing zone
      if (draw)
       {lsprintf(n, 1024, "%lu", currentTagNumber);                             // Format line number
        cairo_save          (cr);
        cairo_translate     (cr, editLineNumbers.x, y + H);

        const typeof(backgroundColour) b = backgroundColour;                                                   // Background colour of line number
        cairo_set_source_rgb(cr, b.r, b.g, b.b);
        cairo_rectangle     (cr, 0, -H, editLineNumbers.proto->width(editLineNumbers), 0);
        cairo_fill          (cr);

        lineNumberFont      ();                                                 // Text of line number
        cairo_show_text     (cr, n);
        cairo_restore       (cr);
       }
      cairo_move_to         (cr, x = editText.x, y += H);                       // Position for first character of new line
     }

    void drawCharOrSymbol(char c, int modeDrawSymbol)                           // Draw a character or a symbol
     {char s[2] = {c, 0};                                                       // Character to be drawn as a string

      int measureCharOrSymbol()                                                 // Measure the width of a character or a symbol
       {if (modeDrawSymbol) return H / 8;                                       // Universal width for symbols
        cairo_text_extents(cr, s, &textExtents);                                // Measure text containing one char
        return textExtents.x_advance;
       }

      const typeof(measureCharOrSymbol()) width = measureCharOrSymbol();                                            // Measure character or symbol

      void drawBackGroundForCharOrSymbol()                                      // Draw the background for the current character or symbol
       {const typeof(backgroundColour) b = backgroundColour;
        cairo_save          (cr);
        cairo_set_source_rgb(cr, b.r, b.g, b.b);
        cairo_rectangle     (cr, x, y - H, width, H);
        cairo_fill          (cr);
        cairo_restore       (cr);
       }

      void drawChar()                                                           // Draw character
       {cairo_move_to  (cr, x, y);
        cairo_show_text(cr, s);
       }

      void drawSymbol()                                                         // Draw symbol
       {Rectangle r = makeRectangleWH(x, y - H*3.0/8, width, H/4);              // Rectangle containing symbol to be drawn
        Colour    s = makeColour(1,0,0,1);
        Colour    f = makeColour(0,0,1,1);
        switch(c)
         {case XmlOpen:  i.proto->leftArrow(i, r, s, f); return;
          case XmlClose: i.proto->rightArrow(i, r, f, s); return;
          case XmlSlash: i.proto->leftArrowWithCircle(i, r, s, f); return;
         }
       }

      if (x + width >= editText.X) startNewLine();                              // Start a new line if the draw would be off the end of the line

      if (draw)                                                                 // Draw character or symbol if drawing
       {drawBackGroundForCharOrSymbol();                                        // Background
        modeDrawSymbol ? drawSymbol() : drawChar();                             // Draw character or symbol
       }

      ++currentPositionInTag; ++currentChar;                                    // Pointer and cursor location
      if (editBuffer.px >= x && editBuffer.px <= x + width &&                   // Pointer location in x
          editBuffer.py <= y && editBuffer.py >= y - H)                         // Line containing pointer
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

      cairo_move_to(cr, x += width, y += 0);                                    // Position ready for the next character
     } // drawCharOrSymbol

    void drawString(char *s, size_t l)                                          // Draw a string
     {for(size_t i = 0; i < l; ++i) drawCharOrSymbol(s[i], 0);
     }

    void open()                                                                 // Add open tag or text
     {currentPositionInTag = 0;                                                 // Position in tag
      if (parent.proto->isText(parent))                                                      // Text
       {currentTagNumber++;                                                     // Count string as a tag
        textFont();
        drawString(parent.proto->tagString(parent), parent.proto->tagStringLength(parent));
       }
      else if (parent.proto->empty(parent))                                                  // Write tag with no children on the same line
       {currentTagNumber++;                                                     // Open tag
        makeLocalCopyOfXmlTagString(s, l, parent);
        openFont();
        drawCharOrSymbol  (XmlOpen,  1);
        drawString(s+1, l-2);
        drawCharOrSymbol  (XmlClose, 1);
        currentTagNumber++;                                                     // Close tag
        closeFont();
//      drawCharOrSymbol  (XmlOpen,  1);
        drawCharOrSymbol  (XmlSlash, 1);
        const typeof(parent.proto->tagName(parent)) n = parent.proto->tagName(parent);
        drawString(n, strlen(n));
        drawCharOrSymbol  (XmlClose, 1);
       }
      else                                                                      // Opener
       {currentTagNumber++;                                                     // Open tag
        openFont();
        startNewLine();   cairo_move_to(cr, x += H * depth, y);
        drawCharOrSymbol  (XmlOpen,  1);
        drawString(parent.proto->tagString(parent) + 1, parent.proto->tagStringLength(parent) - 2);
        drawCharOrSymbol  (XmlClose, 1);
       }
     } // open

    void close()                                                                // Add close tag unless we are on text
     {currentPositionInTag = 0;                                                 // Position in tag
      if (parent.proto->isTag(parent))
       {if (!parent.proto->empty(parent))
         {currentTagNumber++;                                                   // Close tag
          if (!parent.proto->stayInLine(parent))
           {startNewLine(); cairo_move_to(cr, x += H * depth, y);
           }
          closeFont();
        //drawCharOrSymbol  (XmlOpen,  1);
          drawCharOrSymbol  (XmlSlash, 1);
          drawString(XmltagName, strlen(parent.proto->tagName(parent)));
          drawCharOrSymbol  (XmlClose, 1);
         }
       }
     } // close

    open(); Xmlfe(child, parent) drawTag(child, depth+1); close();              // Draw this level and its children
   }

  drawTag(editBuffer.xml.proto->root(editBuffer.xml), -1);                                           // Start at the root

  return editBuffer;                                                            // Return the updated edit buffer
 } // drawEditBuffer

static void maintainCursorPosition_MimagemEditBuffer_MimagemEditBuffer                      // Set the scroll amount of the altered edit buffer so that the position on the screen of the line containing the cursor is as close as possible to the position in the base edit buffer when the altered buffer is drawn in the place of the base buffer despite the altered buffer having been zoomed or having its width changed relative to the base buffer.  Both buffers should have been drawn before this operations (with measureOnly=1 if no drawing is required) so that the current position of the line containing the cursor is known in both buffers at the start of this operation. After this operation the altered buffer can be drawn in the area originally occupied by the base buffer while minimizing the amount the user must move their line of sight to track the cursor position.
 (MimagemEditBuffer   base,                                                           // Base    Mimagem edit buffer
  MimagemEditBuffer * altered)                                                        // Altered Mimagem edit buffer
 {const typeof((base   . cursor.editLine - base   . scroll) * base   . lineHeight) b = (base   . cursor.editLine - base   . scroll) * base   . lineHeight;       // Location of cursor line of base Mimagem edit buffer on display in pixels
  const typeof((altered->cursor.editLine - altered->scroll) * altered->lineHeight) a = (altered->cursor.editLine - altered->scroll) * altered->lineHeight;       // Location of cursor lkne in altered Mimagem edit buffer on display in pixels
  const typeof((a - b) / altered->lineHeight + altered->scroll) s = (a - b) / altered->lineHeight + altered->scroll;                          // Amount we should scroll to minimize the apparent movement of the tag containing the cursor when we change font size or change the edit buffer width
  altered->scroll = nearbyint(s);                                               // Works well with no change in font size
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()
 {const typeof("<a><b><c/><d><e/>ee<f/>ff<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>") xml = "<a><b><c/><d><e/>ee<f/>ff<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>";
     const typeof(parseXmlFromString(xml, 0)) x = parseXmlFromString(xml, 0);
  assert( !x.proto->errors(x));

  void draw(CairoTextImage i)
   {typeof(i.cr) cr = i.cr;
    cairo_set_font_size (cr, 40);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_font_extents_t fontExtents;
    cairo_font_extents  (cr, &fontExtents);

    typeof(0ul) line = 0ul;

    void drawXml(const XmlTag tag, int start, int depth)
     {makeLocalCopyOfXmlTagString(t, l, tag);
      cairo_move_to     (cr, fontExtents.max_x_advance * depth,
                    line++ * fontExtents.height);
      cairo_show_text   (cr, t);
      if(0)start=start;
     }

    x.proto->scan(x, drawXml);
   }

  const typeof(makeCairoTextImage(draw, 2000, 2000, "Mimagem1.png", "49ad")) i = makeCairoTextImage(draw, 2000, 2000, "Mimagem1.png", "49ad");
  i.proto->free(i);
 }

void test1()                                                                    //TdrawEditBuffer //TmaintainCursorPosition
 {const typeof("<a><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j><B><C/><D><E/>EE<F/>1 2 3 4 5 6 7 8 9 0<G>GGG</G></D><H>HH HH</H></B><i/>i<j></j><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>") xml = "<a><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j><B><C/><D><E/>EE<F/>1 2 3 4 5 6 7 8 9 0<G>GGG</G></D><H>HH HH</H></B><i/>i<j></j><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>";

     const typeof(parseXmlFromString(xml, 0)) X = parseXmlFromString(xml, 0);
  assert( !X.proto->errors(X));

  void draw(CairoTextImage i)                                                   // Draw the xml into an image
   {const typeof(makeRectangleWH(0, 0, i.width, i.height)) page = makeRectangleWH(0, 0, i.width, i.height);
    const typeof(4) wScroll = 4; const typeof(100) fontSize = 100;                                                // Scroll amount in wide mode, font size of text in image

    const typeof(page.proto->right(page, 0)) ww = page.proto->right(page, 0);                                                       // Measure in wide mode to find the location of the pointer expected to be the middle G in GGG
    const typeof(newMimagemEditBuffer(({struct MimagemEditBuffer t = {cti: i, xml: X, fontSize: fontSize, px: 660, py: 660, scroll: wScroll, zone: ww.a, proto: &ProtoTypes_MimagemEditBuffer}; t;}))) we = newMimagemEditBuffer(({struct MimagemEditBuffer t = {cti: i, xml: X, fontSize: fontSize, px: 660, py: 660, scroll: wScroll, zone: ww.a, proto: &ProtoTypes_MimagemEditBuffer}; t;}));
    typeof(we.proto->drawEditBuffer(we)) wr = we.proto->drawEditBuffer(we);
         i.proto->save(i, "Mimagem1_wide.png", "a"); i.proto->clearWhite(i);

    const typeof(X.tree.proto->nodeFromOffset(X.tree, wr.pointer.tag)) wn = X.tree.proto->nodeFromOffset(X.tree, wr.pointer.tag);                               // Pointer location in wide version
    assert( wn.proto->equalsString(wn, "GGG"));
    assert( wr.pointer.positionInTag ==  2);
    assert( wr.pointer.character     == 137);
    assert( wr.pointer.editLine      == 12);

    const typeof(page.proto->left(page, i.width * 4 / 8)) nw = page.proto->left(page, i.width * 4 / 8);                                          // Measure in narrow mode to find position of cursor as set by pointer in previous image
    typeof(newMimagemEditBuffer(({struct MimagemEditBuffer t = {cti: i, xml: X, fontSize: fontSize * 9.0 / 8, cursor: wr.pointer, zone: nw.a, proto: &ProtoTypes_MimagemEditBuffer}; t;}))) ne = newMimagemEditBuffer(({struct MimagemEditBuffer t = {cti: i, xml: X, fontSize: fontSize * 9.0 / 8, cursor: wr.pointer, zone: nw.a, proto: &ProtoTypes_MimagemEditBuffer}; t;}));
    typeof(ne.proto->drawEditBuffer(ne)) nr = ne.proto->drawEditBuffer(ne);
    i.proto->save(i, "Mimagem1_narrow.png", "a"); i.proto->clearWhite(i);

    const typeof(X.tree.proto->nodeFromOffset(X.tree, nr.cursor.tag)) nn = X.tree.proto->nodeFromOffset(X.tree, nr.cursor.tag);                                // Cursor location in narrow mode
    assert( nn.proto->equalsString(nn, "GGG"));
    assert( nr.cursor.positionInTag == wr.pointer.positionInTag);
    assert( nr.cursor.character     == wr.pointer.character);
//    assert( nr.cursor.editLine      == 20);                                            // New font size

    wr.cursor = wr.pointer;                                                     // Simulate a click - the cursor position is set to match the pointer position
    wr.proto->maintainCursorPosition(wr, &nr);                                           // Position the narrow display so that GGG is in much the same screen position as the wide display

    nr.measureOnly = 0;                                                         // Request draw of the edit buffer
    const typeof(nr.proto->drawEditBuffer(nr)) nR = nr.proto->drawEditBuffer(nr);                                                   // Draw scrolled edit buffer
    i.proto->save(i, "Mimagem1_narrowScrolled.png", "a");

    const typeof(X.tree.proto->nodeFromOffset(X.tree, nR.cursor.tag)) nN = X.tree.proto->nodeFromOffset(X.tree, nR.cursor.tag);                                // Cursor location in narrow mode
    assert( nN.proto->equalsString(nN, "GGG"));
    assert( nR.cursor.positionInTag == wr.pointer.positionInTag);
    assert( nR.cursor.character     == wr.pointer.character);
    assert( nR.cursor.editLine      == nr.cursor.editLine);
   }

  const typeof(makeCairoTextImage(draw, 2000, 2000, "Mimagem2.png", "a")) i = makeCairoTextImage(draw, 2000, 2000, "Mimagem2.png", "a");                      // Create image containing some text and check its digest
  i.proto->free(i);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, 0};
  run_tests("Mimagem", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/xml/xml
