#include "option.h"

int OptionParse(Option* this, int argc, char** argv) {

    this->help = false;
    this->training = false;
    this->rate = 0.1;
    this->from = 0;
    this->count = 10000;
    this->delay = 200;
    memset(this->networkPath, 0, 128);
    memset(this->imageSetPath, 0, 128);
    memset(this->labelSetPath, 0, 128);
    memset(this->outputPath, 0, 128);

    struct option longOptions[] = {
        {"help" ,  no_argument,       0, 'h'},
        {"train",  no_argument,       0, 't'},
        {"use"  ,  required_argument, 0, 'u'},
        {"image",  required_argument, 0, 'i'},
        {"label",  required_argument, 0, 'l'},
        {"output", required_argument, 0, 'o'},
        {"from" ,  required_argument, 0, 'f'},
        {"count",  required_argument, 0, 'c'},
        {"delay",  required_argument, 0, 'd'},
        {"rate" ,  required_argument, 0, 'r'},
        {0      ,  0,                 0,  0 }
    };

    int c;
    while ((c = getopt_long(argc, argv, "htu:i:l:o:f:c:d:r:", longOptions, NULL)) != -1) {

        switch (c) {
            case 'h':
                this->help = 1;
                break;
            case 't':
                this->training = 1;
                break;
            case 'u':
                strcpy(this->networkPath, optarg);
                break;
            case 'i':
                strcpy(this->imageSetPath, optarg);
                break;
            case 'l':
                strcpy(this->labelSetPath, optarg);
                break;
            case 'o':
                strcpy(this->outputPath, optarg);
                break;
            case 'f':
                this->from = atoi(optarg);
                break;
            case 'c':
                this->count = atoi(optarg);
                break;
            case 'd':
                this->delay = atoi(optarg);
                break;
            case 'r':
                this->rate = atof(optarg);
                break;
            default:
                fprintf(stderr, "Unknown option: %d\n", c);
        }
    }
    
}
