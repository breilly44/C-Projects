#include <stdio.h>
//#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
//#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>
#include <ctype.h>
#define MAX_DICTIONARY_SIZE 100000
#define MAX_LINE_LENGTH 1024


int is_directory(const char *path);
void process_file(const char *file_path);
void process_directory(const char *dir_path);
int binary_search(const char *word, const char **dictionary, int size);
void load_dictionary(const char *path, char ***dictionary, int *size);
void report_misspelled_word(const char *word, const char *file_path, int line, int col);
void free_dictionary(char **dictionary, int size);
const char *dictionary_path;


int main(int argc, char *argv[]) {
    
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <dictionary> <file1> ... <fileN>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    dictionary_path = argv[1];
    char **dictionary;
    int dictionary_size;
    if (dictionary_path != NULL) {
            printf("Dictionary path: %s\n", dictionary_path);
            load_dictionary(dictionary_path, &dictionary, &dictionary_size);
        } else {
            fprintf(stderr, "Error: dictionary_path is NULL\n");
            exit(EXIT_FAILURE);
        }
    for (int i = 2; i < argc; i++) {
        if (is_directory(argv[i])) {
            process_directory(argv[i]);
        } else {
            process_file(argv[i]);
        }
    }
}

int is_directory(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

void process_directory(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;
        }

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
        
        if (is_directory(path)) {
            process_directory(path);
        } else if (strstr(entry->d_name, ".txt") != NULL) {
            process_file(path);
        }
    }

    closedir(dir);
}

void load_dictionary(const char *path, char ***dictionary, int *size) {
    //printf("Trying to open dictionary at: %s\n", path);
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("Error opening dictionary file");
        exit(EXIT_FAILURE);
    }
    //printf("fopen sucess!\n");

    *size = 0;
    *dictionary = NULL;

    //clock_t start = clock();
    char buffer[MAX_LINE_LENGTH];
    int i = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        size_t word_length = strlen(buffer);
        if (word_length >= MAX_LINE_LENGTH) {
            fprintf(stderr, "Error: Word in dictionary file is too long\n");
            exit(EXIT_FAILURE);
        }
        buffer[word_length - 1] = '\0';

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
    //clock_t end = clock();
    //double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    
    /* For Debugging
    printf("Loaded dictionary with %d words in %.3f seconds\n", *size, time_taken);
    printf("Loaded dictionary with %d words:\n", *size);
    for (int j = 0; j < *size; j++) {
        printf("%s\n", (*dictionary)[j]);
    }*/
    
}

void to_lowercase(const char *input, char *output) {
    while (*input) {
        *output = tolower((unsigned char)*input);
        input++;
        output++;
    }
    *output = '\0';
}

int binary_search(const char *word, const char **dictionary, int size) {
    int low = 0;
    int high = size - 1;
    int mid;
    char lowercase_word[strlen(word) + 1];
    to_lowercase(word, lowercase_word);

    while (low <= high) {
        mid = (low + high) / 2;
        char lowercase_dictionary_word[strlen(dictionary[mid]) + 1];
        to_lowercase(dictionary[mid], lowercase_dictionary_word);

        int cmp_result = strcmp(lowercase_word, lowercase_dictionary_word);

        if (cmp_result == 0) {
            return mid; // word found
        } else if (cmp_result < 0) {
            high = mid - 1; // Search in the left half
        } else {
            low = mid + 1; // Search in the right half
        }
    }

    return -1; // Word not found
}

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
        //printf("Line %d: %s", line_number, line_buffer); // Add this line to print out the line content
        char *token = strtok(line_buffer, " \t\n\r.,;:!?");
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
    printf("%s (%d,%d): %s\n", file_path, line, col, word);
    //printf("Misspelled word '%s' found in file '%s' at line %d, column %d.\n", word, file_path, line, col);
}

// Function to tokenize a string into words
void tokenize(const char *line, char **words, int *num_words) {
    const char *delim = " \t\n\r.,;:'!?()[]{}<>";
    char *token = strtok( (char *)line, delim );
    *num_words = 0;
    while (token != NULL) {
        // Remove trailing punctuation
        size_t word_length = strlen(token);
        while (word_length > 0 && ispunct(token[word_length - 1])) {
            token[word_length - 1] = '\0';
            word_length--;
        }

        // Ignore quotation marks at the start of a word
        if (strlen(token) > 0 && (token[0] == '\'' || token[0] == '"')) {
            token++;
            continue;
        }

        words[*num_words] = token;
        (*num_words)++;
        token = strtok(NULL, delim);
    }
}
