FSR 算法思想

参考来源：
    ref: https://zhuanlan.zhihu.com/p/401030221

算法分为两步：EASU 和 RCAS (Robust Contrast Adaptive Sharpening)

- EASU (Edge Adaptive Spatial Upsampling)

          +---+---+
	      |   |   |
	      +--(0)--+
	      | b | c |
	  +---F---+---+---+
	  | e | f | g | h |
	  +--(1)--+--(2)--+
	  | i | j | k | l |
	  +---+---+---+---+
	      | n | o |
	      +--(3)--+
	      |   |   |
	      +---+---+

    - 对于一个待插值点 P 的像素值，通过以下算法计算

        - 1. 生成自适应 Lanczos2 （多项式拟合）函数（依据边缘特征确定超参数 w）

            - 依据 十字滤波结构 得到 4个区域，针对每个区域进行如下计算
                - 计算 P 在该区域的边缘特征 Feature
                - 依据 Feature 与 w 的公式计算 w
                - 计算各个区域的梯度以及相位信息

            - 针对计算完四个区域的 w 进行双线性插值，获取 最终的 w 代入 Lanczos2 函数

            实现思路：
                1. 一个 Point 类，支持获取 上下左右 四个点的坐标以及像素
                2. 一个遍历函数，遍历每个放大后的点坐标到原图的映射坐标（floor）
                3. 对这个点进行一下处理：
                    - 计算w
                    3.1 获取这个点 [(0,0), (1,0), (0,1), (1,1)] 偏移量的 4个点
                    3.2 依据 公式计算出四个点 的 Feature，并依据 Feature 计算 4 个 w
                    3.3 依据 w 以及像素点的位置进行双线性插值得到最终的 w ，生成 Lanczos2 函数

                    - 计算 12个 点的梯度以及相位角信息
                    - 针对偏移量进行旋转缩放，代入 Lanczos2 函数 生成对应的 weight
                    - 乘累加计算该像素点


        - 2. 基于以上产生的 Lanczos2 函数，计算待插值点 P 周围12个点的权重，像素加权平均可得 P 的像素值

            - 计算 12 个点的偏移量信息
            - 针对这些偏移量，依据上个步骤中区域的梯度和幅度信息进行旋转缩放（注意截断），最后再代入到 Lanczos2 函数中获取权重
            - 依据上述权重加权平均计算即可

- RCAS

    - 根据像素周围的对比度来生成滤波模板，对图像进行滤波得到最终结果

