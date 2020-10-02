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

typedef struct MimagemEditBuffer                                                      // Mimagem window edit buffer
 {const struct ProtoTypes_MimagemEditBuffer *proto;                                   // Prototypes for methods
  const XmlParse xml;                                                           // Xml parse tree being editted
 } MimagemEditBuffer;

#include <mimagem_prototypes.h>
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

  const typeof(createCairoTextImage(draw, 2000, 2000, "xml1.png", "a")) i = createCairoTextImage(draw, 2000, 2000, "xml1.png", "a");                  // Create image containing some text and check its digest
  i.proto->free(i);
 }

void test1()
 {const typeof("<a><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>") xml = "<a><b><c/><d><e/>ee<f/>1 2 3 4 5 6 7 8 9 0<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>";

     const typeof(parseXmlFromString(xml, 0)) X = parseXmlFromString(xml, 0);
  assert( !X.proto->errors(X));


  void draw(CairoTextImage i)                                                   // Draw the xml into an image
   {typeof(i.cr) cr = i.cr;

    typeof(100) fontSize = 100;                                                             // Font size
    const typeof(1066) cx = 1066; const typeof(381) cy = 381; size_t pointerTag = 0, pointerPositionInTag = 0;       // Hypothetical cursor location

    cairo_set_font_size (cr, fontSize);                                         // Cairo
    cairo_font_extents_t fontExtents;
    cairo_font_extents  (cr, &fontExtents);
    cairo_text_extents_t textExtents;

    const typeof(fontExtents.height) H = fontExtents.height;                                                     // Font details

    const typeof(makeRectangleWH(0, 0, i.width, i.height)) page = makeRectangleWH(0, 0, i.width, i.height);
    const typeof(page.proto->right(page, 0)) editZoneAndStatus = page.proto->right(page, 0);
    const typeof(editZoneAndStatus.a.proto->left(editZoneAndStatus.a, H)) editLineNumbersAndText = editZoneAndStatus.a.proto->left(editZoneAndStatus.a, H);
    const typeof(editLineNumbersAndText.a) editLineNumbers = editLineNumbersAndText.a;
    const typeof(editLineNumbersAndText.b) editText = editLineNumbersAndText.b;

    void drawRectangle(Rectangle r)
     {cairo_rectangle(cr, r.x, r.y, r.proto->width(r), r.proto->height(r));
     }

    cairo_set_source_rgb(cr, 1, 0.9, 0.9);                                      // Shade line numbers
    drawRectangle(editLineNumbers);
    cairo_fill(cr);

    size_t currentTagOffset = 0, currentTagNumber = -1, currentPositionInTag = 0;// Current tag and position within current tag
    double x = editText.x, y = editText.y;                                      // Initial text position
    cairo_move_to(cr, x, y);

    void drawTag(const XmlTag parent, const int depth)                          // Print the specified parent and its children
     {currentTagOffset = parent.node.offset;                                    // In case the pointer is located in this tag

      void startNewLine()                                                       // Move to next line
       {cairo_move_to       (cr, editLineNumbers.x, y + H);
        cairo_set_source_rgb(cr, 0, 0, 0);                                      // Color for line number
        lsprintf(n, 1024, "%lu", ++currentTagNumber);                           // Format line number
        cairo_show_text     (cr, n);                                            // Write char
        cairo_move_to       (cr, x = editText.x, y += H);
       }

      void drawChar(char c)                                                     // Draw a character
       {char s[2] = {c, 0};
        cairo_text_extents(cr, s, &textExtents);
        if (!editText.proto->containsPoint(editText, x + textExtents.x_advance, y))
         {startNewLine();
         }

        cairo_show_text   (cr, s);                                              // Write char
        cairo_text_extents(cr, s, &textExtents);

        ++currentPositionInTag;                                                 // Cursor location
        if (cx >= x && cx <= x + textExtents.x_advance && cy <= y && cy >= y-H)
         {pointerTag           = currentTagOffset;
          pointerPositionInTag = currentPositionInTag;
         }

        cairo_move_to     (cr, x += textExtents.x_advance, y += 0);             // Position ready for the next character
       }

      void drawString(char *s, size_t l)                                        // Draw a string
       {for(size_t i = 0; i < l; ++i) drawChar(s[i]);
       }

      void open()                                                               // Add open tag or text
       {currentPositionInTag = 0;                                               // Position in tag
        if (parent.proto->isText(parent))                                                    // Text
         {cairo_set_source_rgb(cr, 0, 0, 0);
          drawString(parent.proto->tagString(parent), parent.proto->tagStringLength(parent));
          currentTagNumber++;                                                   // Count string as a tag
         }
        else if (parent.proto->empty(parent))                                                // Write tag with no children on the same line
         {cairo_set_source_rgb(cr, 1, 0, 0);
          makeLocalCopyOfXmlTagString(s, l, parent);
          drawChar  (XmlOpen);
          drawString(s+1, l-2);
          drawChar  (XmlClose);
          drawChar  (XmlOpen);
          drawChar  (XmlSlash);
          const typeof(parent.proto->tagName(parent)) n = parent.proto->tagName(parent);
          drawString(n, strlen(n));
          drawChar  (XmlClose);
          currentTagNumber += 2;                                                // Count open and close empty non text tag
         }
        else                                                                    // Opener
         {cairo_set_source_rgb(cr, 0, 0, 1);
          startNewLine();   cairo_move_to(cr, x += H * depth, y);
          drawString(parent.proto->tagString(parent), parent.proto->tagStringLength(parent));
         }
       }

      void close()                                                              // Add close tag unless we are on text
       {currentPositionInTag = 0;                                               // Position in tag
        if (parent.proto->isTag(parent))
         {if (!parent.proto->empty(parent))
           {if (!parent.proto->stayInLine(parent))
             {startNewLine(); cairo_move_to(cr, x += H * depth, y);
             }
            else currentTagNumber++;                                            // Count inline closing tag
            cairo_set_source_rgb(cr, 0, 0, 1);
            drawChar  (XmlOpen);
            drawChar  (XmlSlash);
            drawString(XmltagName, strlen(parent.proto->tagName(parent)));
            drawChar  (XmlClose);
           }
         }
       }

      open(); Xmlfe(child, parent) drawTag(child, depth+1); close();            // Draw this level and its children
     }

    drawTag(X.proto->root(X), -1);                                                      // Start at the root

      const typeof(X.tree.proto->nodeFromOffset(X.tree, pointerTag)) n = X.tree.proto->nodeFromOffset(X.tree, pointerTag);                                  // Pointer location
    assert( n.proto->equalsString(n, "ee"));
    assert( pointerPositionInTag == 1);
   }

  const typeof(createCairoTextImage(draw, 2000, 2000, "xml2.png", "a")) i = createCairoTextImage(draw, 2000, 2000, "xml2.png", "a");                  // Create image containing some text and check its digest
  i.proto->free(i);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, 0};
  run_tests("Mimagem", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/xml/xml
