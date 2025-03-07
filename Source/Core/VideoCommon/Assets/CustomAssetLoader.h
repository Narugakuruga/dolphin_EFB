// Copyright 2023 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <thread>

#include "Common/Flag.h"
#include "Common/WorkQueueThread.h"
#include "VideoCommon/Assets/CustomAsset.h"
#include "VideoCommon/Assets/TextureAsset.h"

namespace VideoCommon
{
// This class is responsible for loading data asynchronously when requested
// and watches that data asynchronously reloading it if it changes
class CustomAssetLoader
{
public:
  CustomAssetLoader() = default;
  ~CustomAssetLoader() = default;
  CustomAssetLoader(const CustomAssetLoader&) = delete;
  CustomAssetLoader(CustomAssetLoader&&) = delete;
  CustomAssetLoader& operator=(const CustomAssetLoader&) = delete;
  CustomAssetLoader& operator=(CustomAssetLoader&&) = delete;

  void Init();
  void Shutdown();

  // The following Load* functions will load or create an asset associated
  // with the given asset id
  // Loads happen asynchronously where the data will be set now or in the future
  // Callees are expected to query the underlying data with 'GetData()'
  // from the 'CustomLoadableAsset' class to determine if the data is ready for use
  std::shared_ptr<RawTextureAsset> LoadTexture(const CustomAssetLibrary::AssetID& asset_id,
                                               std::shared_ptr<CustomAssetLibrary> library);

  std::shared_ptr<GameTextureAsset> LoadGameTexture(const CustomAssetLibrary::AssetID& asset_id,
                                                    std::shared_ptr<CustomAssetLibrary> library);

private:
  // TODO C++20: use a 'derived_from' concept against 'CustomAsset' when available
  template <typename AssetType>
  std::shared_ptr<AssetType>
  LoadOrCreateAsset(const CustomAssetLibrary::AssetID& asset_id,
                    std::map<CustomAssetLibrary::AssetID, std::weak_ptr<AssetType>>& asset_map,
                    std::shared_ptr<CustomAssetLibrary> library)
  {
    auto [it, inserted] = asset_map.try_emplace(asset_id);
    if (!inserted)
      return it->second.lock();
    std::shared_ptr<AssetType> ptr(new AssetType(std::move(library), asset_id), [&](AssetType* a) {
      asset_map.erase(a->GetAssetId());
      m_total_bytes_loaded -= a->GetByteSizeInMemory();
      std::lock_guard lk(m_assets_lock);
      m_assets_to_monitor.erase(a->GetAssetId());
      delete a;
    });
    it->second = ptr;
    m_asset_load_thread.Push(it->second);
    return ptr;
  }

  static constexpr auto TIME_BETWEEN_ASSET_MONITOR_CHECKS = std::chrono::milliseconds{500};
  std::map<CustomAssetLibrary::AssetID, std::weak_ptr<RawTextureAsset>> m_textures;
  std::map<CustomAssetLibrary::AssetID, std::weak_ptr<GameTextureAsset>> m_game_textures;
  std::thread m_asset_monitor_thread;
  Common::Flag m_asset_monitor_thread_shutdown;

  std::size_t m_total_bytes_loaded = 0;
  std::size_t m_max_memory_available = 0;

  std::map<CustomAssetLibrary::AssetID, std::weak_ptr<CustomAsset>> m_assets_to_monitor;
  std::mutex m_assets_lock;
  Common::WorkQueueThread<std::weak_ptr<CustomAsset>> m_asset_load_thread;
};
}  // namespace VideoCommon
