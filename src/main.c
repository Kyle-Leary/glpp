#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_INCLUDEPATH_LENGTH 128
// how long can the generated paths from the includepath sprintf-ing be?
#define MAX_PATH_LENGTH 256
#define MAX_INCLUDES 128

void process_file(FILE *input, FILE *output);

#define EXTRA_ARGS_OFFSET 4

// all the different include paths specified on the commandline.
char include_paths[MAX_INCLUDES][MAX_INCLUDEPATH_LENGTH];
static int include_paths_len = 0;

// this argument system is fucked up but it's fine for what this is. it
// basically just supports a basic -o and -Iincludepath option set.
int main(int argc, char *argv[]) {

  // how many dangling args do we need to parse through?
  int n_extra_args = argc - EXTRA_ARGS_OFFSET;

  if (argc < 3) {
    fprintf(stderr, "Usage: %s <input.glsl> -o <output.glsl> <other args>\n",
            argv[0]);
    return EXIT_FAILURE;
  }

  const char *inputPath = argv[1];
  const char *outputPath = NULL;

  if (strcmp(argv[2], "-o") == 0 && argc > 3) {
    outputPath = argv[3];
  } else {
    fprintf(stderr, "Missing -o flag for output file.\n");
    return EXIT_FAILURE;
  }

  while (n_extra_args > 0) {
    // be sure to tick forwards through the argv array.
    int curr_extra = argc - n_extra_args;
    char *curr_extra_string = argv[curr_extra];
    int curr_extra_strlen = strlen(curr_extra_string);

    // parse out an extra include path.
    if (strncmp(curr_extra_string, "-I", 2) == 0) {
      int include_strlen = curr_extra_strlen - 2;
      // strncpy will either stop at the buffer limit or the NULL byte in the
      // string, so this is fine.
      strncpy(include_paths[include_paths_len], curr_extra_string + 2,
              MAX_INCLUDEPATH_LENGTH);
      include_paths_len++;
    }

    n_extra_args--;
  }

  FILE *input = fopen(inputPath, "r");
  if (input == NULL) {
    perror("Failed to open input file");
    return EXIT_FAILURE;
  }

  FILE *output = fopen(outputPath, "w");
  if (output == NULL) {
    perror("Failed to open output file");
    fclose(input);
    return EXIT_FAILURE;
  }

  process_file(input, output);

  fclose(input);
  fclose(output);

  printf("Preprocessing completed. Output written to %s.\n", outputPath);
  return EXIT_SUCCESS;
}

static FILE *open_include_file(const char *include_path) {
  FILE *include_file = NULL;
  // try to open in the pwd implicitly.
  include_file = fopen(include_path, "r");
  if (include_file == NULL) {
    // then, loop through all the include_paths specified on the commandline.
    for (int i = 0; i < include_paths_len; i++) {
      char *global_include_path = include_paths[i];
      char path_buf[MAX_PATH_LENGTH];
      sprintf(path_buf, "%s/%s", global_include_path, include_path);
      include_file = fopen(path_buf, "r");
      // don't do any more checks if we've found it.
      if (include_file != NULL)
        break;
    }
  }
  // if we get to the end, implicitly return null.
  return include_file;
}

void process_file(FILE *input, FILE *output) {
  char line[MAX_LINE_LENGTH];

  while (fgets(line, sizeof(line), input)) {
    if (strncmp(line, "#include", 8) == 0) {

      char include_path[MAX_LINE_LENGTH];

      sscanf(line, "#include \"%[^\"]\"", include_path);

      FILE *include_file = open_include_file(include_path);

      if (include_file) {
        process_file(include_file, output);
        fclose(include_file);
      } else {
        fprintf(stderr, "Failed to open include file: %s\n", include_path);
      }
    } else {

      fputs(line, output);
    }
  }
}
