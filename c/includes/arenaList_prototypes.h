static ArenaList makeArenaListWithWidth
 (const size_t width);
static ArenaList makeArenaList
 ();
static void swap_ArenaList_ArenaList
 (ArenaList *source,
  ArenaList  target);
static ArenaList makeArenaListFromLines
 (const char * str);
static ArenaList makeArenaListFromWords
 (const char * str);
static ArenaList makeArenaListFromLinesOfWords
 (const char * str);
static char * check_string_ArenaList
 (const ArenaList list);
static void * pointer__ArenaList_size
 (const ArenaList   *  list,
  const size_t offset);
static ArenaListContent * content__ArenaListNode
 (const ArenaListNode   * node);
static size_t width_size__ArenaList
 (const ArenaList * list);
static size_t width_size__ArenaListNode
 (const ArenaListNode * node);
static char * key_pointer__ArenaListNode
 (const ArenaListNode * node);
static void   * data_pointer__ArenaListNode
 (const ArenaListNode * node);
static size_t   length_size__ArenaListNode
 (const ArenaListNode * node);
static size_t   maxLength_size__ArenaListNode
 (const ArenaListNode * node);
static  ArenaListNode  offset__ArenaList_size
 (const ArenaList    * list,
  const size_t delta);
static int keyEquals_int__ArenaListNode_pointer_size
 (const ArenaListNode * node,
  const char  * const key,
  const size_t       length);
static int keyEqualsString_int__ArenaListNode_pointer
 (const ArenaListNode * node,
  const char  * const key);
static int equals_int__ArenaListNode_ArenaListNode
 (const ArenaListNode * a,
  const ArenaListNode   b);
static int notEquals_int__ArenaListNode_ArenaListNode
 (const ArenaListNode * a,
  const ArenaListNode   b);
static int equalsPosition_int__ArenaListPosition_ArenaListPosition
 (const ArenaListPosition * a,
  const ArenaListPosition   b);
static  ArenaListNode root_ArenaListNode__ArenaList
 (const ArenaList *   list);
static  ArenaListNode root_ArenaListNode__ArenaListNode
 (const ArenaListNode node);
static  ArenaListNode  allocate_ArenaListNode__ArenaList_size
 (const ArenaList    * list,
  const size_t size);
static size_t used_size__ArenaList
 (const ArenaList * list);
static ArenaListNode node_ArenaListNode__ArenaList_string_size
 (const ArenaList    *       list,
  const void * const key,
  const size_t       length);
static ArenaListNode nodez_ArenaListNode__ArenaList_string
 (const ArenaList    *       list,
  const char * const key);
static void setKey__ArenaListNode_string_size
 (ArenaListNode      *       node,
  const void * const key,
  const size_t       length);
static size_t freedSpace_size__ArenaList
 (const ArenaList * list);
static void setData_ArenaListNode_pointer
 (ArenaListNode       *       node,
  const void  * const data);
static void getData_ArenaListNode_pointer
 (const ArenaListNode * node,
  void * const  data);
static void copyData_ArenaListNode_ArenaListNode
 (      ArenaListNode * target,
  const ArenaListNode   source);
static ArenaListNode  copy_ArenaListNode__ArenaListNode_ArenaList
 (const ArenaListNode * source,
  const ArenaList       target);
static void fromLetters__ArenaList_string
 (const ArenaList    * list,
  const char * str);
static void free__ArenaList
 (const ArenaList * list);
static void free__ArenaListNode
 (ArenaListNode * node);
static void insertChar__ArenaListNode_char_size
 (ArenaListNode    * node,
  const char ins,
  size_t     pos);
static void replaceChar__ArenaListNode_size
 (const ArenaListNode * node,
  const char    repl,
  size_t        pos);
static void swapChars__ArenaListNode_size
 (const ArenaListNode * node,
  size_t        pos);
static void deleteChar__ArenaListNode_size
 (const ArenaListNode * node,
  size_t        pos);
static int valid_int__ArenaListNode
 (const ArenaListNode * child);
static int invalid_int__ArenaListNode
 (const ArenaListNode * child);
static  ArenaListNode parent_ArenaListNode__ArenaListNode
 (const ArenaListNode * child);
static  ArenaListNode first_ArenaListNode__ArenaListNode
 (const ArenaListNode * parent);
static  ArenaListNode last_ArenaListNode__ArenaListNode
 (const ArenaListNode * parent);
static  ArenaListNode next_ArenaListNode__ArenaListNode
 (const ArenaListNode * parent);
static  ArenaListNode prev_ArenaListNode__ArenaListNode
 (const ArenaListNode * parent);
static  ArenaListNode first_ArenaListNode__ArenaList
 (const ArenaList * list);
