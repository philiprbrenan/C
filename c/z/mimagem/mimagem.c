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
  size_t         tagOffset;                                                     // The offset of the tag
  size_t         tagChar;                                                       // The character offset in the tag
  size_t         editLineNumber;                                                // The edit line number containing the position
  size_t         x;                                                             // The x coordinate of the start of the position measured in pixels
  size_t         y;                                                             // The y coordinate of the start of the position measured in pixels
  int            tagClose;                                                      // True if this position is in a closing tag
 } $EditPosition;

typedef struct $EditBuffer                                                      // $ edit buffer
 {const struct ProtoTypes_$EditBuffer *proto;                                   // Prototypes for methods
  XmlParse       xml;                                                           //I Xml parse tree being edited
  CairoTextImage*cti;                                                           //I Cairo text image that we are drawing into
  Rectangle      zone;                                                          //I The rectangle in which the edit buffer will be drawn
  Rectangle      block;                                                         //I A rectangle in the edit buffer that displays options in line with the xml flowing around it so that both the options and the xml to be operated on are simultaneously visible.
  size_t         startTagOffset;                                                //I The offset of the tag at which we are to start drawing
  size_t         startTagChar;                                                  //I The character offset in the tag at which we are to start drawing
  int            startTagClose;                                                 //I If true start drawing at the closing tag for this xml node.  If the tag is a singleton this parameter has no effect
  int            measureOnly;                                                   //I Suppresses drawing if true.  All other operations are performed so that returned measurements of the pointer and cursor position are accurate.
  int            blockIn;                                                       //I Enable the blocked out rectangle if true otherwise the blocked out area is hidden.
  size_t         fontSize;                                                      //I Font size to use in creating text
  size_t         px;                                                            //I Pointer pixel position in x
  size_t         py;                                                            //I Pointer pixel position in y
  size_t         cx;                                                            //I Cursor pixel position in x
  size_t         cy;                                                            //I Cursor pixel position in y
  double         lineHeight;                                                    //O The distance between lines for the specified font size.
  $EditPosition  firstPos;                                                      //O The first item drawn in the edit text zone
  $EditPosition  lastPos;                                                       //O The last  item drawn in the edit text zone
  $EditPosition  pointer;                                                       //O Position of the pointer in the edit buffer
  $EditPosition  cursor;                                                        //O Position of the cursor  in the edit buffer
 } $EditBuffer;

#include <$$_prototypes.h>

