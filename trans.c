
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>

#define MAX_FILES 1024
#define MAX_FILENAME_LENGTH 1024
#define MAX_LINE_LENGTH 1024

/**
 * @brief Сохраняет список файлов в массив.
 *
 * Рекурсивно обходит директорию, находит файлы с заданным суффиксом и сохраняет их пути в массиве.
 *
 * @param dir_path Путь к директории для поиска файлов.
 * @param file_names_array Массив для сохранения имён файлов.
 * @param file_count Указатель на переменную, в которой будет храниться количество файлов.
 * @param file_name_length Максимальная длина имени файла.
 * @param suffin_en Суффикс, по которому идентифицируются файлы.
 */
void save_files_in_array(const char *dir_path, char ***file_names_array, int *file_count, int file_name_length, const char *suffin_en);

/**
 * @brief Сохраняет путь к директории из аргументов командной строки.
 *
 * Если путь не указан, используется текущая директория.
 *
 * @param argc Количество аргументов.
 * @param argv Аргументы командной строки.
 * @param path_name Массив для хранения пути.
 */
void save_path_name(int argc, char *argv[], char path_name[]);

/**
 * @brief Находит количество файлов и максимальную длину имени файла.
 *
 * Функция рекурсивно обходит директорию, считая файлы с заданным суффиксом и переведённые файлы, и определяет максимальную длину имени файла.
 *
 * @param dir_path Путь к директории.
 * @param numbers_files_for_translated Указатель на количество файлов для перевода.
 * @param number_of_translated_files Указатель на количество уже переведённых файлов.
 * @param max_string_size Указатель на максимальный размер строки (имени файла).
 * @param suffin_en Суффикс, по которому идентифицируются файлы для перевода.
 */
void find_the_number_of_files_and_the_maximum_filename_length(const char *dir_path, int *numbers_files_for_translated, int *number_of_translated_files, int *max_string_size, const char *suffin_en);

/**
 * @brief Проверяет, заканчивается ли строка заданным суффиксом.
 *
 * @param str Строка, которую нужно проверить.
 * @param suffin_en Суффикс, который должен быть в конце строки.
 * @return true, если строка заканчивается на суффикс, иначе false.
 */
bool ends_with(const char *str, const char *suffin_en);

/**
 * @brief Проверяет, состоит ли строка только из цифр.
 *
 * @param str Строка для проверки.
 * @return 1, если строка содержит только цифры, иначе 0.
 */
int is_all_digits(const char *str);

/**
 * @brief Переводит содержимое файла и записывает результат в новый файл.
 *
 * Использует команду `trans` для перевода каждой строки из файла, если она не является номером или меткой времени.
 *
 * @param input_filename Имя файла для чтения.
 * @param output_filename Имя файла для записи результата.
 */
void translate_and_write(const char *input_filename, const char *output_filename);

/**
 * @brief Добавляет символ подчёркивания к имени файла.
 *
 * @param originalFilename Исходное имя файла.
 */
void add_underscore_to_filename(const char *originalFilename);

/**
 * @brief Заменяет подстроку в строке.
 *
 * Заменяет первое вхождение старой подстроки на новую.
 *
 * @param originalString Исходная строка.
 * @param oldSubstring Подстрока для замены.
 * @param newSubstring Подстрока для вставки.
 * @return Новая строка с заменённой подстрокой.
 */
char *replaceSubstring(const char *originalString, const char *oldSubstring, const char *newSubstring);

