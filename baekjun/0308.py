import random
import sklearn
from sklearn.neighbors import KNeighborsClassifier


guessing_number = random.randint(1,100)

n = int(input("몇번의 기회를 드릴까요?"))

count = 1
while count <= n:
    input_number = int(input("숫자를 입력하세요"))
    count += 1
    if input_number < guessing_number:
        print(f"{input_number} is too low")
    elif input_number > guessing_number:
        print(f"{input_number} is too high")
    elif input_number == guessing_number:
        print("you are correct")
        break
print("finished")

num = [2,2,3,4,5,5,6,9]
j = 0
for i in num:
    if j < i:
        j = i
print(j)

