#include <stdio.h>
#include <iostream>

#include "Klangstrom.h"
#include "KlangstromAudioCodec.h"
#include "KlangstromSerialDebug.h"
#include "KlangstromLEDs.h"

SerialDebug console;
Klangstrom klangstrom;
AudioCodec audiocodec;
LEDs leds;

void setup() {
    std::cout << "setup()" << std::endl;
    #ifdef KLST_CATERPILLAR_EMU
    std::cout << "KLST_CATERPILLAR_EMU" << std::endl;
    #endif // KLST_CATERPILLAR_EMU
    #ifdef KLST_PANDA_EMU
    std::cout << "KLST_PANDA_EMU" << std::endl;
    #endif // KLST_CATERPILLAR_EMU

      klangstrom.init();
  console.init();
  console.info();
  console.timestamp();
  console.println("starting init");
  audiocodec.init();
  leds.init();

  console.timestamp();
  console.println("finished init");

  /* setup section */
  console.timestamp();
  console.println("starting setup");
  klangstrom.setup();

  console.timestamp();
  console.println("finished setup");
  console.println("---------------------------------------------------------");

}

void loop() {
    printf("loop()\n\r");
}
