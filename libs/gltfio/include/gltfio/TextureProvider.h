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

#ifndef GLTFIO_TEXTUREPROVIDER_H
#define GLTFIO_TEXTUREPROVIDER_H

#include <stddef.h>
#include <stdint.h>

#include <utils/compiler.h>

namespace filament {
    class Texture;
}

namespace gltfio {

class UTILS_PUBLIC TextureProvider {
public:
    using Texture = filament::Texture;

    virtual Texture* createTexture(const char* mimeType, bool srgb, const char* filePath) = 0;

    virtual Texture* createTexture(const char* mimeType, bool srgb,
            const uint8_t* data, size_t byteCount) = 0;

    virtual const char* getFailureMessage() const = 0;

    virtual float getProgress() const = 0;

    virtual void update() = 0;

    virtual void wait() = 0;

    virtual ~TextureProvider() = default;
};

} // namespace gltfio

#endif // GLTFIO_TEXTUREPROVIDER_H
