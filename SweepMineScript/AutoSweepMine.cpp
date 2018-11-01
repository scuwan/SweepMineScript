#include "AutoSweepMine.h"
#include "WriteToBMP.h"
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

//#define _SHOWIMAGE
#ifdef _SHOWIMAGE
#define ShowImage(WINNAME, IMAGENAME){	\
	namedWindow(#WINNAME, CV_WINDOW_NORMAL); \
	imshow(#WINNAME, IMAGENAME);	\
	cvWaitKey();	\
}
#else
#define ShowImage(WINNAME, IMAGENAME)
#endif


AutoSweepMine::AutoSweepMine()
{
	m_windowHandle.MineWinClass = Window_ClassName;
	m_windowHandle.MineWinName = Window_WndName;
}


AutoSweepMine::~AutoSweepMine()
{
}

HWND AutoSweepMine::GetWindowHwnd()
{
	HWND hwnd = FindWindow(m_windowHandle.MineWinClass.c_str(),
		m_windowHandle.MineWinName.c_str());
	if (hwnd == nullptr) {
		throw std::exception("没有找到扫雷程序!!!");
	}
	else {
		cout << "成功找到扫雷程序.\n";
	}
	m_windowHandle.MineWinHandle = hwnd;
	DWORD process_id;
	GetWindowThreadProcessId(hwnd, &process_id);
	m_windowHandle.ProcessId = process_id;
	cout << "扫雷程序进程id = " << process_id<<endl;
	return hwnd;
}

void AutoSweepMine::ReadMemry()
{
	HANDLE h_process = OpenProcess(PROCESS_VM_READ, FALSE, m_windowHandle.ProcessId);
	if (nullptr == h_process)
	{
		throw std::exception("读取扫雷程序内存失败!!!");
	}
	SIZE_T bytes_read;
	//读取雷区
	if (!ReadProcessMemory(h_process, (LPCVOID)m_memery.zone_adr, (LPVOID)m_memery.mine_zone, m_memery.len, &bytes_read))
	{
		throw std::exception("读取扫雷程序内存失败!!!");
	}
	//读取行和列
	if (!ReadProcessMemory(h_process, (LPVOID)m_memery.rows_adr, (LPVOID)(&m_memery.rows), 1, &bytes_read))
	{
		throw std::exception("读取扫雷程序内存失败!!!");
	}
	if (!ReadProcessMemory(h_process, (LPVOID)m_memery.columns_adr, (LPVOID)(&m_memery.columns), 1, &bytes_read))
	{
		throw std::exception("读取扫雷程序内存失败!!!");
	}
	//读取雷的个数
	if (!ReadProcessMemory(h_process, (LPVOID)m_memery.mines_adr, (LPVOID)(&m_memery.mine_number), 2, &bytes_read))
	{
		throw std::exception("读取扫雷程序内存失败!!!");
	}
	//提取安全的行和列
	m_memery.safe_row_columns.clear();
	for(int r=1;r<=m_memery.rows;++r)
		for (int c = 1; c <= m_memery.columns; ++c)
		{
			if (m_memery.mine_zone[r][c] == 0x0F)
			{
				sRowColumn a;
				a.r = r;
				a.c = c;
				m_memery.safe_row_columns.push_back(a);
			}	
		}
	//m_memery.Print();
	CloseHandle(h_process);
}

void AutoSweepMine::GetWindowArea()
{
	RECT rect;
	bool ret = GetClientRect(m_windowHandle.MineWinHandle, &rect);
	if (!ret) {
		throw std::exception("获取扫雷窗口区域失败！！！");
	}
	m_windowArea.window_area = rect;
}

void AutoSweepMine::CutWindowBmp()
{
	HDC hDC = ::GetDC(m_windowHandle.MineWinHandle);    // 以窗口句柄为基准进行绘制
	bool ret = WriteBmp("1.bmp", hDC, m_windowArea.window_area);
	::ReleaseDC(m_windowHandle.MineWinHandle, hDC);
	if (!ret) {
		throw std::exception("截取扫雷窗口区域图形失败！！！");
	}
}

struct sort_index
{
	int index;
	int v;
};
bool cmp(sort_index a, sort_index b)
{
	if (a.v > b.v)
		return true;
	else
		return false;
}
void AutoSweepMine::SplitWindowBmp()
{
	Mat src_img = imread("1.bmp");
	if (src_img.empty())
	{
		cout << "读取雷区图片失败!" << endl;
		throw exception("读取雷区图片失败");
	}
	ShowImage(src_img, src_img);
	cvtColor(src_img, src_img, CV_RGB2GRAY);
	threshold(src_img, src_img, 0, 255, cv::THRESH_OTSU);
	ShowImage(bin, src_img);
	
	morphologyEx(src_img, src_img, MORPH_CLOSE, Mat());
	ShowImage(close, src_img);

	vector<vector<Point>> contours;
	findContours(src_img, contours, RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	vector<Rect> rects;
	vector<vector<Point>> contours_poly(contours.size());
	for (unsigned int i = 0; i <contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		rects.push_back(boundingRect(contours_poly[i]));
	}

	Mat res(src_img.size(), CV_8UC3, Scalar(255));
	RNG rng;
	for (unsigned int i = 0; i < contours.size(); i++) {
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(res, vector<vector<Point>>{contours[i]}, -1, Scalar(0, 0, 0), 3);
		rectangle(res, rects[i].tl(), rects[i].br(), color, 2, 8, 0);
	}
	ShowImage(contours, res);

	if (contours.size() != 5) {
		throw exception("提取扫雷区域异常，请检查！！！");
	}
	//提取窗口各个区域
	vector<sort_index> i_v(5);
	for (int i = 0; i < 5; ++i)
	{
		i_v[i].index = i;
		i_v[i].v = rects[i].height*rects[i].width;
	}
	sort(i_v.begin(), i_v.end(), cmp);
	m_windowArea.whole_area = rects[i_v[0].index];
	m_windowArea.mines_area = rects[i_v[1].index];
	m_windowArea.face_area = rects[i_v[4].index];
	if (rects[i_v[2].index].x > rects[i_v[3].index].x)
	{
		m_windowArea.mine_num_area = rects[i_v[3].index];
		m_windowArea.time_area = rects[i_v[2].index];
	}
	else
	{
		m_windowArea.mine_num_area = rects[i_v[2].index];
		m_windowArea.time_area = rects[i_v[3].index];
	}
	m_imageProess.contours = contours;
	m_imageProess.rects = rects;
}

void AutoSweepMine::MouseClicked(int x, int y)
{
	SendMessage(m_windowHandle.MineWinHandle, WM_LBUTTONDOWN, 0, MAKELPARAM(x, y));
	SendMessage(m_windowHandle.MineWinHandle, WM_LBUTTONUP, 0, MAKELPARAM(x, y));
}

void AutoSweepMine::AutoMouseClicked()
{
	int x, y;
	for (int i = 0; i < m_memery.safe_row_columns.size(); ++i)
	{
		
		x = m_windowArea.mines_area.x+m_memery.safe_row_columns[i].c * 16 - 8 + 2;
		y = m_windowArea.mines_area.y+m_memery.safe_row_columns[i].r * 16 - 8 + 2;
		MouseClicked(x, y);
	}
}

void AutoSweepMine::SweepMine()
{
	try
	{
		GetWindowHwnd();
		ReadMemry();
		GetWindowArea();
		CutWindowBmp();
		SplitWindowBmp();
	}
	catch (exception e)
	{
		std::cout << e.what() << endl;
		return;
	}
	AutoMouseClicked();
}

//void AutoSweepMine::GetCountorsRect()
//{
//	/*for (unsigned int i = 0; i < m_imageProess.contours.size(); i++) {
//		m_imageProess.rects.push_back(boundingRect(m_imageProess.contours[i]).area());
//	}*/
//}
