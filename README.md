# Subtitle Translator Script

This program is designed to recursively traverse a directory, identify subtitle files with a specified suffix (e.g., "_en.srt"), and translate their content from English to Russian using the `trans` command-line tool. The translated subtitles are saved as new files with the suffix "_ru.srt". Additionally, the program renames the original files by appending an underscore to their names.

## Features
- **Recursive directory traversal**: The script can process subtitle files in subdirectories.
- **Translation of subtitle content**: Only text that is not a number or timestamp is translated.
- **Suffix management**: Original subtitle files are renamed by adding an underscore.
- **Multi-file support**: The program handles multiple files in one execution.

## Dependencies
- `trans`: Command-line tool for translation. The script uses `trans` to translate subtitles from English to Russian.
- A Unix-like environment (Linux/macOS).

## How It Works
1. The program searches for subtitle files ending with the suffix `_en.srt` within the specified directory or the current directory if no path is provided.
2. It reads each subtitle file and translates its content line by line, except for numbers (e.g., subtitle indices) and timestamps (e.g., `00:01:23 --> 00:01:26`).
3. Translated content is saved into new subtitle files with the suffix `_ru.srt`.
4. Original subtitle files are renamed by appending an underscore to their names.
5. The script outputs information such as the number of files processed and the time taken for each translation.

## Usage
To run the script, simply compile it and execute it with the directory path as an argument.

```bash
gcc -o subtitle_translator subtitle_translator.c
./subtitle_translator /path/to/subtitles
```

If no directory path is provided, it will default to the current directory.

## Example
Assume you have a directory with English subtitle files ending in `_en.srt`. The script will:
- Translate them to Russian and save them with the suffix `_ru.srt`.
- Rename the original files by adding an underscore at the end.

For instance:
- `movie_en.srt` → `movie_ru.srt` (translated)
- `movie_en.srt` → `movie_en.srt_` (original file renamed)

## Functions
- `save_files_in_array`: Recursively collects the names of files with a specific suffix from the directory.
- `save_path_name`: Extracts the directory path from command-line arguments or defaults to the current directory.
- `find_the_number_of_files_and_the_maximum_filename_length`: Counts the number of files for translation and identifies the longest file name.
- `translate_and_write`: Translates the content of a subtitle file and writes the result to a new file.
- `add_underscore_to_filename`: Renames the original file by appending an underscore.
- `replaceSubstring`: Replaces a substring in a string with another substring (used for changing suffixes in file names).
  
## License
This project is licensed under the MIT License.
