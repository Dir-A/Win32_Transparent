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
[Add DirectComposition sample #2129](https://github.com/microsoft/windows-rs/pull/2129)  
[Windows with C++ : Embracing the Windows Composition Engine](https://learn.microsoft.com/en-us/archive/msdn-magazine/2014/july/windows-with-c-embracing-the-windows-composition-engine)  
[Windows with C++ : High-Performance Window Layering Using the Windows Composition Engine](https://learn.microsoft.com/en-us/archive/msdn-magazine/2014/june/windows-with-c-high-performance-window-layering-using-the-windows-composition-engine)  
[Windows with C++ - Layered Windows with Direct2D](https://learn.microsoft.com/en-us/archive/msdn-magazine/2009/december/windows-with-c-layered-windows-with-direct2d)  
[TransparentWindow](https://github.com/riley-x/TransparentWindow)  
[imgui](https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_win32.cpp#L761)  
[glfw](https://github.com/glfw/glfw/blob/master/src/win32_window.c#L368C27-L368C27)  
