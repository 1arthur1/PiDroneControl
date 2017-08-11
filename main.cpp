#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include "SBUS.h"
#include "json.hpp"

using json = nlohmann::json;

using namespace std;

#define SBUS_MIN            11
#define SBUS_MIDDLE         992
#define SBUS_MAX            1977
#define SETTINGS_FILENAME   "settings.json"

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

int main(int argc, char *argv[])
{
    char key;
    bool armed = false;
    uint16_t throttle = SBUS_MIN;
    uint16_t yaw = SBUS_MIDDLE, pitch = SBUS_MIDDLE, roll = SBUS_MIDDLE;
    uint16_t channels[16];
    int step = 100;

    //*************// check args //*************//
    if(argc < 2)
    {
        cout << "please give tty name" << endl;
        return -1;
    }

    //*************// load settings file //*************//
    if(access( SETTINGS_FILENAME, F_OK ) == -1)
    {
        // create settings file
        ofstream settings(SETTINGS_FILENAME);
        json json_settings = {
            {"init_values", {
                {"throttle", throttle},
                {"yaw", yaw},
                {"pitch", pitch},
                {"roll", roll}
            }}
        };

        cout << "Writing initial JSON settings file to " << SETTINGS_FILENAME << endl;

        settings << json_settings.dump(4) << endl;
        settings.close();
    }
    else
    {
        // read from settings file
        ifstream settings(SETTINGS_FILENAME);
        json json_settings;
        settings >> json_settings;

        throttle = json_settings["init_values"]["throttle"];
        yaw = json_settings["init_values"]["yaw"];
        pitch = json_settings["init_values"]["pitch"];
        roll = json_settings["init_values"]["roll"];
    }

    //*************// main loop //*************//
    SBUS::SBUS sbusport(argv[1]);
    sbusport.begin();

    while(true)
    {
        if(kbhit())
        {
            key = getchar();

            if(key == 27) // ESC
                break;

            if(key == 'e')
                throttle = (throttle+step) > SBUS_MAX ? SBUS_MAX : throttle+step;
            if(key == 'a')
                throttle = (throttle-step) < SBUS_MIN ? SBUS_MIN : throttle-step;

            if(key == 'z')
                pitch = (pitch+step) > SBUS_MAX ? SBUS_MAX : pitch+step;
            if(key == 's')
                pitch = (pitch-step) < SBUS_MIN ? SBUS_MIN : pitch-step;

            if(key == 'd')
                roll = (roll+step) > SBUS_MAX ? SBUS_MAX : roll+step;
            if(key == 'q')
                roll = (roll-step) < SBUS_MIN ? SBUS_MIN : roll-step;

            if(key == 'c')
                yaw = (yaw+step) > SBUS_MAX ? SBUS_MAX : yaw+step;
            if(key == 'w')
                yaw = (yaw-step) < SBUS_MIN ? SBUS_MIN : yaw-step;

            if(key == 'x')
                armed = !armed;

            cout << "a t y p r: " << armed << " " << throttle << " " << yaw << " " << pitch << " " << roll << endl;
        }

        channels[0] = roll;
        channels[1] = pitch;
        channels[3] = yaw;
        channels[2] = throttle;
        channels[4] = armed ? 992 : 11;
        sbusport.write(channels);

        usleep(7000); // send every 7ms (SBUS high speed mode)
    }

    cout << "exiting" << endl;

    return 0;
}
