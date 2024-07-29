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

#include <iostream>

#include "ArduinoFunctions.h"
#include "KlangstromEmulator.h"
#include "KlangstromEnvironment.h"

using namespace umgebung;

extern "C" void KLST_BSP_audiocodec_process_audioblock_data(AudioBlock* audio_block);

static void sketch_setup() {
    setup();
}

static void sketch_loop() {
    loop();
}

KlangstromEmulator* KlangstromEmulator::fInstance = nullptr;

void KlangstromEmulator::arguments(std::vector<std::string> args) {
    for (auto& s: args) {
        println("> ", s);
        if (begins_with(s, "--fontpath=")) {
            println("found fontpath: ", get_string_from_argument(s));
            mFontPath = get_string_from_argument(s);
        }
    }
}

void KlangstromEmulator::settings() {
    size(1024, 768);
    antialiasing          = 8;
    enable_retina_support = true;
    headless              = false;
    no_audio              = false;
    monitor               = DEFAULT;

    mOutputBuffers = new float*[audio_output_channels];
    for (int i = 0; i < audio_output_channels; i++) {
        mOutputBuffers[i] = new float[DEFAULT_FRAMES_PER_BUFFER];
    }
    mInputBuffers = new float*[audio_input_channels];
    for (int i = 0; i < audio_input_channels; i++) {
        mInputBuffers[i] = new float[DEFAULT_FRAMES_PER_BUFFER];
    }
}

void KlangstromEmulator::setup() {
    println("sketchpath: ", sketchPath());
    println("fontpath  : ", mFontPath);
    println("width     : ", width);
    println("height    : ", height);

    if (exists(mFontPath + "/" + mFontName)) {
        mFont = loadFont(mFontPath + "/" + mFontName, DEFAULT_FONT_SIZE);
        textFont(mFont);
    } else {
        println("could not load font: ", (mFontPath + "/" + mFontName));
    }

    sketch_setup();

    task.set_callback(sketch_loop);
    task.set_frequency(1000);
    task.start();
}

void KlangstromEmulator::draw() {
    background(0.2);

    // fill(0);
    // text(get_emulator_name(), 10, 10 + 32);

    // stroke(0);
    // noFill();
    // rect(10, 10, width / 2 - 20, height / 2 - 20);

    // noStroke();
    // fill(random(0, 0.2));
    // rect(20, 20, width / 2 - 40, height / 2 - 40);

    fill(1);
    text(get_emulator_name(), 25, 10 + DEFAULT_FONT_SIZE);

    for (auto& drawable: drawables) {
        drawable->draw(g);
    }
}

/**
 * called from umgebung to request and process audio data
 * @param input
 * @param output
 * @param length
 */
void KlangstromEmulator::audioblock(float** input, float** output, int length) {
    // TODO here we need to collect data from all audio devices and mix them into the output buffer
    // TODO assuming that the underlying audio system always has 2 output and 1 or 2 input channels how would this be mapped. e.g:
    // - mono output is mapped to both LEFT and RIGHT, stereo output is mapped to channel LEFT and RIGHT each, 3 channels are mapped to LEFT, CENTER;, RIGHT, etcetera
    //    audiocodec_callback_class_f(input, output, length);
    if (fAudioDevices.size() > 1) {
        println("multiple audio devices detected. currently only one device supported");
    }
    for (auto* device: fAudioDevices) {
        // TODO create method to copy device info into audioblock
        AudioBlock audio_block; // "how to include `AudioBLock`?"
        audio_block.sample_rate     = device->get_audioinfo()->sample_rate;
        audio_block.output_channels = audio_output_channels;
        audio_block.input_channels  = audio_input_channels;
        audio_block.block_size      = length; // TODO what if blocksizes do not align?!?
        audio_block.output          = output; // TODO this needs to be handle for each device
        audio_block.input           = input;  // TODO this needs to be handle for each device
        audio_block.device_id       = device->get_id();
        KLST_BSP_audiocodec_process_audioblock_data(&audio_block);
    }
    // TODO merge into `float** input, float** output`

    /* fill buffers for oscilloscope visualization */
    // TODO this is complicated if buffer sizes do not align, currently
    // `mOutputBuffers` and `mInputBuffers` are statically allocated in the beginning
    // which will not work ...
    if (mOutputBuffers == nullptr || mInputBuffers == nullptr) {
        return;
    }
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < audio_output_channels; j++) {
            mOutputBuffers[j][i] = constrain(output[j][i], -1.0f, 1.0f);
        }
    }
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < audio_input_channels; j++) {
            mInputBuffers[j][i] = constrain(input[j][i], -1.0f, 1.0f);
        }
    }
}

void KlangstromEmulator::keyPressed() {
    if (key == 'q') {
        exit();
    }
}

std::string KlangstromEmulator::get_emulator_name() {
#if defined(GENERIC_EMU)
    return "GENERIC";
#elif defined(KLST_CORE_EMU)
    return "KLST_CORE";
#elif defined(KLST_TINY_EMU)
    return "KLST_TINY";
#elif defined(KLST_SHEEP_EMU)
    return "KLST_SHEEP";
#elif defined(KLST_PANDA_EMU)
    return "KLST_PANDA";
#elif defined(KLST_CATERPILLAR_EMU)
    return "KLST_CATERPILLAR";
#else
    return "(UNDEFINED)";
#endif
}

KlangstromEmulator* KlangstromEmulator::instance() {
    if (fInstance == nullptr) {
        fInstance = new KlangstromEmulator();
    }
    return fInstance;
}

PApplet* umgebung::instance() {
    return KlangstromEmulator::instance();
}

void KlangstromEmulator::register_drawable(Drawable* drawable) {
    drawables.push_back(drawable);
}

uint8_t KlangstromEmulator::register_audio_device(AudioInfo* audioinfo) {
    // TODO here we need to communicate with the underlying layer. thoughts are:
    // - to create a device ID and return it
    // - emulate sample rate and bit depth
    // - resepct output channels and input channels … maybe mix them in underlying layer into stereo
    auto* mAudioDevice = new KlangstromEmulatorAudioDevice(audioinfo, audio_device_id);
    fAudioDevices.push_back(mAudioDevice);
    audio_device_id++;
    return mAudioDevice->get_id();
}

void KlangstromEmulator::delay_loop(uint32_t microseconds) {
    task.sleep_for(microseconds);
}