static void drawEditBuffer_$EditBuffer_$EditBuffer                              // Draw the edit buffer and return the location of the pointer and cursor
 ($EditBuffer * editBuffer)                                                     // $ edit buffer
 {i                    ◀ editBuffer->cti;                                       // Image description
  cr                   ◁ i->cr;                                                 // Cairo context to draw in
  draw                 ◁ !editBuffer->measureOnly;                              // Draw unless we only want to measure
  closestSoFar         ◀ ULONG_MAX;                                             // Shortest distance so far to pointer
  lineNumberGutterText ◁ 8;                                                     // Pixels Gutter between line numbers and text
  firstDrawn           ◀ 0;                                                     // First character drawn

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

  size_t getLineNumberWidth()                                                   // Width of line numbers
   {N ◁ 8ul;
    root ◁ editBuffer->xml ▷ root;
    n ◁ root ▷ charPosition(1);                                                 // Maximum width we will allow
    char z[2] = {'0', 0};                                                       // A string of one zero
    a ◁ i ▶ textAdvance(z);                                                     // Width of string of one zero
    t ◀ 1ul;                                                                    // Power of ten
    for(size_t i = 1; i <= N; ++i)                                              // Find power of ten neede to allow all tag numbers
     {t *= 10ul;                                                                // Next power
      if (t > n) return i * a;                                                  // Big enough
     }
    return a * N;                                                               // Use default in extremis
   } // getLineNumberWidth

  lineNumberWidth     ◁ getLineNumberWidth();                                   // Width of line numbers
  editLineNumbersText ◁ editBuffer->zone      ▷ left(lineNumberWidth);          // Split the drawing area into line numbers and text
  editLineNumbers     ◁ editLineNumbersText.a;                                  // Line numbers
  editGutterAndText   ◁ editLineNumbersText.b ▷ left(lineNumberGutterText);     // Split the drawing area into line numbers and text
  editGutter          ◁ editGutterAndText.a;                                    // Gutter just before text
  editText            ◁ editGutterAndText.b;                                    // Text

  size_t currentTagNumber = 0, currentPositionInTag = 0,                        // Current tag and position within current tag
         currentChar      = 0, currentEditLine      = 1;                        // Current character number counted over all tags drawn, current edit line

  double x = editText.X, y = editText.y - i->fontHeight;                        // Initial text position allowing for the new line operation which always occurs first.
  XmlTag invalidNode = new XmlTag();
  lastIndentedOpen ◀ invalidNode;

  void drawTagOrText                                                            // Draw the specified tag
   (XmlTag parent,                                                              // Tag
    int closeTag,                                                               // True if a closing tag
    int depth,                                                                  // Depth of tag
    int startAtChar)                                                            // Character to start at - normally zero except for the first tag to be drawn
   {pcN ◁ paleColours.N;
    backgroundColour  ◁ paleColours.p[(abs(depth - (parent ▷ isText ? 1 : 0))) % pcN];        // Choose the back ground colour for this depth and tag
    backgroundColour1 ◁ paleColours.p[(abs(depth - (parent ▷ isText ? 2 : 1))) % pcN];        // Background colour for previous layer

    void startNewLine(int indent, char * label)                                 // Move to next line or move to indent point on this line if possible else the next line
     {ip ◁ i->fontHeight * depth;                                               // Indentation position
      nr ◁ !indent || (indent && x > editText.x + ip);                          // New line required
      if (draw && nr)                                                           // Finish current line if we are moving to the next line
       {i ▶ save;
        r ◁ makeRectangleWH(x, y, editText.X - x, i->fontHeight);               // Rectangle for line number
        i ▶ rectangle(r, lastBackGroundColourDrawn);                            // Background for line number
        i ▶ restore;
       }

      if (nr) {y += i->fontHeight; ++currentEditLine;}                          // Y coordinate of top of new line if moving to a new line

      if (draw)                                                                 // Draw the line number if starting a new line
       {i ▶ save;
        i ▶ colour(lastBackGroundColourDrawn);                                  // Background colour of line number
        H ◁ i->fontHeight;

        if (indent)
         {if (nr) i ▶ rectangleWH(editText.x, y,              ip,     H);       // Background of indentation leading to text on new line
          else    i ▶ rectangleWH(         x, y, editText.x + ip - x, H);       // Background of indentation leading to text on same line
         }

        if (nr)                                                                 // Draw line number
         {i ▶ rectangleWH(editLineNumbers.x, y, lineNumberWidth, H);            // Background for line number
          lineNumberFont();                                                     // Text of line number
          l ◁ parent ▷ charPosition(closeTag) + currentChar + 1;                // Format line number as current character position in xml. Current char is zero based so we add 1 to make it one based.
          lsprintf(n, 1024, "%lu", l);                                          // Line number as string
          i ▶ text(editLineNumbers.X - i ▶ textAdvance(n), y, n);
         }
        i ▶ restore;
       }

      x = editText.x + (indent ? ip : 0);                                       // Position for next character
      lastIndentedOpen ≞ invalidNode;                                           // Allow the caller to record the opening tag
      if (0) label = label;
     } // startNewLine

    void drawChar(char * string, int openClose, double width)                   // Draw a string at the current (x,y) position and advance the currrent position to the end of the character drawn. A gradient background is drawn for the first/last letters of a tag (0: no gradient, 1: opening gradient, 2: closing gradient)
     {void drawBackGroundForChar()                                              // Draw the background for the current character
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

      if (x + width >= editText.X)                                              // Start a new line if the draw would be off the end of the line
       {if (lastIndentedOpen ▷ equals(parent)) startNewLine(1, "AA11");         // The opening tag is at the start of the preceding line - the closing tag will not fit on that line - as we are starting a new line we might as well vertically align them.
        else startNewLine(0, "AA22");
       }

      if (draw && editText ▷ containsACorner(charR))                            // Draw character or symbol if drawing and we are in the drawing area
       {drawBackGroundForChar();                                                // Background
        i ▶ text(x, y, string);
       }

      void recordPosition($EditPosition *p)                                     // Record the current position
       {p->tagOffset      = parent.node.offset;
        p->tagChar        = currentPositionInTag;
        p->editLineNumber = currentEditLine;
        p->x              = x;
        p->y              = y;
        p->tagClose       = openClose == XmlClose;
       }

      if (1)                                                                    // Pointer and cursor location
       {l ◁ strlen(string);
        currentPositionInTag += l; currentChar += l;
       }

      if (editBuffer->py <= y + i->fontHeight && editBuffer->py >= y)           // Line containing pointer
       {d ◁ fabs(editBuffer->px - x - width / 2);                               // Distance from the center of the current character to pointer.
        if (d < closestSoFar)                                                   // Best distance so far
         {closestSoFar = d;
          recordPosition(&editBuffer->pointer);
         }
       }

      if (editBuffer->cursor.tagChar == currentChar)                            // Cursor location
       {recordPosition(&editBuffer->cursor);
       }

      if (!firstDrawn) {recordPosition(&editBuffer->firstPos); firstDrawn++;}   // First position written
      if (y < editText.Y) recordPosition(&editBuffer->lastPos);                 // Last  position written

      x += width;                                                               // Position ready for the next character
     } // drawChar

    void drawString(char *string, const size_t length, int openClose)           // Draw a string as one or more words
     {//say("AAAA %s\n", string);

      for(size_t a = startAtChar; a < length; ++a)
       {char word[1024] = {}; int w = 0;                                        // Word, word length
        for(size_t b = 0; a + b < length; ++b)                                  // Look for a space to break on
         {char c = string[a+b];                                                 // Next character
          int  s = isspace(c);                                                  // On a space
          word[w++] = s ? ' ' : c; word[w] = 0;                                 // Normalize white space
          if (s || !string[a + b + 1])                                          // On a space or at the end of the string
           {width ◁ i ▶ textAdvance(word);                                      // Measure word
            drawChar(word, openClose, width);
            a += b;
            break;
           }
         }
        for(size_t b = 0; a + b < length - 1; ++b)                              // Remove further trailing spaces
         {if (!isspace(string[a+b+1])) {a += b; break;}
         }
       }
     } //drawString

    currentPositionInTag = 0;                                                   // Position in tag

    void open()                                                                 // Open tag
     {openFont();
      if (!parent ▷ stayInLine)
       {startNewLine(1, "CCCC");
        lastIndentedOpen ≞ parent;
       }
      makeLocalCopyOfXmlTagString(s, l, parent);
      drawString(s, l, XmlOpen);
     }

    void drawClose()                                                            // Close tag
     {closeFont();
      drawString(XmltagName, strlen(parent ▷ tagName), XmlClose);               // tagName has the side effect of setting XmltagName
     }

    void close()                                                                // Close tag
     {if (parent ▷ isTag)
       {currentTagNumber++;
        if (!parent ▷ stayInLineClose) startNewLine(1, "DDDD");                 // Start a new line unless this tag stays inline because it is preceded or followed by text
        drawClose();
       }
     }

    if (closeTag) close();                                                      // Close
    else                                                                        // Open or singleton tag
     {currentTagNumber++;                                                       // Count string as a tag
      if (parent ▷ isText)                                                      // Text
       {textFont(); drawString(parent ▷ tagString, parent ▷ tagStringLength, 0);
       }
      else if (parent ▷ empty) {open(); drawClose();}                           // Empty
      else open();                                                              // Open with content
     }
   } // drawTagOrText

  if (1)                                                                        // Draw the tags from the starting position in the start tag
   {e ◁ editBuffer;                                                             // Edit buffer
    s ◀ e->xml.tree ▷ offset(e->startTagOffset);                                // Start tag
    startAtChar ◀ e->startTagChar - 1;                                          // Set for first tag

    int sub(ArenaListNode node, int start, int depth)                           // Each node in the parse tree going forward from the start position
     {t ◁ newXmlTag(xml: e->xml, node: node);
      drawTagOrText(t, start < 0 ? 1 : 0, depth, startAtChar);                  // Draw tag
      startAtChar = 0;                                                          // Draw tags after the first one from their start position
      return y > editText.Y;                                                    // Stop drawing when we clear the drawing area with any part of the text
     }

    s ▷ scanFrom(sub, e->startTagClose);                                        // Draw from the current start tag
   }

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
 } // drawEditBuffer