int main(int argc, char **argv)
{
    // Инициализация переменных для подсчета файлов и хранения данных о них
    int numbers_files_for_translated = 0; // Количество файлов, подлежащих переводу
    int number_of_translated_files = 0;   // Количество уже переведенных файлов
    int max_string_size = 0;              // Максимальная длина имени файла
    char path_name[1024];                 // Путь к директории
    int file_count = 0;                   // Счетчик файлов для перевода
    char suffin_en[] = "_en.srt";         // Суффикс файлов на английском языке
    char suffin_ru[] = "_ru.srt";         // Суффикс для переведенных файлов на русском

    // Сохранение пути к директории с файлами
    save_path_name(argc, argv, path_name);

    // Подсчет количества файлов для перевода, переведенных файлов и максимальной длины имени файла
    find_the_number_of_files_and_the_maximum_filename_length(path_name,
                                                             &numbers_files_for_translated, &number_of_translated_files, &max_string_size, suffin_en);

    // Если есть переведенные файлы, вывести их количество
    if (number_of_translated_files)
    {
        printf("\033[34m Переведенных файлов %d \033[0m \n", number_of_translated_files);
    }

    // Если файлов для перевода нет, вывести сообщение и завершить программу
    if (numbers_files_for_translated == 0)
    {
        puts("Нет файлов для перевода");
        return EXIT_FAILURE; // Завершение программы с кодом ошибки
    }

    // Выделение памяти для массива имен файлов, подлежащих переводу
    char **file_names_array = malloc(numbers_files_for_translated * sizeof(char *));
    if (file_names_array == NULL)
    {
        perror("Ошибка при выделении памяти");
        return EXIT_FAILURE; // Если память не удалось выделить, завершить программу с ошибкой
    }

    // Сохранение имен файлов для перевода в массив
    save_files_in_array(path_name, &file_names_array, &file_count, max_string_size, suffin_en);

    // Цикл обработки каждого файла
    for (int i = 0; i < file_count; i++)
    {
        // Сообщение о начале перевода текущего файла
        printf("\033[33m Переводится файл %d из %d \033[0m \n", i + 1, file_count);

        // Перевод файла и сохранение его с новым именем, где суффикс _en.srt заменяется на _ru.srt
        translate_and_write(file_names_array[i], replaceSubstring(file_names_array[i], suffin_en, suffin_ru));

        // Добавление символа подчеркивания к оригинальному имени файла
        add_underscore_to_filename(file_names_array[i]);

        // Освобождение памяти для имени текущего файла
        free(file_names_array[i]);
    }

    puts(""); // Пустая строка для разделения вывода

    // Освобождение памяти, выделенной для массива имен файлов
    free(file_names_array);

    return EXIT_SUCCESS; // Завершение программы с кодом успешного выполнения
}

