/*
 * ConfigView.h – HEIC image translator for Haiku
 *
 * Copyright (c) 2025 Johan Wagenheim <johan@dospuntos.no>
 *
 * Based in part on code shared by Zenja at:
 * https://dev.haiku-os.org/ticket/14909#comment:5
 *
 * Distributed under the terms of the MIT License.
 */

#ifndef CONFIGVIEW_H
#define CONFIGVIEW_H


#include <View.h>


class ConfigView : public BView {
public:
			ConfigView(uint32 flags = B_WILL_DRAW);
			virtual ~ConfigView();
};

#endif // CONFIGVIEW_H
