#pragma once
#include <windows.h>  
#include <gdiplus.h>  

#pragma comment(lib, "gdiplus.lib")

BOOL UpdateLayer(Gdiplus::Bitmap& pBitMap, HWND hWnd, UINT uiX, UINT uiY)
{
	HBITMAP h_bitmap = nullptr;                                       // ��ȡ��ͼ���
	Gdiplus::Color bk_color = (Gdiplus::Color::MakeARGB(0, 0, 0, 0)); // ������ɫ
	pBitMap.GetHBITMAP(bk_color, &h_bitmap);

	BITMAP bmp_info = { 0 };                                          // ��ȡ��ͼ��С
	GetObjectW(h_bitmap, sizeof(bmp_info), &bmp_info);			      
																      
	SIZE bmp_size = { bmp_info.bmWidth, bmp_info.bmHeight };          // ������ͼ��С
																      
	HDC screen_dc = GetDC(NULL);                                      // �������DC
	HDC mem_dc = CreateCompatibleDC(screen_dc);                       // �����ڴ�DC
	HBITMAP old_bitmap = (HBITMAP)SelectObject(mem_dc, h_bitmap);     // ѡ�����ڴ�DC
																      
	COLORREF color = RGB(100, 100, 100);                              // ɫ��ֵ�����ڹ���ָ����ɫ
																      
	BLENDFUNCTION blend = { 0 };                                      // ��Ͻṹ�壬�������û�ϲ���
	blend.BlendOp = 0;											      
	blend.BlendOp = AC_SRC_OVER;								      
	blend.SourceConstantAlpha = 255;							      
	blend.AlphaFormat = AC_SRC_ALPHA;							      
																      
	POINT mem_pos = { 0, 0 };                                          // ������ͼ���ڴ�DC��λ�ã��� bmp_size ֵ֮�Ͳ��ܴ���ͼ�ηֱ��ʣ�������ʾ
	POINT wnd_pos = { (LONG)uiX, (LONG)uiY };                          // ���ô��������棨��Ļ�ϣ���λ��
																      
	BOOL is_update = UpdateLayeredWindow                               // ��ͼ
	(
		hWnd,            // <- Window HWND
		screen_dc,       // <- Screen DC
		&wnd_pos,        // <- ������Ļ�ϵ�
		&bmp_size,       // <- ��ͼ��С
		mem_dc,          // <- ��ҪMemDC����CreateCompatibleDC���
		&mem_pos,        // <- ����DC��λ��
		NULL,            // <- ɫ������������ȥ��ָ������ɫ������Ч������
		&blend,          // <- ��Ͻṹ�����û�ϲ���
		ULW_ALPHA        // <- ���� blend ��ʱ��ʹ��
		//ULW_COLORKEY   // <- ���� ɫ�� ��ʱ��ʹ��
	);

	SelectObject(mem_dc, old_bitmap);
	DeleteDC(mem_dc);
	DeleteObject(h_bitmap);
	ReleaseDC(NULL, screen_dc);
	return is_update;
}

// ע�� UpdateLayeredWindow �󲻻��Զ�����WM_PAINT��Ϣ
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		Gdiplus::Bitmap bitmap(1280, 720, PixelFormat32bppARGB);
		Gdiplus::Graphics render = &bitmap;
		Gdiplus::Color color = (Gdiplus::Color::MakeARGB(100, 0, 0, 0));  // ���屳ɫ
		render.Clear(color);                                              // ��䱳��
		UpdateLayer(bitmap, hWnd, 0, 0);
		return S_OK;
	}
	break;

	case WM_NCHITTEST: // <- ���λ�ã����ر�����λ�ã���ϵͳ��Ϊ����ѡ�еĵط��Ǳ��������Ӷ�ʵ����ק��ͼ
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
	// ���� GDIPlus ��Դ
	ULONG_PTR gdiplus_token;
	Gdiplus::GdiplusStartupInput gdiplus_startup_input;
	Gdiplus::GdiplusStartup(&gdiplus_token, &gdiplus_startup_input, nullptr);

	// ���崰����
	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = GetModuleHandleW(NULL);
	wcex.lpszClassName = L"GDI Transparent Class";

	// ע�ᴰ����
	RegisterClassEx(&wcex);

	// ��������
	// WS_EX_LAYERED �ֲ㴰��
	// WS_EX_NOREDIRECTIONBITMAP
	// WS_EX_TOPMOST �ö�����
	// WS_EX_NOACTIVATE ����������ͼ�꣬���ʹ�� WS_EX_APPWINDOW  ����ʹ�ô���ͼ����ʾ��������
	// WS_EX_NOREDIRECTIONBITMAP DirectComposition��أ��������
	DWORD style = WS_POPUP;
	DWORD ex_style_normal = WS_EX_LAYERED;
	DWORD ex_style_hide = WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE;                // �ö�������������ͼ��
	DWORD ex_style_mouse_penetration = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST;  // �ö�����괩͸
	HWND hWnd = CreateWindowExW
	(
		ex_style_hide, wcex.lpszClassName, L"GDI Transparent", style,
		CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, nullptr, nullptr, wcex.hInstance, nullptr
	);

	// ��ʾ����
	ShowWindow(hWnd, SW_SHOWNORMAL);

	// ��Ϣѭ��
	MSG msg;
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	// ���� GDIPlus ��Դ
	Gdiplus::GdiplusShutdown(gdiplus_token);

	// ע��������
	UnregisterClassW(wcex.lpszClassName, wcex.hInstance);
}