static  ArenaListNode last_ArenaListNode__ArenaList
 (const ArenaList * list);
static int equalsString_int__ArenaListNode_string
 (const ArenaListNode      * node,
  const char * const key);
static  ArenaListNode findFirst_ArenaListNode__ArenaListNode_string
 (const ArenaListNode * node,
  const char  * const key);
static  ArenaListNode findFirst_ArenaListNode__ArenaList_string
 (const ArenaList    *       list,
  const char * const key);
static  ArenaListNode findFirstChild_ArenaListNode__ArenaListNode_string
 (const ArenaListNode *       Parent,
  const char  * const key);
static  ArenaListNode findFirstChild_ArenaListNode__ArenaList_string
 (const ArenaList    *       list,
  const char * const key);
static  ArenaListNode at_ArenaListNode__ArenaListNode_size
 (const ArenaListNode * Parent,
  const size_t  index);
static  ArenaListNode at_ArenaListNode__ArenaList_size
 (const ArenaList    * list,
  const size_t index);
static  size_t index__ArenaListNode
 (const ArenaListNode * child);
static int context_int__ArenaListNode_ArenaListNode_string
 (const ArenaListNode *       child,
        ArenaListNode * const parent,
  const char  * const key);
static int isFirst_int__ArenaListNode
 (const ArenaListNode * child);
static int isLast_int__ArenaListNode
 (const ArenaListNode * child);
static int isEmpty_int__ArenaListNode
 (const ArenaListNode   * node);
static int isOnlyChild_int__ArenaListNode
 (const ArenaListNode    * child);
static int isRoot_int__ArenaListNode
 (const ArenaListNode * node);
static  ArenaListNode putFL_ArenaListNode__int_ArenaListNode_ArenaListNode
 (const int   first,
  const ArenaListNode parent,
  const ArenaListNode child);
static  ArenaListNode   putTreeFirst_ArenaListNode__ArenaListNode
 (const ArenaListNode * child);
static  ArenaListNode   putTreeLast_ArenaListNode__ArenaListNode
 (const ArenaListNode * child);
static  ArenaListNode   putFirst_ArenaListNode__ArenaListNode_ArenaListNode
 (const ArenaListNode * parent,
  const ArenaListNode   child);
static  ArenaListNode   putLast_ArenaListNode__ArenaListNode_ArenaListNode
 (const ArenaListNode * parent,
  const ArenaListNode   child);
static  ArenaListNode putNP_ArenaListNode__int_ArenaListNode_ArenaListNode
 (const int   next,
  const ArenaListNode sibling,
  const ArenaListNode child);
static void setUp__ArenaListNode_ArenaListNode
 (const ArenaListNode child,
  const ArenaListNode parent);
static  ArenaListNode   putNext_ArenaListNode__ArenaListNode_ArenaListNode
 (const ArenaListNode * sibling,
  const ArenaListNode   child);
static  ArenaListNode   putPrev_ArenaListNode__ArenaListNode_ArenaListNode
 (const ArenaListNode * sibling,
  const ArenaListNode   child);
static  void replace__ArenaListNode_ArenaListNode
 (const ArenaListNode * with,
  const ArenaListNode   over);
static void splitKey_ArenaListNode_ArenaListNode
 (const ArenaListNode * Node,
  const size_t  pos);
static void by__ArenaListNode_sub
 (const ArenaListNode * node,
  void (* const function) (const ArenaListNode node));
static void by__ArenaList_sub
 (const ArenaList * list,
  void (* const function) (const ArenaListNode node));
static void scan__ArenaListNode_sub
 (const ArenaListNode * node,
  void (* const function) (ArenaListNode node, int start, int depth));
static void scan__ArenaList_sub
 (const ArenaList * list,
  void (* const function) (ArenaListNode node, int start, int depth));
static void scanFrom__ArenaListNode_sub_int
 (ArenaListNode * node,
  int  (* const sub) (ArenaListNode node, int start, int depth),
  int     close);
static  size_t countChildren_size__ArenaList
 (const ArenaList * list);
static  size_t countChildren_size__ArenaListNode
 (const ArenaListNode * Parent);
static  size_t count_size__ArenaListNode
 (const ArenaListNode * node);
static  size_t count_size__ArenaList
 (const ArenaList * list);
static ArenaListPosition preOrderPosition__ArenaListTag
 (const ArenaListNode * node);
static  ArenaList copyAndCompact_ArenaList__ArenaList
 (const ArenaList * source);
static void printWithBrackets__ArenaListNode_function
 (const ArenaListNode * node,
  void (*printer)(char * string, size_t length));
static void printWithBrackets__string_ArenaList
 (const ArenaList * list,
  void (*printer)(char * string, size_t length));
