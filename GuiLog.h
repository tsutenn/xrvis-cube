#pragma once
#include <QtWidgets/QMainWindow>

class GuiLog : public QObject
{
	Q_OBJECT

signals:
	void Log(const QString& message);
};

