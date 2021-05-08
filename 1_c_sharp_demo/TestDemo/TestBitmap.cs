using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing; // C# 绘图命名空间
using System.Drawing.Imaging; // C# 图像命名空间
using System.Runtime.InteropServices; // 引用调用命名空间

namespace TestDemo
{
    unsafe class TestBitmap
    {

        // 导入C/C++动态链接库
        [DllImport("TestDemo_C.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.None, ExactSpelling = true)]

        // C/C++ 接口函数
        private static extern int f_Test(byte* srcData, int width, int height, int stride);
        
         // C#函数 调用f_Test函数
         public Bitmap Test(Bitmap src)
         {
             // 创建图像
             Bitmap a = new Bitmap(src);
             // 获取图像宽高
             int w = a.Width;
             int h = a.Height;
             // 截取图像区域
             BitmapData srcData = a.LockBits(new Rectangle(0, 0, a.Width, a.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb); // 32位RGB格式

            /*有一个比较特别的 Stride 成员，它表示「一行」图像对应的缓冲区所实际占用的字节数
             *（因为位图文件有一条变态的规则：图片数据在存储时每一行字节数必须是 4 的倍数，
             * 如果真实图片数据宽度不是 4 的倍数则需要用垃圾数据补齐不足的字节数，
             * 于是就造成了 Stride ≠ Width 的现象，即所谓的字节对齐*/
            f_Test((byte*)srcData.Scan0, w, h, srcData.Stride);
             // 解锁图像区域
             a.UnlockBits(srcData);
             return a;
         }

    }
}