static int printsWithBracketsAs_int__ArenaListNode_string
 (const ArenaListNode *       node,
  const char  * const expected);
static int printsWithBracketsAs_int__ArenaList_string
 (const ArenaList    *       list,
  const char * const expected);
static void dumpWithBrackets__ArenaList
 (const ArenaList * list);
static void dump__ArenaList
 (const ArenaList * list);
static void dump__ArenaListNode
 (const ArenaListNode * Node);
static void dumpPosition__ArenaListPosition
 (const ArenaListPosition * position);
static void print__ArenaListNode_function
 (const ArenaListNode * node,
  void (*printer)(char * string, size_t length));
static int printsAs_int__ArenaListNode_string
 (const ArenaListNode *       node,
  const char  * const expected);
static int printsAs_int__ArenaList_string
 (const ArenaList    *       list,
  const char * const expected);
static int printContains_int__ArenaListNode
 (const ArenaListNode * node,
  const char  * expected);
static int printContains_int__ArenaList
 (const ArenaList    *  list,
  const char *  expected);
static int cmp_int__ArenaList_ArenaList
 (ArenaListNode * First,
  ArenaListNode   second);
static void sort__ArenaListNode
 (const           ArenaListNode * parent);
static void sort__ArenaList
 (const ArenaList * list);
static ArenaListNode lowest_ArenaListNode__ArenaListNode
 (const ArenaListNode * Parent);
static ArenaListNode highest_ArenaListNode__ArenaListNode
 (const ArenaListNode * Parent);
static ArenaListNode lowest_ArenaListNode__ArenaList
 (const  ArenaList * Parent);
static ArenaListNode highest_ArenaListNode__ArenaList
 (const ArenaList  * Parent);
static  ArenaListNode cut_ArenaListNode__ArenaListNode
 (const ArenaListNode * child);
static  ArenaListNode unwrap_ArenaListNode__ArenaListNode
 (const ArenaListNode * parent);
static  ArenaListNode wrap_ArenaListNode__string
 (const ArenaListNode *       child,
  const char  * const key);
static void write__ArenaList_string
 (const ArenaList    * list,
  const char * const file);
