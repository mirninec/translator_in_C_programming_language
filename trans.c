
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

void save_files_in_array(const char *dir_path, char ***file_names_array, int *file_count, int file_name_length, const char *suffin_en);
void save_path_name(int argc, char *argv[], char path_name[]);
void find_the_number_of_files_and_the_maximum_filename_length(const char *dir_path, int *numbers_files_for_translated, int *number_of_translated_files, int *max_string_size, const char *suffin_en);
bool ends_with(const char *str, const char *suffin_en);
int is_all_digits(const char *str);
void translate_and_write(const char *input_filename, const char *output_filename);
void add_underscore_to_filename(const char *originalFilename);
char *replaceSubstring(const char *originalString, const char *oldSubstring, const char *newSubstring);

int main(int argc, char **argv)
{
    int numbers_files_for_translated = 0;
    int number_of_translated_files = 0;
    int max_string_size = 0;
    char path_name[1024];
    int file_count = 0;
    char suffin_en[] = "_en.srt";
    char suffin_ru[] = "_ru.srt";

    // проверяем задан ли путь к директории, если нет, то работаем в текущей
    save_path_name(argc, argv, path_name);

    // находим количество файлов для переводи и, если есть, уже переведенные и максимальную длину имени файла
    find_the_number_of_files_and_the_maximum_filename_length(path_name, &numbers_files_for_translated, &number_of_translated_files, &max_string_size, suffin_en);

    if (number_of_translated_files)
    { // если есть переведенные файлы, сообщаем их количество
        printf("\033[34m Переведенных файлов %d \033[0m \n", number_of_translated_files);
    }

    if (numbers_files_for_translated == 0)
    {
        puts("Нет файлов для перевода");
        return EXIT_FAILURE;
    }

    // выделяем динамическую память под массив названий файлов
    char **file_names_array = malloc(numbers_files_for_translated * sizeof(char *));

    if (file_names_array == NULL)
    {
        perror("Ошибка при выделении памяти");
        return EXIT_FAILURE;
    }

    // находим количество файлов для перевода и сохраняем в file_names_array
    save_files_in_array(path_name, &file_names_array, &file_count, max_string_size, suffin_en);

    // printf("Файлов для перевода: %d\n", numbers_files_for_translated);

    for (int i = 0; i < file_count; i++)
    {
        printf("\033[33m Переводится файл %d из %d \033[0m \n", i + 1, file_count);
        translate_and_write(file_names_array[i], replaceSubstring(file_names_array[i], suffin_en, suffin_ru));
        add_underscore_to_filename(file_names_array[i]);
        free(file_names_array[i]); // Освобождаем память для каждого элемента массива
    }
    puts("");

    free(file_names_array); // Освобождаем память для массива

    return EXIT_SUCCESS;
}

/*
 * сохраняет в переменную первый аргумент командной строки если он есть, иначе сохраняет "."
 */
void save_path_name(int argc, char *argv[], char path_name[])
{
    if (argc > 1)
    {
        sprintf(path_name, "%s", argv[1]);
    }
    else
    {
        sprintf(path_name, ".");
    }
}

/*
 * находит файлы с заданным суффиксом и сохраняет
 */
