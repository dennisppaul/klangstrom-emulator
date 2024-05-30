#!/bin/bash

arduino-cli compile -v -b klangstrom:emulator:KLST_EMU:board=$2 $1
