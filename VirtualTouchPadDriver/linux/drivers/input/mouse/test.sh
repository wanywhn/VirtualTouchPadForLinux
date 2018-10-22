#!/bin/bash

pkill vtp_server
rmmod virtual_touchpad
insmod virtual_touchpad.ko
vtp_server -sd &
