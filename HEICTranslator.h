#ifndef HEIC_TRANSLATOR_H
#define HEIC_TRANSLATOR_H

#include <Translator.h>
#include <TranslatorFormats.h>
#include <Bitmap.h>
#include <DataIO.h>
#include <libheif/heif.h>

class HEICTranslator : public BTranslator {
public:
    HEICTranslator();
    virtual ~HEICTranslator();

    virtual status_t DerivedIdentify(BPositionIO* inSource,
                                     const translation_format* inFormat,
                                     BMessage* ioExtension,
                                     translator_info* outInfo,
                                     uint32 outType);

    virtual status_t DerivedTranslate(BPositionIO* inSource,
                                      const translator_info* inInfo,
                                      BMessage* ioExtension,
                                      uint32 outType,
                                      BPositionIO* outDestination);

    virtual status_t GetConfigurationMessage(BMessage* ioExtension);

    // Optional: Implement these if needed
    virtual status_t AcquireSettings(BMessage* ioExtension);
    virtual BView* MakeConfigurationView(BMessage* ioExtension);

private:
    BBitmap* LoadHeic(BPositionIO* inSource);
};

#endif // HEIC_TRANSLATOR_H