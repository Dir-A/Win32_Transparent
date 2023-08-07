#pragma once
#include <windows.h>  
#include <gdiplus.h>  

#pragma comment(lib, "gdiplus.lib")

BOOL UpdateLayer(Gdiplus::Bitmap& pBitMap, HWND hWnd, UINT uiX, UINT uiY)
{
	HBITMAP h_bitmap = nullptr;                                       // 获取贴图句柄
	Gdiplus::Color bk_color = (Gdiplus::Color::MakeARGB(0, 0, 0, 0)); // 背景颜色
	pBitMap.GetHBITMAP(bk_color, &h_bitmap);

	BITMAP bmp_info = { 0 };                                          // 获取贴图大小
	GetObjectW(h_bitmap, sizeof(bmp_info), &bmp_info);			      
																      
	SIZE bmp_size = { bmp_info.bmWidth, bmp_info.bmHeight };          // 设置贴图大小
																      
	HDC screen_dc = GetDC(NULL);                                      // 获得桌面DC
	HDC mem_dc = CreateCompatibleDC(screen_dc);                       // 创建内存DC
	HBITMAP old_bitmap = (HBITMAP)SelectObject(mem_dc, h_bitmap);     // 选入新内存DC
																      
	COLORREF color = RGB(100, 100, 100);                              // 色键值，用于过滤指定颜色
																      
	BLENDFUNCTION blend = { 0 };                                      // 混合结构体，用于设置混合参数
	blend.BlendOp = 0;											      
	blend.BlendOp = AC_SRC_OVER;								      
	blend.SourceConstantAlpha = 255;							      
	blend.AlphaFormat = AC_SRC_ALPHA;							      
																      
	POINT mem_pos = { 0, 0 };                                          // 设置贴图在内存DC的位置，与 bmp_size 值之和不能大于图形分辨率，否则不显示
	POINT wnd_pos = { (LONG)uiX, (LONG)uiY };                          // 设置窗口在桌面（屏幕上）的位置
																      
	BOOL is_update = UpdateLayeredWindow                               // 贴图
	(
		hWnd,            // <- Window HWND
		screen_dc,       // <- Screen DC
		&wnd_pos,        // <- 层在屏幕上的
		&bmp_size,       // <- 贴图大小
		mem_dc,          // <- 需要MemDC，由CreateCompatibleDC获得
		&mem_pos,        // <- 兼容DC层位置
		NULL,            // <- 色键，可以用来去除指定的颜色，但是效果拉跨
		&blend,          // <- 混合结构，设置混合参数
		ULW_ALPHA        // <- 设置 blend 的时候使用
		//ULW_COLORKEY   // <- 设置 色键 的时候使用
	);

	SelectObject(mem_dc, old_bitmap);
	DeleteDC(mem_dc);
	DeleteObject(h_bitmap);
	ReleaseDC(NULL, screen_dc);
	return is_update;
}

// 注意 UpdateLayeredWindow 后不会自动发送WM_PAINT消息
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		Gdiplus::Bitmap bitmap(1280, 720, PixelFormat32bppARGB);
		Gdiplus::Graphics render = &bitmap;
		Gdiplus::Color color = (Gdiplus::Color::MakeARGB(100, 0, 0, 0));  // 定义背色
		render.Clear(color);                                              // 填充背景
		UpdateLayer(bitmap, hWnd, 0, 0);
		return S_OK;
	}
	break;

	case WM_NCHITTEST: // <- 鼠标位置，返回标题栏位置，让系统以为我们选中的地方是标题栏，从而实现拖拽贴图
	{
		return HTCAPTION;
	}
	break;

	case WM_NCRBUTTONDOWN:
	{
		DestroyWindow(hWnd);
	}
	break;

	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return S_OK;
	}
	break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
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
	wcex.lpfnWndProc = WndProc;
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
	DWORD style = WS_POPUP;
	DWORD ex_style_normal = WS_EX_LAYERED;
	DWORD ex_style_hide = WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE;                // 置顶，隐藏任务栏图标
	DWORD ex_style_mouse_penetration = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST;  // 置顶，鼠标穿透
	HWND hWnd = CreateWindowExW
	(
		ex_style_hide, wcex.lpszClassName, L"GDI Transparent", style,
		CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, nullptr, nullptr, wcex.hInstance, nullptr
	);

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