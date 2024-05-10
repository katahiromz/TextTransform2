// win32_emoji_draw.cpp --- Win32 emoji drawing sample
// License: MIT
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <cassert>
#include <cstdio>

#define WIDTH 640
#define HEIGHT 400
#define CLASSNAME L"TextTransform2"

HINSTANCE g_hInst = nullptr;
HWND g_hMainWnd = nullptr;

/////////////////////////////////////////////////////////////////////////////////////////

void OnDrawBitmap(HWND hwnd, HDC hDC, RECT& rc)
{
    FillRect(hDC, &rc, GetStockBrush(WHITE_BRUSH));

    WCHAR szText[] = L"TextTransform2";
    INT cchText = lstrlenW(szText);

    LOGFONTW lf;
    ZeroMemory(&lf, sizeof(lf));

    SetTextColor(hDC, RGB(255, 0, 0));
    SetBkColor(hDC, RGB(255, 255, 0));
    SetBkMode(hDC, OPAQUE);

    if (IsDlgButtonChecked(hwnd, chx1))
        SetGraphicsMode(hDC, GM_ADVANCED);
    else
        SetGraphicsMode(hDC, GM_COMPATIBLE);

    INT iItem = SendDlgItemMessage(hwnd, cmb1, CB_GETCURSEL, 0, 0);
    switch (iItem)
    {
    case 0:
        SetTextAlign(hDC, TA_BASELINE);
        break;
    case 1:
        SetTextAlign(hDC, TA_TOP);
        break;
    case 2:
        SetTextAlign(hDC, TA_BOTTOM);
        break;
    }

    lf.lfHeight = 50;
    lf.lfCharSet = DEFAULT_CHARSET;
    lstrcpyW(lf.lfFaceName, L"MS UI Gothic");

    lf.lfEscapement = GetDlgItemInt(hwnd, edt1, nullptr, TRUE);
    lf.lfOrientation = GetDlgItemInt(hwnd, edt2, nullptr, TRUE);

#define X 150
#define Y 170
    MoveToEx(hDC, 0, Y, nullptr);
    LineTo(hDC, rc.right, Y);
    MoveToEx(hDC, X, 0, nullptr);
    LineTo(hDC, X, rc.bottom);

    HFONT hFont = CreateFontIndirectW(&lf);

    HGDIOBJ hFontOld = SelectObject(hDC, hFont);
    TextOutW(hDC, X, Y, szText, cchText);
    SelectObject(hDC, hFontOld);

    DeleteObject(hFont);
}

void OnDraw(HWND hwnd, HDC hDC)
{
    RECT rc;
    GetClientRect(hwnd, &rc);

    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hbm = CreateCompatibleBitmap(hDC, rc.right, rc.bottom);
    HGDIOBJ hbmOld = SelectObject(hMemDC, hbm);
    OnDrawBitmap(hwnd, hMemDC, rc);
    BitBlt(hDC, 0, 0, rc.right, rc.bottom, hMemDC, 0, 0, SRCCOPY);
    SelectObject(hMemDC, hbmOld);
}

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    g_hMainWnd = hwnd;
    SendDlgItemMessage(hwnd, scr1, UDM_SETRANGE, 0, MAKELONG(3600, -3600));
    SendDlgItemMessage(hwnd, scr2, UDM_SETRANGE, 0, MAKELONG(3600, -3600));
    CheckDlgButton(hwnd, chx3, BST_CHECKED);
    SendDlgItemMessage(hwnd, cmb1, CB_ADDSTRING, 0, (LPARAM)TEXT("TA_BASELINE"));
    SendDlgItemMessage(hwnd, cmb1, CB_ADDSTRING, 0, (LPARAM)TEXT("TA_TOP"));
    SendDlgItemMessage(hwnd, cmb1, CB_ADDSTRING, 0, (LPARAM)TEXT("TA_BOTTOM"));
    SendDlgItemMessage(hwnd, cmb1, CB_SETCURSEL, 0, 0);
    return TRUE;
}

void OnDestroy(HWND hwnd)
{
    g_hMainWnd = nullptr;
    PostQuitMessage(0);
}

void OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(hwnd, &ps);
    OnDraw(hwnd, hDC);
    EndPaint(hwnd, &ps);
}

