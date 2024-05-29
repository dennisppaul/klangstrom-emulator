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

    PVector           mVector{16, 16};
    PShape            mShape;
    int               mouseMoveCounter = 0;
    std::string       mFontPath        = sketchPath();
    const std::string mFontName        = "JetBrainsMonoNL-Regular.ttf";
    PFont*            mFont;

    void arguments(std::vector<std::string> args) {
        for (auto& s: args) {
            println("> ", s);
            if (begins_with(s, "--fontpath=")) {
                println("found fontpath: ", get_string_from_argument(s));
                mFontPath = get_string_from_argument(s);
            }
        }
    }

    void settings() {
        size(1024, 768);
        antialiasing          = 8;
        enable_retina_support = true;
        headless              = false;
        no_audio              = false;
        monitor               = DEFAULT;
    }

    void setup() {
        println("sketchpath: ", sketchPath());
        println("fontpath  : ", mFontPath);
        println("width     : ", width);
        println("height    : ", height);

        if (exists(mFontPath + "/" + mFontName)) {
            mFont = loadFont(mFontPath + "/" + mFontName, 32);
            textFont(mFont);
        } else {
            println("could not load font: ", (mFontPath + "/" + mFontName));
        }

        sketch_setup();
    }

    void draw() {
        background(1);

        fill(0);
        text("23", 10, height - 20);

        stroke(0);
        noFill();
        rect(10, 10, width / 2 - 20, height / 2 - 20);

        noStroke();
        fill(random(0, 0.2));
        rect(20, 20, width / 2 - 40, height / 2 - 40);

        sketch_loop();
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
    }
};

PApplet* umgebung::instance() {
    return new UmgebungApp();
}
