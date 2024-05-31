/*
 * Klangstrom
 *
 * This file is part of the *wellen* library (https://github.com/dennisppaul/wellen).
 * Copyright (c) 2024 Dennis P Paul.
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#define ARDUINO_MAIN
#include "Arduino.h"
#include <stdio.h>
#include <SDL.h>
#include <iostream>

#include "Umgebung.h"

using namespace umgebung;

// static std::function<void(float**, float**, int)> fAudioDeviceCallback;

extern "C" void audiocodec_callback_class_f(float** input, float** output, uint16_t length);

// void audiocodec_register_audio_device_cpp(const std::function<void(float**, float**, int)> callback) {
//     fAudioDeviceCallback = callback;
// }

static void sketch_setup() {
    setup();
}

static void sketch_loop() {
    loop();
}

class KlangstromEmulator : public PApplet {

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
        // if (fAudioDeviceCallback) {
        //     fAudioDeviceCallback(input, output, length);
        // }
        audiocodec_callback_class_f(input, output, length);
    }

    void keyPressed() {
        if (key == 'q') {
            exit();
        }
    }
};

PApplet* umgebung::instance() {
    return new KlangstromEmulator();
}
