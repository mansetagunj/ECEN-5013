#!/bin/bash

find . \( -name "*.c" -o -name "*.h" \) -exec cat {} \; > codedump.doc
