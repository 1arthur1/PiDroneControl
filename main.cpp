#include <iostream>
#include <inttypes.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include "SBUS.h"

using namespace std;

int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

#define MIN 11
#define MIDDLE 992
#define MAX 1977

int main(int argc, char *argv[])
{
    char key;
    bool armed = false;
    uint16_t throttle = MIN;
    uint16_t yaw = MIDDLE, pitch = MIDDLE, roll = MIDDLE;
    uint16_t channels[16];
    int step = 100;

    if(argc < 2)
    {
        cout << "please give tty name" << endl;
        return -1;
    }

    for(int i=0;i<16;i++)
        channels[i] = MIN;

    SBUS::SBUS sbusport(argv[1]);

    sbusport.begin();

    while(true)
    {
        if(kbhit())
        {
            key = getchar();

            if(key == 27) // ESC
                break;

            if(key == 97) // a
                throttle = (throttle+step) > MAX ? MAX : throttle+step;
            if(key == 101) // e
                throttle = (throttle-step) < MIN ? MIN : throttle-step;

            if(key == 'z') // z
                pitch = (pitch+step) > MAX ? MAX : pitch+step;
            if(key == 's') // s
                pitch = (pitch-step) < MIN ? MIN : pitch-step;

            if(key == 'q') // z
                roll = (roll+step) > MAX ? MAX : roll+step;
            if(key == 'd') // s
                roll = (roll-step) < MIN ? MIN : roll-step;

            if(key == 'w')
                armed = !armed;
        }

        channels[0] = roll;
        channels[1] = pitch;
        channels[3] = yaw;
        channels[2] = throttle;
        channels[4] = armed ? 992 : 11;
        sbusport.write(channels);

        cout << "a t y p r" << armed << " " << throttle << " " << yaw << " " << pitch << " " << roll << endl;

        usleep(7000); // send every 7ms (SBUS high speed mode)
    }

    cout << "exiting" << endl;

    return 0;
}
