#자원배분
import math

slot = int(input("슬롯 갯수 입력"))
resource = int(input("resource 양 입력"))
print("Resource assignment:")
print(math.ceil(resource / slot))
for _ in range(slot-1):
    resource -= math.ceil(resource / slot)
    slot -= 1
    print(math.ceil(resource / slot))
print("Assignmnet ends")