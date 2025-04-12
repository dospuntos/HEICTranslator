/*
 * HEICTranslator.cpp – HEIC image translator for Haiku
 *
 * Copyright (c) 2025 Johan Wagenheim <johan@dospuntos.no>
 *
 * Based in part on code shared by Zenja at:
 * https://dev.haiku-os.org/ticket/14909#comment:5
 *
 * Distributed under the terms of the MIT License.
 */


// Includes
#include <Bitmap.h>
#include <Catalog.h>
#include <DataIO.h>
#include <File.h>
#include <FindDirectory.h>
#include <Path.h>
#include <Screen.h>
#include <SupportKit.h>
#include <TranslatorAddOn.h>
#include <TranslatorFormats.h>
#include <libheif/heif.h>
#include <string.h>
#include "HEICTranslator.h"
#include "ConfigView.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "HEICTranslator"


// The input formats that this translator supports.
static const translation_format sInputFormats[] = {
	{
		HEIC_IMAGE_FORMAT,
		B_TRANSLATOR_BITMAP,
		1,
		1,
		"image/heic",
		"HEIC"
	}
};

// The output formats that this translator supports.
static const translation_format sOutputFormats[] = {
	{
		B_TRANSLATOR_BITMAP,
		B_TRANSLATOR_BITMAP,
		1,
		1,
		"image/x-be-bitmap",
		"Be Bitmap Format (HEICTranslator)"
	},
};

// Default settings for the Translator
static const TranSetting sDefaultSettings[] = {
	{B_TRANSLATOR_EXT_HEADER_ONLY, TRAN_SETTING_BOOL, false},
	{B_TRANSLATOR_EXT_DATA_ONLY, TRAN_SETTING_BOOL, false}
};

const uint32 kNumInputFormats = sizeof(sInputFormats) / sizeof(translation_format);
const uint32 kNumOutputFormats = sizeof(sOutputFormats) / sizeof(translation_format);
const uint32 kNumDefaultSettings = sizeof(sDefaultSettings) / sizeof(TranSetting);

HEICTranslator::HEICTranslator()
		: BaseTranslator(B_TRANSLATE("HEIC images"),
				B_TRANSLATE("HEIC image translator"),
				HEIC_TRANSLATOR_VERSION,
				sInputFormats, kNumInputFormats,
				sOutputFormats, kNumOutputFormats,
				"HEICTranslator_Settings",
				sDefaultSettings, kNumDefaultSettings,
				B_TRANSLATOR_BITMAP, HEIC_IMAGE_FORMAT)
{
}


HEICTranslator::~HEICTranslator()
{
}


status_t
HEICTranslator::DerivedIdentify(
	BPositionIO *inSource,
	const translation_format *inFormat,
	BMessage * /*ioExtension*/,
	translator_info *outInfo,
	uint32 outType)
{

	if (outType && (outType != B_TRANSLATOR_BITMAP))
		return B_NO_TRANSLATOR;

	// Check for HEIC magic bytes
	char magic[12];
	ssize_t readSize = inSource->Read(magic, 12);
	if (readSize < 12)
		return B_NO_TRANSLATOR;

	// HEIC files start with ftypheic or ftyphevc
	if (memcmp(magic + 4, "ftypheic", 8) == 0 || memcmp(magic + 4, "ftyphevc", 8) == 0) {
		inFormat = &sInputFormats[0];
		outInfo->type = inFormat->type;
		outInfo->group = inFormat->group;
		outInfo->quality = inFormat->quality;
		outInfo->capability = inFormat->capability;
		strcpy(outInfo->name, inFormat->name);
		strcpy(outInfo->MIME, inFormat->MIME);

		return B_OK;
	}

	return B_NO_TRANSLATOR;
}


