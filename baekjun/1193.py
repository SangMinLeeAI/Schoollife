a = int(input())
n = 0
while(a > n*(n+1)/2) :
    n += 1
j = a - n*(n-1)//2 - 1
if n % 2 != 1:
    print(f"{1+j}/{n-j}")
else:
    print(f"{n - j}/{1 + j}")