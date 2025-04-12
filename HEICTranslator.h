/*
 * HEICTranslator.h – HEIC image translator for Haiku
 *
 * Copyright (c) 2025 Johan Wagenheim <johan@dospuntos.no>
 *
 * Based in part on code shared by Zenja at:
 * https://dev.haiku-os.org/ticket/14909#comment:5
 *
 * Distributed under the terms of the MIT License.
 */

#ifndef HEICTRANSLATOR_H
#define HEICTRANSLATOR_H

#include "shared/BaseTranslator.h"
#include "shared/TranslatorSettings.h"
#include <DataIO.h>
#include <Message.h>
#include <SupportDefs.h>
#include <TranslationDefs.h>

#define HEIC_TRANSLATOR_VERSION B_TRANSLATION_MAKE_VERSION(0,2,0)
#define HEIC_IMAGE_FORMAT	'HEIC'

class HEICTranslator : public BaseTranslator {
public:
				HEICTranslator();
				virtual ~HEICTranslator();

				virtual status_t DerivedIdentify(BPositionIO *inSource,
					const translation_format *inFormat, BMessage *ioExtension,
					translator_info *outInfo, uint32 outType);

				virtual status_t DerivedTranslate(BPositionIO *inSource,
					const translator_info *inInfo, BMessage *ioExtension,
					uint32 outType, BPositionIO *outDestination, int32 baseType);

				virtual BView *NewConfigView(TranslatorSettings *settings);

private:

};

#endif // HEICTRANSLATOR_H
