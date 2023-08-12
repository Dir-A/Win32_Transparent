# Win32_Transparent
Win32 透明窗口 / 点击穿透 实现

## Direct2D + DWM
![](./image/Direct2D_DWM.png)

## GDI Plus + DWM
![](./image/GDIPlus_DWM.png)

## Direct2D + UpdateLayeredWindow 
![](./image/Direct2D_GDI.png)

## GDI Plus + UpdateLayeredWindow
![](./image/GDIPlus_GDI.png)

## 参考
[glfw](https://github.com/glfw/glfw/blob/master/src/win32_window.c#L368C27-L368C27)  
[imgui](https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_win32.cpp#L761)  
[TransparentWindow](https://github.com/riley-x/TransparentWindow)  
[Add DirectComposition sample #2129](https://github.com/microsoft/windows-rs/pull/2129)  
[Windows with C++ : Embracing the Windows Composition Engine](https://learn.microsoft.com/en-us/archive/msdn-magazine/2014/july/windows-with-c-embracing-the-windows-composition-engine)  
[Windows with C++ : High-Performance Window Layering Using the Windows Composition Engine](https://learn.microsoft.com/en-us/archive/msdn-magazine/2014/june/windows-with-c-high-performance-window-layering-using-the-windows-composition-engine)  
[Windows with C++ - Layered Windows with Direct2D](https://learn.microsoft.com/en-us/archive/msdn-magazine/2009/december/windows-with-c-layered-windows-with-direct2d)  
[[随笔]关于如何实现鼠标穿透窗口和窗口半透明](https://www.cnblogs.com/xnzzj/p/4524085.html)  
[一种结合CEF离屏渲染技术实现Windows桌面异形窗口应用的技术方案](https://sq.sf.163.com/blog/article/214097332706783232)  
[How do I clear a Direct2D render target to fully transparent](https://stackoverflow.com/questions/2603276/how-do-i-clear-a-direct2d-render-target-to-fully-transparent)  
[Make a win32 window transparent to messages (click, touch, keyboard)](https://stackoverflow.com/questions/65286495/make-a-win32-window-transparent-to-messages-click-touch-keyboard)  
[Win32: How do I make an HWND transparent to clicks?](https://stackoverflow.com/questions/1420924/win32-how-do-i-make-an-hwnd-transparent-to-clicks)   
[Like the cake, WS_EX_TRANSPARENT is a lie, or at least not the entire truth](https://devblogs.microsoft.com/oldnewthing/20121217-00/?p=5823)  
[How do I create a simple transparent window in Win32?](https://learn.microsoft.com/en-us/answers/questions/1190872/how-do-i-create-a-simple-transparent-window-in-win)  