void OnSize(HWND hwnd, UINT state, int cx, int cy)
{
}

BOOL OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    return TRUE;
}

static INT s_i = 0;

void OnTimer(HWND hwnd, UINT id)
{
    switch (id)
    {
    case 888:
        if (IsDlgButtonChecked(hwnd, chx3))
        {
            INT value = (s_i * 30) % 3600;
            SendDlgItemMessage(hwnd, scr1, UDM_SETPOS, 0, value);
            SendDlgItemMessage(hwnd, scr2, UDM_SETPOS, 0, value);
        }
        else
        {
            INT value = (s_i * 30) % 3600;
            SendDlgItemMessage(hwnd, scr1, UDM_SETPOS, 0, value);
        }
        InvalidateRect(hwnd, nullptr, TRUE);
        s_i %= 3600 / 30;
        ++s_i;
        break;
    case 999:
        if (IsDlgButtonChecked(hwnd, chx3))
        {
            INT value = (s_i * 30) % 3600;
            SendDlgItemMessage(hwnd, scr1, UDM_SETPOS, 0, value);
            SendDlgItemMessage(hwnd, scr2, UDM_SETPOS, 0, value);
        }
        else
        {
            INT value = (s_i * 30) % 3600;
            SendDlgItemMessage(hwnd, scr2, UDM_SETPOS, 0, value);
        }
        InvalidateRect(hwnd, nullptr, TRUE);
        s_i %= 3600 / 30;
        ++s_i;
        break;
    }
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    static INT s_nUpdating = 0;
    switch (id)
    {
    case IDOK:
    case IDCANCEL:
        EndDialog(hwnd, id);
        break;
    case edt1:
    case edt2:
        if (codeNotify == EN_CHANGE)
        {
            if (IsDlgButtonChecked(hwnd, chx3))
            {
                INT value = GetDlgItemInt(hwnd, id, nullptr, TRUE);
                if (id == edt1)
                    SendDlgItemMessage(hwnd, scr2, UDM_SETPOS, 0, value);
                else
                    SendDlgItemMessage(hwnd, scr1, UDM_SETPOS, 0, value);
            }
            InvalidateRect(hwnd, nullptr, TRUE);
        }
        break;
    case chx1:
        if (codeNotify == BN_CLICKED)
            InvalidateRect(hwnd, nullptr, TRUE);
        break;
    case chx3:
        if (codeNotify == BN_CLICKED)
        {
            INT value = GetDlgItemInt(hwnd, edt1, nullptr, TRUE);
            SendDlgItemMessage(hwnd, scr2, UDM_SETPOS, 0, value);
        }
        break;
    case chx4:
        if (codeNotify == BN_CLICKED)
        {
            if (IsDlgButtonChecked(hwnd, chx4))
            {
                EnableWindow(GetDlgItem(hwnd, chx5), FALSE);
                SetTimer(hwnd, 888, 100, NULL);
                s_i = 0;
            }
            else
            {
                EnableWindow(GetDlgItem(hwnd, chx5), TRUE);
                KillTimer(hwnd, 888);
            }
        }
        break;
    case chx5:
        if (codeNotify == BN_CLICKED)
        {
            if (IsDlgButtonChecked(hwnd, chx5))
            {
                EnableWindow(GetDlgItem(hwnd, chx4), FALSE);
                SetTimer(hwnd, 999, 100, NULL);
                s_i = 0;
            }
            else
            {
                EnableWindow(GetDlgItem(hwnd, chx4), TRUE);
                KillTimer(hwnd, 999);
            }
        }
        break;
    case cmb1:
        if (codeNotify == CBN_SELENDOK)
            InvalidateRect(hwnd, nullptr, TRUE);
    }
}

INT_PTR CALLBACK
DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
        HANDLE_MSG(hwnd, WM_ERASEBKGND, OnEraseBkgnd);
        HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
        HANDLE_MSG(hwnd, WM_TIMER, OnTimer);
        HANDLE_MSG(hwnd, WM_SIZE, OnSize);
    }
    return 0;
}

INT WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
    g_hInst = hInstance;

    InitCommonControls();

    DialogBox(hInstance, MAKEINTRESOURCE(1), nullptr, DialogProc);
    return 0;
}
