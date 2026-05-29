#include <Arduino.h>
#include <esp_system.h>

#if ENABLE_BT
#include <BluetoothSerial.h>
#endif

#include "AudioTools.h"
#include "AudioTools/AudioCodecs/CodecOpus.h"

using namespace audio_tools;

namespace {

constexpr uint32_t kSampleRate = 16000;
constexpr uint8_t kChannels = 1;
constexpr uint8_t kBitsPerSample = 16;
constexpr uint32_t kReportIntervalMs = 1000;

#if ENABLE_BT
BluetoothSerial bt;
#endif

AnalogAudioStream adc;
MeasuringStream encodedBytes;
OpusAudioEncoder opus;
EncodedAudioStream opusStream(&encodedBytes, &opus);
StreamCopy copier(opusStream, adc);

uint32_t last_report_ms = 0;

const char *reset_reason_name(esp_reset_reason_t reason) {
  switch (reason) {
    case ESP_RST_POWERON:
      return "poweron";
    case ESP_RST_EXT:
      return "external";
    case ESP_RST_SW:
      return "software";
    case ESP_RST_PANIC:
      return "panic";
    case ESP_RST_INT_WDT:
      return "interrupt_wdt";
    case ESP_RST_TASK_WDT:
      return "task_wdt";
    case ESP_RST_WDT:
      return "other_wdt";
    case ESP_RST_BROWNOUT:
      return "brownout";
    default:
      return "unknown";
  }
}

void print_memory(const char *label) {
  Serial.printf(
      "%s: free_heap=%u min_free_heap=%u largest_block=%u free_psram=%u loop_stack_free=%u\n",
      label,
      ESP.getFreeHeap(),
      ESP.getMinFreeHeap(),
      heap_caps_get_largest_free_block(MALLOC_CAP_8BIT),
      ESP.getFreePsram(),
      uxTaskGetStackHighWaterMark(nullptr) * sizeof(StackType_t));
}

}  // namespace

void setup() {
  Serial.begin(115200);
  delay(100);

  AudioLogger::instance().begin(Serial, AudioLogger::Info);
  Serial.printf("reset_reason=%s (%d)\n",
                reset_reason_name(esp_reset_reason()),
                static_cast<int>(esp_reset_reason()));
  print_memory("boot");

#if ENABLE_BT
  if (!bt.begin("esp32-opus-sram-test")) {
    Serial.println("BluetoothSerial failed to start");
  }
  print_memory("after BluetoothSerial.begin");
#else
  Serial.println("BluetoothSerial disabled by build flag");
#endif

  auto adc_config = adc.defaultConfig(RX_MODE);
  adc_config.sample_rate = kSampleRate;
  adc_config.channels = kChannels;
  adc_config.bits_per_sample = kBitsPerSample;
  adc_config.buffer_count = 8;
  adc_config.buffer_size = 1024;

  if (!adc.begin(adc_config)) {
    Serial.println("ADC failed to start");
    while (true) {
      delay(1000);
    }
  }
  print_memory("after ADC begin");

  auto &opus_config = opus.config();
  opus_config.sample_rate = kSampleRate;
  opus_config.application = OPUS_APPLICATION_AUDIO;
  opus_config.frame_sizes_ms_x2 = OPUS_FRAMESIZE_40_MS;
  opus_config.vbr = 1;
  opus_config.max_bandwidth = OPUS_BANDWIDTH_NARROWBAND;
  opus.begin(opus_config);
  if (!opusStream.begin(AudioInfo(kSampleRate, kChannels, kBitsPerSample))) {
    Serial.println("Opus encoder stream failed to start");
    while (true) {
      delay(1000);
    }
  }
  print_memory("after Opus begin");
  Serial.println("Encoding ADC samples to Opus. Analog input defaults to GPIO34 on classic ESP32.");
}

void loop() {
  copier.copy();

  const uint32_t now = millis();
  if (now - last_report_ms >= kReportIntervalMs) {
    last_report_ms = now;
    print_memory("running");
  }
}
