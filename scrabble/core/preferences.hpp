#pragma once

#include "core/dictionary.hpp"

namespace core {
class Preferences {
 public:
  Preferences();

  void SetSoundVolume(const int volume) { sound_volume_ = volume; }

  void SetDictionaryType(Dictionary::DictionaryType type) {
    dictionary_type_ = type;
  }

  void SetResolution(const int width, const int height) {
    resolution_width_ = width;
    resolution_height_ = height;
  }

  void SetVSyncStatus(const bool vsync_enabled) {
    vsync_enabled_ = vsync_enabled;
  }

  int GetResolutionWidth() const { return resolution_width_; }
  int GetResolutionHeight() const { return resolution_height_; }
  Dictionary::DictionaryType GetDictionaryType() { return dictionary_type_; }
  int GetSoundVolume() const { return sound_volume_; }
  bool GetVsyncStatus() const { return vsync_enabled_; }

 private:
  Dictionary::DictionaryType dictionary_type_ = Dictionary::CSW;

  int sound_volume_ = kDefaultSoundVolume;

  int resolution_width_ = kDefaultResolutionWidth;
  int resolution_height_ = kDefaultResolutionHeight;

  bool vsync_enabled_ = true;

  static constexpr int kDefaultSoundVolume = 100;
  static constexpr int kDefaultResolutionWidth = 1280;
  static constexpr int kDefaultResolutionHeight = 720;
};
}  // namespace core