status_t
HEICTranslator::DerivedTranslate (
	BPositionIO *source,
	const translator_info *info,
	BMessage * /*ioExtension*/,
	uint32 outType,
	BPositionIO *target, int32 baseType)
{
	status_t ret_val = B_OK;

	//	Check that we handle input and output types
	if (info->type != HEIC_IMAGE_FORMAT)
		return B_NO_TRANSLATOR;

	if (outType == 0)
		outType = B_TRANSLATOR_BITMAP;

	if (outType != B_TRANSLATOR_BITMAP)
		return B_NO_TRANSLATOR;

	// Read input into memory
	size_t fileSize = source->Seek(0, SEEK_END);
	source->Seek(0, SEEK_SET);
	uint8_t *buffer = new uint8_t[fileSize];
	if (source->Read(buffer, fileSize) != fileSize)
	{
		delete[] buffer;
		return B_ERROR;
	}

	// Load HEIC image from memory
	heif_context* ctx = heif_context_alloc();
	heif_context_read_from_memory_without_copy(ctx, buffer, fileSize, nullptr);

	heif_image_handle* handle;
	heif_context_get_primary_image_handle(ctx, &handle);

	heif_image* img;
	heif_decode_image(handle, &img, heif_colorspace_RGB, heif_chroma_interleaved_RGBA, nullptr);

	int width = heif_image_get_primary_width(img);
	int height = heif_image_get_primary_height(img);
	int stride;
	const uint8_t* data = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);

	// Create Haiku bitmap
	BBitmap *bitmap = new BBitmap(BRect(0, 0, width-1, height-1), B_RGBA32);
	if (!bitmap->IsValid())
	{
		heif_image_release(img);
		heif_image_handle_release(handle);
		heif_context_free(ctx);
		delete[] buffer;
		return B_ERROR;
	}

	uint8 *dest = (uint8_t *)bitmap->Bits();

	// Convert BGRA to RGBA
	const uint8 *src = data;
	for (int y=0; y < height; y++)
	{
		for (int x=0; x<width; x++)
		{
			uint8 r=*src++;
			uint8 g=*src++;
			uint8 b=*src++;
			uint8 a=*src++;
			*dest++ = b;
			*dest++ = g;
			*dest++ = r;
			*dest++ = a;
		}
		src += stride - (width *4);
	}

	heif_image_release(img);
	heif_image_handle_release(handle);
	heif_context_free(ctx);
	delete[] buffer;

	// Prepare TranslatorBitmap header
	TranslatorBitmap bmp;
	bmp.magic = B_TRANSLATOR_BITMAP;
	bmp.bounds = bitmap->Bounds();
	bmp.rowBytes = bitmap->BytesPerRow();
	bmp.colors = B_RGBA32;
	bmp.dataSize = bitmap->BitsLength();

	// Convert header to correct endianness
	swap_data(B_UINT32_TYPE, &(bmp.magic), sizeof(uint32), B_SWAP_BENDIAN_TO_HOST);
	swap_data(B_RECT_TYPE, &(bmp.bounds), sizeof(BRect), B_SWAP_BENDIAN_TO_HOST);
	swap_data(B_UINT32_TYPE, &(bmp.rowBytes), sizeof(uint32), B_SWAP_BENDIAN_TO_HOST);
	swap_data(B_UINT32_TYPE, &(bmp.colors), sizeof(color_space), B_SWAP_BENDIAN_TO_HOST);
	swap_data(B_UINT32_TYPE, &(bmp.dataSize), sizeof(uint32), B_SWAP_BENDIAN_TO_HOST);

	// Write bitmap header & pixel data
	if (target->Write(&bmp, sizeof(TranslatorBitmap)) != sizeof(TranslatorBitmap) || target->Write(bitmap->Bits(), bitmap->BitsLength()) != bitmap->BitsLength()) {
		ret_val = B_ERROR;
	}

	delete bitmap;
	return ret_val;
}


BView *
HEICTranslator::NewConfigView(TranslatorSettings *settings)
{
	return new ConfigView();
}


BTranslator *
make_nth_translator(int32 n, image_id you, uint32 flags, ...)
{
	if (n != 0)
		return NULL;

	return new HEICTranslator();
}