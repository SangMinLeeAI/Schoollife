def find_level(height, weight):
    bmi = weight / ((height/100) ** 2)
    if bmi < 18.5:
        return f"bmi: {bmi}, 저체중입니다. 더 열심히 드세요"
    elif bmi < 22.9:
        return f"bmi: {bmi}, 정상입니다 "
    elif bmi < 29.9:
        return f"bmi: {bmi}, 1딘계 비만입니다 "
    elif bmi < 34.9:
        return f"bmi: {bmi}, 2딘계 비만입니다 "
    else:
        return f"bmi: {bmi}, 3딘계 비만입니다 "


height = int(input("키를 입력하세요, cm단위로 입력해주세요"))
weight = int(input("몸무게를 입력하세요, kg단위로 입력해주세요"))

print(find_level(height, weight))