#include "Point.hpp"

ostream& operator << (ostream& os, const Point& pt)
{
    os << "(" << pt.m_row<< "," << pt.m_col << "):";
}

vector<Point> Point::get4RegionPoints()
{
    vector<Point> pts;
    pts[0] = *this; // red
    pts[1] = Point(m_row, m_col+1); // orange
    pts[2] = Point(m_row+1, m_col); // green
    pts[3] = Point(m_row+1, m_col+1); // yellow

    return pts;
}

vector<Point> Point::get4RegionPoints()
{
    vector<Point> pts;
    pts[0] = *this; // red
    pts[1] = Point(m_row, m_col+1); // orange
    pts[2] = Point(m_row+1, m_col); // green
    pts[3] = Point(m_row+1, m_col+1); // yellow

    return pts;
}

vector<Point> Point::get12RegionPoints()
{
    vector<Point> pts;
    for (int i =0; i<12; ++i)
    {
        int x = m_row + m_offsets[i][0];
        int y = m_col + m_offsets[i][1];

        pts.push_back(Point(x, y));
    }

    return pts;
}