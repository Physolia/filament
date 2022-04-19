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
    using FlagBits = uint64_t;

    enum class Flags {
        sRGB = 1 << 0,
    };

    /**
     * Creates a Filament texture and pushes it to the asychronous decoding queue.
     *
     * If construction fails, nothing is pushed to the queue and null is returned.
     * The failure reason can be obtained with getFailureMessage().
     * The given buffer pointer is not held, so the caller can free the data immediately.
     * It is also the caller's responsibility to free the returned Texture object, but it is only
     * safe to do so after it has been popped from the queue.
     */
    virtual Texture* pushTexture(const uint8_t* data, size_t byteCount,
            const char* mimeType, FlagBits flags) = 0;

    /**
     * Checks if any texture is ready to be removed from the asynchronous decoding queue, and if so
     * pops it off.
     *
     * Due to concurrency, textures are not necessarily popped off in the same order they were
     * pushed.
     *
     * Unless an error occurred during the decoding process, the returned texture should have all
     * its miplevels populated.
     *
     * Returns null if there no textures that are ready to be popped.
     */
    virtual Texture* popTexture() = 0;

    /**
     * Updates pending textures on the Filament thread.
     *
     * This is when the provider calls Texture::setImage() and if needed it also calls
     * Texture::generateMipmaps().
     *
     * The public-facing push/pop queue can only be modified during this call.
     */
    virtual void updateQueue() = 0;

    /**
     * Returns a failure message for the most recent call to pushTexture().
     *
     * Note that this method does not pertain to the decoding process. If decoding fails, the
     * lower-level implementation should emit an error and allow clients to pop the incomplete
     * texture off the queue.
     */
    virtual const char* getFailureMessage() const = 0;

    virtual void waitForCompletion() = 0;

    virtual size_t getRemainingCount() const = 0;

    virtual size_t getTotalCount() const = 0;

    virtual ~TextureProvider() = default;
};

} // namespace gltfio

#endif // GLTFIO_TEXTUREPROVIDER_H
