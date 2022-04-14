import sys
import math
a, b, c = map(int, sys.stdin.readline().split())

i = -1
if b < c:
    i = math.ceil(a // (c -b)) + 1
print(i)