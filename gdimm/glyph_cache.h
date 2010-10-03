#pragma once

#include "helper_def.h"
#include "helper_func.h"
#include "type_enum.h"

using namespace std;

class gdimm_glyph_cache
{
	FT_Glyph _empty_glyph_template;
	DWORD _sql_stmt_tls_index;

	sqlite3_stmt *get_sql_stmt(SQL_STMT_TYPE stmt_type) const;
	bool set_sql_stmt(SQL_STMT_TYPE stmt_type, sqlite3_stmt *stmt) const;

public:
	gdimm_glyph_cache();
	~gdimm_glyph_cache();

	bool initialize();
	const FT_BitmapGlyph load_bmp_glyph(uint64_t font_trait, FT_UInt index, bool is_glyph_index) const;
	bool store_bmp_glyph(uint64_t font_trait, FT_UInt index, bool is_glyph_index, const FT_BitmapGlyph bmp_glyph) const;
	bool lookup_glyph_run(uint64_t font_trait, uint64_t str_hash, glyph_run &a_glyph_run) const;
	bool store_glyph_run(uint64_t font_trait, uint64_t str_hash, const glyph_run &a_glyph_run) const;
};