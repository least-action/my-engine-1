#include <d3d11.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <wrl/client.h>
#include <iostream>
#include <vector>
#include <directxmath.h>
#include <D3DCompiler.h>
#include <winerror.h>

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
        HDC hdc = BeginPaint(mHwnd, &ps);

        // All painting occurs here, between BeginPaint and EndPaint.

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(mHwnd, &ps);
    }
    return 0;

    }
    return DefWindowProc(mHwnd, uMsg, wParam, lParam);
}
