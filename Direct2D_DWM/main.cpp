#include <d2d1.h>
#include <dwmapi.h>
#include <versionhelpers.h>
#pragma comment(lib, "dwmapi")
#pragma comment(lib, "d2d1.lib")


static ID2D1Factory* sg_pID2D1Factory = nullptr;
static ID2D1HwndRenderTarget* sg_pRenderTarget = nullptr;


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
		sg_pRenderTarget->BeginDraw();
		sg_pRenderTarget->Clear(D2D1::ColorF(0.2f, 0.3f, 0.5f, 0.5f));
		sg_pRenderTarget->EndDraw();
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
	// 窗口类定义
	WNDCLASSEXW wcx = { 0 };
	wcx.cbSize = sizeof(wcx);
	wcx.lpfnWndProc = WindowProc;
	wcx.lpszClassName = L"Direct2D Transparent Class";
	wcx.hInstance = GetModuleHandleW(nullptr);

	// 注册窗口类
	::RegisterClassExW(&wcx);

	// 创建窗口
	HWND hwnd = ::CreateWindowExW
	(
		NULL, wcx.lpszClassName, L"Direct2D Transparent DWM", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
		nullptr, nullptr, wcx.hInstance, nullptr
	);

	// 开启 Alpha 混合
	EnableAlphaCompositing(hwnd);

	// 创建Direct2D资源
	{
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &sg_pID2D1Factory);

		RECT rc = { 0 };
		GetClientRect(hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties
		(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
		);
		sg_pID2D1Factory->CreateHwndRenderTarget(props, D2D1::HwndRenderTargetProperties(hwnd, size), &sg_pRenderTarget);
	}

	// 显示窗口
	ShowWindow(hwnd, SW_NORMAL);

	// 消息循环
	MSG msg = { 0 };
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
	}

	// 清理Direct2D资源
	sg_pRenderTarget->Release();
	sg_pID2D1Factory->Release();

	// 注销窗口类
	UnregisterClassW(wcx.lpszClassName, wcx.hInstance);
}