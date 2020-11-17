#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void)
 {  a ≋ 12; b ≋ 12; c ≋ 12;
    a ≈ "aaaa";
    b ≈ "bbbb";
    A ◁ c +≈ a; B ◁ A +≈ b;
    C ◁  c ∼ b;

  ✓ a ≈≈ "aaaa";
  ✓ b ≈≈ "bbbb";
  ✓ b !≈ a;
  ✓ c ≈≈ "aaaabbbb";
  ✓ A ≈≈ b;
  ✓ B ≈≈ "";
  ✓ C ≈≈ b;

  printf(◉);
success
◉
 }
