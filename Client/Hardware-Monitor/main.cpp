#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#include "resource.h"
#include "Client.h"
#include "aida64.h"
#include "utility.h"
#include "json_spirit/json_spirit.h"

#include <string>
#include <fstream>
#include <sstream>

HWND g_hWnd;
HMENU g_hMenu;
NOTIFYICONDATA g_notifyIconData;

LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
void initTrayIcon();
std::string readServerIP();
void websocket_onFailedOrDisconnected();
void CALLBACK timerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
	const char *szClassName = "Hardware-Monitor-AIDA64";

	if (OpenMutexA(MUTEX_ALL_ACCESS, TRUE, szClassName))
	{
		MessageBoxA(0, "Only one instance of this program can be executed at once", "Error", 0);
		return 0;
	}

	CreateMutexA(NULL, TRUE, szClassName);

	auto ip = readServerIP();
	if (!ip.length())
	{
		MessageBoxA(0, "The configuration file 'server_ip.txt' is invalid", "Error", 0);
		return 0;
	}

	websocket_registerCallback(callback_on_fail, (uint32_t) &websocket_onFailedOrDisconnected);
	websocket_registerCallback(callback_on_disconnect, (uint32_t) &websocket_onFailedOrDisconnected);
	
	if (!websocket_connect(ip.c_str()))
	{
		MessageBoxA(0, "The connection can not be established", "Error", 0);
		return 0;
	}

	MSG messages;
	WNDCLASSEX wincl;

	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = wndProc; 
	wincl.style = CS_DBLCLKS;
	wincl.cbSize = sizeof (WNDCLASSEX);

	wincl.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wincl.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground = (HBRUSH) (CreateSolidBrush(RGB(255, 255, 255)));

	if (!RegisterClassEx(&wincl))
		return 0;

	g_hWnd = CreateWindowEx(0, szClassName, "Hardware-Monitor", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, HWND_DESKTOP, NULL, hThisInstance, NULL);
	
	initTrayIcon();

	ShowWindow(g_hWnd, nCmdShow);
	ShowWindow(g_hWnd, SW_HIDE);

	SetTimer(0, 0, 500, timerProc);

	while (GetMessage(&messages, NULL, 0, 0))
	{
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}

	websocket_disconnect();
	return messages.wParam;
}


LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ACTIVATE:
		Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);
		break;
	case WM_CREATE:
		g_hMenu = CreatePopupMenu();
		AppendMenu(g_hMenu, MF_STRING, ID_TRAY_EXIT, TEXT("Exit Hardware-Monitor"));
		break;

	case WM_SYSCOMMAND:
		switch (wParam & 0xFFF0)
		{
		case SC_MINIMIZE:
		case SC_CLOSE:
			return 0;
			break;
		}
		break;
	case WM_SYSICON:
	{
		if (lParam == WM_RBUTTONDOWN)
		{
			POINT curPoint;
			GetCursorPos(&curPoint);
			
			UINT clicked = TrackPopupMenu(g_hMenu, TPM_RETURNCMD | TPM_NONOTIFY, curPoint.x, curPoint.y, 0, hwnd, NULL);
			if (clicked == ID_TRAY_EXIT)
			{
				Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
				PostQuitMessage(0);
			}
		}
		break;
	}
	
	case WM_NCHITTEST:
	{
		UINT uHitTest = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
		if (uHitTest == HTCLIENT)
			return HTCAPTION;
		else
			return uHitTest;
	}

	case WM_CLOSE:
		return 0;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}


void initTrayIcon()
{
	const char *hover = "Hardware-Monitor";

	memset(&g_notifyIconData, 0, sizeof(NOTIFYICONDATA));

	g_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	g_notifyIconData.hWnd = g_hWnd;
	g_notifyIconData.uID = ID_TRAY_APP_ICON;
	g_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	g_notifyIconData.uCallbackMessage = WM_SYSICON;
	g_notifyIconData.hIcon = (HICON) LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	strncpy(g_notifyIconData.szTip, hover, strlen(hover));
}

std::string readServerIP()
{
	std::fstream fs("server_ip.txt", std::fstream::in);
	if (!fs)
		return{};

	std::stringstream ss;
	ss << fs.rdbuf();

	return ss.str();
}

void websocket_onFailedOrDisconnected()
{
	Sleep(500);
	websocket_connect(readServerIP().c_str());
}

void CALLBACK timerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	execute_safe([&]()
	{
		aida64::API api;
		if (!api.refreshData())
			return;

		using namespace json_spirit;

		TCHAR computerName[MAX_COMPUTERNAME_LENGTH + 1] = { 0 };
		DWORD dwSize = MAX_COMPUTERNAME_LENGTH;
		GetComputerName(computerName, &dwSize);

		Object mainJsonObject;
		Array sensorValues;
		for (const auto& i : api.data())
		{
			Object sensorValue;
			sensorValue.push_back(Pair("category", i.category));
			sensorValue.push_back(Pair("id", i.id));
			sensorValue.push_back(Pair("label", i.label));
			sensorValue.push_back(Pair("value", i.value));
			sensorValues.push_back(sensorValue);
		}

		mainJsonObject.push_back(Pair("computerName", computerName));
		mainJsonObject.push_back(Pair("sensorValues", sensorValues));

		std::string data = write_formatted(Value(mainJsonObject));

		websocket_send(data.c_str(), data.length(), false);
	});
}