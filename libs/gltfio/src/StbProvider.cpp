/*
 * Copyright (C) 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "TextureProvider.h"

#include <string>
#include <vector>

#include <utils/JobSystem.h>

#include <filament/Engine.h>
#include <filament/Texture.h>

#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM

#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

using namespace filament;
using namespace utils;

using std::string;

namespace gltfio {

static const auto FREE_CALLBACK = [](void* mem, size_t, void*) { free(mem); };

class StbProvider : public TextureProvider {
public:
    StbProvider(Engine* engine) : mEngine(engine) {}

    Texture* pushTexture(const uint8_t* data, size_t byteCount,
            const char* mimeType, uint64_t flags) final;

    Texture* popTexture() final;

    const char* getFailureMessage() const final { return mErrorMessage.c_str(); }

    size_t getRemainingCount() const { return mDecodingCount; }

    size_t getTotalCount() const { return mTextures.size(); }

    void updateQueue() final;

    void waitForCompletion() final;

private:
    enum class TextureState {
        DECODING, // Texture has been pushed, mipmap levels are not yet complete.
        READY,    // Mipmap levels are available but texture has not been popped yet.
        POPPED,   // Client has popped the texture from the queue.
    };

    struct TextureInfo {
        Texture* texture;
        TextureState state;
        std::atomic<uint8_t*> decodedTexelsBaseMipmap;
    };

    void decodeSingleTexture();

    // Number of textures that are in the DECODING state.
    size_t mDecodingCount = 0;

    // size_t mDecoderTasksCount;
    // size_t mDecoderTasksFinishedCount;

    std::vector<TextureInfo> mTextures;

    JobSystem::Job* mDecoderRootJob = nullptr;
    std::string mErrorMessage;
    Engine* const mEngine;
};

Texture* StbProvider::pushTexture(const uint8_t* data, size_t byteCount,
            const char* mimeType, FlagBits flags) {
    int width, height, numComponents;
    if (!stbi_info_from_memory(data, byteCount, &width, &height, &numComponents)) {
        mErrorMessage = string("Unable to parse texture: ") + stbi_failure_reason();
        return nullptr;
    }

    using InternalFormat = Texture::InternalFormat;
    const FlagBits sRGB = FlagBits(Flags::sRGB);

    Texture* texture = Texture::Builder()
            .width(width)
            .height(height)
            .levels(0xff)
            .format((flags & sRGB) ? InternalFormat::SRGB8_A8 : InternalFormat::RGBA8)
            .build(*mEngine);

    if (texture == nullptr) {
        mErrorMessage = "Unable to build Texture object.";
        return nullptr;
    }

    #error TODO: create jobs and push to queue

    return texture;
}

Texture* StbProvider::popTexture() {
    for (TextureInfo& texture : mTextures) {
        if (texture.state == TextureState::READY) {
            texture.state = TextureState::POPPED;
            return texture.texture;
        }
    }
    return nullptr;
}

void StbProvider::updateQueue() {
    if (!UTILS_HAS_THREADING) {
        decodeSingleTexture();
    }
    for (TextureInfo& info : mTextures) {
        if (info.state != TextureState::DECODING) {
            continue;
        }
        Texture* texture = info.texture;
        if (uint8_t* data = info.decodedTexelsBaseMipmap.load()) {
            Texture::PixelBufferDescriptor pbd(data,
                    texture->getWidth() * texture->getHeight() * 4,
                    Texture::Format::RGBA, Texture::Type::UBYTE, FREE_CALLBACK);
            texture->setImage(*mEngine, 0, std::move(pbd));
            texture->generateMipmaps(*mEngine);
            info.state = TextureState::READY;
        }
    }
}

void StbProvider::waitForCompletion() {
    //
}

TextureProvider* createStbProvider(Engine* engine) {
    return new StbProvider(engine);
}

} // namespace gltfio
