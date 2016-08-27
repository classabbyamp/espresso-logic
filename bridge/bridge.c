/*
  Entry point of the espresso source code.
  Replaces main.c
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bridge.h"
#include "espresso.h"

char ** get_solution(pPLA);
void add_to_buffer(char **, unsigned int *, unsigned int*, char);

char ** run_espresso(char ** data, unsigned int length) {
  FILE * tempPLA = tmpfile();
  pPLA PLA;
  bool error;
  cost_t cost;
  pcover fold;
  char ** result = NULL;

  if (length == 0) {
    return NULL;
  }

  for(unsigned int i = 0; i < length; ++i) {
    fputs(data[i], tempPLA);
    fputc('\n', tempPLA);
  }

  rewind(tempPLA);

  if (read_pla(tempPLA, TRUE, TRUE, FD_type, &PLA) == EOF) {
    fclose(tempPLA);
    return NULL;
  }

  // makes sure free() won't crash on this variable
  PLA->filename = NULL;

  fold = sf_save(PLA->F);
  PLA->F = espresso(PLA->F, PLA->D, PLA->R);
  EXECUTE(error = verify(PLA->F, fold, PLA->D), VERIFY_TIME, PLA->F, cost);

  if (error) {
    PLA->F = fold;
    (void) check_consistency(PLA);
  } else {
    free_cover(fold);
    result = get_solution(PLA);
  }

  /* cleanup all used memory */
  free_PLA(PLA);
  FREE(cube.part_size);
  setdown_cube();             /* free the cube/cdata structure data */
  sf_cleanup();               /* free unused set structures */
  sm_cleanup();               /* sparse matrix cleanup */
  fclose(tempPLA);

  return result;
}

char ** get_solution(pPLA PLA) {
  register pcube last, p;
  char ** solutions = malloc(((PLA->F)->count +1) * sizeof(char*));
  unsigned int current_solution = 0, buffer_size = 128;
  char *buffer = malloc(buffer_size); // dynamic alloc to allow realloc if necessary

  if (solutions == NULL || buffer == NULL) {
    fprintf(stderr, "[espresso-logic-minimizer] FATAL: memory allocation failed\n");

    if (solutions) free(solutions);
    if (buffer) free(buffer);
    return NULL;
  }

  // we set the last item +1 to NULL to let caller functions know when to stop
  // reading the array
  solutions[(PLA->F)->count] = NULL;

  foreach_set(PLA->F, last, p) {
    unsigned int cursor = 0;

    /*
      Rewrite of cvrout.c:print_cube to replace stdout printing with
      variable storage
     */
     int i, var;

     for(var = 0; var < cube.num_binary_vars; var++) {
       add_to_buffer(&buffer, &buffer_size, &cursor, "?01-" [GETINPUT(p, var)]);
     }

     for(var = cube.num_binary_vars; var < cube.num_vars - 1; var++) {
       add_to_buffer(&buffer, &buffer_size, &cursor, ' ');

       for(i = cube.first_part[var]; i <= cube.last_part[var]; i++) {
         add_to_buffer(&buffer, &buffer_size, &cursor, "01" [is_in_set(p, i) != 0]);
       }
     }

     if (cube.output != -1) {
       int last_part = cube.last_part[cube.output];
      add_to_buffer(&buffer, &buffer_size, &cursor, ' ');

       for(i = cube.first_part[cube.output]; i <= last_part; i++) {
         add_to_buffer(&buffer, &buffer_size, &cursor, "01" [is_in_set(p, i) != 0]);
       }
     }

     solutions[current_solution] = malloc(cursor + 1);

     if (solutions[current_solution] == NULL) {
       fprintf(stderr, "[espresso-logic-minimizer] FATAL: memory allocation failed\n");

       for(unsigned int j = 0; j < current_solution; j++) free(solutions[j]);
       free(buffer);
       return NULL;
     }

     buffer[cursor] = '\0';
     strcpy(solutions[current_solution], buffer);
     current_solution++;
  }

  free(buffer);
  return solutions;
}

void add_to_buffer(char ** buffer, unsigned int *buffer_size, unsigned int* cursor, char chr) {
  if (*cursor + 1 > *buffer_size) {
    char * new_buffer = realloc(*buffer, *buffer_size * 2);

    if (new_buffer == NULL) {
      fprintf(stderr, "[espresso-logic-minimizer] FATAL: memory allocation failed\n");
      return;
    }

    *buffer_size *= 2;
    *buffer = new_buffer;
  }

  (*buffer)[*cursor] = chr;
  (*cursor)++;
}
