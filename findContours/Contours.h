#pragma once
#include "readfile.h"

#define HOLE_BORDER 1
#define OUTER_BORDER 2

struct Border {
	int seq_num; // �߽� NBD ��LNBD��ֵ
	int border_type; // �߽�����
};

struct Point {
	int row;
	int col;

	Point() {
	}

	Point(int r, int c) {
		row = r;
		col = c;
	}

	void setPoint(int r, int c) {
		row = r;
		col = c;
	}

	bool samePoint(Point p) {
		return row == p.row && col == p.col;
	}
};

//struct for storing information on the current border, the first child, next sibling, and the parent.
struct Node {
	int parent;
	int first_child;
	int next_sibling;
	Border border;
	Node(int p, int fc, int ns) {
		parent = p;
		first_child = fc;
		next_sibling = ns;
	}
	void reset() {
		parent = -1;
		first_child = -1;
		next_sibling = -1;
	}
};

//step around a pixel in the 8-connect neighborhood.
Point step(Point &start, Point center) {
	Point p1;
	if (start.row == center.row && start.col > center.col)
		p1.setPoint(center.row + 1, center.col + 1);
	else if (start.col > center.col && start.row < center.row)
		p1.setPoint(center.row, center.col + 1);
	else if (start.row < center.row && start.col == center.col)
		p1.setPoint(center.row - 1, center.col + 1);
	else if (start.row < center.row && start.col < center.col)
		p1.setPoint(center.row - 1, center.col);
	else if (start.row == center.row && start.col < center.col)
		p1.setPoint(center.row - 1, center.col - 1);
	else if (start.row > center.row && start.col < center.col)
		p1.setPoint(center.row, center.col - 1);
	else if (start.row > center.row && start.col == center.col)
		p1.setPoint(center.row + 1, center.col - 1);
	else if (start.row > center.row && start.col > center.col)
		p1.setPoint(center.row + 1, center.col);

	return p1;
}

//step around a pixel inv in the 8-connect neighborhood.
Point stepINV(Point &current, Point center) {
	Point p4;
	if (current.row == center.row && current.col > center.col)
		p4.setPoint(center.row - 1, center.col + 1);
	else if (current.col > center.col && current.row < center.row)
		p4.setPoint(center.row - 1, center.col);
	else if (current.row < center.row && current.col == center.col)
		p4.setPoint(center.row - 1, center.col - 1);
	else if (current.row < center.row && current.col < center.col)
		p4.setPoint(center.row, center.col - 1);
	else if (current.row == center.row && current.col < center.col)
		p4.setPoint(center.row + 1, center.col - 1);
	else if (current.row > center.row && current.col < center.col)
		p4.setPoint(center.row + 1, center.col);
	else if (current.row > center.row && current.col == center.col)
		p4.setPoint(center.row + 1, center.col + 1);
	else if (current.row > center.row && current.col > center.col)
		p4.setPoint(center.row, center.col + 1);

	return p4;
}

//checks if given pixel has already been examined
bool isExamined(bool checked[4]) {
	//p3.row, p3.col + 1
	return checked[0];
}

//checks if a given pixel is out of bounds of the image
bool pixelOutOfBounds(Point p, int numrows, int numcols)
{
	return (p.col >= numcols || p.row >= numrows || p.col < 0 || p.row < 0);
}

//marks a pixel as examined after passing through in the 8-connected case
void markExamined(Point mark, Point center, bool checked[8]) {
	//p3.row, p3.col + 1
	int loc = -1;
	//  5 6 7
	//  4 x 0
	//  3 2 1
	if (mark.row == center.row && mark.col > center.col)
		loc = 0;
	else if (mark.col > center.col && mark.row < center.row)
		loc = 7;
	else if (mark.row < center.row && mark.col == center.col)
		loc = 6;
	else if (mark.row < center.row && mark.col < center.col)
		loc = 5;
	else if (mark.row == center.row && mark.col < center.col)
		loc = 4;
	else if (mark.row > center.row && mark.col < center.col)
		loc = 3;
	else if (mark.row > center.row && mark.col == center.col)
		loc = 2;
	else if (mark.row > center.row && mark.col > center.col)
		loc = 1;

	if (loc == -1)
		throw exception("Error: markExamined Failed");

	checked[loc] = true;
	return;
}

