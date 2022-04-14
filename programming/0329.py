month_eng = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug","Sept", "Oct", "Nov", "Dec"]
month_num = [*range(1,13)]
dic = { name:value for name, value in zip(month_num, month_eng) }


day = input("날짜를 입력하세요")
day_year = day.index("년")
day_month = day.index("월")
day_day = day.index("일")

print(f"{day[day_day-2:day_day]}, {dic[int(day[day_month-2:day_month])]}, {day[day_year-4:day_year]}")

