#로또게임

import random

def a_sub_b(a,b):
    return [x for x in a if x not in b]

lottery_number = []
for i in range(6):
    lottery_number.append(int(input(f"{i+1}번째 숫자를 입력해주세요, 1~45의 범위의 자연수로 입력해주세요")))
for k in range(len(lottery_number)):
    print(f"Lottery Number({k+1}): {lottery_number[k]}")
#교수님 요구사항
winning_numbers = [15, 23, 29]
winning_numbers.append(random.sample(range(1,46),3))


#올랜덤
# winning_numbers = []
# winning_numbers.append(random.sample(range(1,46),6))

bonus_number = winning_numbers.pop()

matching_num = a_sub_b(lottery_number, winning_numbers)
if len(matching_num) == 0:
    if matching_num.pop() == bonus_number:
        print("first")
    else: print("second")
elif len(matching_num) == 1:
    print("third")
elif len(matching_num) == 2:
    print("4th")
elif len(matching_num) == 3:
    print("5th")
else: print("꽝")



