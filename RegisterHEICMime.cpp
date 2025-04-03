/*
 * Copyright 2025, Johan Wagenheim <johan@dospuntos.no>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include <Application.h>
#include <Mime.h>
#include <Message.h>

void RegisterHEICMimeType() {
	BMimeType mime("image/heic");
	mime.SetShortDescription("HEIC image");
	mime.SetLongDescription("High-Efficiency Image File Format (HEIC)");

	BMessage extMsg;
	extMsg.AddString("extensions", "heic");
	extMsg.AddString("extensions", "heif");
	mime.SetFileExtensions(&extMsg);

	mime.SetPreferredApp("application/x-vnd.Haiku-ShowImage");

	mime.Install();
}


int main() {
	BApplication app("application/x-vnd.RegisterHEICMime");
	RegisterHEICMimeType();
	return 0;
}
