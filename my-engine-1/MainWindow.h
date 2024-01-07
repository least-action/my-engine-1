#pragma once
#include "BaseWindow.h"


class MainWindow : public BaseWindow<MainWindow>
{
	bool keyDowns[100] = { false, };
	/*
	* mouseDowns[0] > left
	* mouseDowns[1] > right
	*/
	bool mouseDowns[2] = { false, };

public:
	bool IsLeftDown();
	bool IsRightDown();
	bool IsUpDown();
	bool IsDownDown();

	bool IsRightClickDown();

	PCWSTR  ClassName() const { return L"Main Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};
