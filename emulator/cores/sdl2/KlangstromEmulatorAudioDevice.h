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

#include <stdint.h>
#include "KlangstromAudio.h"

class KlangstromEmulatorAudioDevice {
public:
    KlangstromEmulatorAudioDevice(AudioInfo* audioinfo, uint8_t device_id) : fAudioinfo(audioinfo), id(device_id) {
        // TODO here we need to communicate with the underlying layer. thoughts are:
        // - to create a device ID and return it
        // - emulate sample rate and bit depth
        // - resepct output channels and input channels … maybe mix them in underlying layer into stereo
        // TODO this is just a quick hack and needs to come from underyling layer
    }

    uint8_t    get_id() const { return id; }
    AudioInfo* get_audioinfo() const { return fAudioinfo; }

private:
    const uint8_t id;
    AudioInfo*    fAudioinfo;
};
