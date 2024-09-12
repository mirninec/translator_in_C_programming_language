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

    // Check if the directory path is given; if not, use the current directory.
    save_path_name(argc, argv, path_name);

    // Find the number of files to translate, already translated files (if any), and the maximum filename length.
    find_the_number_of_files_and_the_maximum_filename_length(path_name, &numbers_files_for_translated, &number_of_translated_files, &max_string_size, suffin_en);

    if (number_of_translated_files)
    { // If there are translated files, report their count.
        printf("\033[34m Translated files %d \033[0m \n", number_of_translated_files);
    }

    if (numbers_files_for_translated == 0)
    {
        puts("No files to translate");
        return EXIT_FAILURE;
    }

    // Allocate dynamic memory for the file names array.
    char **file_names_array = malloc(numbers_files_for_translated * sizeof(char *));
    if (file_names_array == NULL)
    {
        perror("Memory allocation error");
        return EXIT_FAILURE;
    }

    // Find the number of files to translate and store them in file_names_array.
    save_files_in_array(path_name, &file_names_array, &file_count, max_string_size, suffin_en);

    // printf("Files to translate: %d\n", numbers_files_for_translated);

    for (int i = 0; i < file_count; i++)
    {
        printf("\033[33m Translating file %d of %d \033[0m \n", i + 1, file_count);
        translate_and_write(file_names_array[i], replaceSubstring(file_names_array[i], suffin_en, suffin_ru));
        add_underscore_to_filename(file_names_array[i]);
        free(file_names_array[i]); // Free memory for each element of the array.
    }
    puts("");

    free(file_names_array); // Free memory for the array.

    return EXIT_SUCCESS;
}

/*
 * Saves the first command line argument to the variable if present, otherwise saves "."
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
 * Finds files with the specified suffix and saves them
 */
