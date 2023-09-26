#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#ifndef OPTION_H
#define OPTION_H

typedef struct Option {
    bool help;
    bool training;
    int from;
    int count;
    int delay;
    double rate;
    char networkPath[128];
    char imageSetPath[128];
    char labelSetPath[128];
    char outputPath[128];
} Option;

int OptionParse(Option* this, int argc, char** argv);

#endif
