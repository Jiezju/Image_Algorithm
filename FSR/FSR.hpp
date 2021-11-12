#ifndef AMD_FSR_FSR_H
#define AMD_FSR_FSR_H

#include "utils.hpp"

class FSR {
private:
    float* m_rImage;
    float* m_gImage;
    float* m_bImage;
    float* m_grayImage;
    float** m_dstImage;
    float m_scale;
    int m_height;
    int m_width;

    void _preprocessing(const uint8_t* image)
    {
         // 分配 rgb 内存

         // 分配 目标图像内存

         // 计算灰度图

         // 归一化灰度图
    }

    int _calPointMinusPointEdgeCharacter(Point point1, Point Point2, float* image) 
    {
        return image[point1.getRow()*m_width + point1.getCol()] - image[Point2.getRow()*m_width + Point2.getCol()];
    }

    float _calPointEdgeCharacterX(Point point) 
    {
        Point* rightPoint = point.getRightPoint();
        Point* leftPoint = point.getLeftPoint();
        int rightMinusCur = _calPointMinusPointEdgeCharacter(point, *rightPoint, m_grayImage);
        int leftMinusCur = _calPointMinusPointEdgeCharacter(point, *leftPoint, m_grayImage);
        int leftMinusRight = _calPointMinusPointEdgeCharacter(*rightPoint, *leftPoint, m_grayImage);
        int denominator = MaxAbs(rightMinusCur, leftMinusCur);
        if(denominator == 0) 
            return 0;
        return float(abs(leftMinusRight)) / denominator;
    }

    float _calPointEdgeCharacterY(Point point) 
    {
        Point *upPoint = point.getUpPoint();
        Point *downPoint = point.getDownPoint();
        int downMinusCur = _calPointMinusPointEdgeCharacter(point, *downPoint, m_grayImage);
        int upMinusCur = _calPointMinusPointEdgeCharacter(point, *upPoint, m_grayImage);
        int upMinusDown = _calPointMinusPointEdgeCharacter(*downPoint, *upPoint, m_grayImage);
        int denominator = MaxAbs(downMinusCur, upMinusCur);
        if(denominator == 0) 
            return 0;
        return float(abs(upMinusDown)) / denominator;
    }

    float _getPointEdgeCharacter(Point point) 
    {
        float fx = pow(_calPointEdgeCharacterX(point), 2);
        float fy = pow(_calPointEdgeCharacterY(point), 2);

        return pow((fx+fy) / 2, 2);
    } 

    float _getLanczos2W(float px, float py)
    {
        float ws[4];
        Point Q(int(px), int(py));
        vector<Point> region_pts = Q.get4RegionPoints();

        for (int i=0; i<4; ++i)
        {
            /*
                计算 4 个点的特征值

                *      *
                   · Q

                *      *
            
            */
            float feature = _getPointEdgeCharacter(region_pts[i]);
            ws[i] = 0.5 - 0.25*feature;
        }

        // 对 w 双线性插值
        float x = px - int(px);
        float y = py - int(py);

        return ws[0]*(1-x)*(1-y) + ws[1]*x*(1-y) + ws[2]*(1-x)*y + ws[3]*x*y;
        
    }

    int _calColor(Point Q, vector<Point> region_points, float w, float* image)
    {
        Point* rightPoint = point.getRightPoint();
        Point* leftPoint = point.getLeftPoint();
        int gradx = _calPointMinusPointEdgeCharacter(*leftPoint, *rightPoint, image);
        Point *upPoint = point.getUpPoint();
        Point *downPoint = point.getDownPoint();
        int grady = _calPointMinusPointEdgeCharacter(*upPoint, *downPoint, image);

        float r = sqrt(gradx * gradx + grady * grady);
        float dir_x = gradx / r;
        float dir_y = grady / r;
        
        float feature = 2 - 4*w;
        float Stretch = (1.0f / (max(abs(dir_x), abs(dir_y))));

        float sx = 1 + (Stretch - 1) * feature;
        float sy = 1 - 0.5 * feature;

        float sumColor = 0, sumWeight = 0;

        int** offsets = Q.getOffsets();
        int height = CommonProperty::getHeight();
        int width = CommonProperty::getWidth();

        for (int i=0; i<12; i++)
        {
            int row = region_points[i].getRow();
            int col = region_points[i].getCol();

            int color = image[row * width + col];

            float offsetx = offsets[i][0];
            float offsety = offsets[i][1];
            // 旋转
            float rx = offsetx * dir_x + offsety * dir_y;
            float ry = - offsety * dir_y + offsetx * dir_x;
            // 缩放
            rx *= sx;
            ry *= sy;

            float x2 = rx * rx + ry * ry;

            float window = Lanczos2(w, x2);

            sumColor += color * window;
            sumWeight += window;
        }

        return int(sumColor/sumWeight);

    }

    void _fsrEasu()
    {
        // 计算输出像素每个点的像素值
        for (int row=0; row<m_height; ++row)
        {
            for (int col=0; col<m_width; ++col)
            {
                // 获取映射到原图的坐标点(floor)
                Point Q(row / m_scale, col / m_scale);

                // 从 灰度图 确定 Lanczos2 函数的 w 参数
                float w = _getLanczos2W(row / m_scale, col / m_scale);

                // 获取周围 12 个 像素点
                vector<Point> region_points = Q.get12RegionPoints();

                // 计算像素点的 R G B 值
                int R = _calRColor(Q, region_points, w, m_rImage);
                int G = _calRColor(Q, region_points, w, m_gImage);
                int B = _calRColor(Q, region_points, w, m_bImage);
                
                m_dstImage[row*m_width + col][0] = R;
                m_dstImage[row*m_width + col][1] = G;
                m_dstImage[row*m_width + col][2] = B;
            }
        }

    }


public:
    FSR(const uint8_t* image, int height, int width, float scale)
    :m_scale(scale), m_height(scale * height), m_width(scale * width)
    {
        CommonProperty::setSize(height, width);

        // 原图padding（边界处理），分配resize 内存，灰度化
        _preprocessing(image);
        
        // EASU Pass
        _fsrEasu();

        // RCAS Pass
        _fsrRcas();
    }

    ~FSR()
    {
        
    }
};

#endif //AMD_FSR_FSR_H
