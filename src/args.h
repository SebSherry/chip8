#ifndef ARGS_H_
#define ARGS_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  char *rom;
  bool debug;
  bool help;
  uint32_t scale;
  uint32_t foreground;
  uint32_t background;
  uint32_t target_cycles;
} Args;

void usage();
int read_args(Args *args, int argc, char *argv[]);
bool convert_input_to_colour(long input, uint32_t *colour);
#endif
