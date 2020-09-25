static void free_CairoText
 (CairoTextImage i);
static void assertResult
 (CairoTextImage i,
  const char * const digest);
struct ProtoTypes_CairoTextImage {
  void  (*assertResult)(                                                        // Check the image via a digest
    CairoTextImage i,                                                           // CairoTextImage
    const char * const digest);                                                 // Expected digest
  void  (*free)(                                                                // Free an image
    CairoTextImage i);                                                          // CairoTextImage
 } const ProtoTypes_CairoTextImage =
{assertResult, free_CairoText};
CairoTextImage newCairoTextImage(CairoTextImage allocator) {return allocator;}

