/*
 * Copyright 2025, Johan Wagenheim <johan@dospuntos.no>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include <Application.h>
#include <Mime.h>


void UnregisterHEICMimeType() {
	BMimeType mime("image/heic");
	mime.Delete();

	BMimeType mimeHeif("image/heif");
	mimeHeif.Delete();
}


int main() {
	BApplication app("application/x-vnd.UnregisterHEICMime");
	UnregisterHEICMimeType();
	return 0;
}