void save_files_in_array(const char *dir_path, char ***file_names_array, int *file_count, int file_name_length, const char *suffin_en)
{
    DIR *dir = opendir(dir_path);

    if (dir == NULL)
    {
        perror("Не удалось открыть директорию");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        // Проверяем, является ли элемент директорией
        if (entry->d_type == DT_DIR)
        {
            // Игнорируем "." и ".."
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                // Составляем новый путь к поддиректории
                char new_path[file_name_length];
                snprintf(new_path, sizeof(new_path), "%s/%s", dir_path, entry->d_name);

                // Рекурсивно вызываем функцию для новой поддиректории
                save_files_in_array(new_path, file_names_array, file_count, file_name_length, suffin_en);
            }
        }
        else
        {
            // Проверяем, заканчивается ли файл заданным суффиксом
            if (ends_with(entry->d_name, suffin_en))
            {
                // Выделяем память для нового элемента массива
                (*file_names_array)[*file_count] = malloc(file_name_length);
                if ((*file_names_array)[*file_count] == NULL)
                {
                    perror("Ошибка при выделении памяти");
                    exit(EXIT_FAILURE);
                }

                // Копируем название файла в массив
                snprintf((*file_names_array)[*file_count], file_name_length, "%s/%s", dir_path, entry->d_name);

                (*file_count)++;

                // Проверка на максимальное количество файлов
                if (*file_count >= MAX_FILES)
                {
                    fprintf(stderr, "Превышено максимальное количество файлов\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    closedir(dir);
}
/*
 * функция ищет в указанной директории и всех её поддиректориях файлы с заданным суффиксом
 * и возвращает количество файлов и максимальный размер имени файла
 * включая полный путь
 */
void find_the_number_of_files_and_the_maximum_filename_length(const char *dir_path, int *numbers_files_for_translated, int *number_of_translated_files, int *max_string_size, const char *suffin_en)
{
    DIR *dir = opendir(dir_path);
    struct dirent *entry;

    if (dir == NULL)
    {
        perror("Ошибка при открытии директории");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR)
        {
            // Игнорируем . и ..
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }

            // Формируем полный путь к вложенной директории
            char subpath[1024];
            snprintf(subpath, sizeof(subpath), "%s/%s", dir_path, entry->d_name);

            // Рекурсивно вызываем find_the_number_of_files_and_the_maximum_filename_length для вложенной директории
            find_the_number_of_files_and_the_maximum_filename_length(subpath, numbers_files_for_translated, number_of_translated_files, max_string_size, suffin_en);
        }
        else if (entry->d_type == DT_REG)
        {
            if (ends_with(entry->d_name, "_")) // если файл оканчивается символом "_"
            {
                (*number_of_translated_files)++; // то увеличиваем счетчик уже переведенных файлов
            }
            if (ends_with(entry->d_name, suffin_en)) // если файл оканчивается на суффикс
            {
                // Увеличиваем количество найденных файлов
                (*numbers_files_for_translated)++;

                // Считаем длину имени файла, включая полный путь от корня и
                // увеличиваем на единицу т.к. функция snprintf убирает завершающий '\n'
                int full_size = snprintf(NULL, 0, "%s/%s", dir_path, entry->d_name) + 1;
                if (full_size > *max_string_size)
                {
                    *max_string_size = full_size;
                }
            }
        }
    }

    closedir(dir);
}

/*
 * определяет оканчивается ли строка на подстроку, если да, то возвращает true
 */
bool ends_with(const char *str, const char *suffin_en)
{
    if (str == NULL || suffin_en == NULL)
    {
        printf("\x1b[31m Не задана строка или суффикс! \x1b[0m\n");
        exit(1);
    }

    size_t str_len = strlen(str);
    size_t suffin_en_len = strlen(suffin_en);

    // if (suffin_en_len > str_len)
    // {
    //     printf("\x1b[31m Подстрока не может быть длиннее строки! \x1b[0m\n");
    //     exit(1);
    // }

    const char *str_suffin_en = str + (str_len - suffin_en_len);
    // printf("%s\n",str_suffin_en);

    return strcmp(str_suffin_en, suffin_en) == 0;
}

/*
 * функция проверяет, состоит ли строка str только из цифр.
 */
int is_all_digits(const char *str)
{
    while (*str)
    {
        if (!isdigit((unsigned char)*str))
        {
            return 0; // Не все символы - цифры
        }
        str++;
    }
    return 1; // Все символы - цифры
}

void translate_and_write(const char *input_filename, const char *output_filename)
{
    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL)
    {
        perror("Не удалось открыть файл для чтения");
        return;
    }
    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL)
    {
        perror("Не удалось открыть файл для записи");
        return;
    }
    FILE *fp; // файловый дескриптор для процесса переводчика

    struct timeval start_time, end_time; // секундомер
    gettimeofday(&start_time, NULL);

    char line[MAX_LINE_LENGTH];   // Предполагаем максимальную длину строки в 1024 символов
    char buffer[MAX_LINE_LENGTH]; // буфер где будут сохраняться результаты работы процесса trans

    while (fgets(line, sizeof(line), input_file) != NULL)
    {
        // Удаляем символ новой строки, который добавляется функцией fgets
        char *newline = strchr(line, '\n');
        if (newline != NULL)
        {
            *newline = '\0';
        }

        // Проверяем, состоит ли строка только из цифр
        if (is_all_digits(line))
        {
            // printf("%s\n", line);
            fprintf(output_file, "%s\n", line);
            continue;
        }
        // Проверяем, содержится ли подстрока "-->" в строке
        if (strstr(line, "-->") != NULL)
        {
            // printf("%s\n", line);
            fprintf(output_file, "%s\n", line);
            continue;
        }
        // Проверяем, является ли строка пустой
        if (strlen(line) == 0)
        {
            // printf("\n");
            fprintf(output_file, "\n");
            continue;
        }
        // создаем строку-команду для перевода
        char command[] = "trans en:ru -b \"";
        strcat(command, line);
        strcat(command, "\"");

        // создаем новый процесс
        fp = popen(command, "r");
        if (fp == NULL)
        {
            printf("Ошибка открытия процесса trans\n");
            exit(EXIT_FAILURE);
        }
        // сохраняем переведенную строку в файл
        if (fgets(buffer, sizeof(buffer), fp) != NULL)
        {
            // printf("%s", buffer);
            fprintf(output_file, "%s", buffer);
        }
        // закрываем процесс
        pclose(fp);
    }

    fclose(input_file);
    fclose(output_file);

    gettimeofday(&end_time, NULL);
    // Вычисляем время в миллисекундах
    long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000 +
                        (end_time.tv_usec - start_time.tv_usec) / 1000;

    printf("\033[35m Переведено за %ld,%ld секунд \033[0m\n", elapsed_time / 1000, elapsed_time % 1000);
}

