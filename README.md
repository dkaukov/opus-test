# ESP32 ADC + Opus SRAM Pressure Demo

Minimal PlatformIO project that starts ESP32 ADC capture with Arduino Audio Tools and feeds samples into the Arduino Audio Tools Opus encoder.

Two environments are provided:

```sh
pio run -e esp32dev_no_bt
pio run -e esp32dev_bt
```

The Bluetooth build enables classic `BluetoothSerial` before ADC and Opus setup:

```sh
pio run -e esp32dev_bt -t upload
pio device monitor --port /dev/cu.usbserial-0001 --baud 115200 --filter printable
```

