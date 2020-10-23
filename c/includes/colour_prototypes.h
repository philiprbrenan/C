static Colour makeColour
 (const double r,
  const double g,
  const double b,
  const double a);
static void dump_Colour
  (Colour colour);
struct ProtoTypes_Colour {
  void  (*dump)(                                                                // Dump a colour
    Colour colour);                                                             // Colour
 } const ProtoTypes_Colour =
{dump_Colour};
Colour newColour(Colour allocator) {return allocator;}

struct ProtoTypes_ColourPale {
 } const ProtoTypes_ColourPale =
{};
ColourPale newColourPale(ColourPale allocator) {return allocator;}