void save_files_in_array(const char *dir_path, char ***file_names_array, int *file_count, int file_name_length, const char *suffin_en)
{
    DIR *dir = opendir(dir_path); // Открываем директорию

    if (dir == NULL)
    {
        perror("Не удалось открыть директорию"); // Обработка ошибки при открытии директории
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    // Цикл для чтения содержимого директории
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR) // Если это поддиректория
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                // Создаём новый путь для рекурсивного вызова
                char new_path[file_name_length];
                snprintf(new_path, sizeof(new_path), "%s/%s", dir_path, entry->d_name);

                // Рекурсивно обрабатываем поддиректорию
                save_files_in_array(new_path, file_names_array, file_count, file_name_length, suffin_en);
            }
        }
        else
        {
            // Проверяем, заканчивается ли имя файла на заданный суффикс
            if (ends_with(entry->d_name, suffin_en))
            {
                (*file_names_array)[*file_count] = malloc(file_name_length); // Выделяем память для хранения имени файла
                if ((*file_names_array)[*file_count] == NULL)
                {
                    perror("Ошибка при выделении памяти");
                    exit(EXIT_FAILURE);
                }

                // Формируем полный путь к файлу и сохраняем его
                snprintf((*file_names_array)[*file_count], file_name_length, "%s/%s", dir_path, entry->d_name);
                (*file_count)++; // Увеличиваем счётчик файлов

                // Проверка на превышение максимального количества файлов
                if (*file_count >= MAX_FILES)
                {
                    fprintf(stderr, "Превышено максимальное количество файлов\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    closedir(dir); // Закрываем директорию
}

void save_path_name(int argc, char *argv[], char path_name[])
{
    // Если пользователь передал аргумент с путём, сохраняем его, иначе используем текущую директорию
    if (argc > 1)
    {
        sprintf(path_name, "%s", argv[1]);
    }
    else
    {
        sprintf(path_name, "."); // Текущая директория по умолчанию
    }
}

void find_the_number_of_files_and_the_maximum_filename_length(const char *dir_path, int *numbers_files_for_translated, int *number_of_translated_files, int *max_string_size, const char *suffin_en)
{
    // Открываем директорию по заданному пути
    DIR *dir = opendir(dir_path);
    struct dirent *entry;

    // Проверяем, удалось ли открыть директорию
    if (dir == NULL)
    {
        // Если не удалось, выводим сообщение об ошибке и выходим
        perror("Ошибка при открытии директории");
        exit(EXIT_FAILURE);
    }

    // Читаем содержимое директории
    while ((entry = readdir(dir)) != NULL)
    {
        // Проверяем, является ли элемент поддиректорией
        if (entry->d_type == DT_DIR)
        {
            // Игнорируем текущую и родительскую директории ('.' и '..')
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue; // Пропускаем их
            }

            // Создаем путь для поддиректории
            char subpath[1024];
            snprintf(subpath, sizeof(subpath), "%s/%s", dir_path, entry->d_name);

            // Рекурсивно вызываем функцию для обработки поддиректории
            find_the_number_of_files_and_the_maximum_filename_length(subpath, numbers_files_for_translated, number_of_translated_files, max_string_size, suffin_en);
        }
        else if (entry->d_type == DT_REG)
        {
            // Если файл уже переведен (заканчивается на '_'), увеличиваем счетчик переведенных файлов
            if (ends_with(entry->d_name, "_"))
            {
                (*number_of_translated_files)++;
            }

            // Если файл подходит для перевода (заканчивается на заданный суффикс)
            if (ends_with(entry->d_name, suffin_en))
            {
                (*numbers_files_for_translated)++; // Увеличиваем количество файлов для перевода

                // Рассчитываем полный размер пути к файлу
                int full_size = snprintf(NULL, 0, "%s/%s", dir_path, entry->d_name) + 1;

                // Если размер больше максимального зарегистрированного, обновляем максимальный размер
                if (full_size > *max_string_size)
                {
                    *max_string_size = full_size;
                }
            }
        }
    }

    // Закрываем директорию
    closedir(dir);
}

bool ends_with(const char *str, const char *suffin_en)
{
    // Проверяем, что строка и суффикс не пустые
    if (str == NULL || suffin_en == NULL)
    {
        printf("\x1b[31m Не задана строка или суффикс! \x1b[0m\n");
        exit(1); // Выходим с ошибкой, если параметры некорректны
    }

    // Определяем длину строки и суффикса
    size_t str_len = strlen(str);
    size_t suffin_en_len = strlen(suffin_en);

    // Если длина строки меньше длины суффикса, строка не может оканчиваться этим суффиксом
    if (str_len < suffin_en_len)
    {
        return false;
    }

    // Указатель на начало предполагаемого суффикса в строке
    const char *str_suffin_en = str + (str_len - suffin_en_len);

    // Сравниваем конец строки с суффиксом
    return strcmp(str_suffin_en, suffin_en) == 0; // Возвращаем true, если строка заканчивается на заданный суффикс
}

int is_all_digits(const char *str)
{
    // Проходим по всем символам строки
    while (*str)
    {
        // Проверяем, является ли текущий символ цифрой
        if (!isdigit((unsigned char)*str))
        {
            return 0; // Если найден нецифровой символ, возвращаем 0 (ложь)
        }
        str++; // Переходим к следующему символу
    }
    return 1; // Если все символы цифры, возвращаем 1 (истина)
}

void translate_and_write(const char *input_filename, const char *output_filename)
{
    // Открываем файл для чтения
    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL)
    {
        perror("Не удалось открыть файл для чтения");
        return; // Выходим, если не удалось открыть файл
    }

    // Открываем файл для записи
    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL)
    {
        perror("Не удалось открыть файл для записи");
        fclose(input_file); // Закрываем файл ввода
        return;
    }

    FILE *fp; // Для работы с внешней командой

    // Получаем текущее время для отслеживания продолжительности операции
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    char line[MAX_LINE_LENGTH];   // Буфер для строки из файла
    char buffer[MAX_LINE_LENGTH]; // Буфер для результата перевода

    // Читаем файл построчно
    while (fgets(line, sizeof(line), input_file) != NULL)
    {
        // Ищем и убираем символ новой строки
        char *newline = strchr(line, '\n');
        if (newline != NULL)
        {
            *newline = '\0';
        }

        // Проверяем, состоит ли строка только из цифр (например, для номеров субтитров)
        if (is_all_digits(line))
        {
            fprintf(output_file, "%s\n", line); // Записываем строку без изменений
            continue;                           // Переходим к следующей строке
        }

        // Если строка содержит метку времени для субтитров (например, "00:01:23 --> 00:01:26")
        if (strstr(line, "-->") != NULL)
        {
            fprintf(output_file, "%s\n", line); // Записываем строку без изменений
            continue;                           // Переходим к следующей строке
        }

        // Если строка пустая, просто записываем пустую строку
        if (strlen(line) == 0)
        {
            fprintf(output_file, "\n");
            continue; // Переходим к следующей строке
        }

        // Формируем команду для перевода строки с помощью внешнего инструмента (trans)
        char command[] = "trans en:ru -b \"";
        strcat(command, line); // Добавляем строку в команду
        strcat(command, "\"");

        // Открываем процесс для выполнения команды перевода
        fp = popen(command, "r");
        if (fp == NULL)
        {
            printf("Ошибка открытия процесса trans\n");
            fclose(input_file); // Закрываем файлы
            fclose(output_file);
            exit(EXIT_FAILURE);
        }

        // Считываем результат перевода из процесса
        if (fgets(buffer, sizeof(buffer), fp) != NULL)
        {
            fprintf(output_file, "%s", buffer); // Записываем переведенную строку
        }

        // Закрываем процесс
        pclose(fp);
    }

    // Закрываем файлы
    fclose(input_file);
    fclose(output_file);

    // Получаем время завершения и вычисляем длительность операции
    gettimeofday(&end_time, NULL);
    long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000 +
                        (end_time.tv_usec - start_time.tv_usec) / 1000;

    // Выводим время выполнения
    printf("\033[35m Переведено за %ld,%ld секунд \033[0m\n", elapsed_time / 1000, elapsed_time % 1000);
}

