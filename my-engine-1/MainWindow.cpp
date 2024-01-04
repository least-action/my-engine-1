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


bool MainWindow::IsLeftDown() { return keyDowns[65]; }

bool MainWindow::IsRightDown() { return keyDowns[68]; }

bool MainWindow::IsUpDown() { return keyDowns[87]; }

bool MainWindow::IsDownDown() { return keyDowns[83]; }

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
    case WM_KEYDOWN:
        switch (wParam) {
        case 27:
            PostQuitMessage(0);
            break;
        default:
            keyDowns[wParam] = true;
            break;
        }
        break;

    case WM_KEYUP:
        keyDowns[wParam] = false;
        break;
        
    return 0;

    }
    return DefWindowProc(mHwnd, uMsg, wParam, lParam);
}
