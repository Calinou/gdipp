#pragma once

#include "MurmurHash2_64.h"
#include "helper_def.h"
#include "helper_func.h"
#include "type_enum.h"

using namespace std;

class gdimm_glyph_cache
{
	FT_Glyph _empty_glyph_template;
	DWORD _sql_stmt_tls_index;
	map<uint64_t, FT_BitmapGlyph> _bmp_glyph_cache;

	sqlite3_stmt *get_sql_stmt(SQL_STMT_TYPE stmt_type) const;
	bool set_sql_stmt(SQL_STMT_TYPE stmt_type, sqlite3_stmt *stmt) const;

public:
	gdimm_glyph_cache();
	~gdimm_glyph_cache();

	bool initialize();
	const FT_BitmapGlyph load_bmp_glyph(unsigned int font_trait, FT_UInt index, bool is_glyph_index);
	bool store_bmp_glyph(unsigned int font_trait, FT_UInt index, bool is_glyph_index, const FT_BitmapGlyph bmp_glyph);
	bool lookup_glyph_run(unsigned int font_trait, uint64_t str_hash, glyph_run &a_glyph_run) const;
	bool store_glyph_run(unsigned int font_trait, uint64_t str_hash, const glyph_run &a_glyph_run) const;
};