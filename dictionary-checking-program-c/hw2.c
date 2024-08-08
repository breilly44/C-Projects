#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define MAX_DICTIONARY_SIZE 100000
#define MAX_LINE_LENGTH 1024


// Function prototypes
int is_directory(const char *path);
void process_file(const char *file_path);
void process_directory(const char *dir_path);
int binary_search(const char *word, const char **dictionary, int size);
void load_dictionary(const char *path, char ***dictionary, int *size);
void report_misspelled_word(const char *word, const char *file_path, int line, int col);
void free_dictionary(char **dictionary, int size);
const char *dictionary_path;


int main(int argc, char *argv[]) {
    
    // Check arguments
    if (argc < 2) { // The program requires at least a dictionary file
        fprintf(stderr, "Usage: %s <dictionary> <file1> ... <fileN>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // The first argument is always the dictionary
    dictionary_path = argv[1];
    // Load the dictionary here...
    char **dictionary;
    int dictionary_size;
    if (is_directory(dictionary_path)) {
        // Load the default dictionary file
        printf("show me the money: %s\n", dictionary_path);
        load_dictionary("dictionary.txt", &dictionary, &dictionary_size);
    } else {
        if (dictionary_path != NULL) {
            // Load the dictionary here...
            printf("Dictionary path: %s\n", dictionary_path);
            load_dictionary(dictionary_path, &dictionary, &dictionary_size);
        } else {
            fprintf(stderr, "Error: dictionary_path is NULL\n");
            exit(EXIT_FAILURE);
        }
    }
    // Process each argument
    for (int i = 2; i < argc; i++) {
        // Check if the argument is a directory or a file
        if (is_directory(argv[i])) {
            // It's a directory, recursively process all .txt files within
            process_directory(argv[i]);
        } else {
            // It's a file, process it
            process_file(argv[i]);
        }
    }
    
    // Your code for spell checking goes here...

    return EXIT_SUCCESS;
}

// Helper function to check if a path is a directory
int is_directory(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

// Function to process directories
void process_directory(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    struct dirent *entry;
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files and directories
        if (entry->d_name[0] == '.') {
            continue;
        }

        // Construct full path
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
        
        // Check if the entry is a directory or a file with .txt extension
        if (is_directory(path)) {
            process_directory(path); // Recursive call
        } else if (strstr(entry->d_name, ".txt") != NULL) {
            process_file(path);
        }
    }

    closedir(dir);
}

void load_dictionary(const char *path, char ***dictionary, int *size) {
    /*printf("Trying to open dictionary at: %s\n", path);
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("Error opening dictionary file");
        exit(EXIT_FAILURE);
    }
    printf("fopen sucess!\n");
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    printf("next breakpoint\n");

    *size = file_size / sizeof(char) + 1; // Approximate size
    *dictionary = malloc(*size * sizeof(char *));

    clock_t start = clock();
    int i = 0;
    char *word = NULL;
    while (fscanf(file, "%ms", &word) == 1) {
        (*dictionary)[i] = word;
        i++;
    }
    printf("next breakpoint\n");
    *size = i;

    fclose(file);
    clock_t end = clock();
    // Add debug statements to print out the memory usage and the time taken to load the dictionary
    //clock_t end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Loaded dictionary with %d words in %.3f seconds\n", *size, time_taken);
    //printf("Memory usage: %ld bytes\n", malloc_size(*dictionary));

    // Add debug statements to print out the size and contents of the dictionary
    printf("Loaded dictionary with %d words:\n", *size);
    for (int j = 0; j < *size; j++) {
        printf("%s\n", (*dictionary)[j]);
    }*/
    printf("Trying to open dictionary at: %s\n", path);
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("Error opening dictionary file");
        exit(EXIT_FAILURE);
    }
    printf("fopen sucess!\n");

    *size = 0;
    *dictionary = NULL;

    clock_t start = clock();
    char buffer[MAX_LINE_LENGTH];
    int i = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        size_t word_length = strlen(buffer);
        if (word_length >= MAX_LINE_LENGTH) {
            fprintf(stderr, "Error: Word in dictionary file is too long\n");
            exit(EXIT_FAILURE);
        }
        buffer[word_length - 1] = '\0'; // Remove newline character

        char *word = malloc((word_length + 1) * sizeof(char));
        if (word == NULL) {
            fprintf(stderr, "Error: Failed to allocate memory for word\n");
            exit(EXIT_FAILURE);
        }
        strcpy(word, buffer);

        (*dictionary) = realloc(*dictionary, (i + 1) * sizeof(char *));
        if (*dictionary == NULL) {
            fprintf(stderr, "Error: Failed to reallocate memory for dictionary\n");
            exit(EXIT_FAILURE);
        }
        (*dictionary)[i] = word;
        i++;
    }
    *size = i;

    fclose(file);
    clock_t end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Loaded dictionary with %d words in %.3f seconds\n", *size, time_taken);

    // Add debug statements to print out the size and contents of the dictionary
    printf("Loaded dictionary with %d words:\n", *size);
    for (int j = 0; j < *size; j++) {
        printf("%s\n", (*dictionary)[j]);
    }
}

