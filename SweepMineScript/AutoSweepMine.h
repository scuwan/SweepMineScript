#pragma once
#include "Structure.h"

#define Window_ClassName "Minesweeper"
#define Window_WndName "Minesweeper"
class AutoSweepMine
{
public:
	AutoSweepMine();
	~AutoSweepMine();
	// Get窗口句柄
	HWND GetWindowHwnd();
	// Read 扫雷内存区域
	void ReadMemry();
	// Get窗口界面rectangle
	void GetWindowArea();
	// 截取窗口图像
	void CutWindowBmp();
	// 分割窗口图像
	void SplitWindowBmp();
	//// Get 窗口分割区域矩形
	//void GetCountorsRect();
	// 模拟鼠标点击
	void MouseClicked(int x,int y);
	// 自动点击鼠标扫雷
	void AutoMouseClicked();
	// 自动扫雷
	void SweepMine();
private:
	sSweepMineWindow m_windowHandle;
	sSweepMineMemery m_memery;
	sWindowArea m_windowArea;
	sImageProcess m_imageProess;
};