struct ProtoTypes_ArenaList {
  ArenaListNode   (*allocate)(                                                  // Allocate a node within the arena of a ArenaList
    const ArenaList    * list,                                                  // ArenaList in which to allocate
    const size_t size);                                                         // Amount of memory required
  ArenaListNode  (*at)(                                                         // The child at the specified index under the root node of the ArenaList.
    const ArenaList    * list,                                                  // ArenaList
    const size_t index);                                                        // Index
  void  (*by)(                                                                  // Traverse a ArenaList in post-order calling the specified function to process each child node.  The ArenaList is buffered allowing changes to be made to the structure of the ArenaList without disruption as long as each child checks its context.
    const ArenaList * list,                                                     // ArenaList
    void (* const function) (const ArenaListNode node));                        // Function
  char *  (*check)(                                                             // Return a string describing the structure
    const ArenaList list);                                                      // ArenaList
  ArenaList  (*copyAndCompact)(                                                 // Copy a ArenaList compacting any free space.  This method assumes that there are no direct external references to the nodes in the list as this process might change the location of one or more nodes in the arena.
    const ArenaList * source);                                                  // Source ArenaList
  size_t  (*countChildren)(                                                     // Count the number of children directly under a parent.
    const ArenaList * list);                                                    // ArenaList
  size_t  (*count)(                                                             // Count the number of nodes in a ArenaList
    const ArenaList * list);                                                    // ArenaList
  void  (*dumpWithBrackets)(                                                    // Dump a ArenaList printed with brackets to stderr
    const ArenaList * list);                                                    // ArenaList
  void  (*dump)(                                                                // Dump a ArenaList on stderr
    const ArenaList * list);                                                    // ArenaList
  ArenaListNode  (*findFirstChild)(                                             // Find the first child immediately under the root with the specified key.
    const ArenaList    * list,                                                  // ArenaList
    const char * const key);                                                    // Key to find
  ArenaListNode  (*findFirst)(                                                  // Find the first node with the specified key in a post-order traversal of the ArenaList.
    const ArenaList    * list,                                                  // ArenaList
    const char * const key);                                                    // Key to find
  ArenaListNode  (*first)(                                                      // Get the first child in the specified ArenaList.
    const ArenaList * list);                                                    // Parent
  void  (*free)(                                                                // Free an entire ArenaList.
    const ArenaList * list);                                                    // ArenaList to free
  size_t  (*freedSpace)(                                                        // Amount of space on the free chains
    const ArenaList * list);                                                    // ArenaList
  void  (*fromLetters)(                                                         // Load ArenaList from a string of letters and brackets.  The root node of the ArenaList so constructed is always given the letter 'a'.
    const ArenaList    * list,                                                  // ArenaList
    const char * str);                                                          // String of letters and brackets describing desired list structure
  ArenaListNode  (*highest)(                                                    // Find an example of the highest key under the root node of a ArenaList
    const ArenaList  * Parent);                                                 // ArenaListNode
  ArenaListNode  (*last)(                                                       // Get the last child in the specified ArenaList.
    const ArenaList * list);                                                    // Parent
  ArenaListNode  (*lowest)(                                                     // Find an example of the lowest key under the root node of a ArenaList
    const  ArenaList * Parent);                                                 // ArenaListNode
  ArenaListNode  (*node)(                                                       // Create a new ArenaList node with the specified key.
    const ArenaList    * list,                                                  // ArenaList in which to create the node
    const void * const key,                                                     // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the ArenaList is determined solely by the user.
    const size_t length);                                                       // Length of the key.
  ArenaListNode  (*nodez)(                                                      // Create a new ArenaList node with a key specified by a zero terminated string.
    const ArenaList    * list,                                                  // ArenaList in which to create the node
    const char * const key);                                                    // Zero terminated string representing the key for this node.
  ArenaListNode   (*offset)(                                                    // Create a node to locate an allocation within the arena of a ArenaList.
    const ArenaList    * list,                                                  // ArenaList
    const size_t delta);                                                        // Delta within arena. A delta of zero represents no such node.
  void *  (*pointer)(                                                           // Return a temporary pointer to data in the arena of the specified ArenaList
    const ArenaList   * list,                                                   // ArenaList
    const size_t offset);                                                       // Offset
  int  (*printContains)(                                                        // Check the print of an ArenaList contains the expected string.
    const ArenaList    * list,                                                  // ArenaList parse ArenaList
    const char * expected);                                                     // Expected string
  void  (*printWithBrackets)(                                                   // Print an entire ArenaList in preorder as a string with brackets around each sub ArenaList
    const ArenaList * list,                                                     // ArenaList
    void (*printer) (char * string, size_t length));                            // Function to apply to printed ArenaList
  int  (*printsAs)(                                                             // Check that the specified ArenaList prints as expected.
    const ArenaList    * list,                                                  // ArenaList
    const char * const expected);                                               // Expected string when printed
  int  (*printsWithBracketsAs)(                                                 // Check that the specified ArenaList prints with brackets as expected.
    const ArenaList    * list,                                                  // ArenaList
    const char * const expected);                                               // Expected string when printed
  ArenaListNode  (*root)(                                                       // Return the root node of a ArenaList.
    const ArenaList * list);                                                    // ArenaList
  void  (*scan)(                                                                // Traverse a ArenaList calling the specified function before(+1) and after(-1) processing the children of each node - or - if the node has no children the function is called once(0) . The ArenaList is buffered allowing changes to be made to the structure of the ArenaList without disruption as long as each child checks its context.
    const ArenaList * list,                                                     // ArenaList
    void (* const function) (ArenaListNode node, int start, int depth));        // Function: start is set to +1 before the children are processed, -1 afterwards. if the parent has no children the function is called once with start set to zero.
  void  (*sort)(                                                                // Sort the children of the root of a ArenaList in situ
    const ArenaList * list);                                                    // ArenaList
  void  (*swap)(                                                                // Swap the arenas of two ArenaList
    ArenaList * source,                                                         // Source ArenaList
    ArenaList target);                                                          // Target ArenaList
  size_t  (*used)(                                                              // Amount of space currently being used within the arena of a ArenaList.
    const ArenaList * list);                                                    // ArenaList
  size_t  (*width)(                                                             // Get the width of the data area in a node for a ArenaList
    const ArenaList * list);                                                    // ArenaList
  void  (*write)(                                                               // Write a ArenaList to a named file or abort
    const ArenaList    * list,                                                  // ArenaList
    const char * const file);                                                   // File
 } const ProtoTypes_ArenaList =
{allocate_ArenaListNode__ArenaList_size, at_ArenaListNode__ArenaList_size, by__ArenaList_sub, check_string_ArenaList, copyAndCompact_ArenaList__ArenaList, countChildren_size__ArenaList, count_size__ArenaList, dumpWithBrackets__ArenaList, dump__ArenaList, findFirstChild_ArenaListNode__ArenaList_string, findFirst_ArenaListNode__ArenaList_string, first_ArenaListNode__ArenaList, free__ArenaList, freedSpace_size__ArenaList, fromLetters__ArenaList_string, highest_ArenaListNode__ArenaList, last_ArenaListNode__ArenaList, lowest_ArenaListNode__ArenaList, node_ArenaListNode__ArenaList_string_size, nodez_ArenaListNode__ArenaList_string, offset__ArenaList_size, pointer__ArenaList_size, printContains_int__ArenaList, printWithBrackets__string_ArenaList, printsAs_int__ArenaList_string, printsWithBracketsAs_int__ArenaList_string, root_ArenaListNode__ArenaList, scan__ArenaList_sub, sort__ArenaList, swap_ArenaList_ArenaList, used_size__ArenaList, width_size__ArenaList, write__ArenaList_string};
ArenaList newArenaList(ArenaList allocator) {return allocator;}

