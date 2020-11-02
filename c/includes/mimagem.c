#line 1 "/home/phil/c/z/mimagem/mimagem.c"
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

#ifndef Mimagem_included
#define Mimagem_included

//D1 Structures                                                                 // Structures describing an Arena Tree.

typedef struct MimagemEditPosition                                                    // A position in a Mimagem edit buffer
 {const struct ProtoTypes_MimagemEditPosition *proto;                                 // Prototypes for methods
  size_t         character;                                                     // The number of the character
  size_t         tag;                                                           // The number of the tag
  size_t         positionInTag;                                                 // The character offset in the tag
  size_t         editLine;                                                      // The edit line containing the position
  size_t         x;                                                             // The x coordinate of the start of the position
  size_t         y;                                                             // The y coordinate of the start of the position
 } MimagemEditPosition;

typedef struct MimagemEditBuffer                                                      // Mimagem edit buffer
 {const struct ProtoTypes_MimagemEditBuffer *proto;                                   // Prototypes for methods
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
  MimagemEditPosition  pointer;                                                       // Position of the pointer in Mimagem edit buffer
  MimagemEditPosition  cursor;                                                        // Position of the cursor in Mimagem edit buffer
 } MimagemEditBuffer;

#include <mimagem_prototypes.h>

static void drawEditBuffer_MimagemEditBuffer_MimagemEditBuffer                              // Draw the edit buffer and return the location of the pointer and cursor
 (MimagemEditBuffer * editBuffer)                                                     // Mimagem edit buffer
 {typeof(editBuffer->cti) i = editBuffer->cti;                                                       // Image description
  const typeof(i->cr) cr = i->cr;                                                                 // Cairo context to draw in
  const typeof(!editBuffer->measureOnly) draw = !editBuffer->measureOnly;                                              // Draw unless we only want to measure
  typeof(ULONG_MAX) closestSoFar = ULONG_MAX;                                                     // Shortest distance so far to pointer
  const typeof(8) lineNumberGutterText = 8;                                                     // Pixels Gutter between line numbers and text

  const typeof(makeColourPale()) paleColours = makeColourPale();                                               // Background colours for each tag by depth with text getting the same colour as its parent.
  const typeof(paleColours.p2) lastBackGroundColourDrawn = paleColours.p2;                                   // Keep track of the last character background colour used so that we can match the line numbers

  void                openTagFillColor() {i->proto->rgb(i, 0,   0,   0.4);}              // Color for opening tag
  void               closeTagFillColor() {i->proto->rgb(i, 0,   0,   0.4);}              // Color for closing tag
  void                   textFillColor() {i->proto->rgb(i, 0,   0,   0  );}              // Color for text
  void             lineNumberFillColor() {i->proto->rgb(i, 0.3, 0.3, 0.3);}              // Color for line numbers
  void lineNumberGutterTextFillColor  () {i->proto->rgb(i, 0.3, 0.3, 0.3);}              // Fill   color for the gutter between the line numbers and the text being editted.
  void lineNumberGutterTextStrokeColor() {i->proto->rgb(i, 1,   1,   1  );}              // Stroke color for the gutter between the line numbers and the text being editted.
  void   blockedOutAreaBackgroundColor() {i->proto->rgb(i, 1,   1,   1  );}              // Fill color for the background of the blocked out area

  void       openFont() {i->proto->font(i, CairoTextFont_sansBold);    openTagFillColor();} // Font  for opening tag
  void      closeFont() {i->proto->font(i, CairoTextFont_sans);       closeTagFillColor();} // Font  for closing tag
  void       textFont() {i->proto->font(i, CairoTextFont_serif);          textFillColor();} // Font  for text
  void lineNumberFont() {i->proto->font(i, CairoTextFont_sansMono); lineNumberFillColor();} // Font  for line numbers

  i->proto->fontSize(i, editBuffer->fontSize);                                           // Cairo font
  editBuffer->lineHeight = i->fontHeight;                                       // Record line height
  const typeof(i->fontHeight * editBuffer->scroll) scrollPixels = i->fontHeight * editBuffer->scroll;                            // Number of pixels scrolled down

  size_t getLineNumberWidth()                                                   // Width of line numbers
   {const typeof(8ul) N = 8ul; const typeof(editBuffer->xml.proto->count(&editBuffer->xml)) n = editBuffer->xml.proto->count(&editBuffer->xml);                                       // Maximum width we will allow
    char z[2] = {'0', 0};                                                       // A string of one zero
    const typeof(i->proto->textAdvance(i, z)) a = i->proto->textAdvance(i, z);                                                     // Width of string of one zero
    typeof(1ul) t = 1ul;                                                                    // Power of ten
    for(size_t i = 1; i <= N; ++i)                                              // Find power of ten neede to allow all tag numbers
     {t *= 10ul;                                                                // Next power
      if (t > n) return i * a;                                                  // Big enough
     }
    return a * N;                                                               // Use default in extremis
   }

  const typeof(getLineNumberWidth()) lineNumberWidth = getLineNumberWidth();                                   // Width of line numbers
  const typeof(editBuffer->zone.proto->left(&editBuffer->zone, lineNumberWidth)) editLineNumbersText = editBuffer->zone.proto->left(&editBuffer->zone, lineNumberWidth);          // Split the drawing area into line numbers and text
  const typeof(editLineNumbersText.a) editLineNumbers = editLineNumbersText.a;                                  // Line numbers
  const typeof(editLineNumbersText.b.proto->left(&editLineNumbersText.b, lineNumberGutterText)) editGutterAndText = editLineNumbersText.b.proto->left(&editLineNumbersText.b, lineNumberGutterText);     // Split the drawing area into line numbers and text
  const typeof(editGutterAndText.a) editGutter = editGutterAndText.a;                                    // Gutter just before text
  const typeof(editGutterAndText.b) editText = editGutterAndText.b;                                    // Text

  size_t currentTagOffset = 0, currentTagNumber =  0, currentPositionInTag = 0, // Current tag and position within current tag
         currentChar      = 0, currentEditLine  =  1;                           // Current character number counted over all tags drawn, current edit line

  double x = editText.x, y = editText.y - scrollPixels - i->fontHeight;         // Initial text position allowing for the new line operation which always occurs first.

  void drawTagOrText(XmlTag parent, int depth)                                  // Print the specified parent and its children
   {currentTagOffset = parent.node.offset;                                      // In case the pointer is located in this tag

    const typeof(parent.proto->isText(&parent)) t = parent.proto->isText(&parent); const typeof(paleColours.N) pcN = paleColours.N;
    const typeof(paleColours.p[(abs(depth - (t ? 1 : 0))) % pcN]) backgroundColour = paleColours.p[(abs(depth - (t ? 1 : 0))) % pcN];        // Choose the back ground colour for this depth and tag
    const typeof(paleColours.p[(abs(depth - (t ? 2 : 1))) % pcN]) backgroundColour1 = paleColours.p[(abs(depth - (t ? 2 : 1))) % pcN];        // Background colour for previous layer

    void startNewLine(int indent)                                               // Move to next line and indent if requested
     {++currentEditLine;                                                        // Edit line in the edit buffer drawing zone
      const typeof((indent ? i->fontHeight * depth : 0)) dx = (indent ? i->fontHeight * depth : 0);                                // Requested indentation

      if (draw)                                                                 // Finish current line
       {i->proto->save(i);
        const typeof(makeRectangleWH(x, y, editText.X - x, i->fontHeight)) r = makeRectangleWH(x, y, editText.X - x, i->fontHeight);               // Rectangle for line number
        i->proto->rectangle(i, r, lastBackGroundColourDrawn);                            // Background for line number
        i->proto->restore(i);
       }

      y += i->fontHeight;                                                       // Y coordinate of top of new line

      if (draw)
       {lsprintf(n, 1024, "%lu", currentTagNumber);                             // Format line number
        i->proto->save(i);

        const typeof(lastBackGroundColourDrawn) b = lastBackGroundColourDrawn;                                          // Background colour of line number
        i->proto->colour(i, b);
        cairo_rectangle     (cr, editLineNumbers.x, y,                          // Background for line number
                                 lineNumberWidth,   i->fontHeight);
        if (indent)
         {cairo_rectangle   (cr, editText.x, y, dx, i->fontHeight);             // Indentation for text
         }
        cairo_fill          (cr);

        lineNumberFont      ();                                                 // Text of line number
        const typeof(i->proto->textAdvance(i, n)) a = i->proto->textAdvance(i, n);                                                // Width of this line number
        const typeof(editLineNumbers.x + lineNumberWidth - a) tx = editLineNumbers.x + lineNumberWidth - a;
        i->proto->text(i, tx, y + i->fontAscent, n);
        i->proto->restore(i);
       }
      i->proto->move(i, x = editText.x + dx, y);                                         // Position for first character of new line
     }

    void drawChar(char c, int openClose)                                        // Draw a character at the current (x,y) position and advance the currrent position to the end of the character drawn. A gradient background is drawn for the first/last letters of a tag (0: no gradient, 1: opening gradient, 2: closing gradient)
     {char s[2] = {c, 0};                                                       // Character to be drawn as a string

      const typeof(i->proto->textAdvance(i, s)) width = i->proto->textAdvance(i, s);                                               // Measure character

      void drawBackGroundForChar()                                              // Draw the background for the current character
       {const typeof(backgroundColour) b = backgroundColour;
        const typeof(backgroundColour1) B = backgroundColour1;
        const typeof(width) w = width; const typeof(i->fontHeight) h = i->fontHeight;

        i->proto->save(i);
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
        i->proto->restore(i);
       }

      const typeof(makeRectangleWH(x, y, width, i->fontHeight)) charR = makeRectangleWH(x, y, width, i->fontHeight);                      // Rectangle occupied by current character

      if (editBuffer->blockIn && editBuffer->block.proto->containsACorner(&editBuffer->block, charR))    // Skip over blocked out area
       {const typeof(lastBackGroundColourDrawn) b = lastBackGroundColourDrawn;                                          // Background colour of line number
        i->proto->save(i);
        const typeof(makeRectangleWH(x, y, editBuffer->block.X - x, i->fontHeight)) r = makeRectangleWH(x, y, editBuffer->block.X - x, i->fontHeight);      // Rectangle for line number
        i->proto->rectangle(i, r, b);
        i->proto->restore(i);
        x = editBuffer->block.X;                                                // Restart after the blocked out area
       }

      if (x + width >= editText.X) startNewLine(0);                             // Start a new line if the draw would be off the end of the line

      if (draw && editText.proto->containsACorner(&editText, charR))                            // Draw character or symbol if drawing and we are in the drawing area
       {drawBackGroundForChar();                                                // Background
        i->proto->text(i, x, y, s);
       }

      ++currentPositionInTag; ++currentChar;                                    // Pointer and cursor location
      if (editBuffer->py <= y + i->fontHeight && editBuffer->py >= y)           // Line containing pointer
       {const typeof(fabs(editBuffer->px - x - width / 2)) d = fabs(editBuffer->px - x - width / 2);                               // Distance from the center of the current character to pointer.
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

      if (parent.proto->isText(&parent))                                                      // Text
       {textFont();
        drawString(parent.proto->tagString(&parent), parent.proto->tagStringLength(&parent), 0);
       }
      else if (parent.proto->empty(&parent))                                                  // Write tag with no children on the same line
       {makeLocalCopyOfXmlTagString(s, l, parent);
        openFont();
//      drawChar  (XmlOpen);
        drawString(s, l, XmlOpen);
//      drawChar  (XmlClose);
        currentTagNumber++;                                                     // Close tag
        closeFont();
//      drawChar  (XmlSlash);
        const typeof(parent.proto->tagName(&parent)) n = parent.proto->tagName(&parent);
        drawString(n, strlen(n), XmlClose);
//      drawChar  (XmlClose);
       }
      else                                                                      // Opener
       {openFont();
        startNewLine(1);
//      drawChar  (XmlOpen);
        drawString(parent.proto->tagString(&parent), parent.proto->tagStringLength(&parent), XmlOpen);
//      drawChar  (XmlClose);
       }
     } // open

    void close()                                                                // Add close tag unless we are on text
     {currentPositionInTag = 0;                                                 // Position in tag
      if (parent.proto->isTag(&parent))
       {if (!parent.proto->empty(&parent))
         {currentTagNumber++;                                                   // Close tag
          if (!parent.proto->stayInLine(&parent))
           {startNewLine(1);
           }
          closeFont();
//        drawChar  (XmlSlash);
          drawString(XmltagName, strlen(parent.proto->tagName(&parent)), XmlClose);
//        drawChar  (XmlClose);
         }
       }
     } // close

    open(); Xmlfe(child, parent) drawTagOrText(child, depth+1); close();        // Draw this level and its children
   }

  const typeof(editBuffer->xml.proto->root(&editBuffer->xml)) root = editBuffer->xml.proto->root(&editBuffer->xml);
  const typeof(root.proto->first(&root)) rootTag = root.proto->first(&root);
  drawTagOrText(rootTag, 0);                                                    // Start at the root tag

  if (1)                                                                        // Draw gutter between line numbers and text
   {const typeof(editGutter) g = editGutter;
    lineNumberGutterTextFillColor();
    i->proto->rectangleWH(i, g.x, g.y, g.proto->width(&g),   g.proto->height(&g));
    lineNumberGutterTextStrokeColor();
    i->proto->rectangleWH(i, g.x, g.y, g.proto->width(&g)/2, g.proto->height(&g));
   }

  if (editBuffer->blockIn)                                                      // Clear the blocked out area
   {blockedOutAreaBackgroundColor();
    const typeof(editBuffer->block) b = editBuffer->block;
    i->proto->rectangleWH(i, b.x, b.y, b.proto->width(&b), b.proto->height(&b));
   }
 } // drawEditBuffer

static void maintainCursorPosition_MimagemEditBuffer_MimagemEditBuffer                      // Set the scroll amount of the altered edit buffer so that the position on the screen of the line containing the cursor is as close as possible to the position in the base edit buffer when the altered buffer is drawn in the place of the base buffer despite the altered buffer having been zoomed or having its width changed relative to the base buffer.  Both buffers should have been drawn before this operations (with measureOnly=1 if no drawing is required) so that the current position of the line containing the cursor is known in both buffers at the start of this operation. After this operation the altered buffer can be drawn in the area originally occupied by the base buffer while minimizing the amount the user must move their line of sight to track the cursor position.
 (MimagemEditBuffer * base,                                                           // Base    Mimagem edit buffer
  MimagemEditBuffer * altered)                                                        // Altered Mimagem edit buffer
 {const typeof(base->   cursor.y) b = base->   cursor.y;                                                        // Location of cursor line of base Mimagem edit buffer on display in pixels
  const typeof(altered->cursor.y) a = altered->cursor.y;                                                        // Location of cursor line in altered Mimagem edit buffer on display in pixels

  const typeof((a - b) / altered->lineHeight + altered->scroll) s = (a - b) / altered->lineHeight + altered->scroll;                          // Amount we should scroll to minimize the apparent movement of the tag containing the cursor when we change font size or change the edit buffer width

  altered->scroll = nearbyint(s);
//altered->scroll = floor(s);
//altered->scroll = ceil(s);
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()
 {const typeof("<a><b><c/><d><e/>ee<f/>ff<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>") xml = "<a><b><c/><d><e/>ee<f/>ff<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>";
     const typeof(parseXmlFromString(xml, 0)) x = parseXmlFromString(xml, 0);
  assert( !x.proto->errors(&x));

  void draw(CairoTextImage * i)
   {i->proto->fontSize(i, 40);
    typeof(0ul) line = 0ul;

    void drawXml(const XmlTag tag, int start, int depth)
     {makeLocalCopyOfXmlTagString(t, l, tag);
      const typeof(i->fontHeight * depth) x = i->fontHeight * depth;
      const typeof(i->fontHeight * line++) y = i->fontHeight * line++;
      i->proto->text(i, x, y, t);
      if(0)start=start;
     }

    x.proto->scan(&x, drawXml);
   }

  typeof(makeCairoTextImage(draw, 2000, 2000, "Mimagem0.png", "deb8")) i = makeCairoTextImage(draw, 2000, 2000, "Mimagem0.png", "deb8");
  i.proto->free(&i);
 }

void test1()                                                                    //TdrawEditBuffer //TmaintainCursorPosition
 {const typeof("<a><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j><B><C/><D><E/>EE<F/>1 2 3 4 5 6 7 8 9 0<G>GGG</G></D><H>HH HH</H></B><i/>i<j></j><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>") xml = "<a><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j><B><C/><D><E/>EE<F/>1 2 3 4 5 6 7 8 9 0<G>GGG</G></D><H>HH HH</H></B><i/>i<j></j><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>";

     const typeof(parseXmlFromString(xml, 0)) X = parseXmlFromString(xml, 0);
  assert( !X.proto->errors(&X));

  void draw(CairoTextImage * const i)                                           // Draw the xml into an image
   {const typeof(makeRectangleWH(0, 0, i->width, i->height)) page = makeRectangleWH(0, 0, i->width, i->height);
    const typeof(4) wScroll = 4; const typeof(100) fontSize = 100;                                                // Scroll amount in wide mode, font size of text in image

    const typeof(page.proto->right(&page, 0)) ww = page.proto->right(&page, 0);                                                       // Measure in wide mode to find the location of the pointer expected to be the middle G in GGG
    typeof(newMimagemEditBuffer(({struct MimagemEditBuffer t = {cti: i, xml: X, fontSize: fontSize, px: 715, py: 894, scroll: wScroll, zone: ww.a, proto: &ProtoTypes_MimagemEditBuffer}; t;}))) we = newMimagemEditBuffer(({struct MimagemEditBuffer t = {cti: i, xml: X, fontSize: fontSize, px: 715, py: 894, scroll: wScroll, zone: ww.a, proto: &ProtoTypes_MimagemEditBuffer}; t;}));
    we.proto->drawEditBuffer(&we);
    i->proto->saveAsPng(i, "Mimagem1_wide.png", "a"); i->proto->clearWhite(i);

    const typeof(X.tree.proto->offset(&X.tree, we.pointer.tag)) wn = X.tree.proto->offset(&X.tree, we.pointer.tag);                                       // Pointer location in wide version
    assert( wn.proto->equalsString(&wn, "GGG"));
    assert( we.pointer.positionInTag ==  2);
    assert( we.pointer.character     == 81);
    assert( we.pointer.editLine      == 12);

    const typeof(page.proto->left(&page, i->width * 4 / 8)) nw = page.proto->left(&page, i->width * 4 / 8);                                         // Measure in narrow mode to find position of cursor as set by pointer in previous image
    typeof(newMimagemEditBuffer(({struct MimagemEditBuffer t = {cti: i, xml: X, fontSize: fontSize * 9.0 / 8, cursor: we.pointer, zone: nw.a, proto: &ProtoTypes_MimagemEditBuffer}; t;}))) ne = newMimagemEditBuffer(({struct MimagemEditBuffer t = {cti: i, xml: X, fontSize: fontSize * 9.0 / 8, cursor: we.pointer, zone: nw.a, proto: &ProtoTypes_MimagemEditBuffer}; t;}));
    ne.proto->drawEditBuffer(&ne);
    i->proto->saveAsPng(i, "Mimagem1_narrow.png", "a"); i->proto->clearWhite(i);

    const typeof(X.tree.proto->offset(&X.tree, ne.cursor.tag)) nn = X.tree.proto->offset(&X.tree, ne.cursor.tag);                                        // Cursor location in narrow mode
    assert( nn.proto->equalsString(&nn, "GGG"));
    assert( ne.cursor.positionInTag == we.pointer.positionInTag);
    assert( ne.cursor.character     == we.pointer.character);
    assert( ne.cursor.editLine      == 14);

    we.cursor = we.pointer;                                                     // Simulate a click - the cursor position is set to match the pointer position
    we.proto->maintainCursorPosition(&we, &ne);                                           // Position the narrow display so that GGG is in much the same screen position as the wide display

    ne.measureOnly = 0;                                                         // Request draw of the edit buffer
    ne.proto->drawEditBuffer(&ne);                                                        // Draw scrolled edit buffer
//  i.proto->save(&i, "Mimagem1_narrowScrolled.png", "8d73");

    const typeof(X.tree.proto->offset(&X.tree, ne.cursor.tag)) nN = X.tree.proto->offset(&X.tree, ne.cursor.tag);                                        // Cursor location in narrow mode
    assert( nN.proto->equalsString(&nN, "GGG"));
    assert( ne.cursor.positionInTag == we.pointer.positionInTag);
    assert( ne.cursor.character     == we.pointer.character);
    assert( ne.cursor.editLine      == ne.cursor.editLine);
   }

  typeof(makeCairoTextImage(draw, 2000, 2000, "Mimagem1.png", "a")) i = makeCairoTextImage(draw, 2000, 2000, "Mimagem1.png", "a");                      // Create image containing some text and check its digest
  i.proto->free(&i);
 }

void test2()                                                                    // Block out an area
 {const typeof("<a><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j><B><C/><D><E/>EE<F/>1 2 3 4 5 6 7 8 9 0<G>GGG</G></D><H>HH HH</H></B><i/>i<j></j><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>") xml = "<a><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j><B><C/><D><E/>EE<F/>1 2 3 4 5 6 7 8 9 0<G>GGG</G></D><H>HH HH</H></B><i/>i<j></j><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>";

     const typeof(parseXmlFromString(xml, 0)) X = parseXmlFromString(xml, 0);
  assert( !X.proto->errors(&X));

  void draw(CairoTextImage * i)                                                 // Draw the xml into an image
   {const typeof(makeRectangleWH(0, 0, i->width, i->height)) page = makeRectangleWH(0, 0, i->width, i->height);
    const typeof(100) fontSize = 100;                                                             // Font size of text in image

    const typeof(makeRectangleWH(500, 500, 1000, 1000)) b = makeRectangleWH(500, 500, 1000, 1000);                                  // Block out this area
    typeof(newMimagemEditBuffer(({struct MimagemEditBuffer t = {cti: i, xml: X, fontSize: fontSize, zone: page, block: b, blockIn: 1, proto: &ProtoTypes_MimagemEditBuffer}; t;}))) e = newMimagemEditBuffer(({struct MimagemEditBuffer t = {cti: i, xml: X, fontSize: fontSize, zone: page, block: b, blockIn: 1, proto: &ProtoTypes_MimagemEditBuffer}; t;}));
    e.proto->drawEditBuffer(&e);
   }

  typeof(makeCairoTextImage(draw, 2000, 2000, "Mimagem2.png", "a")) i = makeCairoTextImage(draw, 2000, 2000, "Mimagem2.png", "a");                      // Create image containing some text and check its digest
  i.proto->free(&i);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, 0};
  run_tests("Mimagem", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/xml/xml
