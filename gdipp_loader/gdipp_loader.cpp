#include "stdafx.h"
#include "gdipp_lib/helper.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	BOOL b_ret;
	NTSTATUS eh_ret;

	if (lpCmdLine == NULL || wcslen(lpCmdLine) == 0)
	{
		MessageBox(NULL, L"Drag an exe file to me and I will load it with gdimm.dll.", L"gdipp Loader", MB_OK | MB_ICONINFORMATION);
		return EXIT_SUCCESS;
	}

	int argc;
	LPWSTR *argv = CommandLineToArgvW(lpCmdLine, &argc);
	assert(argv != NULL);

	wchar_t working_dir[MAX_PATH];
	wcsncpy_s(working_dir, argv[0], MAX_PATH);
	b_ret = PathRemoveFileSpecW(working_dir);
	assert(b_ret);

	STARTUPINFO si = {};
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi;

	b_ret = CreateProcessW(argv[0], lpCmdLine, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, working_dir, &si, &pi);
	LocalFree(argv);

	if (b_ret)
	{
#ifdef _M_X64
		const wchar_t *client_name = L"gdipp_client_64.dll";
#else
		const wchar_t *client_name = L"gdipp_client_32.dll";
#endif

		wchar_t _client_path[MAX_PATH];
		b_ret = gdipp::get_dir_file_path(NULL, client_name, _client_path);
		assert(b_ret);

#ifdef _M_X64
		eh_ret = RhInjectLibrary(pi.dwProcessId, pi.dwThreadId, EASYHOOK_INJECT_DEFAULT, NULL, _client_path, NULL, 0);
#else
		eh_ret = RhInjectLibrary(pi.dwProcessId, pi.dwThreadId, EASYHOOK_INJECT_DEFAULT, _client_path, NULL, NULL, 0);
#endif

		if (eh_ret == 0)
		{
			WaitForSingleObject(pi.hProcess, INFINITE);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			return EXIT_SUCCESS;
		}
		else
		{
			b_ret = TerminateProcess(pi.hProcess, 0);
			assert(b_ret);

			std::wstring error_msg = L"Unable to inject gdimm.dll to the new process";

			// STATUS_WOW_ASSERTION
			if (eh_ret == 0xC0009898L)
				error_msg += L" due to different bitness. Try the other gdipp Loader";

			error_msg += L".";

			MessageBoxW(NULL, error_msg.c_str(), L"gdipp Loader", MB_OK | MB_ICONERROR);
			return EXIT_FAILURE;
		}
	}
	else
	{
		MessageBoxW(NULL, L"Unable to create the target process.", L"gdipp Loader", MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}
}
