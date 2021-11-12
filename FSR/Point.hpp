#ifndef AMD_FSR_POINT_H
#define AMD_FSR_POINT_H

#include <iostream>
#include <vector>
#include<algorithm>

using namespace std;

class CommonProperty 
{
private:
    CommonProperty();
public:
    static int m_Height;
    static int m_Width;
    static int getHeight() { return m_Height; }

    static int getWidth() { return m_Width; }

    static void setSize(int height, int width) {
        m_Height = height;
        m_Width = width;
    }
};



class Point 
{
private:
    int m_row;
    int m_col;

    int m_offsets[12][2] = {{0, -1},{1, -1}, {-1, 1}, {0,1}, {0,0}, {-1,0}, 
                          {1, 1}, {2, 1}, {2, 0}, {1,0}, {1,2}, {0, 2}}

public:
    Point(int row, int col):m_row(row),m_col(col) {}

    int getRow() const { return m_row; }

    int getCol() const { return m_col; }

    int** getOffsets() const { return m_offsets; }
    
    Point* getUpPoint(){
        if (m_row <= 0) 
            return this;
        else
            return new Point(m_row - 1, m_col);
    }
    
    Point* getDownPoint() {
        if (m_row >= CommonProperty::getHeight() - 1) 
            return this;
        else
            return new Point(m_row + 1,m_col);
    }

    Point* getLeftPoint(){
        if (m_col <= 0) 
            return this;
        else
            return new Point(m_row, m_col - 1);
    }

    Point* getRightPoint() {
        if (m_col >= CommonProperty::getWidth() - 1) 
            return this;
        else
            return new Point(m_row, m_col + 1);
    }

    // 获取当前点的 4-邻域节点
    vector<Point> get4RegionPoints();

    // 获取当前点的 12 个周围像素点
    vector<Point> get12RegionPoints();
    
    friend ostream& operator << (ostream& os, const Point& pt);
    
};

#endif //AMD_FSR_POINT_H