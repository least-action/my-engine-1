#pragma once
#include "BaseWindow.h"


class MainWindow : public BaseWindow<MainWindow>
{
public:
	PCWSTR  ClassName() const { return L"Main Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	MainWindow();
	int Run();

private:
	int nCmdShow;
};