## Program Description

This C program performs the following tasks:

1. **Directory Path Determination:** If the first command-line argument is specified, the program uses it as the directory path. Otherwise, it operates in the current directory.

2. **Counting the Number of Files for Translation and Maximum Filename Length:** The program recursively traverses the directory and its subdirectories, counting the number of files ending with "_en.srt" and determining the maximum filename length.

3. **Saving File Names for Translation in an Array:** The program saves the names of files for translation in a dynamic array `file_names_array`.

4. **Translation and Writing to a File:** For each file, the program uses the external "trans" command to translate the lines inside the file from English to Russian. The translated text is saved in a new file.

5. **Adding "_" to File Names:** The original file names have "_" added to them, and the files are renamed.

6. **Measuring Execution Time:** The total execution time of the program is measured using the `sys/time.h` library. The time is displayed in milliseconds.

The program uses standard C libraries as well as libraries for file handling (`stdio.h`), string operations (`string.h`), directory operations (`dirent.h`), dynamic memory (`stdlib.h`), time (`sys/time.h`), character operations (`ctype.h`), and processes (`unistd.h`).

It also includes auxiliary functions for working with files, strings, and time.


## Описание программы

Данная программа на языке C выполняет следующие задачи:

1. **Определение пути к директории:** Если в командной строке указан первый аргумент, то программа использует его как путь к директории. В противном случае, программа работает в текущей директории.

2. **Подсчет количества файлов для перевода и максимальной длины имени файла:** Программа рекурсивно проходит по директории и её поддиректориям, подсчитывая количество файлов, оканчивающихся на "_en.srt", и определяет максимальную длину имени файла.

3. **Сохранение имен файлов для перевода в массив:** Программа сохраняет имена файлов для перевода в динамический массив `file_names_array`.

4. **Перевод и запись в файл:** Для каждого файла программа использует внешнюю команду "trans" для перевода строк внутри файла с английского на русский язык. Переведенный текст сохраняется в новый файл.

5. **Добавление символа "_" к именам файлов:** К оригинальным именам файлов добавляется символ "_", и файлы переименовываются.

6. **Измерение времени выполнения:** Измеряется общее время выполнения программы с использованием библиотеки `sys/time.h`. Время выводится в миллисекундах.

Программа использует стандартные библиотеки C, а также библиотеки для работы с файлами (`stdio.h`), строками (`string.h`), директориями (`dirent.h`), динамической памятью (`stdlib.h`), временем (`sys/time.h`), символьными операциями (`ctype.h`), и процессами (`unistd.h`).

Она также включает в себя вспомогательные функции для работы с файлами, строками и временем.


