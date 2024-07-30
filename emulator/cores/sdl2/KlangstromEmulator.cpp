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

    std::string font_file;
    if (exists(mFontPath + "/" + mFontName)) {
        font_file = mFontPath + "/" + mFontName;
    } else if (exists(mFontPath + "/../" + mFontName)) {
        font_file = mFontPath + "/../" + mFontName;
    }

    if (!font_file.empty()) {
        mFont = loadFont(font_file, DEFAULT_FONT_SIZE);
        textFont(mFont);
    } else {
        println("could not load font: ",
                (mFontPath + "/" + mFontName),
                " or ",
                (mFontPath + "/../" + mFontName));
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

void KlangstromEmulator::process_device(KlangstromEmulatorAudioDevice* device,
                                        AudioBlock*                    audio_block) {
    audio_block->sample_rate = device->get_audioinfo()->sample_rate;
    audio_block->block_size  = device->get_audioinfo()->block_size;
    audio_block->device_id   = device->get_id();
    KLST_BSP_audiocodec_process_audioblock_data(audio_block);
}

/**
 * called from umgebung to request and process audio data
 * @param input
 * @param output
 * @param length
 */
void KlangstromEmulator::audioblock(float** input, float** output, int length) {
    if (fAudioDevices.size() > 1) {
        println("multiple audio devices detected. currently only one device supported. only the last audio device will be audible ...");
    }
    for (auto* device: fAudioDevices) {
        AudioBlock audio_block;
        const int  block_size = device->get_audioinfo()->block_size;

        if (block_size > length) {
            println("block size mismatch: reduce device block size (", block_size, ") to either equal or smaller multiple of ", length);
            continue;
        }

        if (length % block_size != 0) {
            println("block size mismatch: device block size (", block_size, ") must be multiple of ", length);
            continue;
        }

        // TODO here we need to collect data from all audio devices and mix them into the output buffer
        // TODO assuming that the underlying audio system always has 2 output and 1 or 2 input channels how would this be mapped. e.g:
        // - mono output is mapped to both LEFT and RIGHT, stereo output is mapped to channel LEFT and RIGHT each, 3 channels are mapped to LEFT, CENTER;, RIGHT, etcetera
        //    audiocodec_callback_class_f(input, output, length);
        // TODO handle cases where number of output and input channels do not match.
        // TODO especially when device has or expects more channels than audio system
        if (device->get_audioinfo()->output_channels > audio_output_channels) {
            println("output channels mismatch: device output channels (", static_cast<int>(device->get_audioinfo()->output_channels), ") must match audio system output channels (", audio_output_channels, ")");
            continue;
        }
        if (device->get_audioinfo()->input_channels > audio_input_channels) {
            println("input channels mismatch: device input channels (", static_cast<int>(device->get_audioinfo()->input_channels), ") must match audio system input channels (", audio_input_channels, ")");
            continue;
        }
        audio_block.output_channels = device->get_audioinfo()->output_channels;
        audio_block.input_channels  = device->get_audioinfo()->input_channels;

        /* process audio data ( if need be in multiple passes ) */
        const uint8_t mPasses = length / block_size;
        if (mPasses == 1) {
            audio_block.output = output;
            audio_block.input  = input;
            process_device(device, &audio_block);
        } else {
            for (uint8_t i = 0; i < mPasses; ++i) {
                float* output_ptr[audio_output_channels];
                float* input_ptr[audio_input_channels];

                for (int ch = 0; ch < audio_output_channels; ++ch) {
                    output_ptr[ch] = output[ch] + i * block_size;
                }

                for (int ch = 0; ch < audio_input_channels; ++ch) {
                    input_ptr[ch] = input[ch] + i * block_size;
                }

                audio_block.output = output_ptr;
                audio_block.input  = input_ptr;

                process_device(device, &audio_block);
            }
        }
    }

    /* fill buffers for oscilloscope visualization */
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
    auto* mAudioDevice = new KlangstromEmulatorAudioDevice(audioinfo, audio_device_id);
    fAudioDevices.push_back(mAudioDevice);
    audio_device_id++;
    return mAudioDevice->get_id();
}

void KlangstromEmulator::delay_loop(uint32_t microseconds) {
    task.sleep_for(microseconds);
}
