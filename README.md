# ESP32 ADC + Opus SRAM Pressure Demo

Minimal PlatformIO project that starts ESP32 ADC capture with Arduino Audio Tools and feeds samples into the Arduino Audio Tools Opus encoder.

Two environments are provided:

```sh
pio run -e esp32dev_no_bt
pio run -e esp32dev_bt
pio run -e esp32dev_patch_no_bt
pio run -e esp32dev_patch_bt
```

Failure:
```

assert failed: opus_encode_native opus_encoder.c:1631 (pcm_buf!=NULL)


Backtrace: 0x4008394d:0x3ffccf10 0x40094fa1:0x3ffccf30 0x4009a4f9:0x3ffccf50 0x400d8f5b:0x3ffcd080 0x400d9e05:0x3ffcd1b0 0x400d4828:0x3ffcd1f0 0x400d497a:0x3ffcda20 0x400d3a15:0x3ffcda40 0x400d3a73:0x3ffcda60 0x400d6a8a:0x3ffcda80 0x400d6cc6:0x3ffcdaa0 0x400d6dcf:0x3ffcdae0 0x400f55fd:0x3ffcdb00
  #0  0x4008394d in uart_ll_set_baudrate at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/hal/esp32/include/hal/uart_ll.h:121
      (inlined by) esp_rom_uart_set_clock_baudrate at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/esp_rom/patches/esp_rom_uart.c:35
  #1  0x400d8f5b in opus_encode_native at .pio/libdeps/esp32dev_no_bt/arduino-libopus/src/opus-1.3.1/src/opus_encoder.c:1826
  #2  0x400d9e05 in opus_encoder_ctl at .pio/libdeps/esp32dev_no_bt/arduino-libopus/src/opus-1.3.1/src/opus_encoder.c:2441
  #3  0x400d4828 in audio_tools::OpusAudioEncoder::settings() at .pio/libdeps/esp32dev_no_bt/arduino-audio-tools/src/AudioTools/AudioCodecs/CodecOpus.h:447 (discriminator 1)
  #4  0x400d497a in audio_tools::TransformationReader<audio_tools::ReformatBaseStream>::begin(audio_tools::ReformatBaseStream*, Stream*) at .pio/libdeps/esp32dev_no_bt/arduino-audio-tools/src/AudioTools/CoreAudio/AudioIO.h:30
  #5  0x400d3a15 in audio_tools::AudioOutput::setAudioInfo(audio_tools::AudioInfo) at .pio/libdeps/esp32dev_no_bt/arduino-audio-tools/src/AudioTools/CoreAudio/AudioOutput.h:47 (discriminator 2)
  #6  0x400d3a73 in non-virtual thunk to audio_tools::AudioOutput::setAudioInfo(audio_tools::AudioInfo) at ??:?
  #7  0x400d6a8a in audio_tools::ConverterAutoCenterT<int>::convert(unsigned char*, unsigned int) at .pio/libdeps/esp32dev_no_bt/arduino-audio-tools/src/AudioTools/CoreAudio/BaseConverter.h:130 (discriminator 2)
  #8  0x400d6cc6 in audio_tools::int24_4bytes_t::set(int const&) at .pio/libdeps/esp32dev_no_bt/arduino-audio-tools/src/AudioTools/CoreAudio/AudioBasic/Int24_4bytes_t.h:59
      (inlined by) audio_tools::int24_4bytes_t::operator=(float const&) at .pio/libdeps/esp32dev_no_bt/arduino-audio-tools/src/AudioTools/CoreAudio/AudioBasic/Int24_4bytes_t.h:71
      (inlined by) audio_tools::ConverterAutoCenterT<audio_tools::int24_4bytes_t>::convert(unsigned char*, unsigned int) at .pio/libdeps/esp32dev_no_bt/arduino-audio-tools/src/AudioTools/CoreAudio/BaseConverter.h:129
  #9  0x400d6dcf in audio_tools::ConverterAutoCenterT<short>::setup(short*, unsigned int) at .pio/libdeps/esp32dev_no_bt/arduino-audio-tools/src/AudioTools/CoreAudio/BaseConverter.h:174
      (inlined by) audio_tools::ConverterAutoCenterT<short>::setup(short*, unsigned int) at .pio/libdeps/esp32dev_no_bt/arduino-audio-tools/src/AudioTools/CoreAudio/BaseConverter.h:150
  #10 0x400f55fd in uart_set_parity at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/driver/uart.c:281 (discriminator 2)
```

