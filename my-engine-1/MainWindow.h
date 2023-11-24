#include <windows.h>


class MainWindow
{
public:
    MainWindow();
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    BOOL Create();

private:
    HINSTANCE hInstance;
    int nCmdShow;
};
