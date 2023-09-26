#include <stdio.h>
#include "SDL.h"

#include "neural_network.h"
#include "visual.h"
#include "dataset.h"
#include "save.h"
#include "option.h"

void help() {
    printf("Options:\n");
    printf("  -h --help            display this menu\n");
    printf("  -t --train           enable back propagation\n");
    printf("  -u --use   [PATH]    path of network to use\n");
    printf("  -i --image [PATH]    path of image set\n");
    printf("  -l --label [PATH]    path of label set\n");
    printf("  -o --ouput [PATH]    path to save trained network\n");
    printf("  -f --from  [N]       start from N-th test, default 0\n");
    printf("  -c --count [N]       try against N tests, default 10000\n");
    printf("  -d --delay [N]       delay between each test, in ms, default 200\n");
    printf("  -r --rate  [N]       learning rate, default 0.1\n");
}

int main(int argc, char** argv) {

    Option option;
    OptionParse(&option, argc, argv);

    if (option.help) {
        help();
        return 0;
    }

    NeuralNetwork network;
    if (option.networkPath[0] != '\0')
        NeuralNetworkLoad(&network, option.networkPath);
    else
        NeuralNetworkInit(&network, 4, (int[4]){784, 16, 16, 10});

    TestManager testManager;
    if (TestManagerInit(&testManager, option.imageSetPath, option.labelSetPath)) {
        fprintf(stderr, "Failed to load image set or label set, exiting...\n");
        return -1;
    };
    TestManagerSeek(&testManager, option.from);

    Visualizer visualizer;
    VisualizerInit(&visualizer, 940, 1118, "/usr/local/share/fonts/hack/HackNerdFont-Regular.ttf", 16);

    bool quit = false;
    while (
        !quit && 
        testManager.idx < option.from + option.count &&
        TestManagerLoad(&testManager)
    ) {

        NeuralNetworkPropagate(&network, testManager.input);
        if (option.training)
            NeuralNetworkBackPropagate(&network, testManager.expected, option.rate);

        VisualizerClear(&visualizer);
        NeuralNetworkDraw(&network, visualizer.renderer, 50, 50, 850, 650);
        NeuralNetworkDrawMeta(&network, &visualizer, testManager.expected, 50, 750);
        TestManagerDraw(&testManager, &visualizer, 600, 750);
        VisualizerPresent(&visualizer);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                quit = SDL_TRUE;
        }
        SDL_Delay(option.delay);
    }

    if (option.outputPath[0] != '\0')
        NeuralNetworkSave(&network, option.outputPath);

    TestManagerFree(&testManager);
    VisualizerFree(&visualizer);
    NeuralNetworkFree(&network);
    return 0;
}
