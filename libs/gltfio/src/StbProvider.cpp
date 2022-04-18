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

#include <filament/Engine.h>
#include <filament/Texture.h>

#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM

// For emscripten and Android builds, we never load from the file
// system, so we-opt out of the stdio functionality in stb.
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
#define STBI_NO_STDIO
#endif

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

using namespace filament;

namespace gltfio {

class StbProvider : public TextureProvider {
public:

    StbProvider(Engine* engine) : mEngine(engine) {}

    Texture* createTexture(const char* mimeType, const char* resourceUri) final;

    Texture* createTexture(const char* mimeType, const uint8_t* data, size_t byteCount) final;

    float getProgress() const final;

    void update() final;

    void wait() final;

private:
    Engine* const mEngine;
};

Texture* StbProvider::createTexture(const char* mimeType, const char* resourceUri) {
    //
    return nullptr;
}

Texture* StbProvider::createTexture(const char* mimeType, const uint8_t* data, size_t byteCount) {
    //
    return nullptr;
}

float StbProvider::getProgress() const {
    //
    return 0.0f;
}

void StbProvider::update() {
    //
}

void StbProvider::wait() {
    //
}

TextureProvider* createStbProvider(Engine* engine) {
    return new StbProvider(engine);
}

} // namespace gltfio
