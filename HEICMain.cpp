/*
 * HEICMain.cpp – HEIC image translator for Haiku
 * Version 0.1.0 (March 31st, 2025)
 *
 * Copyright (c) 2025 Johan Wagenheim <johan@dospuntos.no>
 *
 * Based in part on code shared by Zenja at:
 * https://dev.haiku-os.org/ticket/14909#comment:5
 *
 * Distributed under the terms of the MIT License.
 * See the accompanying LICENSE file for details.
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