void add_underscore_to_filename(const char *originalFilename)
{
    if (originalFilename == NULL)
    {
        printf("Ошибка: не указано имя файла.\n");
        return; // Возвращаемся, если имя файла не указано
    }

    // Выделяем память для нового имени файла (добавляем +2 для символа подчеркивания и конца строки)
    char *newFilename = malloc(strlen(originalFilename) + 2);
    if (newFilename == NULL)
    {
        printf("Ошибка: не удалось выделить память.\n");
        return;
    }

    // Копируем исходное имя файла
    strcpy(newFilename, originalFilename);

    // Добавляем символ подчеркивания к новому имени
    strcat(newFilename, "_");

    // Переименовываем файл с помощью функции rename
    if (rename(originalFilename, newFilename) != 0)
    {
        perror("Ошибка переименования файла");
    }

    // Освобождаем память
    free(newFilename);
}

char *replaceSubstring(const char *originalString, const char *oldSubstring, const char *newSubstring)
{
    // Проверяем, что все входные строки не равны NULL.
    if (originalString == NULL || oldSubstring == NULL || newSubstring == NULL)
    {
        return NULL; // Если хоть один из параметров равен NULL, возвращаем NULL.
    }

    // Ищем вхождение подстроки `oldSubstring` в строке `originalString`.
    const char *foundSubstring = strstr(originalString, oldSubstring);
    if (foundSubstring == NULL)
    {
        // Если подстрока `oldSubstring` не найдена в строке `originalString`, возвращаем копию исходной строки.
        char *result = malloc(strlen(originalString) + 1); // Выделяем память для копии строки.
        strcpy(result, originalString);                    // Копируем исходную строку в `result`.
        return result;                                     // Возвращаем копию строки.
    }

    // Рассчитываем длину префикса — части строки до найденной подстроки.
    size_t prefixLength = foundSubstring - originalString;

    // Рассчитываем длину суффикса — части строки после подстроки `oldSubstring`.
    size_t suffixLength = strlen(foundSubstring + strlen(oldSubstring));

    // Выделяем память для новой строки: сумма длин префикса, новой подстроки и суффикса, плюс один для нуль-терминатора.
    char *result = malloc(prefixLength + strlen(newSubstring) + suffixLength + 1);

    // Копируем префикс — часть строки до подстроки `oldSubstring` в новую строку.
    strncpy(result, originalString, prefixLength);

    // Вставляем новую подстроку вместо старой подстроки `oldSubstring`.
    strcpy(result + prefixLength, newSubstring);

    // Копируем оставшуюся часть строки (суффикс) после `oldSubstring` в новую строку.
    strcpy(result + prefixLength + strlen(newSubstring), foundSubstring + strlen(oldSubstring));

    // Возвращаем указатель на новую строку с замененной подстрокой.
    return result;
}
