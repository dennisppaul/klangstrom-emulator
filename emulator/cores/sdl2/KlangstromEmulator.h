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

#pragma once

#include "Arduino.h"
#include <iostream>
#include <vector>

#include "Umgebung.h"
#include "Drawable.h"
#include "PeriodicalTask.h"
#include "KlangstromAudio.h"
#include "KlangstromEmulatorAudioDevice.h"

using namespace umgebung;

class KlangstromEmulator : public PApplet {
    PVector           mVector{16, 16};
    PShape            mShape;
    std::string       mFontPath = sketchPath();
    const std::string mFontName = "JetBrainsMonoNL-Light.ttf";
    PFont*            mFont     = nullptr;

public:
    ~KlangstromEmulator() {
        for (auto* device: fAudioDevices) {
            delete device;
        }
        task.stop();
    }
    static KlangstromEmulator* instance();
    void                       arguments(std::vector<std::string> args) override;
    void                       settings() override;
    void                       setup() override;
    void                       draw() override;
    void                       audioblock(float** input, float** output, int length) override;
    void                       keyPressed() override;
    static std::string         get_emulator_name();
    void                       register_drawable(Drawable* drawable);
    void                       delay_loop(uint32_t ms);
    void                       set_emulator_speed(float loop_frequency_hz) { task.set_frequency(loop_frequency_hz); }
    float**                    get_audio_output_buffers() { return mOutputBuffers; }
    float**                    get_audio_input_buffers() { return mInputBuffers; }
    uint8_t                    register_audio_device(AudioInfo* audioinfo);

private:
    static KlangstromEmulator*                  fInstance;
    float                                       DEFAULT_FONT_SIZE = 24;
    PeriodicalTask                              task;
    std::vector<Drawable*>                      drawables;
    float**                                     mOutputBuffers  = nullptr;
    float**                                     mInputBuffers   = nullptr;
    uint8_t                                     audio_device_id = 0;
    std::vector<KlangstromEmulatorAudioDevice*> fAudioDevices;

    void process_device(KlangstromEmulatorAudioDevice* device, AudioBlock* audio_block);
};
