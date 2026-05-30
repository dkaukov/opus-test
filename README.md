# ESP32 ADC + Opus SRAM Pressure Demo

Minimal PlatformIO project that starts ESP32 ADC capture with Arduino Audio Tools and feeds samples into the Arduino Audio Tools Opus encoder.

Two environments are provided:

```sh
pio run -e esp32dev_no_bt -t upload -t monitor
pio run -e esp32dev_bt -t upload -t monitor
pio run -e esp32dev_patch_no_bt -t upload -t monitor
pio run -e esp32dev_patch_bt -t upload -t monitor
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

```

assert failed: block_locate_free heap_tlsf.c:441 (block_size(block) >= size)


Backtrace: 0x40083805:0x3ffb3de0 0x4008a045:0x3ffb3e00 0x4008f031:0x3ffb3e20 0x4008e48a:0x3ffb3f50 0x4008eb4f:0x3ffb3f70 0x4008ec8c:0x3ffb3f90 0x40083ba4:0x3ffb3fb0 0x40083dd3:0x3ffb3fe0 0x400ece9c:0x3ffb4000 0x400ee058:0x3ffb4040 0x400eea69:0x3ffb4090 0x400d6392:0x3ffb40d0 0x400d437d:0x3ffb4130 0x400d43b5:0x3ffb4180 0x400d6d08:0x3ffb4200 0x400ec94d:0x3ffb4290
  #0  0x40083805 in panic_abort at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/esp_system/panic.c:408
  #1  0x4008a045 in esp_system_abort at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/esp_system/esp_system.c:137
  #2  0x4008f031 in __assert_func at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/newlib/assert.c:85
  #3  0x4008e48a in block_locate_free at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/heap/heap_tlsf.c:441
      (inlined by) tlsf_malloc at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/heap/heap_tlsf.c:850
  #4  0x4008eb4f in multi_heap_malloc_impl at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/heap/multi_heap.c:187
  #5  0x4008ec8c in multi_heap_malloc at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/heap/multi_heap_poisoning.c:234
      (inlined by) multi_heap_malloc at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/heap/multi_heap_poisoning.c:223
  #6  0x40083ba4 in heap_caps_malloc_base at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/heap/heap_caps.c:175
      (inlined by) heap_caps_malloc_base at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/heap/heap_caps.c:120
  #7  0x40083dd3 in heap_caps_calloc_base at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/heap/heap_caps.c:487
      (inlined by) heap_caps_calloc at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/heap/heap_caps.c:496
  #8  0x400ece9c in i2s_alloc_dma_buffer at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/driver/i2s.c:740
      (inlined by) i2s_realloc_dma_buffer at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/driver/i2s.c:788
  #9  0x400ee058 in i2s_set_clk at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/driver/i2s.c:1739
  #10 0x400eea69 in i2s_driver_install at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/driver/i2s.c:2027
  #11 0x400d6392 in audio_tools::AnalogDriverESP32::begin(audio_tools::AnalogConfigESP32) at .pio/libdeps/esp32dev_patch_no_bt/arduino-audio-tools/src/AudioTools/CoreAudio/AudioAnalog/AnalogDriverESP32.h:78
  #12 0x400d437d in audio_tools::AnalogAudioStream::begin(audio_tools::AnalogConfigESP32) at .pio/libdeps/esp32dev_patch_no_bt/arduino-audio-tools/src/AudioTools/CoreAudio/AudioAnalog/AnalogAudioStream.h:67 (discriminator 3)
  #13 0x400d43b5 in (anonymous namespace)::start_adc() at src/main.cpp:156
  #14 0x400d6d08 in loop() at src/main.cpp:289
  #15 0x400ec94d in loopTask(void*) at /Users/ziss/.platformio/packages/framework-arduinoespressif32/cores/esp32/main.cpp:50
```
