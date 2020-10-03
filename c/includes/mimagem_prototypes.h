static MimagemEditBuffer drawEditBuffer_MimagemEditBuffer_MimagemEditBuffer
 (MimagemEditBuffer editBuffer);
struct ProtoTypes_MimagemEditBuffer {
  MimagemEditBuffer  (*drawEditBuffer)(                                         // Draw the edit buffer and return the location of the pointer and cursor
    MimagemEditBuffer editBuffer);                                              // Mimagem edit buffer
 } const ProtoTypes_MimagemEditBuffer =
{drawEditBuffer_MimagemEditBuffer_MimagemEditBuffer};
MimagemEditBuffer newMimagemEditBuffer(MimagemEditBuffer allocator) {return allocator;}

