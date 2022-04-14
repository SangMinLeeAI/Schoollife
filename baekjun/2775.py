# import sys
#
# t = int(sys.stdin.readline())
#
# for _ in range(t):
#     floor = int(sys.stdin.readline())
#     num = int(sys.stdin.readline())
#     n = [x for x in range(1,num + 1)]
#     for _ in range(floor):
#         for i in range(1,num):
#             n[i] += n[i-1]
#     print(n[-1])
import pyautogui
import time

a = 0
while a <= 85:
    pyautogui.moveRel(20, 30, 0.5)
    pyautogui.moveRel(-20,-30, 0.5)
    pyautogui.click(clicks=1, interval = 20)
    a += 1




