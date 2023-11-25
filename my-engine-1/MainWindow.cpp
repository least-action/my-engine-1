#include "MainWindow.h"


LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);

        // All painting occurs here, between BeginPaint and EndPaint.

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(m_hwnd, &ps);
    }
    return 0;

    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

int MainWindow::WinMain()
{
    if (!Create(L"my-engine-1", WS_OVERLAPPEDWINDOW))
    {
        return 0;
    }

    ShowWindow(Window(), SW_SHOWDEFAULT);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
    return 0;
}
