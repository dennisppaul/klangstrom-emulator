/*
  main.cpp - Main loop for Arduino sketches
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#define ARDUINO_MAIN
#include "Arduino.h"
#include <stdio.h>
#include <SDL.h>
#include <iostream>

#include "Umgebung.h"

using namespace umgebung;

void sketch_setup() {
    setup();
}

void sketch_loop() {
    loop();
}

class UmgebungApp : public PApplet {

    PVector mVector{16, 16};
    PShape  mShape;
    int     mouseMoveCounter = 0;

    void settings() {
        size(1024, 768);
        antialiasing          = 8;
        enable_retina_support = true;
        headless              = false;
        no_audio              = false;
        monitor               = DEFAULT;
    }

    void setup() {
        println("width : ", width);
        println("height: ", height);
        sketch_setup();
    }

    void draw() {
        sketch_loop();
        background(1);

        stroke(0);
        noFill();
        rect(10, 10, width / 2 - 20, height / 2 - 20);

        noStroke();
        fill(random(0, 0.2));
        rect(20, 20, width / 2 - 40, height / 2 - 40);
    }

    void audioblock(float** input, float** output, int length) {
        for (int i = 0; i < length; i++) {
            float sample = random(-0.1, 0.1);
            for (int j = 0; j < audio_output_channels; ++j) {
                output[j][i] = sample;
            }
        }
    }

    void keyPressed() {
        if (key == 'q') {
            exit();
        }
        println((char) key, " pressed");
    }
};

PApplet* umgebung::instance() {
    return new UmgebungApp();
}
