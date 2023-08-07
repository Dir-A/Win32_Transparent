#include <Windows.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")


// 注意 UpdateLayeredWindow 后不会自动发送WM_PAINT消息
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
    // 窗口类定义
    WNDCLASSEXW wcx = { 0 };
    wcx.cbSize = sizeof(wcx);
    wcx.lpfnWndProc = WindowProc;
    wcx.lpszClassName = L"Test Class";
    wcx.hInstance = GetModuleHandleW(nullptr);

    // 注册窗口类
    ::RegisterClassExW(&wcx);

    //创建窗口
    DWORD ex_style_normal = WS_EX_LAYERED;                                                // 正常
    DWORD ex_style_hide = WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE;               // 置顶，隐藏任务栏图标
    DWORD ex_style_mouse_penetration = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST; // 置顶，鼠标穿透
    HWND hwnd = ::CreateWindowExW
    (
        ex_style_hide, wcx.lpszClassName, L"Direct2D_GDI", WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        nullptr, nullptr, wcx.hInstance, nullptr
    );

    // 创建内存DC
    HDC h_dc = GetDC(hwnd);
    HDC h_mem_dc = CreateCompatibleDC(h_dc);

    // 在内存DC创建 HBITMAP
    HBITMAP h_bitmap = CreateCompatibleBitmap(h_dc, 1280, 720);

    // 选入 HBITMAP
    HBITMAP h_bitmap_old = (HBITMAP)SelectObject(h_mem_dc, h_bitmap);
    ReleaseDC(hwnd, h_dc);

    // 定义 Direct2D 对象指针
    ID2D1Factory* sg_pID2D1Factory = nullptr;
    ID2D1DCRenderTarget* sg_pRenderTarget = nullptr;

    // 创建 Direct2D 资源
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

    // 绘制
    {
        sg_pRenderTarget->BeginDraw();
        sg_pRenderTarget->Clear(D2D1::ColorF(0.2f, 0.3f, 0.5f, 0.5f));
        sg_pRenderTarget->EndDraw();
    }

    // 更新 Layer
    {
        SIZE draw_size = { 1280, 720 };
        HDC h_screen_dc = GetDC(NULL);

        BLENDFUNCTION blend = { 0 };          // 混合结构体，用于设置混合参数
        blend.BlendOp = 0;
        blend.BlendOp = AC_SRC_OVER;
        blend.SourceConstantAlpha = 255;
        blend.AlphaFormat = AC_SRC_ALPHA;

        POINT mem_pos = { 0, 0 };             // 设置贴图在内存DC的位置，与 draw_size 值之和不能大于图形分辨率，否则不显示
        POINT wnd_pos = { (LONG)0, (LONG)0 }; // 设置窗口在桌面（屏幕上）的位置

        BOOL is_update = UpdateLayeredWindow  // 贴图
        (
            hwnd,            // <- Window HWND
            h_screen_dc,     // <- Screen DC
            &wnd_pos,        // <- 层在屏幕上的
            &draw_size,      // <- 贴图大小
            h_mem_dc,        // <- 需要MemDC，由CreateCompatibleDC获得
            &mem_pos,        // <- 兼容DC层位置
            NULL,            // <- 色键，可以用来去除指定的颜色，但是效果拉跨
            &blend,          // <- 混合结构，设置混合参数
            ULW_ALPHA        // <- 设置 blend 的时候使用
            //ULW_COLORKEY   // <- 设置 色键 的时候使用
        );
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

    // 清理 Direct2D 资源
    sg_pRenderTarget->Release();
    sg_pID2D1Factory->Release();

    // 清理 GDI 资源
    SelectObject(h_mem_dc, h_bitmap_old);
    DeleteObject(h_bitmap);
    ReleaseDC(hwnd, h_mem_dc);

    // 注销窗口类
    UnregisterClassW(wcx.lpszClassName, wcx.hInstance);
}