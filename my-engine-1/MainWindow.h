#pragma once
#include "BaseWindow.h"


class MainWindow : public BaseWindow<MainWindow>
{
	bool keyDowns[100] = { false, };
public:
	bool IsLeftDown();
	bool IsRightDown();
	bool IsUpDown();
	bool IsDownDown();

	PCWSTR  ClassName() const { return L"Main Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};