//follows a border from start to finish given a starting point
void followBorder(vector<vector<int>> &image, int row, int col, Point p2, Border NBD, vector<vector<Point>> &contours)
{
	int numrows = image.size();
	int numcols = image[0].size();
	
	Point start; // ���ҿ�ʼָ��
	
	// 2.1  ��ʼ������ָ��
	start.setPoint(p2.row, p2.col);
	Point p0(row, col); // center

	Point p1;

	// �洢�ҵ��ı߽�ĵ�
	vector<Point> point_storage;
	
	// ��[r,c] Ϊ���ģ�i_2 Ϊ��ʼ�㣬˳ʱ���ҵ���һ����0���ص� ��Ϊ i_1
	do {
		p1 = step(start, p0);
		// ��� û�з� 0 ���ص�
		if (start.samePoint(p2)) 
		{
			image[start.row][start.col] = -NBD.seq_num;
			point_storage.push_back(start);
			contours.push_back(point_storage);
			return;
		}
	} while (pixelOutOfBounds(start, numrows, numcols) || image[start.row][p1.col] == 0);

	// 3.1 i_2 = i_1; i_3 = i_0 
	p2 = p1;
	Point p3 = p0;
	
	Point p4;
	bool checked[8];
	// 3.2 ��������
	while (true) {
		// 3.3 �趨��ʼ�� p2
		start.setPoint(p2.row, p2.col);

		// ���8��ͨ�� �Ƿ��ڲ���i_4��·����
		for (int i = 0; i < 8; i++)
			checked[i] = false;

		// 3.4 ��ʱ����� i_4
		do {
			markExamined(start, p3, checked); // ���·�������ĵ�
			p4 = stepINV(start, p3);
		} while (pixelOutOfBounds(start, numrows, numcols) || image[start.row][start.col] == 0);

		// ���� i_3 ��ֵ
		if ((p3.col + 1 >= numcols || image[p3.row][p3.col + 1] == 0) && isExamined(checked)) 
		{
			image[p3.row][p3.col] = -NBD.seq_num;
		}
		else if (p3.col + 1 < numcols && image[p3.row][p3.col] == 1 && isExamined(checked)) 
		{
			image[p3.row][p3.col] = NBD.seq_num;
		}

		point_storage.push_back(p3);
		// 3.5 �ж���ֹ����������ص�ԭ��
		if (p4.samePoint(p0) && p3.samePoint(p1)) {
			contours.push_back(point_storage);
			return;
		}

		// ����
		p2 = p3;
		p3 = p4;
	}
}


void findContours(vector<vector<int>>& image, int numrows, int numcols)
{
	// a vector of vectors to store each contour.
	// contour n will be stored in contours[n-2]
	// contour 2 will be stored in contours[0]
	vector<vector<Point>> contours;
	
	Border NBD, LNBD;
	LNBD.border_type = HOLE_BORDER;
	NBD.border_type = HOLE_BORDER;
	NBD.seq_num = 1;

	// hierarchy tree will be stored as an vector of nodes instead of using an actual tree since we need to access a node based on its index
	// see definition for Node ��¼ NBD ֵ��Ӧ�� �߽�����
	vector<Node> hierarchy;

	// 
	Node temp_node(-1, -1, -1);
	temp_node.border = NBD;
	hierarchy.push_back(temp_node);

	Point p2; // ��ʼ��i_2
	bool border_start_found; // ����Ƿ�Ϊ�߽���ʼ��

	// �����������ص�

	for (int r = 0; r < numrows; r++) {
		LNBD.seq_num = 1; // ÿ�����ǿ�ʼɨ���µ�һ�У����� LNBD=1
		LNBD.border_type = HOLE_BORDER;
		for (int c = 0; c < numcols; c++) {
			border_start_found = false;
			// Step 1: ���ڱ߽����� Find border
			// ��߽磺 starting point of an outer border, increment NBD, and i_2 <- (i, j - 1).
			if ((image[r][c] == 1 && c - 1 < 0) || (image[r][c] == 1 && image[r][c - 1] == 0)) {
				NBD.border_type = OUTER_BORDER;
				NBD.seq_num += 1;
				p2.setPoint(r, c - 1);
				border_start_found = true;
			}
			// ���߽磺starting point of a hole border, increment NBD, i_2 ��(i, j + 1), and LNBD �� f[i,j] in case f[i,j] > 1.
			else if (c + 1 < numcols && (image[r][c] >= 1 && image[r][c + 1] == 0)) {
				NBD.border_type = HOLE_BORDER;
				NBD.seq_num += 1;
				if (image[r][c] > 1) // �����õ�ʱ��һ���߽�ı��,���¸��ױ��
				{
					LNBD.seq_num = image[r][c];
					LNBD.border_type = hierarchy[LNBD.seq_num - 1].border.border_type;
				}
				p2.setPoint(r, c + 1);
				border_start_found = true;
			}

			if (!border_start_found)
				continue;

			// Step 2: Store Parent
			temp_node.reset();
			if (NBD.border_type == LNBD.border_type) {
				temp_node.parent = hierarchy[LNBD.seq_num - 1].parent;
				temp_node.next_sibling = hierarchy[temp_node.parent - 1].first_child;
				hierarchy[temp_node.parent - 1].first_child = NBD.seq_num;
				temp_node.border = NBD;
				hierarchy.push_back(temp_node);
			}
			else {
				if (hierarchy[LNBD.seq_num - 1].first_child != -1) {
					temp_node.next_sibling = hierarchy[LNBD.seq_num - 1].first_child;
				}

				temp_node.parent = LNBD.seq_num;
				hierarchy[LNBD.seq_num - 1].first_child = NBD.seq_num;
				temp_node.border = NBD;
				hierarchy.push_back(temp_node);
			}

			// Step 3: �߽�׷��
			followBorder(image, r, c, p2, NBD, contours);

			// Step 4: Continue to next border ����ǰ�㸺ֵ������
			// If fij != 1, then LNBD <- abs( fij ) and resume the raster scan from the pixel(i, j + 1).
			// The algorithm terminates when the scan reaches the lower right corner of the picture.
			if (abs(image[r][c]) > 1) {
				LNBD.seq_num = abs(image[r][c]);
				LNBD.border_type = hierarchy[LNBD.seq_num - 1].border.border_type;
			}
		}

	}
}

