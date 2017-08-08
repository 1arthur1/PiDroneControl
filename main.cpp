#include <iostream>

#include "SBUS.h"

using namespace std;

int main(int argc, char *argv[])
{
    uint16_t channels[16];

    if(argc < 2)
    {
        cout << "please give tty name" << endl;
        return -1;
    }

    SBUS::SBUS sbusport(argv[1]);

    sbusport.begin();
    channels[0] = 0;
    channels[1] = 50;
    channels[2] = 1000;
    channels[3] = 2000;
    sbusport.write(channels);

    return 0;
}
