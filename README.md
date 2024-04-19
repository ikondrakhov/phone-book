# Телефонная Книга

## Сервер

Язык: С++ \

Сервер собирался под:
- Windows (GNU Make 3.81, cmake version 3.29.1)
- Linux (GNU Make 4.3, cmake version 3.22.1)


Сборка (из папки проекта):
```
mkdir build 
cd build
cmake ../server
make
```

Принимает команды:
- ADD name:Имя,surname:Фамилия,middlename:Отчество,phoneNumber:Номер телефона,description:Описание
- DELTE id
- FIND "Название параметра" "Значение параметра"
- FINDALL

Перед каждой командой идет размер передаваемых данных

Примеры:\
92 ADD name:Имя,surname:Фамилия,middlename:Отчество, phoneNumber:+79111111111,description:Описание

9 DELETE 12

14 FIND name Имя

7 FINDALL

Данные сохраняются при перезапуске сервера.\
В качестве хранилища служит файл contacts.db

## Клиент

Клиент представляет собой консольное приложение, которое принимает команды и отправляет их серверу

Язык: Python

Клиент собирался под:
- Windows (Python 3.12.3)
- Linux (Python 3.10.6)

Запуск (из папки проекта):
```
cd client
python main.py
```
