/*
Copyright (c) 2012, "David Hilton" <dhiltonp@gmail.com>
Copyright (c) 2013, "Koen Kooi" <koen@dominion.thruhere.net>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies, 
either expressed or implied, of the FreeBSD Project.
*/

#include <hexbright.h>

#include <Wire.h>

#define OFF_MODE 0
#define CYCLE_MODE 2
#define ACCEL_MODE 4

int mode = 0;
static const int click = 350; // maximum time for a "short" click
static const int hold = 700; // maximum time for a "medium" click
static const int longhold = 2000; // maximum time for a "long" click


hexbright hb;

void setup() {
  hb.init_hardware();
} 

void loop() {
  hb.update();
  static int brightness_level = 0;

  //// Button actions to recognize, one-time actions to take as a result
  if(hb.button_just_released()) {
    if(hb.button_pressed_time()<click) { //<300 milliseconds
      mode = CYCLE_MODE;
      int levels[] = {-1, 1,250,500,750,1000};
      brightness_level = (brightness_level+1)%6;
      hb.set_light(CURRENT_LEVEL, levels[brightness_level], 150);
    } 
  }
    if(hb.button_pressed() && hb.button_pressed_time()>hold && hb.button_pressed_time() < longhold){ 
      mode = OFF_MODE;
      hb.set_light(CURRENT_LEVEL, OFF_LEVEL, NOW);
      // in case we are under usb power, reset state
      brightness_level = 0;
    }
  if(hb.button_pressed() && hb.button_pressed_time()>longhold) { 
      if (mode == OFF_MODE) {
        double d = hb.difference_from_down(); // (0,1)
        int di = (int)(d*400.0); // (0,400)
        int i = (di)/10; // (0,40)
        i *= 50; // (0,2000)
        i = i>1000 ? 1000 : i;
        i = i<=0 ? 1 : i;
        hb.set_light(CURRENT_LEVEL, i, 100);
      } 
  }   

  //// Actions over time for a given mode
  if (mode == OFF_MODE) { // charging, or turning off
    if(!hb.printing_number()) {
      hb.print_charge(GLED);
    }
  }
}
