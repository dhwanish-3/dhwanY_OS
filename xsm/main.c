/*
The Main function.
*/

#include "simulator.h"

/* Main function */
int main(int argc, char **argv)
{
    if (!simulator_parse_args(argc, argv))
        return EXIT_FAILURE;

    simulator_run();

    return EXIT_SUCCESS;
}