void add_underscore_to_filename(const char *originalFilename)
{
    if (originalFilename == NULL)
    {
        printf("Ошибка: не указано имя файла.\n");
        return;
    }

    // Создаем новую строку для нового имени файла
    char *newFilename = malloc(strlen(originalFilename) + 2); // +1 для символа '_', +1 для '\0'
    if (newFilename == NULL)
    {
        printf("Ошибка: не удалось выделить память.\n");
        return;
    }

    // Копируем оригинальное имя файла в новую строку
    strcpy(newFilename, originalFilename);

    // Добавляем символ '_' в конец нового имени файла
    strcat(newFilename, "_");

    // Переименовываем файл
    if (rename(originalFilename, newFilename) != 0)
    {
        perror("Ошибка переименования файла");
    }
    else
    {
        // printf("Файл переименован. Новое имя: %s\n", newFilename);
    }

    // Освобождаем выделенную память
    free(newFilename);
}

char *replaceSubstring(const char *originalString, const char *oldSubstring, const char *newSubstring)
{
    if (originalString == NULL || oldSubstring == NULL || newSubstring == NULL)
    {
        return NULL; // Возвращаем NULL в случае некорректных входных данных
    }

    // Находим первое вхождение подстроки, которую нужно заменить
    const char *foundSubstring = strstr(originalString, oldSubstring);
    if (foundSubstring == NULL)
    {
        // Подстрока не найдена, возвращаем копию оригинальной строки
        char *result = malloc(strlen(originalString) + 1);
        strcpy(result, originalString);
        return result;
    }

    // Вычисляем длины частей строки перед и после подстроки
    size_t prefixLength = foundSubstring - originalString;
    size_t suffin_enLength = strlen(foundSubstring + strlen(oldSubstring));

    // Выделяем память под новую строку
    char *result = malloc(prefixLength + strlen(newSubstring) + suffin_enLength + 1);

    // Копируем часть строки до подстроки
    strncpy(result, originalString, prefixLength);

    // Копируем новую подстроку
    strcpy(result + prefixLength, newSubstring);

    // Копируем часть строки после подстроки
    strcpy(result + prefixLength + strlen(newSubstring), foundSubstring + strlen(oldSubstring));

    return result;
}
