#include "VoxelAssetLoader.h"

#include <fstream>
#include <string>
#include <Engine/Core/Log.h>

#define OGT_VOX_IMPLEMENTATION
#include <Engine/vendor/ogt_vox/ogt_vox.h>

namespace Engine
{
    std::shared_ptr<VoxelAsset> VoxelAssetLoader::Load(const std::filesystem::path& path)
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);

        if (!file.is_open())
        {
            ENGINE_ERROR("Failed to open voxel file: {}", path.string());
            return nullptr;
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> buffer(static_cast<size_t>(size));

        if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
        {
            ENGINE_ERROR("Failed to read voxel file: {}", path.string());
            return nullptr;
        }

        const ogt_vox_scene* scene = ogt_vox_read_scene(buffer.data(), buffer.size());

        if (!scene)
        {
            ENGINE_ERROR("Failed to parse MagicaVoxel scene: {}", path.string());
            return nullptr;
        }

        std::shared_ptr<VoxelAsset> asset = std::make_shared<VoxelAsset>();

        VoxelChunk& chunk = asset->GetChunk();
        chunk.Clear();

        if (scene->num_models == 0)
        {
            ogt_vox_destroy_scene(scene);
            return asset;
        }

        const ogt_vox_model* model = scene->models[0];

        ENGINE_INFO("Loaded .vox model: {}x{}x{}", model->size_x, model->size_y, model->size_z);

        for (uint32_t z = 0; z < model->size_z; z++)
        {
            for (uint32_t y = 0; y < model->size_y; y++)
            {
                for (uint32_t x = 0; x < model->size_x; x++)
                {
                    uint32_t index = x + y * model->size_x + z * model->size_x * model->size_y;

                    uint8_t colorIndex = model->voxel_data[index];

                    if (colorIndex == 0)
                        continue;

                    const ogt_vox_rgba& paletteColor = scene->palette.color[colorIndex];

                    glm::u8vec4 color = {
                        paletteColor.r,
                        paletteColor.g,
                        paletteColor.b,
                        paletteColor.a
                    };

                    chunk.SetVoxel(static_cast<int>(x), static_cast<int>(y), static_cast<int>(z), colorIndex, color);
                }
            }
        }

        ogt_vox_destroy_scene(scene);

        return asset;
    }
}