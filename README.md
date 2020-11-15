![Test](https://github.com/philiprbrenan/C/workflows/Test/badge.svg)

Use the Minimal Operator Preprocessor:

https://metacpan.org/pod/Preprocess::Ops

to make ANSI-C code object oriented.

See the last few lines of :

https://github.com/philiprbrenan/C/blob/master/c/z/arenaList/arenaList.c

for a working example.

(We reject: kings, presidents and voting. We believe in: rough consensus and running code. https://en.wikiquote.org/wiki/David_D._Clark)

## Quicksort

An implementation of QuickSort:

```
static int cmp_int__$_$                                                         //P Compare two keys
 ($Node * First,                                                                // First node
  $Node   second)                                                               // Second node
 {first ◁ *First;
  makeLocalCopyOf$Key(K, L, first);                                             // Key of first node
  makeLocalCopyOf$Key(k, l, second);                                            // Key of second node
  if (l < L) {i ◁ strncmp(K, k, l); return i ? i : +1;}                         // First key longer than second key
  if (L < l) {i ◁ strncmp(K, k, L); return i ? i : -1;}                         // First key shorter then second key
  return strncmp(K, k, L);                                                      // Equal length keys
 }

static void sort__$Node                                                         // Quick sort the children of a node in the $ in situ
 (const           $Node * Parent)                                               // Parent node
 {void sort(      $Node   first, $Node last)                                    // Start and end of range to be sorted which must already be in their correct position
   {next ◁ first ▷ next;                                                        // Parent key
    if (next.offset != last.offset)                                             // Range has more than two nodes
     {for(p ◀ next ▷ next; p.offset != last.offset; p = p ▷ next)               // Partition interior
       {if (p ▷ cmp (next) < 0) next ▷ putPrev(p ▷ cut);                        // Partition around next
       }
      sort(first, next); sort(next,  last);                                     // Sort each partition
     }
   }

  N ◀ 0; p ◁ *Parent; $fe(c, p) if (++N > 3) break;                             // Check for special cases

  if (N > 1)                                                                    // Already sorted if no children or just one child
   {l ◀ p ▷ lowest;  if (!l ▷ isFirst) p ▷ putFirst(l ▷ cut);                   // Place child with  lowest key first
    h ◁ p ▷ highest; if (!h ▷ isLast)  p ▷ putLast (h ▷ cut);                   // Place child with highest key last

    if (N > 3)                                                                  // Already sorted if two or three children
     {p ◀ l;
      for(q ◀ p ▷ next; q ▷ valid; p = q, q = q ▷ next)                         // Sort if still out of order
       {if (q ▷ cmp(p) < 0) {sort(l, h); return;}                               // Lowest and highest are in position so we can use them to to delimit the interior range to be sorted
       }
     }
   }
 }
```

### Test results

```

    char *S = ◉;
aaaa0 aaa1 aa2 a3 acc4
cccc5 bbaa6 ccc7 cc8 c9
bbbb10 bbb11 bb12 b13
14
◉
    a ◁ make$FromWords(S);
  ✓ a ▷ countChildren == 15;

    a ▷ sort;

  ✓ a ▷ countChildren == 15;

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
```
