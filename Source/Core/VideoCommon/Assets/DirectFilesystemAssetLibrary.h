// Copyright 2023 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <chrono>
#include <filesystem>
#include <map>
#include <string>

#include "VideoCommon/Assets/CustomAssetLibrary.h"

namespace VideoCommon
{
class CustomTextureData;

// This class implements 'CustomAssetLibrary' and loads any assets
// directly from the filesystem
class DirectFilesystemAssetLibrary final : public CustomAssetLibrary
{
public:
  LoadInfo LoadTexture(const AssetID& asset_id, CustomTextureData* data) override;

  // Gets the latest time from amongst all the files in the asset map
  TimeType GetLastAssetWriteTime(const AssetID& asset_id) const override;

  // Assigns the asset id to a map of files, how this map is read is dependent on the data
  // For instance, a raw texture would expect the map to have a single entry and load that
  // file as the asset.  But a model file data might have its data spread across multiple files
  void SetAssetIDMapData(const AssetID& asset_id,
                         std::map<std::string, std::filesystem::path> asset_path_map);

private:
  // Loads additional mip levels into the texture structure until _mip<N> texture is not found
  bool LoadMips(const std::filesystem::path& asset_path, CustomTextureData* data);
  std::map<AssetID, std::map<std::string, std::filesystem::path>> m_assetid_to_asset_map_path;
};
}  // namespace VideoCommon
