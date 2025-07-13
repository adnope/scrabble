#pragma once

#include "core/dictionary.hpp"

namespace core {
class Preferences {
 public:
  Preferences();

  void SetSoundVolume(const int volume) { sound_volume = volume; }

  void SetDictionaryType(Dictionary::DictionaryType type) {
    dictionary_type = type;
  }

  void SetResolution(const int width, const int height) {
    resolution_width = width;
    resolution_height = height;
  }

  void SetVSyncStatus(const bool vsync_enabled) { vsync_on = vsync_enabled; }

  int GetResolutionWidth() const { return resolution_width; }
  int GetResolutionHeight() const { return resolution_height; }
  Dictionary::DictionaryType GetDictionaryType() { return dictionary_type; }
  int GetSoundVolume() const { return sound_volume; }
  bool GetVsyncStatus() const { return vsync_on; }

 private:
  Dictionary::DictionaryType dictionary_type = Dictionary::CSW;

  int sound_volume = DEFAULT_AUDIO_VOLUME;

  int resolution_width = DEFAULT_RESOLUTION_WIDTH;
  int resolution_height = DEFAULT_RESOLUTION_HEIGHT;

  bool vsync_on = true;

  static constexpr int DEFAULT_AUDIO_VOLUME = 100;
  static constexpr int DEFAULT_RESOLUTION_WIDTH = 1280;
  static constexpr int DEFAULT_RESOLUTION_HEIGHT = 720;
};
}  // namespace core