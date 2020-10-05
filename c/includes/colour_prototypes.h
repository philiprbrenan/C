static Colour makeColour
  (const double r,
   const double g,
   const double b,
   const double a);
struct ProtoTypes_Colour {
 } const ProtoTypes_Colour =
{};
Colour newColour(Colour allocator) {return allocator;}

struct ProtoTypes_ColourPale {
 } const ProtoTypes_ColourPale =
{};
ColourPale newColourPale(ColourPale allocator) {return allocator;}