static void maintainCursorPosition_$EditBuffer_$EditBuffer                      // Set the scroll amount of the altered edit buffer so that the position on the screen of the line containing the cursor is as close as possible to the position in the base edit buffer when the altered buffer is drawn in the place of the base buffer despite the altered buffer having been zoomed or having its width changed relative to the base buffer.  Both buffers should have been drawn before this operations (with measureOnly=1 if no drawing is required) so that the current position of the line containing the cursor is known in both buffers at the start of this operation. After this operation the altered buffer can be drawn in the area originally occupied by the base buffer while minimizing the amount the user must move their line of sight to track the cursor position.
 ($EditBuffer * base,                                                           // Base    $ edit buffer
  $EditBuffer * altered)                                                        // Altered $ edit buffer
 {b ◁ base->   cursor.y;                                                        // Location of cursor line of base $ edit buffer on display in pixels
  a ◁ altered->cursor.y;                                                        // Location of cursor line in altered $ edit buffer on display in pixels

  s ◁ (a - b) / altered->lineHeight;                                            // Amount we should scroll to minimize the apparent movement of the tag containing the cursor when we change font size or change the edit buffer width

  say("SSSS %f\n", s);

//altered->scroll = nearbyint(s);
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
 {   X ◁ parseXmlFromString(XmlsampleDita1(), 0);
  ✓ !X ▷ errors;

  pos1 ◁ X ▷ root;

  void draw(CairoTextImage * const i)                                           // Draw the xml into an image
   {page ◁ makeRectangleWH(0, 0, i->width, i->height);
    fontSize ◁ 100;                                                             // Scroll amount in wide mode, font size of text in image

    ww ◁ page ▷ right(0);                                                       // Measure in wide mode to find the location of the pointer expected to be the middle G in GGG
    we ◀ new $EditBuffer(cti: i, xml: X, fontSize: fontSize, px: 715, py: 894, startTagOffset: pos1.node.offset, startTagChar: 1, zone: ww.a);
    we ▷ drawEditBuffer;
    i  ▶ saveAsPng("$1_wide1.png", "e4b"); i ▶ clearWhite;

    char *(sum[]) = {"", "", "", "", "", "", 0};
    pageNumber ◀ 1;
    for(char **page = sum+2; *page; ++page)
     {we.startTagOffset = we.lastPos.tagOffset;                                 // Position so the last drawn character becomes the first one
      we.startTagChar   = we.lastPos.tagChar;
      we.startTagClose  = we.lastPos.tagClose;
      we ▷ drawEditBuffer;
      lsprintf(file, 32, "$1_wide%d.png", ++pageNumber);
      i ▶ saveAsPng(file, *page); i ▶ clearWhite;
     }

    wn ◁ X.tree ▷ offset(we.pointer.tagOffset);                                 // Pointer location in wide version
    ✓ wn ▷ equalsString("GGG");
    ✓ we.pointer.tagChar        == 81;
    ✓ we.pointer.editLineNumber == 12;

    nw ◁ page ▷ left(i->width * 4 / 8);                                         // Measure in narrow mode to find position of cursor as set by pointer in previous image
    ne ◀ new $EditBuffer(cti: i, xml: X, fontSize: fontSize * 9.0 / 8, cursor: we.pointer, zone: nw.a);
    ne ▷ drawEditBuffer;
    i  ▶ saveAsPng("$1_narrow.png", "a"); i ▶ clearWhite;

    nn ◁ X.tree ▷ offset(ne.cursor.tagOffset);                                        // Cursor location in narrow mode
    ✓ nn ▷ equalsString("GGG");
    ✓ ne.cursor.tagChar        == we.pointer.tagChar;
    ✓ ne.cursor.editLineNumber == 14;

    we.cursor = we.pointer;                                                     // Simulate a click - the cursor position is set to match the pointer position
    we ▷ maintainCursorPosition(&ne);                                           // Position the narrow display so that GGG is in much the same screen position as the wide display

    ne.measureOnly = 0;                                                         // Request draw of the edit buffer
    ne ▷ drawEditBuffer;                                                        // Draw scrolled edit buffer
//  i  ▷ save("$1_narrowScrolled.png", "8d73");

    nN ◁ X.tree ▷ offset(ne.cursor.tagOffset);                                  // Cursor location in narrow mode
    ✓ nN ▷ equalsString("GGG");
    ✓ ne.cursor.tagChar        == we.pointer.tagChar;
    ✓ ne.cursor.editLineNumber == ne.cursor.editLineNumber;
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
 {void (*tests[])(void) = {0, test0, test1, test2, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/xml/xml
