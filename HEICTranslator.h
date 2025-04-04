/*
 * Copyright 2024, My Name <my@email.address>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef HEICTRANSLATOR_H
#define HEICTRANSLATOR_H

#include "shared/BaseTranslator.h"
#include "shared/TranslatorSettings.h"
#include <DataIO.h>
#include <Message.h>
#include <SupportDefs.h>
#include <TranslationDefs.h>

#define HEIC_TRANSLATOR_VERSION B_TRANSLATION_MAKE_VERSION(0,1,0)
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
