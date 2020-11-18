![Test](https://github.com/philiprbrenan/C/workflows/Test/badge.svg)

Use the Minimal Operator Preprocessor:

https://metacpan.org/pod/Preprocess::Ops

to make ANSI-C code object oriented.  See below or the last few lines of:

https://github.com/philiprbrenan/C/blob/master/c/z/arenaList/arenaList.c

for a working example.

## Quicksort

[We reject: kings, presidents and voting. We believe in: rough consensus and running code.](https://en.wikiquote.org/wiki/David_D._Clark)

A non recursive implementation of the QuickSort algorithm:

```
static void sort__$Node                                                         // Quick sort the children of a node in the $ in situ
 (const           $Node * parent)                                               // Parent node
 {typedef struct {$Node   first, last;} Range;                                  // A range of nodes

  stack ◀ makeArenaArray(sizeof(Range));                                        // Arena stack of ranges to use heap rather than local storage

  void range($Node first, $Node last)                                           // Start and end of range to be sorted: the start and end nodes must already be in their correct positions.
   {Range r = {first, last}; s ◁ stack ▷ push; s ◧ r;                           // Push a range onto the stack
   }

  p ◁ *parent; N ◁ p ▷ countChildren;                                           // Check for special cases

  if (N > 1)                                                                    // Already sorted if no children or just one child
   {l ◀ p ▷ lowest;  p ▷ putFirst(l ▷ cut);                                     // Place child with  lowest key first
    h ◁ p ▷ highest; p ▷ putLast (h ▷ cut);                                     // Place child with highest key last

    if (N > 3)                                                                  // Already sorted if two or three children
     {p ◀ l;
      for  (q ◀ p ▷ next; q ▷ valid; p = q, q = q ▷ next)                       // Sort if still out of order
       {if (q ▷ cmp(p) < 0) {range(l, h); break;}                               // Lowest and highest are in position so we can use them to to delimit the interior range to be sorted
       }
     }
   }

  while(stack ▷ notEmpty)                                                       // Perform all the sorts outstanding
   {Range r; s ◁ stack ▷ pop; r ◨ s;                                            // Pop the next range to be sorted off the stack
        next ◁ r.first ▷ next;                                                  // Parent key
    if (next ▷ notEquals(r.last))                                               // Range has more than two nodes
     {for(p ◀ next ▷ next; p ▷ notEquals(r.last); p = p ▷ next)                 // Partition interior
       {if (p ▷ cmp(next) < 0) next ▷ putPrev(p ▷ cut);                         // Partition around next
       }
      range(r.first, next); range(next, r.last);                                // Sort each partition
     }
   }

  stack ▷ free;
 }
```

### Test results

```

    S ◁ "aaaa0 aaa1 aa2 a3 acc4 cccc5 bbaa6 ccc7 cc8 c9 bbbb10 bbb11 bb12 b13 14";

    a ◁ make$FromWords(S);    ✓ a ▷ countChildren == 15;

    a ▷ sort;                 ✓ a ▷ countChildren == 15;

    s ◁ makeStringBuffer(); $fe(A, a) s ▷ addFormat("%s\n", A ▷ key);

  ✓ s ▷ equalsString(◉);
14
a3
aa2
aaa1
aaaa0
acc4
b13
bb12
bbaa6
bbb11
bbbb10
c9
cc8
ccc7
cccc5
◉

    l ◁ a ▷ lowest;   ✓ l ▷ equalsString("14");
    h ◁ a ▷ highest;  ✓ h ▷ equalsString("cccc5");

SUCCESS: All 21 ArenaList tests passed successfully in 506 microseconds

```
