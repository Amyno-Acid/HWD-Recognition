### About
- A very basic hand-written digit recognition model written in C,  
with the least fancy neural network and back propagation techniques.
- The dataset is obtained from [the MNIST Database](http://yann.lecun.com/exdb/mnist/).
- Visualization is done with [SDL](https://github.com/libsdl-org/SDL).
- A project of interest.

### Prerequisites
- [SDL](https://github.com/libsdl-org/SDL)
- [SDL_ttf](https://github.com/libsdl-org/SDL_ttf)

### Get Started

1. Compile the code
	```
	> make
	```

2. Check the list of options
	```
	> bin/main --help
	Options:
	  -h --help            display this menu
	  -t --train           enable back propagation
	  -u --use   [PATH]    path of network to use
	  -i --image [PATH]    path of image set
	  -l --label [PATH]    path of label set
	  -o --ouput [PATH]    path to save trained network
	  -f --from  [N]       start from N-th test, default 0
	  -c --count [N]       try against N tests, default 10000
	  -d --delay [N]       delay between each test, in ms, default 200
	  -r --rate  [N]       learning rate, default 0.1
	```

3. Start training your model (a minimal example)
	```
	> bin/main -t \
	> -i dataset/train-images-idx3-ubyte -l dataset/train-labels-idx1-ubyte \
	> -o save/attempt.sav -c 60000
	```
	**The default delay is 200 ms per test, which allows you to observe the training process. If you want a fast training, you may set the delay to 10 ms (only take 15 mins to finish)**

4. Observe the performance of the neural network
	```
	> bin/main -u save/attempt.sav -d 5000 \
	> -i dataset/t10k-images-idx3-ubyte -l t10k-labels-idx1-ubyte
	```

### Todo

- reLU function implementation
- Headless option (without visualization)
- Record training history with SQLite
- Interactive sketchpad for custom testing

