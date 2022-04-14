import sys
import math
a, b, c = map(int,sys.stdin.readline().split())
print(math.ceil((c-a) / (a-b)) + 1)
