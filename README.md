# Исследовательский проект (project with research goal)

Внимание: Не запускать! (Wartging: do not run!)
Программа умеет обходить защиту антивируса Касперского предыдущих версий.

Симуляция цифрового организма. Цель кода - распространение без нанесения ущерба.
Контейнером исполняемого кода может являться исполняемый (exe) или любой другой файл (doc, xls, txt).
Распространение выполняется на локальные файлы и с некоторой вероятностью на внешние носители.

Не запускать бинарный файл, если только на сервере не имеете Антидот.

- Антидот: содать файл с содержимым volNumber*3^0xd59EC64A
	volNumber узнается cmd.exe команда vol.
	4 байта младший байт следуют первыми
- Запуск с командной строкой сливает файл указанный в параметре с файлом кода. Полученный файл имеет то же расширение что и исходный и его нужно менять. Посли подготовки слияния дальнейшая работа кода определяется п.1
Второй параметр заключать в кавычки не надо даже при наличии пробелов в пути к файлу.