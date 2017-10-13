#!/bin/bash

rm -rf build/C*
rm -rf build/s*
rm -rf build/c*
rm -rf build/M*

rm -rf lib/*
rm -rf bin/p*
rm -rf bin/i*

find . -name '.DS_Store'  -print -exec rm -rfv {} \;
find . -name '*~'  -print -exec rm -rfv {} \;
find . -name '#*'  -print -exec rm -rfv {} \;
find . -name '*.moos++'  -print -exec rm -rfv {} \;

#find . -name 'MOOSLog*'  -print -exec rm -rfv {} \;

