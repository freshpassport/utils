// casync1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "afxsock.h"
#include "casync1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		CString strHello;
		strHello.LoadString(IDS_HELLO);
		cout << (LPCTSTR)strHello << endl;
	}

	char buff[128] = "aaaaaaaaaaaaaaaaaaaaaaaa";
	AfxSocketInit();
	CSocket sock;
	sock.Create();
	sock.Connect("127.0.0.1", 1234);
	sock.Send(buff, 128);
	sock.Close();

	return nRetCode;
}


