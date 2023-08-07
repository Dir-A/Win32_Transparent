#include <Windows.h>
#include <gdiplus.h>
#include <dwmapi.h>
#include <versionhelpers.h>

#pragma comment(lib, "dwmapi")
#pragma comment(lib, "gdiplus.lib")


// https://github.com/glfw/glfw/blob/master/src/win32_window.c#L368C27-L368C27
bool EnableAlphaCompositing(HWND hWnd)
{
	if (!::IsWindowsVistaOrGreater()) { return false; }

	BOOL is_composition_enable = false;
	::DwmIsCompositionEnabled(&is_composition_enable);
	if (!is_composition_enable) { return true; }

	DWORD current_color = 0;
	BOOL is_opaque = false;
	::DwmGetColorizationColor(&current_color, &is_opaque);

	if (!is_opaque || IsWindows8OrGreater())
	{
		HRGN region = ::CreateRectRgn(0, 0, -1, -1);
		DWM_BLURBEHIND bb = { 0 };
		bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
		bb.hRgnBlur = region;
		bb.fEnable = TRUE;
		::DwmEnableBlurBehindWindow(hWnd, &bb);
		::DeleteObject(region);
		return true;
	}
	else // For Window7
	{
		DWM_BLURBEHIND bb = { 0 };
		bb.dwFlags = DWM_BB_ENABLE;
		::DwmEnableBlurBehindWindow(hWnd, &bb);
		return false;
	}
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
	{
		Gdiplus::Graphics graphics = hWnd;
		graphics.Clear(Gdiplus::Color::MakeARGB(100, 0, 0, 0));
	}
	break;

	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return S_OK;
	}
	break;
	}

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}


int main()
{
	// 创建 GDIPlus 资源
	ULONG_PTR gdiplus_token;
	Gdiplus::GdiplusStartupInput gdiplus_startup_input;
	Gdiplus::GdiplusStartup(&gdiplus_token, &gdiplus_startup_input, nullptr);

	// 定义窗口类
	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WindowProc;
	wcex.hInstance = GetModuleHandleW(NULL);
	wcex.lpszClassName = L"GDI Transparent Class";

	// 注册窗口类
	RegisterClassEx(&wcex);

	// 创建窗口
	// WS_EX_LAYERED 分层窗口
	// WS_EX_NOREDIRECTIONBITMAP
	// WS_EX_TOPMOST 置顶窗口
	// WS_EX_NOACTIVATE 隐藏任务栏图标，组合使用 WS_EX_APPWINDOW  可以使得窗口图标显示在任务栏
	// WS_EX_NOREDIRECTIONBITMAP DirectComposition相关，提高性能
	DWORD style = WS_OVERLAPPEDWINDOW;
	DWORD ex_style_hide = WS_EX_TOPMOST | WS_EX_NOACTIVATE;                // 置顶，隐藏任务栏图标
	DWORD ex_style_mouse_penetration = WS_EX_TRANSPARENT | WS_EX_TOPMOST;  // 置顶，鼠标穿透
	HWND hWnd = CreateWindowExW
	(
		ex_style_hide, wcex.lpszClassName, L"GDI Transparent DWM", style,
		CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, nullptr, nullptr, wcex.hInstance, nullptr
	);

	// 开启 Alpha 混合
	EnableAlphaCompositing(hWnd);

	// 显示窗口
	ShowWindow(hWnd, SW_SHOWNORMAL);

	// 消息循环
	MSG msg;
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	// 清理 GDIPlus 资源
	Gdiplus::GdiplusShutdown(gdiplus_token);

	// 注销窗口类
	UnregisterClassW(wcex.lpszClassName, wcex.hInstance);
}