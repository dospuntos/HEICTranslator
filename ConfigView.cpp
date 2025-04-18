/*
 * ConfigView.cpp – HEIC image translator for Haiku
 *
 * Copyright (c) 2025 Johan Wagenheim <johan@dospuntos.no>
 *
 * Based in part on code shared by Zenja at:
 * https://dev.haiku-os.org/ticket/14909#comment:5
 *
 * Distributed under the terms of the MIT License.
 */


#include "ConfigView.h"
#include "HEICTranslator.h"
#include <Catalog.h>
#include <StringView.h>
#include <LayoutBuilder.h>
#include <libheif/heif.h>
#include <stdio.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "ConfigView"


ConfigView::ConfigView(uint32 flags)
	: BView("HEICTranslator Settings", flags)
{
	SetViewUIColor(B_PANEL_BACKGROUND_COLOR);

	BStringView *titleView = new BStringView("title",
		B_TRANSLATE("HEIC image translator"));
	titleView->SetFont(be_bold_font);

	char version[256];
	sprintf(version, B_TRANSLATE("Version %d.%d.%d, %s"),
		int(B_TRANSLATION_MAJOR_VERSION(HEIC_TRANSLATOR_VERSION)),
		int(B_TRANSLATION_MINOR_VERSION(HEIC_TRANSLATOR_VERSION)),
		int(B_TRANSLATION_REVISION_VERSION(HEIC_TRANSLATOR_VERSION)),
		__DATE__);
	BStringView *versionView = new BStringView("version", version);

	BStringView *copyrightView = new BStringView("copyright",
		B_UTF8_COPYRIGHT "2025 Johan Wagenheim");

	BString openExrInfo = B_TRANSLATE("Based on libheif %version%");
	openExrInfo.ReplaceAll("%version%", heif_get_version());
	BStringView *copyrightView2 = new BStringView("copyright2",
		openExrInfo.String());

	BStringView *copyrightView3 = new BStringView("copyright3",
		"Based in part on code shared by Zenja at:");

	BStringView *copyrightView4 = new BStringView("copyright4",
		B_TRANSLATE("https://dev.haiku-os.org/ticket/14909#comment:5"));

	// Build the layout
	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetInsets(B_USE_DEFAULT_SPACING)
		.Add(titleView)
		.Add(versionView)
		.Add(copyrightView)
		.AddGlue()
		.Add(copyrightView2)
		.Add(copyrightView3)
		.Add(copyrightView4);
}


ConfigView::~ConfigView()
{
}

