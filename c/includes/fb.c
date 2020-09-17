#line 1 "/home/phil/z/courses/python/fb.c"
main() {for(int i = 1, f, b; i <= 100; f = ++i % 3, b = i % 5)
          printf("%d  %s%s%.0d\n", i, f ? "":"Fizz", b ? "":"Buzz", f&&b?i:0);}
