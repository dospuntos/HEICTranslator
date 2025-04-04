/*
 * Copyright 2025, Johan Wagenheim <johan@dospuntos.no>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include <Application.h>
#include <Catalog.h>
#include <Alert.h>

#include "shared/TranslatorWindow.h"
#include "HEICTranslator.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "main"

int main()
{
	(new BAlert("Test", "Main", "OK"))->Go();
	BApplication app("application/x-vnd.Haiku-HEICTranslator");
	if (LaunchTranslatorWindow(new HEICTranslator,
		B_TRANSLATE("HEIC Settings")) != B_OK)
		return 1;

	app.Run();
	return 0;
}