void save_files_in_array(const char *dir_path, char ***file_names_array, int *file_count, int file_name_length, const char *suffin_en)
{
    DIR *dir = opendir(dir_path);

    if (dir == NULL)
    {
        perror("Failed to open directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        // Check if the item is a directory
        if (entry->d_type == DT_DIR)
        {
            // Ignore "." and ".."
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                // Form a new path to the subdirectory
                char new_path[file_name_length];
                snprintf(new_path, sizeof(new_path), "%s/%s", dir_path, entry->d_name);

                // Recursively call the function for the new subdirectory
                save_files_in_array(new_path, file_names_array, file_count, file_name_length, suffin_en);
            }
        }
        else
        {
            // Check if the file ends with the specified suffix
            if (ends_with(entry->d_name, suffin_en))
            {
                // Allocate memory for the new element of the array
                (*file_names_array)[*file_count] = malloc(file_name_length);
                if ((*file_names_array)[*file_count] == NULL)
                {
                    perror("Memory allocation error");
                    exit(EXIT_FAILURE);
                }

                // Copy the file name into the array
                snprintf((*file_names_array)[*file_count], file_name_length, "%s/%s", dir_path, entry->d_name);

                (*file_count)++;

                // Check for the maximum number of files
                if (*file_count >= MAX_FILES)
                {
                    fprintf(stderr, "Exceeded the maximum number of files\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    closedir(dir);
}

/*
 * The function searches in the specified directory and all its subdirectories for files with the given suffix
 * and returns the number of files and the maximum filename size, including the full path
 */
void find_the_number_of_files_and_the_maximum_filename_length(const char *dir_path, int *numbers_files_for_translated, int *number_of_translated_files, int *max_string_size, const char *suffin_en)
{
    DIR *dir = opendir(dir_path);
    struct dirent *entry;

    if (dir == NULL)
    {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR)
        {
            // Ignore "." and ".."
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }

            // Form the full path to the nested directory
            char subpath[1024];
            snprintf(subpath, sizeof(subpath), "%s/%s", dir_path, entry->d_name);

            // Recursively call find_the_number_of_files_and_the_maximum_filename_length for the nested directory
            find_the_number_of_files_and_the_maximum_filename_length(subpath, numbers_files_for_translated, number_of_translated_files, max_string_size, suffin_en);
        }
        else if (entry->d_type == DT_REG)
        {
            if (ends_with(entry->d_name, "_")) // if the file ends with the character "_"
            {
                (*number_of_translated_files)++; // increment the count of already translated files
            }
            if (ends_with(entry->d_name, suffin_en)) // if the file ends with the suffix
            {
                // Increment the number of found files
                (*numbers_files_for_translated)++;

                // Calculate the length of the file name, including the full path from the root and
                // add one since snprintf function removes the trailing '\n'
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
 * Determines if the string ends with a substring; if so, returns true
 */
bool ends_with(const char *str, const char *suffin_en)
{
    if (str == NULL || suffin_en == NULL)
    {
        printf("\x1b[31m String or suffix not specified! \x1b[0m\n");
        exit(1);
    }

    size_t str_len = strlen(str);
    size_t suffin_en_len = strlen(suffin_en);

    const char *str_suffin_en = str + (str_len - suffin_en_len);

    return strcmp(str_suffin_en, suffin_en) == 0;
}

/*
 * Function checks if the string str consists only of digits.
 */
int is_all_digits(const char *str)
{
    while (*str)
    {
        if (!isdigit((unsigned char)*str))
        {
            return 0; // Not all characters are digits
        }
        str++;
    }
    return 1; // All characters are digits
}

/*
 * Translates each line of the input file and writes the translated lines to the output file.
 */
void translate_and_write(const char *input_filename, const char *output_filename)
{
    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL)
    {
        perror("Failed to open file for reading");
        return;
    }

    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL)
    {
        perror("Failed to open file for writing");
        fclose(input_file);
        return;
    }

    FILE *fp; // File descriptor for the translator process

    struct timeval start_time, end_time; // Stopwatch
    gettimeofday(&start_time, NULL);

    char line[MAX_LINE_LENGTH];   // Assuming maximum line length is 1024 characters
    char buffer[MAX_LINE_LENGTH]; // Buffer to store the result from the translation process

    while (fgets(line, sizeof(line), input_file) != NULL)
    {
        // Remove the newline character added by fgets
        char *newline = strchr(line, '\n');
        if (newline != NULL)
        {
            *newline = '\0';
        }

        // Check if the line consists only of digits
        if (is_all_digits(line))
        {
            fprintf(output_file, "%s\n", line);
            continue;
        }

        // Check if the line contains the substring "-->"
        if (strstr(line, "-->") != NULL)
        {
            fprintf(output_file, "%s\n", line);
            continue;
        }

        // Check if the line is empty
        if (strlen(line) == 0)
        {
            fprintf(output_file, "\n");
            continue;
        }

        // Create the translation command string
        char command[1024]; // Adjust size as needed
        snprintf(command, sizeof(command), "trans en:ru -b \"%s\"", line);

        // Create a new process
        fp = popen(command, "r");
        if (fp == NULL)
        {
            perror("Error opening the translation process");
            fclose(input_file);
            fclose(output_file);
            return;
        }

        // Save the translated line to the file
        if (fgets(buffer, sizeof(buffer), fp) != NULL)
        {
            fprintf(output_file, "%s", buffer);
        }

        // Close the process
        pclose(fp);
    }

    fclose(input_file);
    fclose(output_file);

    gettimeofday(&end_time, NULL);
    // Calculate the elapsed time in milliseconds
    long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000 +
                        (end_time.tv_usec - start_time.tv_usec) / 1000;

    printf("\033[35m Translated in %ld,%ld seconds \033[0m\n", elapsed_time / 1000, elapsed_time % 1000);
}

/*
 * Adds an underscore to the end of the filename.
 */
void add_underscore_to_filename(const char *originalFilename)
{
    if (originalFilename == NULL)
    {
        printf("Error: filename not specified.\n");
        return;
    }

    // Create a new string for the new filename
    char *newFilename = malloc(strlen(originalFilename) + 2); // +1 for '_', +1 for '\0'
    if (newFilename == NULL)
    {
        printf("Error: memory allocation failed.\n");
        return;
    }

    // Copy the original filename into the new string
    strcpy(newFilename, originalFilename);

    // Add '_' to the end of the new filename
    strcat(newFilename, "_");

    // Rename the file
    if (rename(originalFilename, newFilename) != 0)
    {
        perror("Error renaming file");
    }

    // Free the allocated memory
    free(newFilename);
}

/*
 * Replaces the old substring with the new substring in the original string.
 */
char *replaceSubstring(const char *originalString, const char *oldSubstring, const char *newSubstring)
{
    if (originalString == NULL || oldSubstring == NULL || newSubstring == NULL)
    {
        return NULL; // Return NULL if invalid input
    }

    // Find the first occurrence of the old substring
    const char *foundSubstring = strstr(originalString, oldSubstring);
    if (foundSubstring == NULL)
    {
        // Substring not found, return a copy of the original string
        char *result = malloc(strlen(originalString) + 1);
        strcpy(result, originalString);
        return result;
    }

    // Calculate the lengths of the parts of the string before and after the substring
    size_t prefixLength = foundSubstring - originalString;
    size_t suffixLength = strlen(foundSubstring + strlen(oldSubstring));

    // Allocate memory for the new string
    char *result = malloc(prefixLength + strlen(newSubstring) + suffixLength + 1);

    // Copy the part of the string before the substring
    strncpy(result, originalString, prefixLength);

    // Copy the new substring
    strcpy(result + prefixLength, newSubstring);

    // Copy the part of the string after the substring
    strcpy(result + prefixLength + strlen(newSubstring), foundSubstring + strlen(oldSubstring));

    return result;
}
