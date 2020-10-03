static MimagemEditBuffer drawEditBuffer_MimagemEditBuffer_MimagemEditBuffer
 (MimagemEditBuffer editBuffer);
struct ProtoTypes_MimagemEditBuffer {
  MimagemEditBuffer  (*drawEditBuffer)(                                         // Draw the edit buffer and return th location of the pointer
    MimagemEditBuffer editBuffer);                                              // Mimagem edit buffer
 } const ProtoTypes_MimagemEditBuffer =
{drawEditBuffer_MimagemEditBuffer_MimagemEditBuffer};
MimagemEditBuffer newMimagemEditBuffer(MimagemEditBuffer allocator) {return allocator;}

