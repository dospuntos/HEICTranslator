#include "HEICTranslator.h"
#include <TranslatorFormats.h>

HEICTranslator::HEICTranslator() {
    // Initialize libheif
    heif_init(nullptr);
}

HEICTranslator::~HEICTranslator() {
    // Clean up libheif
    heif_deinit();
}

status_t HEICTranslator::DerivedIdentify(BPositionIO* inSource,
                                         const translation_format* inFormat,
                                         BMessage* ioExtension,
                                         translator_info* outInfo,
                                         uint32 outType) {
    // Check if the input is an HEIC file
    uint8_t header[12];
    inSource->Read(header, 12);
    inSource->Seek(0, SEEK_SET); // Reset the stream position

    if (heif_check_filetype(header, 12) == heif_filetype_yes_supported) {
        outInfo->type = B_TRANSLATOR_BITMAP;
        outInfo->group = B_TRANSLATOR_BITMAP;
        outInfo->quality = 0.9;
        outInfo->capability = 0.8;
        strcpy(outInfo->name, "HEIC Image");
        strcpy(outInfo->MIME, "image/heic");
        return B_OK;
    }

    return B_NO_TRANSLATOR;
}

status_t HEICTranslator::DerivedTranslate(BPositionIO* inSource,
                                          const translator_info* inInfo,
                                          BMessage* ioExtension,
                                          uint32 outType,
                                          BPositionIO* outDestination) {
    // Load the HEIC image into a BBitmap
    BBitmap* bitmap = LoadHeic(inSource);
    if (!bitmap) {
        return B_ERROR;
    }

    // Write the bitmap data to the output stream
    status_t result = B_OK;

    // Write the bitmap header
    TranslatorBitmap header;
    header.magic = B_TRANSLATOR_BITMAP;
    header.bounds = bitmap->Bounds();
    header.rowBytes = bitmap->BytesPerRow();
    header.colors = (color_space)bitmap->ColorSpace();
    header.dataSize = bitmap->BitsLength();

    ssize_t written = outDestination->Write(&header, sizeof(header));
    if (written != sizeof(header)) {
        result = B_ERROR;
    }

    // Write the bitmap data
    if (result == B_OK) {
        written = outDestination->Write(bitmap->Bits(), bitmap->BitsLength());
        if (written != bitmap->BitsLength()) {
            result = B_ERROR;
        }
    }

    // Clean up
    delete bitmap;
    return result;
}

status_t HEICTranslator::GetConfigurationMessage(BMessage* ioExtension) {
    // No special configuration needed
    return B_OK;
}

status_t HEICTranslator::AcquireSettings(BMessage* ioExtension) {
    // No settings to acquire
    return B_OK;
}

BView* HEICTranslator::MakeConfigurationView(BMessage* ioExtension) {
    // No configuration view
    return nullptr;
}

BBitmap* HEICTranslator::LoadHeic(BPositionIO* inSource) {
    // Read the entire input stream into a buffer
    off_t size = inSource->Seek(0, SEEK_END);
    inSource->Seek(0, SEEK_SET);
    uint8_t* data = new uint8_t[size];
    inSource->Read(data, size);

    // Load the HEIC image using libheif
    heif_context* ctx = heif_context_alloc();
    heif_context_read_from_memory(ctx, data, size, nullptr);

    heif_image_handle* handle;
    heif_context_get_primary_image_handle(ctx, &handle);

    heif_image* img;
    heif_decode_image(handle, &img, heif_colorspace_RGB, heif_chroma_interleaved_RGBA, nullptr);

    int stride;
    const uint8_t* imgData = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);
    int width = heif_image_get_primary_width(img);
    int height = heif_image_get_primary_height(img);

    // Create a BBitmap to hold the decoded image
    BBitmap* bitmap = new BBitmap(BRect(0, 0, width - 1, height - 1), B_RGBA32);
    bitmap->Lock();
    uint8_t* dest = (uint8_t*)bitmap->Bits();

    // Convert BGRA to RGBA
    const uint8_t* src = imgData;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t b = *src++;
            uint8_t g = *src++;
            uint8_t r = *src++;
            uint8_t a = *src++;
            *dest++ = r;
            *dest++ = g;
            *dest++ = b;
            *dest++ = a;
        }
        dest += (bitmap->BytesPerRow() - (width * 4));
    }
    bitmap->Unlock();

    // Clean up resources
    heif_image_release(img);
    heif_image_handle_release(handle);
    heif_context_free(ctx);
    delete[] data;

    return bitmap;
}

// Register the translator
extern "C" BTranslator* make_nth_translator(int32 n, image_id you, uint32 flags, ...) {
    if (n == 0)
        return new HEICTranslator();
    return nullptr;
}