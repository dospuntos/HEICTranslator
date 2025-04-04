/*
 * Copyright 2024, My Name <my@email.address>
 * All rights reserved. Distributed under the terms of the MIT license.
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
