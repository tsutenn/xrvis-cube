#pragma once

#include<qstring.h>

#include"gui.h"

class MyLog
{
public:
	MyLog(gui* instance) {
		this->instance = instance;
	}

	void Log(const QString& message) {
		instance->Log(message);
	}

protected:
	gui* instance;
};

