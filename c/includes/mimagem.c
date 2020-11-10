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
  size_t         tagOffset;                                                     // The offset of the tag
  size_t         tagChar;                                                       // The character offset in the tag
  size_t         editLineNumber;                                                // The edit line number containing the position
  size_t         x;                                                             // The x coordinate of the start of the position measured in pixels
  size_t         y;                                                             // The y coordinate of the start of the position measured in pixels
  int            tagClose;                                                      // True if this position is in a closing tag
 } MimagemEditPosition;

typedef struct MimagemEditBuffer                                                      // Mimagem edit buffer
 {const struct ProtoTypes_MimagemEditBuffer *proto;                                   // Prototypes for methods
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
  MimagemEditPosition  firstPos;                                                      //O The first item drawn in the edit text zone
  MimagemEditPosition  lastPos;                                                       //O The last  item drawn in the edit text zone
  MimagemEditPosition  pointer;                                                       //O Position of the pointer in the edit buffer
  MimagemEditPosition  cursor;                                                        //O Position of the cursor  in the edit buffer
 } MimagemEditBuffer;

#include <mimagem_prototypes.h>

static void drawEditBuffer_MimagemEditBuffer_MimagemEditBuffer                              // Draw the edit buffer and return the location of the pointer and cursor
 (MimagemEditBuffer * editBuffer)                                                     // Mimagem edit buffer
 {typeof(editBuffer->cti) i = editBuffer->cti;                                       // Image description
  const typeof(i->cr) cr = i->cr;                                                 // Cairo context to draw in
  const typeof(!editBuffer->measureOnly) draw = !editBuffer->measureOnly;                              // Draw unless we only want to measure
  typeof(ULONG_MAX) closestSoFar = ULONG_MAX;                                             // Shortest distance so far to pointer
  const typeof(8) lineNumberGutterText = 8;                                                     // Pixels Gutter between line numbers and text
  typeof(0) firstDrawn = 0;                                                     // First character drawn

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

  size_t getLineNumberWidth()                                                   // Width of line numbers
   {const typeof(8ul) N = 8ul;
    const typeof(editBuffer->xml.proto->root(&editBuffer->xml)) root = editBuffer->xml.proto->root(&editBuffer->xml);
    const typeof(root.proto->charPosition(&root, 1)) n = root.proto->charPosition(&root, 1);                                                 // Maximum width we will allow
    char z[2] = {'0', 0};                                                       // A string of one zero
    const typeof(i->proto->textAdvance(i, z)) a = i->proto->textAdvance(i, z);                                                     // Width of string of one zero
    typeof(1ul) t = 1ul;                                                                    // Power of ten
    for(size_t i = 1; i <= N; ++i)                                              // Find power of ten neede to allow all tag numbers
     {t *= 10ul;                                                                // Next power
      if (t > n) return i * a;                                                  // Big enough
     }
    return a * N;                                                               // Use default in extremis
   } // getLineNumberWidth

  const typeof(getLineNumberWidth()) lineNumberWidth = getLineNumberWidth();                                   // Width of line numbers
  const typeof(editBuffer->zone.proto->left(&editBuffer->zone, lineNumberWidth)) editLineNumbersText = editBuffer->zone.proto->left(&editBuffer->zone, lineNumberWidth);          // Split the drawing area into line numbers and text
  const typeof(editLineNumbersText.a) editLineNumbers = editLineNumbersText.a;                                  // Line numbers
  const typeof(editLineNumbersText.b.proto->left(&editLineNumbersText.b, lineNumberGutterText)) editGutterAndText = editLineNumbersText.b.proto->left(&editLineNumbersText.b, lineNumberGutterText);     // Split the drawing area into line numbers and text
  const typeof(editGutterAndText.a) editGutter = editGutterAndText.a;                                    // Gutter just before text
  const typeof(editGutterAndText.b) editText = editGutterAndText.b;                                    // Text

  size_t currentTagNumber = 0, currentPositionInTag = 0,                        // Current tag and position within current tag
         currentChar      = 0, currentEditLine      = 1;                        // Current character number counted over all tags drawn, current edit line

  double x = editText.X, y = editText.y - i->fontHeight;                        // Initial text position allowing for the new line operation which always occurs first.
  XmlTag invalidNode = newXmlTag(({struct XmlTag t = {proto: &ProtoTypes_XmlTag};   t;}));
  typeof(invalidNode) lastIndentedOpen = invalidNode;

  void drawTagOrText                                                            // Draw the specified tag
   (XmlTag parent,                                                              // Tag
    int closeTag,                                                               // True if a closing tag
    int depth,                                                                  // Depth of tag
    int startAtChar)                                                            // Character to start at - normally zero except for the first tag to be drawn
   {const typeof(paleColours.N) pcN = paleColours.N;
    const typeof(paleColours.p[(abs(depth - (parent.proto->isText(&parent) ? 1 : 0))) % pcN]) backgroundColour = paleColours.p[(abs(depth - (parent.proto->isText(&parent) ? 1 : 0))) % pcN];        // Choose the back ground colour for this depth and tag
    const typeof(paleColours.p[(abs(depth - (parent.proto->isText(&parent) ? 2 : 1))) % pcN]) backgroundColour1 = paleColours.p[(abs(depth - (parent.proto->isText(&parent) ? 2 : 1))) % pcN];        // Background colour for previous layer

    void startNewLine(int indent, char * label)                                 // Move to next line or move to indent point on this line if possible else the next line
     {const typeof(i->fontHeight * depth) ip = i->fontHeight * depth;                                               // Indentation position
      const typeof(!indent || (indent && x > editText.x + ip)) nr = !indent || (indent && x > editText.x + ip);                          // New line required
      if (draw && nr)                                                           // Finish current line if we are moving to the next line
       {i->proto->save(i);
        const typeof(makeRectangleWH(x, y, editText.X - x, i->fontHeight)) r = makeRectangleWH(x, y, editText.X - x, i->fontHeight);               // Rectangle for line number
        i->proto->rectangle(i, r, lastBackGroundColourDrawn);                            // Background for line number
        i->proto->restore(i);
       }

      if (nr) {y += i->fontHeight; ++currentEditLine;}                          // Y coordinate of top of new line if moving to a new line

      if (draw)                                                                 // Draw the line number if starting a new line
       {i->proto->save(i);
        i->proto->colour(i, lastBackGroundColourDrawn);                                  // Background colour of line number
        const typeof(i->fontHeight) H = i->fontHeight;

        if (indent)
         {if (nr) i->proto->rectangleWH(i, editText.x, y,              ip,     H);       // Background of indentation leading to text on new line
          else    i->proto->rectangleWH(i,          x, y, editText.x + ip - x, H);       // Background of indentation leading to text on same line
         }

        if (nr)                                                                 // Draw line number
         {i->proto->rectangleWH(i, editLineNumbers.x, y, lineNumberWidth, H);            // Background for line number
          lineNumberFont();                                                     // Text of line number
          const typeof(parent.proto->charPosition(&parent, closeTag) + currentChar + 1) l = parent.proto->charPosition(&parent, closeTag) + currentChar + 1;                // Format line number as current character position in xml. Current char is zero based so we add 1 to make it one based.
          lsprintf(n, 1024, "%lu", l);                                          // Line number as string
          i->proto->text(i, editLineNumbers.X - i->proto->textAdvance(i, n), y, n);
         }
        i->proto->restore(i);
       }

      x = editText.x + (indent ? ip : 0);                                       // Position for next character
      ({typeof(lastIndentedOpen) sourcesourcesource = invalidNode; memcpy((void *)&lastIndentedOpen, (void *)&sourcesourcesource, sizeof(lastIndentedOpen));});                                           // Allow the caller to record the opening tag
      if (0) label = label;
     } // startNewLine

    void drawChar(char * string, int openClose, double width)                   // Draw a string at the current (x,y) position and advance the currrent position to the end of the character drawn. A gradient background is drawn for the first/last letters of a tag (0: no gradient, 1: opening gradient, 2: closing gradient)
     {void drawBackGroundForChar()                                              // Draw the background for the current character
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

      if (x + width >= editText.X)                                              // Start a new line if the draw would be off the end of the line
       {if (lastIndentedOpen.proto->equals(&lastIndentedOpen, parent)) startNewLine(1, "AA11");         // The opening tag is at the start of the preceding line - the closing tag will not fit on that line - as we are starting a new line we might as well vertically align them.
        else startNewLine(0, "AA22");
       }

      if (draw && editText.proto->containsACorner(&editText, charR))                            // Draw character or symbol if drawing and we are in the drawing area
       {drawBackGroundForChar();                                                // Background
        i->proto->text(i, x, y, string);
       }

      void recordPosition(MimagemEditPosition *p)                                     // Record the current position
       {p->tagOffset      = parent.node.offset;
        p->tagChar        = currentPositionInTag;
        p->editLineNumber = currentEditLine;
        p->x              = x;
        p->y              = y;
        p->tagClose       = openClose == XmlClose;
       }

      if (1)                                                                    // Pointer and cursor location
       {const typeof(strlen(string)) l = strlen(string);
        currentPositionInTag += l; currentChar += l;
       }

      if (editBuffer->py <= y + i->fontHeight && editBuffer->py >= y)           // Line containing pointer
       {const typeof(fabs(editBuffer->px - x - width / 2)) d = fabs(editBuffer->px - x - width / 2);                               // Distance from the center of the current character to pointer.
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
           {const typeof(i->proto->textAdvance(i, word)) width = i->proto->textAdvance(i, word);                                      // Measure word
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
      if (!parent.proto->stayInLine(&parent))
       {startNewLine(1, "CCCC");
        ({typeof(lastIndentedOpen) sourcesourcesource = parent; memcpy((void *)&lastIndentedOpen, (void *)&sourcesourcesource, sizeof(lastIndentedOpen));});
       }
      makeLocalCopyOfXmlTagString(s, l, parent);
      drawString(s, l, XmlOpen);
     }

    void drawClose()                                                            // Close tag
     {closeFont();
      drawString(XmltagName, strlen(parent.proto->tagName(&parent)), XmlClose);               // tagName has the side effect of setting XmltagName
     }

    void close()                                                                // Close tag
     {if (parent.proto->isTag(&parent))
       {currentTagNumber++;
        if (!parent.proto->stayInLineClose(&parent)) startNewLine(1, "DDDD");                 // Start a new line unless this tag stays inline because it is preceded or followed by text
        drawClose();
       }
     }

    if (closeTag) close();                                                      // Close
    else                                                                        // Open or singleton tag
     {currentTagNumber++;                                                       // Count string as a tag
      if (parent.proto->isText(&parent))                                                      // Text
       {textFont(); drawString(parent.proto->tagString(&parent), parent.proto->tagStringLength(&parent), 0);
       }
      else if (parent.proto->empty(&parent)) {open(); drawClose();}                           // Empty
      else open();                                                              // Open with content
     }
   } // drawTagOrText

  if (1)                                                                        // Draw the tags from the starting position in the start tag
   {const typeof(editBuffer) e = editBuffer;                                                             // Edit buffer
    typeof(e->xml.tree.proto->offset(&e->xml.tree, e->startTagOffset)) s = e->xml.tree.proto->offset(&e->xml.tree, e->startTagOffset);                                // Start tag
    typeof(e->startTagChar - 1) startAtChar = e->startTagChar - 1;                                          // Set for first tag

    int sub(ArenaListNode node, int start, int depth)                           // Each node in the parse tree going forward from the start position
     {const typeof(newXmlTag(({struct XmlTag t = {xml: e->xml, node: node, proto: &ProtoTypes_XmlTag}; t;}))) t = newXmlTag(({struct XmlTag t = {xml: e->xml, node: node, proto: &ProtoTypes_XmlTag}; t;}));
      drawTagOrText(t, start < 0 ? 1 : 0, depth, startAtChar);                  // Draw tag
      startAtChar = 0;                                                          // Draw tags after the first one from their start position
      return y > editText.Y;                                                    // Stop drawing when we clear the drawing area with any part of the text
     }

    s.proto->scanFrom(&s, sub, e->startTagClose);                                        // Draw from the current start tag
   }

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

  const typeof((a - b) / altered->lineHeight) s = (a - b) / altered->lineHeight;                                            // Amount we should scroll to minimize the apparent movement of the tag containing the cursor when we change font size or change the edit buffer width

  say("SSSS %f\n", s);

//altered->scroll = nearbyint(s);
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
 {   const typeof(parseXmlFromString(XmlsampleDita1(), 0)) X = parseXmlFromString(XmlsampleDita1(), 0);
  assert( !X.proto->errors(&X));

  const typeof(X.proto->root(&X)) pos1 = X.proto->root(&X);

  void draw(CairoTextImage * const i)                                           // Draw the xml into an image
   {const typeof(makeRectangleWH(0, 0, i->width, i->height)) page = makeRectangleWH(0, 0, i->width, i->height);
    const typeof(100) fontSize = 100;                                                             // Scroll amount in wide mode, font size of text in image

    const typeof(page.proto->right(&page, 0)) ww = page.proto->right(&page, 0);                                                       // Measure in wide mode to find the location of the pointer expected to be the middle G in GGG
    typeof(newMimagemEditBuffer(({struct MimagemEditBuffer t = {cti: i, xml: X, fontSize: fontSize, px: 715, py: 894, startTagOffset: pos1.node.offset, startTagChar: 1, zone: ww.a, proto: &ProtoTypes_MimagemEditBuffer}; t;}))) we = newMimagemEditBuffer(({struct MimagemEditBuffer t = {cti: i, xml: X, fontSize: fontSize, px: 715, py: 894, startTagOffset: pos1.node.offset, startTagChar: 1, zone: ww.a, proto: &ProtoTypes_MimagemEditBuffer}; t;}));
    we.proto->drawEditBuffer(&we);
    i->proto->saveAsPng(i, "Mimagem1_wide1.png", "e4b"); i->proto->clearWhite(i);

    char *(sum[]) = {"", "", "", "", "", "", 0};
    typeof(1) pageNumber = 1;
    for(char **page = sum+2; *page; ++page)
     {we.startTagOffset = we.lastPos.tagOffset;                                 // Position so the last drawn character becomes the first one
      we.startTagChar   = we.lastPos.tagChar;
      we.startTagClose  = we.lastPos.tagClose;
      we.proto->drawEditBuffer(&we);
      lsprintf(file, 32, "Mimagem1_wide%d.png", ++pageNumber);
      i->proto->saveAsPng(i, file, *page); i->proto->clearWhite(i);
     }

    const typeof(X.tree.proto->offset(&X.tree, we.pointer.tagOffset)) wn = X.tree.proto->offset(&X.tree, we.pointer.tagOffset);                                 // Pointer location in wide version
    assert( wn.proto->equalsString(&wn, "GGG"));
    assert( we.pointer.tagChar        == 81);
    assert( we.pointer.editLineNumber == 12);

    const typeof(page.proto->left(&page, i->width * 4 / 8)) nw = page.proto->left(&page, i->width * 4 / 8);                                         // Measure in narrow mode to find position of cursor as set by pointer in previous image
    typeof(newMimagemEditBuffer(({struct MimagemEditBuffer t = {cti: i, xml: X, fontSize: fontSize * 9.0 / 8, cursor: we.pointer, zone: nw.a, proto: &ProtoTypes_MimagemEditBuffer}; t;}))) ne = newMimagemEditBuffer(({struct MimagemEditBuffer t = {cti: i, xml: X, fontSize: fontSize * 9.0 / 8, cursor: we.pointer, zone: nw.a, proto: &ProtoTypes_MimagemEditBuffer}; t;}));
    ne.proto->drawEditBuffer(&ne);
    i->proto->saveAsPng(i, "Mimagem1_narrow.png", "a"); i->proto->clearWhite(i);

    const typeof(X.tree.proto->offset(&X.tree, ne.cursor.tagOffset)) nn = X.tree.proto->offset(&X.tree, ne.cursor.tagOffset);                                        // Cursor location in narrow mode
    assert( nn.proto->equalsString(&nn, "GGG"));
    assert( ne.cursor.tagChar        == we.pointer.tagChar);
    assert( ne.cursor.editLineNumber == 14);

    we.cursor = we.pointer;                                                     // Simulate a click - the cursor position is set to match the pointer position
    we.proto->maintainCursorPosition(&we, &ne);                                           // Position the narrow display so that GGG is in much the same screen position as the wide display

    ne.measureOnly = 0;                                                         // Request draw of the edit buffer
    ne.proto->drawEditBuffer(&ne);                                                        // Draw scrolled edit buffer
//  i.proto->save(&i, "Mimagem1_narrowScrolled.png", "8d73");

    const typeof(X.tree.proto->offset(&X.tree, ne.cursor.tagOffset)) nN = X.tree.proto->offset(&X.tree, ne.cursor.tagOffset);                                  // Cursor location in narrow mode
    assert( nN.proto->equalsString(&nN, "GGG"));
    assert( ne.cursor.tagChar        == we.pointer.tagChar);
    assert( ne.cursor.editLineNumber == ne.cursor.editLineNumber);
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
 {void (*tests[])(void) = {0, test0, test1, test2, 0};
  run_tests("Mimagem", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/xml/xml
