#!/bin/bash

pkill vtp-server
rmmod virtual_touchpad
insmod ./virtual_touchpad.ko
./vtp-server&
