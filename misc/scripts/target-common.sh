#!/bin/bash

# TODO: use CURDIR or something
export NAME=$(basename $(pwd))
export OUT=./deploy
export SD_OUT=/atmosphere/contents/${PROGRAM_ID}/exefs
