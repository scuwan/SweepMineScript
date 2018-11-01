
#pragma once
#include <string>
#include <Windows.h>
#include <iostream>
#include<iomanip>
#include<opencv2/opencv.hpp>
using namespace std;
// 窗体结构体
struct sSweepMineWindow 
{
	string MineWinClass;
	string MineWinName;
	int MineCellLen;
	HWND MineWinHandle;
	DWORD ProcessId;
	sSweepMineWindow() :MineWinHandle(nullptr) {}
};

//扫雷内存区
struct sRowColumn
{
	int r;
	int c;
};
struct sSweepMineMemery
{
	const int zone_adr = 0x01005340;
	const int rows_adr = 0x010056a8;
	const int columns_adr = 0x010056ac;
	const int mines_adr = 0x010056a4;
	const int len = 32 * 26;
	short mine_number;
	char rows;
	char columns;
	unsigned char mine_zone[26][32];
	vector<sRowColumn> safe_row_columns;
	sSweepMineMemery() { memset((void*)mine_zone, 0, 26 * 32); }
	inline void Print()
	{
		cout << "rows: " << (int)rows << "  ";
		cout << "colums: " << (int)columns << "  "<<endl;
		cout << "mine number: " << mine_number << endl;
		cout << hex<<endl;
		for (int j = 0; j < 26; ++j)
		{
			for (int i = 0; i < 32; ++i)
			{
				cout <<setfill('0') << setw(2) << (int)mine_zone[j][i]<<" ";
			}
			cout << endl;
		}
	}

};

//扫雷窗口区域
struct sWindowArea
{
	RECT window_area;
	cv::Rect whole_area;
	cv::Rect mines_area;
	cv::Rect mine_num_area;
	cv::Rect time_area;
	cv::Rect face_area;
};

//图像处理结构
struct sImageProcess
{
	vector<vector<cv::Point>> contours;
	vector<cv::Rect> rects;
};