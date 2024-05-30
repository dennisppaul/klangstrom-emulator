#!/bin/bash

arduino-cli compile -u -v -b klangstrom:emulator:KLST_EMU:board=$2 $1
