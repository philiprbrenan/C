static void drawEditBuffer_MimagemEditBuffer_MimagemEditBuffer
 (MimagemEditBuffer * editBuffer);
static void maintainCursorPosition_MimagemEditBuffer_MimagemEditBuffer
 (MimagemEditBuffer * base,
  MimagemEditBuffer * altered);
struct ProtoTypes_MimagemEditBuffer {
  void  (*drawEditBuffer)(                                                      // Draw the edit buffer and return the location of the pointer and cursor
    MimagemEditBuffer * editBuffer);                                            // Mimagem edit buffer
  void  (*maintainCursorPosition)(                                              // Set the scroll amount of the altered edit buffer so that the position on the screen of the line containing the cursor is as close as possible to the position in the base edit buffer when the altered buffer is drawn in the place of the base buffer despite the altered buffer having been zoomed or having its width changed relative to the base buffer.  Both buffers should have been drawn before this operations (with measureOnly=1 if no drawing is required) so that the current position of the line containing the cursor is known in both buffers at the start of this operation. After this operation the altered buffer can be drawn in the area originally occupied by the base buffer while minimizing the amount the user must move their line of sight to track the cursor position.
    MimagemEditBuffer * base,                                                   // Base    Mimagem edit buffer
    MimagemEditBuffer * altered);                                               // Altered Mimagem edit buffer
 } const ProtoTypes_MimagemEditBuffer =
{drawEditBuffer_MimagemEditBuffer_MimagemEditBuffer, maintainCursorPosition_MimagemEditBuffer_MimagemEditBuffer};
MimagemEditBuffer newMimagemEditBuffer(MimagemEditBuffer allocator) {return allocator;}

struct ProtoTypes_MimagemEditPosition {
 } const ProtoTypes_MimagemEditPosition =
{};
MimagemEditPosition newMimagemEditPosition(MimagemEditPosition allocator) {return allocator;}

