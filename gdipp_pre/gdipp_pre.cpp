// gdipp_pre.cpp : main source file for gdipp_pre.exe
//

#include "stdafx.h"
#include "gdipp_pre.h"
#include "gdipp_lib/helper.h"
#include "gdipp_pre/resource.h"
#include "gdipp_pre/MainDlg.h"

CAppModule _Module;

HINSTANCE h_instance;
wchar_t config_path[MAX_PATH];
wchar_t *preview_text = NULL;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainDlg dlgMain;

	if (dlgMain.Create(NULL) == NULL)
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}

	dlgMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	BOOL b_ret;

	h_instance = hInstance;

	b_ret = gdipp::get_dir_file_path(hInstance, L"gdipp_config.xml", config_path);
	assert(b_ret);

	wchar_t preview_text_path[MAX_PATH];
	b_ret = gdipp::get_dir_file_path(hInstance, L"gdipp_preview.txt", preview_text_path);
	assert(b_ret);

	FILE *f;
	errno_t err = _wfopen_s(&f, preview_text_path, L"r, ccs=UNICODE");
	if (err == 0)
	{
		fseek(f, 0, SEEK_END);
		const long text_len = ftell(f);
		rewind(f);

		preview_text = new wchar_t[text_len + 1];
		const size_t bytes_read = fread(preview_text, sizeof(wchar_t), text_len, f);
		preview_text[bytes_read] = L'\0';

		fclose(f);
	}

	HMODULE h_gdimm = NULL;
	WCHAR gdimm_path[MAX_PATH];

#ifdef _M_X64
	b_ret = gdipp::get_dir_file_path(NULL, L"gdimm_64.dll", gdimm_path);
#else
	b_ret = gdipp::get_dir_file_path(NULL, L"gdimm_32.dll", gdimm_path);
#endif // _M_X64

	// TODO: incomplete code
	h_gdimm = LoadLibraryW(gdimm_path);
	if (h_gdimm == NULL)
		;

	// If you are running on NT 4.0 or higher you can use the following call instead to
	// make the EXE free threaded. This means that calls come in on a random RPC thread.
	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	if (h_gdimm != NULL)
		FreeLibrary(h_gdimm);

	if (preview_text != NULL)
		delete[] preview_text;

	return nRet;
}
