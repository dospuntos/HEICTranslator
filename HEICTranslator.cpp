/*
 *  HEICTranslator.cpp 2.0.1 (March 31st, 2025)
 *
 *  Translator add-on for HEIC images
 *  Written by Johan Wagenheim <johan@dospuntos.no>
 *  Based on source code from https://dev.haiku-os.org/ticket/14909#comment:5

 *  Public domain.
 */


// Includes
#include <string.h>
#include <TranslatorAddOn.h>
#include <TranslatorFormats.h>
#include <DataIO.h>
#include <Screen.h>
#include <SupportKit.h>
#include <libheif/heif.h>
#include <Alert.h>
#include <Path.h>
#include <File.h>
#include <Bitmap.h>
#include <FindDirectory.h>


// Defines
#define HEIC_FORMAT 'HEIC'


// Translator info
char translatorName[] = "HEICTranslator";
char translatorInfo[] = "By Johan Wagenheim <johan@dospuntos.no>";
int32 translatorVersion = 201;

translation_format inputFormats[] =
{
	{ HEIC_FORMAT, B_TRANSLATOR_BITMAP, 0.9, 0.8, "image/heic", "HEIC Image" },
	{ 0, 0, 0.0, 0.0, "", "" }
};

translation_format outputFormats[] =
{
	{ B_TRANSLATOR_BITMAP, B_TRANSLATOR_BITMAP, 0.3, 0.5, "image/x-be-bitmap", "Be bitmap" },
	{ 0, 0, 0.0, 0.0, "", "" }
};


status_t
Identify(
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
		inFormat = &inputFormats[0];
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


static BBitmap *LoadHeic(const char *path)
{
	FILE *f = fopen(path, "rb");
	if (!f)
	{
		printf("Error opening %s\n", path);
		return nullptr;
	}

	uint8_t buffer[12];
	fread(buffer, 12, 1, f);
	fclose(f);
	printf("MIME = %s\n", heif_get_file_mime_type(buffer, 12));


	heif_context* ctx = heif_context_alloc();
	heif_context_read_from_file(ctx, path, nullptr);

	heif_image_handle* handle;
	heif_context_get_primary_image_handle(ctx, &handle);

	heif_image* img;
	heif_decode_image(handle, &img, heif_colorspace_RGB, heif_chroma_interleaved_RGBA, nullptr);

	int stride;
	const uint8_t* data = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);
	int width = heif_image_get_primary_width(img);
	int height = heif_image_get_primary_height(img);

	BBitmap *bitmap = new BBitmap(BRect(0, 0, width-1, height-1), B_RGBA32);
	bitmap->Lock();
	uint8 *dest = (uint8 *)bitmap->Bits();

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
		dest += stride-(width*4);
	}

	bitmap->Unlock();

	heif_image_release(img);
	heif_image_handle_release(handle);
	heif_context_free(ctx);

	return bitmap;
}


status_t
Translate(
	BPositionIO *inSource,
	const translator_info *inInfo,
	BMessage * /*ioExtension*/,
	uint32 outFormat,
	BPositionIO *outDestination)
{
	status_t ret_val = B_OK;

	//	Check that we handle input and output types
	if (inInfo->type != HEIC_FORMAT)
		return B_NO_TRANSLATOR;

	if (outFormat == 0)
		outFormat = B_TRANSLATOR_BITMAP;

	if (outFormat != B_TRANSLATOR_BITMAP)
		return B_NO_TRANSLATOR;

	// Create a temporary file to store the input
	BPath tempPath;
	if (find_directory(B_SYSTEM_TEMP_DIRECTORY, &tempPath) != B_OK)
		return B_ERROR;
	tempPath.Append("temp.heic");

	BFile tempFile(tempPath.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	status_t status = tempFile.InitCheck();
	if (status != B_OK) {
		printf("Error with create file (%s) - %s\n", tempPath.Path(), strerror(status));
		return B_ERROR;
	}

	size_t fileSize = inSource->Seek(0, SEEK_END);
	inSource->Seek(0, SEEK_SET);
	char *buffer = new char[fileSize];
	if (inSource->Read(buffer, fileSize) != fileSize)
	{
		delete[] buffer;
		return B_ERROR;
	}

	tempFile.Write(buffer, fileSize);
		tempFile.Flush();
		delete[] buffer;

	tempFile.Unset();

	// Load HEIC image
	BBitmap *bitmap = LoadHeic(tempPath.Path());
	if (!bitmap)
		return B_ERROR;

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
	if (outDestination->Write(&bmp, sizeof(TranslatorBitmap)) != sizeof(TranslatorBitmap) || outDestination->Write(bitmap->Bits(), bitmap->BitsLength()) != bitmap->BitsLength()) {
		ret_val = B_ERROR;
	}

	delete bitmap;
	return ret_val;
}
