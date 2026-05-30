#include <Arduino.h>
#include <esp_system.h>

#if ENABLE_BT
#include <BluetoothSerial.h>
#endif

#include "AudioTools.h"
#include "AudioTools/AudioCodecs/CodecOpus.h"

#ifndef ENABLE_BT
#define ENABLE_BT 0
#endif

#ifndef ENABLE_CODEC_TEST
#define ENABLE_CODEC_TEST 0
#endif

using namespace audio_tools;

namespace {

constexpr uint32_t kSampleRate = 16000;
constexpr uint8_t kChannels = 1;
constexpr uint8_t kBitsPerSample = 16;
constexpr uint32_t kReportIntervalMs = 1000;
constexpr uint32_t kCodecCaptureMs = 700;
constexpr uint32_t kCodecPauseMs = 250;
constexpr size_t kProtoMtu = 256;
constexpr size_t kMaxOpusPackets = 32;
constexpr size_t kMaxOpusPacketBytes = kProtoMtu;

#if ENABLE_CODEC_TEST
enum class CodecPhase {
  Capture,
  Playback,
  Pause,
};

class OpusPacketStore : public Print {
 public:
  void clear() {
    packet_count_ = 0;
    dropped_packets_ = 0;
    dropped_bytes_ = 0;
    total_bytes_ = 0;
  }

  size_t write(uint8_t value) override {
    return write(&value, 1);
  }

  size_t write(const uint8_t *data, size_t len) override {
    if (packet_count_ >= kMaxOpusPackets) {
      dropped_packets_++;
      dropped_bytes_ += len;
      return len;
    }

    size_t stored_len = min(len, kMaxOpusPacketBytes);
    memcpy(packets_[packet_count_], data, stored_len);
    packet_lengths_[packet_count_] = stored_len;
    packet_count_++;
    total_bytes_ += stored_len;

    if (stored_len < len) {
      dropped_bytes_ += len - stored_len;
    }
    return len;
  }

  size_t packetCount() const { return packet_count_; }
  size_t totalBytes() const { return total_bytes_; }
  size_t droppedPackets() const { return dropped_packets_; }
  size_t droppedBytes() const { return dropped_bytes_; }
  const uint8_t *packet(size_t index) const { return packets_[index]; }
  size_t packetLength(size_t index) const { return packet_lengths_[index]; }

 private:
  uint8_t packets_[kMaxOpusPackets][kMaxOpusPacketBytes]{};
  uint16_t packet_lengths_[kMaxOpusPackets]{};
  size_t packet_count_ = 0;
  size_t dropped_packets_ = 0;
  size_t dropped_bytes_ = 0;
  size_t total_bytes_ = 0;
};
#endif

#if ENABLE_BT
BluetoothSerial bt;
#endif

AnalogAudioStream adc;

#if ENABLE_CODEC_TEST
OpusPacketStore encodedBytes;
#else
MeasuringStream encodedBytes;
#endif

OpusAudioEncoder opus;
EncodedAudioStream opusStream(&encodedBytes, &opus);
StreamCopy copier(opusStream, adc);

#if ENABLE_CODEC_TEST
AnalogAudioStream dac;
OpusAudioDecoder opusDecoder;
EncodedAudioStream decodedStream(&dac, &opusDecoder);
CodecPhase codec_phase = CodecPhase::Capture;
uint32_t phase_started_ms = 0;
#endif

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

bool start_adc() {
  auto adc_config = adc.defaultConfig(RX_MODE);
  adc_config.sample_rate = kSampleRate;
  adc_config.channels = kChannels;
  adc_config.bits_per_sample = kBitsPerSample;
  adc_config.buffer_count = 8;
  adc_config.buffer_size = 1024;
  return adc.begin(adc_config);
}

#if ENABLE_CODEC_TEST
bool start_dac_decoder() {
  auto dac_config = dac.defaultConfig(TX_MODE);
  dac_config.sample_rate = kSampleRate;
  dac_config.channels = kChannels;
  dac_config.bits_per_sample = kBitsPerSample;
  dac_config.buffer_count = 8;
  dac_config.buffer_size = 1024;
  if (!dac.begin(dac_config)) {
    return false;
  }

  AudioInfo info(kSampleRate, kChannels, kBitsPerSample);
  opusDecoder.setAudioInfo(info);
  auto &decoder_config = opusDecoder.config();
  decoder_config.sample_rate = kSampleRate;
  decoder_config.channels = kChannels;
  decoder_config.bits_per_sample = kBitsPerSample;
  decoder_config.max_buffer_write_size = kProtoMtu;
  if (!opusDecoder.begin(decoder_config)) {
    return false;
  }
  return decodedStream.begin(info);
}

void stop_dac_decoder() {
  // OpusAudioDecoder::end() in arduino-audio-tools v1.0.1/libopus a1.1.0
  // destroys a decoder pointer backed by its Vector storage, which corrupts
  // heap. Keep the decoder object alive and cycle the analog DAC instead.
  dac.end();
}

void play_stored_packets() {
  Serial.printf("playback: packets=%u bytes=%u dropped_packets=%u dropped_bytes=%u\n",
                static_cast<unsigned>(encodedBytes.packetCount()),
                static_cast<unsigned>(encodedBytes.totalBytes()),
                static_cast<unsigned>(encodedBytes.droppedPackets()),
                static_cast<unsigned>(encodedBytes.droppedBytes()));

  for (size_t i = 0; i < encodedBytes.packetCount(); ++i) {
    decodedStream.write(encodedBytes.packet(i), encodedBytes.packetLength(i));
    delay(1);
  }
}
#endif

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

  if (!start_adc()) {
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
#if ENABLE_CODEC_TEST
  encodedBytes.clear();
  phase_started_ms = millis();
  Serial.println("Codec test: ADC encode phase, then DAC decode phase. No ADC/DAC duplex.");
#else
  Serial.println("Encoding ADC samples to Opus. Analog input defaults to GPIO34 on classic ESP32.");
#endif
}

void loop() {
#if ENABLE_CODEC_TEST
  const uint32_t now = millis();

  switch (codec_phase) {
    case CodecPhase::Capture:
      copier.copy();
      if (now - phase_started_ms >= kCodecCaptureMs ||
          encodedBytes.packetCount() >= kMaxOpusPackets) {
        adc.end();
        print_memory("after capture");
        if (start_dac_decoder()) {
          codec_phase = CodecPhase::Playback;
        } else {
          Serial.println("DAC/decoder failed to start");
          codec_phase = CodecPhase::Pause;
        }
        phase_started_ms = now;
      }
      break;

    case CodecPhase::Playback:
      play_stored_packets();
      stop_dac_decoder();
      print_memory("after playback");
      codec_phase = CodecPhase::Pause;
      phase_started_ms = now;
      break;

    case CodecPhase::Pause:
      if (now - phase_started_ms >= kCodecPauseMs) {
        encodedBytes.clear();
        if (!start_adc()) {
          Serial.println("ADC restart failed");
          delay(1000);
          return;
        }
        codec_phase = CodecPhase::Capture;
        phase_started_ms = now;
        print_memory("capture restart");
      }
      break;
  }
#else
  copier.copy();

  const uint32_t now = millis();
#endif
  if (now - last_report_ms >= kReportIntervalMs) {
    last_report_ms = now;
    print_memory("running");
  }
}
