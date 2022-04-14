# 더하기 사이클
def cycle(summation):
    if summation < 10:
        summation = summation * 10 + summation
    else:
        summation = (summation % 10) * 10 + ((summation // 10) + (summation % 10)) % 10
    return summation


count = 1
first = int(input())
while not 0< first < 100:
    first = int(input("다시 입력하세요"))
num = cycle(first)
while num != first:
    num = cycle(num)
    count += 1

print(count)
