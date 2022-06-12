#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)
#define WM_SOCKET WM_USER+1

BOOL CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_SOCKET)
	{
		int ret;
		if (ret = WSAGETSELECTERROR(lParam))
		{
			printf("Co loi xay ra: %d!!!\n", ret);
			closesocket(wParam);
			return FALSE;
		}
		if (WSAGETSELECTEVENT(lParam) == FD_READ)
		{
			char buf[256];
			ret = recv(wParam, buf, sizeof(buf), 0);
			if (ret <= 0)
			{
			}
			buf[ret] = 0;
			printf("Du lieu nhan duoc: %s\n", buf);
		}
		if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
		{
			printf("Ket noi bi dong");
			closesocket(wParam);
		}
	}
}
int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(8000);

	system("pause");
	int ret = connect(client, (SOCKADDR*)&addr, sizeof(addr));

	WNDCLASS wndclass;
	const CHAR* providerClass = "AsyncSelect";
	HWND hwnd;
	wndclass.style = 0;
	wndclass.lpfnWndProc = (WNDPROC)WinProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = NULL;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = (LPCWSTR)providerClass;

	if (RegisterClass(&wndclass) == 0)
		return NULL;
	// Create a window
	if ((hwnd = CreateWindow((LPCWSTR)providerClass, L"", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, NULL, NULL)) == NULL)
		return NULL;

	WSAAsyncSelect(client, hwnd, WM_SOCKET, FD_READ | FD_CLOSE);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}