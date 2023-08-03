#ifndef XSM_SIMULATOR_H

#define XSM_SIMULATOR_H

#include "machine.h"

#define XSM_SIMULATOR_DEFDEBUG FALSE
#define XSM_SIMULATOR_DEFCONSOLE 20
#define XSM_SIMULATOR_DEFTIMER 20
#define XSM_SIMULATOR_DEFDISK 20

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

static xsm_options _options;

int simulator_run();
int simulator_parse_args(int argc, char **argv);

#endif