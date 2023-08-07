#include <Windows.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")


// ע�� UpdateLayeredWindow �󲻻��Զ�����WM_PAINT��Ϣ
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_NCHITTEST)
    {
        return HTCAPTION;
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

int main()
{
    // �����ඨ��
    WNDCLASSEXW wcx = { 0 };
    wcx.cbSize = sizeof(wcx);
    wcx.lpfnWndProc = WindowProc;
    wcx.lpszClassName = L"Test Class";
    wcx.hInstance = GetModuleHandleW(nullptr);

    // ע�ᴰ����
    ::RegisterClassExW(&wcx);

    //��������
    DWORD ex_style_normal = WS_EX_LAYERED;                                                // ����
    DWORD ex_style_hide = WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE;               // �ö�������������ͼ��
    DWORD ex_style_mouse_penetration = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST; // �ö�����괩͸
    HWND hwnd = ::CreateWindowExW
    (
        ex_style_hide, wcx.lpszClassName, L"Direct2D_GDI", WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        nullptr, nullptr, wcx.hInstance, nullptr
    );

    // �����ڴ�DC
    HDC h_dc = GetDC(hwnd);
    HDC h_mem_dc = CreateCompatibleDC(h_dc);

    // ���ڴ�DC���� HBITMAP
    HBITMAP h_bitmap = CreateCompatibleBitmap(h_dc, 1280, 720);

    // ѡ�� HBITMAP
    HBITMAP h_bitmap_old = (HBITMAP)SelectObject(h_mem_dc, h_bitmap);
    ReleaseDC(hwnd, h_dc);

    // ���� Direct2D ����ָ��
    ID2D1Factory* sg_pID2D1Factory = nullptr;
    ID2D1DCRenderTarget* sg_pRenderTarget = nullptr;

    // ���� Direct2D ��Դ
    {
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &sg_pID2D1Factory);

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties
        (
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
        );
        sg_pID2D1Factory->CreateDCRenderTarget(&props, &sg_pRenderTarget);

        RECT rc = { 0 };
        GetClientRect(hwnd, &rc);
        sg_pRenderTarget->BindDC(h_mem_dc, &rc);
    }

    // ����
    {
        sg_pRenderTarget->BeginDraw();
        sg_pRenderTarget->Clear(D2D1::ColorF(0.2f, 0.3f, 0.5f, 0.5f));
        sg_pRenderTarget->EndDraw();
    }

    // ���� Layer
    {
        SIZE draw_size = { 1280, 720 };
        HDC h_screen_dc = GetDC(NULL);

        BLENDFUNCTION blend = { 0 };          // ��Ͻṹ�壬�������û�ϲ���
        blend.BlendOp = 0;
        blend.BlendOp = AC_SRC_OVER;
        blend.SourceConstantAlpha = 255;
        blend.AlphaFormat = AC_SRC_ALPHA;

        POINT mem_pos = { 0, 0 };             // ������ͼ���ڴ�DC��λ�ã��� draw_size ֵ֮�Ͳ��ܴ���ͼ�ηֱ��ʣ�������ʾ
        POINT wnd_pos = { (LONG)0, (LONG)0 }; // ���ô��������棨��Ļ�ϣ���λ��

        BOOL is_update = UpdateLayeredWindow  // ��ͼ
        (
            hwnd,            // <- Window HWND
            h_screen_dc,     // <- Screen DC
            &wnd_pos,        // <- ������Ļ�ϵ�
            &draw_size,      // <- ��ͼ��С
            h_mem_dc,        // <- ��ҪMemDC����CreateCompatibleDC���
            &mem_pos,        // <- ����DC��λ��
            NULL,            // <- ɫ������������ȥ��ָ������ɫ������Ч������
            &blend,          // <- ��Ͻṹ�����û�ϲ���
            ULW_ALPHA        // <- ���� blend ��ʱ��ʹ��
            //ULW_COLORKEY   // <- ���� ɫ�� ��ʱ��ʹ��
        );
    }

    // ��ʾ����
    ShowWindow(hwnd, SW_NORMAL);

    // ��Ϣѭ��
    MSG msg = { 0 };
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessageW(&msg);
    }

    // ���� Direct2D ��Դ
    sg_pRenderTarget->Release();
    sg_pID2D1Factory->Release();

    // ���� GDI ��Դ
    SelectObject(h_mem_dc, h_bitmap_old);
    DeleteObject(h_bitmap);
    ReleaseDC(hwnd, h_mem_dc);

    // ע��������
    UnregisterClassW(wcx.lpszClassName, wcx.hInstance);
}