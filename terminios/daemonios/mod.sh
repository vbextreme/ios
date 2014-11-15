#!/bin/bash
modprobe gpio-pca953x
modprobe i2c-gpio-custom bus0=4,200,199
echo tca6416 0x20 > /sys/devices/platform/i2c-gpio.4/i2c-4/new_device