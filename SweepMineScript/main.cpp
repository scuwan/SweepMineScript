#include "AutoSweepMine.h"
#include <Windows.h>
#include <iostream>

bool RegisterHotKeys();
void UnRegisterHotKeys();
void tips();

int main(int argc, char**argv)
{
	AutoSweepMine asw;
	bool flag = RegisterHotKeys();
	if (!flag)
	{
		cout << "注册热键失败\n";
	}
	else
	{
		tips();
		MSG msg = {0};
		int key_id;
		while (1)
		{
			if (GetMessage(&msg, NULL, 0, 0) != 0)
			{
				if (msg.message == WM_HOTKEY)
				{
					key_id = msg.wParam;
					if(key_id==1)
					{
						asw.SweepMine();
					}
					else
					{
						int ok_cancel=MessageBox(NULL, "退出自动扫雷?", "消息", MB_OKCANCEL);
						if (IDCANCEL == ok_cancel)
							;
						else
						{
							UnRegisterHotKeys();
							break;
						}
					}
				}
			}
		}
	}
	return (0);
}


bool RegisterHotKeys()
{
	if (!RegisterHotKey(NULL, 1,MOD_ALT| MOD_NOREPEAT,0x45))  // 'e'
	{
		cout << "Hotkey 'ALT+e' registered false, using MOD_NOREPEAT flag\n";
		cout << GetLastError();
		return false;
	}
	if (!RegisterHotKey(NULL,2,MOD_ALT| MOD_NOREPEAT,0x51))  // 'q'
	{
		cout << "Hotkey 'ALT+b' registered false, using MOD_NOREPEAT flag\n";
		return false;
	}
	return true;
}

void UnRegisterHotKeys()
{
	UnregisterHotKey(NULL, 1);
	UnregisterHotKey(NULL, 2);
}

void tips()
{
	cout << "\"Alt+e\"" << "执行扫雷  " << "\"Alt+q\"" << "退出脚本" << endl;
}