#ifndef L298N_H
#define L298N_H

#include "config.h"

class L298N
{
private:
    int in1Pin;
    int in2Pin;
    int enaPin;

public:
    L298N(int in1, int in2, int ena);

    void begin();

    void forward(int speed);

    void reverse(int speed);

    void stop();

    void brake();
};

#endif