struct ProtoTypes_ArenaListArena {
 } const ProtoTypes_ArenaListArena =
{};
ArenaListArena newArenaListArena(ArenaListArena allocator) {return allocator;}

struct ProtoTypes_ArenaListContent {
 } const ProtoTypes_ArenaListContent =
{};
ArenaListContent newArenaListContent(ArenaListContent allocator) {return allocator;}

struct ProtoTypes_ArenaListDescription {
 } const ProtoTypes_ArenaListDescription =
{};
ArenaListDescription newArenaListDescription(ArenaListDescription allocator) {return allocator;}

struct ProtoTypes_ArenaListNode {
  ArenaListNode  (*at)(                                                         // The child at the specified index under its parent counting from one.  An invalid node is returned if the index is too large.
    const ArenaListNode * Parent,                                               // Parent node
    const size_t index);                                                        // Index
  void  (*by)(                                                                  // Traverse the ArenaList rooted at the specified node in post-order calling the specified function to process each child node.  The ArenaList is buffered allowing changes to be made to the structure of the ArenaList without disruption as long as each child checks its context.
    const ArenaListNode * node,                                                 // ArenaListNode
    void (* const function) (const ArenaListNode node));                        // Function
  int  (*cmp)(                                                                  // Compare two keys
    ArenaListNode * First,                                                      // First node
    ArenaListNode second);                                                      // Second node
  ArenaListContent *  (*content)(                                               // Convert a node offset to an address so that the content of a node can be updated in situ as long as the ArenaList is not reallocated to a different position.
    const ArenaListNode   * node);                                              // ArenaListNode
  int  (*context)(                                                              // Return true if the parent of the specified child has the specified key.
    const ArenaListNode * child,                                                // Child
    ArenaListNode * const parent,                                               // Parent container
    const char  * const key);                                                   // Key
  void  (*copyData)(                                                            // Copy the data from the source node to the target node.
          ArenaListNode * target,                                               // Target ArenaListNode
    const ArenaListNode source);                                                // Source ArenaListNode
  ArenaListNode   (*copy)(                                                      // Copy a node from one ArenaList to another ArenaList
    const ArenaListNode * source,                                               // Source ArenaListNode
    const ArenaList target);                                                    // Target ArenaList
  size_t  (*countChildren)(                                                     // Count the number of children directly under a node.
    const ArenaListNode * Parent);                                              // Parent
  size_t  (*count)(                                                             // Count the number of nodes under a specified node but not including the specified node.
    const ArenaListNode * node);                                                // ArenaListNode
  ArenaListNode  (*cut)(                                                        // Cut out a child.
    const ArenaListNode * child);                                               // Child to cut out
  void   *  (*data)(                                                            // Get a temporary pointer to the user data of a node.
    const ArenaListNode * node);                                                // ArenaListNode
  void  (*deleteChar)(                                                          // Delete the character at the specified position in the key string of a node.
    const ArenaListNode * node,                                                 // ArenaListNode
    size_t pos);                                                                // Position in key. 0 deletes the first character.  No deletion occurs if the requested character is beyond the end of the key string
  void  (*dump)(                                                                // Dump a ArenaListNode on stderr
    const ArenaListNode * Node);                                                // ArenaListNode
  int  (*equalsString)(                                                         // Check that the key of a node equals a string
    const ArenaListNode      * node,                                            // ArenaListNode
    const char * const key);                                                    // Key
  int  (*equals)(                                                               // Confirm two nodes are equal
    const ArenaListNode * a,                                                    // First offset
    const ArenaListNode b);                                                     // Second offset
  ArenaListNode  (*findFirstChild)(                                             // Find the first child of the specified parent with the specified key.
    const ArenaListNode * Parent,                                               // Parent node
    const char  * const key);                                                   // Key to find immediately under parent
  ArenaListNode  (*findFirst)(                                                  // Find the first node with the specified key in a post-order traversal of the ArenaList starting at the specified node.
    const ArenaListNode * node,                                                 // ArenaListNode at the start of the list to be searched
    const char  * const key);                                                   // Key to find
  ArenaListNode  (*first)(                                                      // Get the first child under a parent.
    const ArenaListNode * parent);                                              // Parent
  void  (*free)(                                                                // Free a node. If the ArenaList is editable the node is made available for reuse otherwise the node wastes space. A new copy of the ArenaList without wasted space can be made with copyAndCompact_ArenaList .
    ArenaListNode * node);                                                      // ArenaListNode to free
  void  (*getData)(                                                             // Get the optional user data associated with a node in an ArenaList
    const ArenaListNode * node,                                                 // ArenaListNode
    void * const data);                                                         // Data
  ArenaListNode  (*highest)(                                                    // Find an example of the highest key under a parent node
    const ArenaListNode * Parent);                                              // ArenaListNode
  size_t  (*index)(                                                             // The index of the specified child under its parent counting from one.
    const ArenaListNode * child);                                               // Child
  void  (*insertChar)(                                                          // Insert the specified character into the key string of a node at the specified position.
    ArenaListNode    * node,                                                    // ArenaListNode
    const char ins,                                                             // Character to insert
    size_t pos);                                                                // Position in key. 0 prepends the char, while >= length appends the char.
  int  (*invalid)(                                                              // Check that a node is not valid.
    const ArenaListNode * child);                                               // ArenaListNode
  int  (*isEmpty)(                                                              // Confirm a node has no children.
    const ArenaListNode   * node);                                              // ArenaListNode
  int  (*isFirst)(                                                              // Confirm a child is first under its parent
    const ArenaListNode * child);                                               // Child
  int  (*isLast)(                                                               // Confirm a child is last under its parent
    const ArenaListNode * child);                                               // Child
  int  (*isOnlyChild)(                                                          // Confirm that this child is the only child of its parent
    const ArenaListNode    * child);                                            // Child
  int  (*isRoot)(                                                               // Confirm a node is the root
    const ArenaListNode * node);                                                // ArenaListNode
  int  (*keyEqualsString)(                                                      // Confirm that the key of a node is equal to the specified zero terminated string
    const ArenaListNode * node,                                                 // ArenaListNode whose key is to be checked
    const char  * const key);                                                   // Expected key value
  int  (*keyEquals)(                                                            // Confirm that the key of a node is equal to the specified memory
    const ArenaListNode * node,                                                 // ArenaListNode whose key is to be checked
    const char  * const key,                                                    // Expected key value
    const size_t length);                                                       // Length of expected key value
  char *  (*key)(                                                               // Get a temporary pointer to the key of a node.
    const ArenaListNode * node);                                                // ArenaListNode
  ArenaListNode  (*last)(                                                       // Get the last child under a parent.
    const ArenaListNode * parent);                                              // Parent
  size_t    (*length)(                                                          // Get the length of the key associated with a node
    const ArenaListNode * node);                                                // ArenaListNode
  ArenaListNode  (*lowest)(                                                     // Find an example of the lowest key under a parent node
    const ArenaListNode * Parent);                                              // ArenaListNode
  size_t    (*maxLength)(                                                       // Get the maximum allowed length of the key associated with a node.
    const ArenaListNode * node);                                                // ArenaListNode
  ArenaListNode  (*next)(                                                       // Get the next child under a parent.
    const ArenaListNode * parent);                                              // Parent
  int  (*notEquals)(                                                            // Confirm two nodes are not equal
    const ArenaListNode * a,                                                    // First offset
    const ArenaListNode b);                                                     // Second offset
  ArenaListNode  (*parent)(                                                     // Get the parent of a child
    const ArenaListNode * child);                                               // Child
  ArenaListPosition  (*preOrderPosition)(                                       // Position of a node in pre-order - nodes are counted from one in preorder, bytes are the number of preceding bytes.
    const ArenaListNode * node);                                                // ArenaListTag
  ArenaListNode  (*prev)(                                                       // Get the prev child under a parent.
    const ArenaListNode * parent);                                              // Parent
  int  (*printContains)(                                                        // Check the print of an ArenaList starting at the specified tag contains the expected string.
    const ArenaListNode * node,                                                 // Starting node
    const char  * expected);                                                    // Expected string
  void  (*printWithBrackets)(                                                   // Apply a function to a string containing a print of the specified  node and the ArenaList below it in preorder as a string with each sub ArenaList enclosed in brackets.
    const ArenaListNode * node,                                                 // ArenaListNode
    void (*printer) (char * string, size_t length));                            // Function to apply to printed ArenaList
  void  (*print)(                                                               // Apply a function to the print of a ArenaListNode and the tree below it.
    const ArenaListNode * node,                                                 // ArenaListNode
    void (*printer) (char * string, size_t length));                            // Function to apply to printed ArenaList
  int  (*printsAs)(                                                             // Check that the specified node prints as expected.
    const ArenaListNode * node,                                                 // ArenaList
    const char  * const expected);                                              // Expected string when printed
  int  (*printsWithBracketsAs)(                                                 // Check that the ArenaList starting at the specified node prints with brackets as expected.
    const ArenaListNode * node,                                                 // ArenaListNode
    const char  * const expected);                                              // Expected string when printed
  ArenaListNode    (*putFirst)(                                                 // Put a child first under its parent
    const ArenaListNode * parent,                                               // Parent
    const ArenaListNode child);                                                 // Child
  ArenaListNode    (*putLast)(                                                  // Put a child last under its parent
    const ArenaListNode * parent,                                               // Parent
    const ArenaListNode child);                                                 // Child
  ArenaListNode    (*putNext)(                                                  // Put a child next after the specified sibling
    const ArenaListNode * sibling,                                              // Sibling
    const ArenaListNode child);                                                 // Child
  ArenaListNode    (*putPrev)(                                                  // Put a child previous after the specified sibling
    const ArenaListNode * sibling,                                              // Sibling
    const ArenaListNode child);                                                 // Child
  ArenaListNode    (*putTreeFirst)(                                             // Put a child first in the ArenaList containing the arena in which the child was allocated.
    const ArenaListNode * child);                                               // Child
  ArenaListNode    (*putTreeLast)(                                              // Put a child last in the ArenaList containing the arena in which the child was allocated.
    const ArenaListNode * child);                                               // Child
  void  (*replaceChar)(                                                         // Replace the character at the specified position in the key string of a node with the specified character.
    const ArenaListNode * node,                                                 // ArenaListNode
    const char repl,                                                            // Replacement character
    size_t pos);                                                                // Position in key. 0 replaces the first character.  No replacement occurs if the requested character is beyond the end of the key string
  void  (*replace)(                                                             // Replace the specified node with this node
    const ArenaListNode * with,                                                 // Replace with this node
    const ArenaListNode over);                                                  // Node to be replaced
  ArenaListNode  (*root)(                                                       // Return the root node of the ArenaList containing the specified node.
    const ArenaListNode node);                                                  // ArenaListNode
  void  (*scanFrom)(                                                            // Traverse the ArenaList starting at the specified node in post-order calling the specified function to process each child node continuing through the siblings of all the specified node's ancestors.  The ArenaList is buffered allowing changes to be made to the structure of the ArenaList without disruption as long as each child checks its context.
    ArenaListNode * node,                                                       // ArenaListNode
    int (* const sub) (ArenaListNode node, int start, int depth),               // Function: start is set to +1 before the children are processed, -1 afterwards. if the parent has no children the sub is called once with start set to zero.  The funstion should return true if processing should continue, else false.
    int close);                                                                 // Start at the close tag of a non singleton tag if true otherwise start at the open.
  void  (*scan)(                                                                // Traverse the ArenaList rooted at the specified node calling the specified function before(+1) and after(-1) processing the children of each node - or - if the node has no children the function is called once(0) . The ArenaList is buffered allowing changes to be made to the structure of the ArenaList without disruption as long as each child checks its context.
    const ArenaListNode * node,                                                 // ArenaListNode
    void (* const function) (ArenaListNode node, int start, int depth));        // Function: start is set to +1 before the children are processed, -1 afterwards. if the parent has no children the function is called once with start set to zero.
  void  (*setData)(                                                             // Set the optional user data associated with a node in an ArenaList
    ArenaListNode       * node,                                                 // ArenaListNode
    const void  * const data);                                                  // Data
  void  (*setKey)(                                                              // Set the key of a node to a different value as long as there is enough room from the last setting.
    ArenaListNode      * node,                                                  // ArenaListNode
    const void * const key,                                                     // Key
    const size_t length);                                                       // Length of key
  void  (*setUp)(                                                               // Make the specified parent node the parent of the specified child node
    const ArenaListNode child,                                                  // Child
    const ArenaListNode parent);                                                // Child
  void  (*sort)(                                                                // Quick sort the children of a node in the ArenaList in situ
    const           ArenaListNode * parent);                                    // Parent node
  void  (*splitKey)(                                                            // Split the specified node at the specified position.
    const ArenaListNode * Node,                                                 // ArenaListNode
    const size_t pos);                                                          // Position in key at which to split.
  void  (*swapChars)(                                                           // Swap the characters on either side of the specified position if it is between 1 and length - 1.
    const ArenaListNode * node,                                                 // ArenaListNode
    size_t pos);                                                                // Position in key. 1 swaps the first two characters.  length - 1 swaps the last two characters.
  ArenaListNode  (*unwrap)(                                                     // Unwrap the specified parent and return it.
    const ArenaListNode * parent);                                              // Parent to unwrap
  int  (*valid)(                                                                // Check that a node is valid.
    const ArenaListNode * child);                                               // ArenaListNode
  size_t  (*width)(                                                             // Get the width of the data area for a node
    const ArenaListNode * node);                                                // ArenaListNode
  ArenaListNode  (*wrap)(                                                       // Wrap the specified child with a new parent and return the new parent optionally setting its L[key] and L[value].
    const ArenaListNode * child,                                                // Child to wrap
    const char  * const key);                                                   // Key for new parent
 } const ProtoTypes_ArenaListNode =
{at_ArenaListNode__ArenaListNode_size, by__ArenaListNode_sub, cmp_int__ArenaList_ArenaList, content__ArenaListNode, context_int__ArenaListNode_ArenaListNode_string, copyData_ArenaListNode_ArenaListNode, copy_ArenaListNode__ArenaListNode_ArenaList, countChildren_size__ArenaListNode, count_size__ArenaListNode, cut_ArenaListNode__ArenaListNode, data_pointer__ArenaListNode, deleteChar__ArenaListNode_size, dump__ArenaListNode, equalsString_int__ArenaListNode_string, equals_int__ArenaListNode_ArenaListNode, findFirstChild_ArenaListNode__ArenaListNode_string, findFirst_ArenaListNode__ArenaListNode_string, first_ArenaListNode__ArenaListNode, free__ArenaListNode, getData_ArenaListNode_pointer, highest_ArenaListNode__ArenaListNode, index__ArenaListNode, insertChar__ArenaListNode_char_size, invalid_int__ArenaListNode, isEmpty_int__ArenaListNode, isFirst_int__ArenaListNode, isLast_int__ArenaListNode, isOnlyChild_int__ArenaListNode, isRoot_int__ArenaListNode, keyEqualsString_int__ArenaListNode_pointer, keyEquals_int__ArenaListNode_pointer_size, key_pointer__ArenaListNode, last_ArenaListNode__ArenaListNode, length_size__ArenaListNode, lowest_ArenaListNode__ArenaListNode, maxLength_size__ArenaListNode, next_ArenaListNode__ArenaListNode, notEquals_int__ArenaListNode_ArenaListNode, parent_ArenaListNode__ArenaListNode, preOrderPosition__ArenaListTag, prev_ArenaListNode__ArenaListNode, printContains_int__ArenaListNode, printWithBrackets__ArenaListNode_function, print__ArenaListNode_function, printsAs_int__ArenaListNode_string, printsWithBracketsAs_int__ArenaListNode_string, putFirst_ArenaListNode__ArenaListNode_ArenaListNode, putLast_ArenaListNode__ArenaListNode_ArenaListNode, putNext_ArenaListNode__ArenaListNode_ArenaListNode, putPrev_ArenaListNode__ArenaListNode_ArenaListNode, putTreeFirst_ArenaListNode__ArenaListNode, putTreeLast_ArenaListNode__ArenaListNode, replaceChar__ArenaListNode_size, replace__ArenaListNode_ArenaListNode, root_ArenaListNode__ArenaListNode, scanFrom__ArenaListNode_sub_int, scan__ArenaListNode_sub, setData_ArenaListNode_pointer, setKey__ArenaListNode_string_size, setUp__ArenaListNode_ArenaListNode, sort__ArenaListNode, splitKey_ArenaListNode_ArenaListNode, swapChars__ArenaListNode_size, unwrap_ArenaListNode__ArenaListNode, valid_int__ArenaListNode, width_size__ArenaListNode, wrap_ArenaListNode__string};
ArenaListNode newArenaListNode(ArenaListNode allocator) {return allocator;}

struct ProtoTypes_ArenaListNodeAndState {
 } const ProtoTypes_ArenaListNodeAndState =
{};
ArenaListNodeAndState newArenaListNodeAndState(ArenaListNodeAndState allocator) {return allocator;}

struct ProtoTypes_ArenaListPosition {
  void  (*dumpPosition)(                                                        // Dump a ArenaListPosition
    const ArenaListPosition * position);                                        // ArenaListPosition
  int  (*equalsPosition)(                                                       // Confirm two positions are equal
    const ArenaListPosition * a,                                                // First position
    const ArenaListPosition b);                                                 // Second position
 } const ProtoTypes_ArenaListPosition =
{dumpPosition__ArenaListPosition, equalsPosition_int__ArenaListPosition_ArenaListPosition};
ArenaListPosition newArenaListPosition(ArenaListPosition allocator) {return allocator;}

