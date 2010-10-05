#include "stdafx.h"
#include "gdimm.h"
#include "freetype.h"
#include "lock.h"
#include <gdipp_common.h>

HMODULE h_self = NULL;
bool os_support_directwrite;

gdimm_font_link font_link_instance;
gdimm_font_store font_store_instance;
gdimm_gamma gamma_instance;
gdimm_glyph_cache glyph_cache_instance;
gdimm_hook hook_instance;
gdimm_setting_cache setting_cache_instance;

sqlite3 *glyph_cache_db = NULL;
sqlite3 *glyph_run_cache_db = NULL;

bool set_db_pragma(sqlite3 *db)
{
	int i_ret;

	i_ret = sqlite3_exec(glyph_cache_db, "PRAGMA default_cache_size = 8192", NULL, NULL, NULL);
	if (i_ret != SQLITE_OK)
		return false;

	i_ret = sqlite3_exec(glyph_cache_db, "PRAGMA journal_mode = OFF", NULL, NULL, NULL);
	if (i_ret != SQLITE_OK)
		return false;

	i_ret = sqlite3_exec(glyph_cache_db, "PRAGMA legacy_file_format = OFF", NULL, NULL, NULL);
	if (i_ret != SQLITE_OK)
		return false;

	i_ret = sqlite3_exec(glyph_cache_db, "PRAGMA page_size = 4096", NULL, NULL, NULL);
	if (i_ret != SQLITE_OK)
		return false;

	i_ret = sqlite3_exec(glyph_cache_db, "PRAGMA synchronous = OFF", NULL, NULL, NULL);
	if (i_ret != SQLITE_OK)
		return false;

	i_ret = sqlite3_exec(glyph_cache_db, "PRAGMA temp_store = MEMORY", NULL, NULL, NULL);
	if (i_ret != SQLITE_OK)
		return false;

	return true;
}

bool initialize_cache_db()
{
	bool b_ret;
	int i_ret;

	if (glyph_cache_db == NULL && glyph_run_cache_db == NULL)
	{
		wchar_t base_db_path[MAX_PATH], curr_db_path[MAX_PATH];
		if (!gdipp_get_dir_file_path(h_self, L"cache", base_db_path))
			return false;
		CreateDirectory(base_db_path, NULL);

		i_ret = sqlite3_initialize();
		assert(i_ret == SQLITE_OK);

		wcscpy_s(curr_db_path, base_db_path);
		PathAppendW(curr_db_path, L"glyph.sqlite");
		i_ret = sqlite3_open16(curr_db_path, &glyph_cache_db);
		if (i_ret == SQLITE_OK)
		{
			b_ret = set_db_pragma(glyph_cache_db);
			assert(b_ret);

			i_ret = sqlite3_exec(glyph_cache_db, "CREATE TABLE IF NOT EXISTS bitmap_glyph ('char_id' INTEGER PRIMARY KEY NOT NULL UNIQUE, 'advance_x' INTEGER, 'advance_y' INTEGER, 'left' INTEGER, 'top' INTEGER, 'rows' INTEGER, 'width' INTEGER, 'pitch' INTEGER, 'buffer' BLOB, 'num_grays' INTEGER, 'pixel_mode' INTEGER)", NULL, NULL, NULL);
			assert(i_ret == SQLITE_OK);
		}
		else
			glyph_cache_db = NULL;
		
		/*wcscpy_s(curr_db_path, base_db_path);
		PathAppendW(curr_db_path, L"glyph_run.sqlite");
		i_ret = sqlite3_open16(curr_db_path, &glyph_run_cache_db);
		if (i_ret == SQLITE_OK)
		{
			i_ret = sqlite3_exec(glyph_run_cache_db, "PRAGMA journal_mode = OFF", NULL, NULL, NULL);
			assert(i_ret == SQLITE_OK);
			i_ret = sqlite3_exec(glyph_run_cache_db, "PRAGMA synchronous = OFF", NULL, NULL, NULL);
			assert(i_ret == SQLITE_OK);
			i_ret = sqlite3_exec(glyph_run_cache_db, "PRAGMA temp_store = MEMORY", NULL, NULL, NULL);
			assert(i_ret == SQLITE_OK);
		}
		else
			glyph_run_cache_db = NULL;*/

		if (glyph_cache_db != NULL || glyph_run_cache_db != NULL)
			return true;
		else
			sqlite3_shutdown();
	}

	return false;
}

bool destroy_cache_db()
{
	int i_ret;
	bool b_ret = true;

	if (glyph_cache_db != NULL)
	{
		i_ret = sqlite3_close(glyph_cache_db);
		b_ret &= (i_ret == SQLITE_OK);
	}

	if (glyph_run_cache_db != NULL)
	{
		i_ret = sqlite3_close(glyph_run_cache_db);
		b_ret &= (i_ret == SQLITE_OK);
	}

	i_ret = sqlite3_shutdown();
	return b_ret & (i_ret == SQLITE_OK);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	BOOL b_ret;

	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			h_self = hModule;
			gdipp_register_minidump_module(hModule);

			// get setting file path
			wchar_t setting_path[MAX_PATH];
			if (!gdipp_get_dir_file_path(hModule, L"gdipp_setting.xml", setting_path))
				return FALSE;

			gdipp_init_setting();

			// return false if setting file does not exist
			gdipp_load_setting(setting_path);

			if (gdipp_is_process_excluded(NULL))
				return FALSE;

			OSVERSIONINFO ver_info = {sizeof(OSVERSIONINFO)};
			b_ret = GetVersionEx(&ver_info);
			if (!b_ret)
				return FALSE;
			os_support_directwrite = (ver_info.dwMajorVersion >= 6);

			initialize_cache_db();
			gdimm_lock::initialize();
			initialize_freetype();
			glyph_cache_instance.initialize();

			if (!hook_instance.hook())
				return FALSE;

			break;
		}
		case DLL_PROCESS_DETACH:
			hook_instance.unhook();
			destroy_freetype();
			gdimm_lock::finalize();
			destroy_cache_db();

			break;
	}

	return TRUE;
}