int binary_search(const char *word, const char **dictionary, int size) {
    int left = 0;
    int right = size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        int cmp = strcmp(word, dictionary[mid]);
        if (cmp == 0) {
            return mid;
        } else if (cmp < 0) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    
    printf("Could not find word '%s' in dictionary\n", word);
    return -1;
}

// Function to process individual files
void process_file(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    if (ferror(file)) {
        perror("Error reading file");
        fclose(file);
        return;
    }
    char **dictionary;
    int dictionary_size;
    load_dictionary(dictionary_path, &dictionary, &dictionary_size);

    char line_buffer[MAX_LINE_LENGTH];
    int line_number = 1;
    int col_number = 1;

    while (fgets(line_buffer, sizeof(line_buffer), file) != NULL) {
        printf("Line %d: %s", line_number, line_buffer); // Add this line to print out the line content
        char *token = strtok(line_buffer, " \t\n\r.,;:!?");
// ASCII ENCODING
// Trailing punctuation To avoid problems with common sentence punctuation, we ignore punctuation marks occurring at the end of a word.
// Similarly, we ignore quotation marks (’ and ") and brackets (( and [ and {) at the start of a word.

// Hyphens A hyphenated word contains two or more smaller words separated by hyphens (-).
// These are correctly spelled if all the component words are correctly spelled.

// Capitalization We allow up to three variations of a word based on capitalization: regular, initial capital, and all capitals.
// That is, if our dictionary contains “hello”, we would consider “hello”, “Hello”, and “HELLO” to be correctly spelled, but not “HeLlO”.
// Note that capital letters in the dictionary must be capital: if the dictionary contains “MacDonald”, we would accept “MACDONALD” but not “Macdonald” or “macdonald”.

        while (token != NULL) {
            int index = binary_search(token, (const char **)dictionary, dictionary_size);
            if (index == -1) {
                report_misspelled_word(token, file_path, line_number, col_number);
            }
            token = strtok(NULL, " \t\n\r.,;:!?");
            col_number++;
        }

        col_number = 1;
        line_number++;
    }

    fclose(file);
    free_dictionary(dictionary, dictionary_size);

}

void free_dictionary(char **dictionary, int size) {
    for (int i = 0; i < size; i++) {
        free(dictionary[i]);
    }
    free(dictionary);
}

void report_misspelled_word(const char *word, const char *file_path, int line, int col) {
    printf("Misspelled word '%s' found in file '%s' at line %d, column %d.\n", word, file_path, line, col);
}

// Function to tokenize a string into words
void tokenize(const char *line, char **words, int *num_words) {
    const char *delim = " \t\n\r.,;:!?()[]{}<>";
    char *token = strtok( (char*)line, delim );
    *num_words = 0;
    while (token != NULL) {
        words[*num_words] = token;
        (*num_words)++;
        token = strtok(NULL, delim);
    }
}