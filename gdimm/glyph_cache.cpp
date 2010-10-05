#include "stdafx.h"
#include "glyph_cache.h"
#include "gdimm.h"

gdimm_glyph_cache::gdimm_glyph_cache()
	: _empty_glyph_template(NULL),
	_sql_stmt_tls_index(0)
{
}

gdimm_glyph_cache::~gdimm_glyph_cache()
{
	int i_ret;
	
	if (_sql_stmt_tls_index != 0)
	{
		sqlite3_stmt **sql_stmts = reinterpret_cast<sqlite3_stmt **>(TlsGetValue(_sql_stmt_tls_index));
		if (sql_stmts != NULL)
		{
			for (int i = 0; i < _SQL_STMT_COUNT_; i++)
			{
				if (sql_stmts[i] != NULL)
				{
					i_ret = sqlite3_finalize(sql_stmts[i]);
					assert(i_ret == SQLITE_OK);
				}
			}
		}

		free(sql_stmts);
		free_tls_index(_sql_stmt_tls_index);
	}
}

sqlite3_stmt *gdimm_glyph_cache::get_sql_stmt(SQL_STMT_TYPE stmt_type) const
{
	sqlite3_stmt **sql_stmts = reinterpret_cast<sqlite3_stmt **>(TlsGetValue(_sql_stmt_tls_index));

	if (sql_stmts == NULL)
		return NULL;
	else
		return sql_stmts[stmt_type];
}

bool gdimm_glyph_cache::set_sql_stmt(SQL_STMT_TYPE stmt_type, sqlite3_stmt *stmt) const
{
	BOOL b_ret;
	sqlite3_stmt **sql_stmts = reinterpret_cast<sqlite3_stmt **>(TlsGetValue(_sql_stmt_tls_index));

	if (sql_stmts == NULL)
	{
		sql_stmts = reinterpret_cast<sqlite3_stmt **>(calloc(_SQL_STMT_COUNT_, sizeof(sqlite3_stmt *)));
		b_ret = TlsSetValue(_sql_stmt_tls_index, sql_stmts);
		if (!b_ret)
		{
			delete[] sql_stmts;
			return false;
		}
	}

	sql_stmts[stmt_type] = stmt;
	return true;
}

bool gdimm_glyph_cache::initialize()
{
	if (_empty_glyph_template == NULL)
	{
		_empty_glyph_template = make_empty_outline_glyph();
		assert(_empty_glyph_template != NULL);

		_empty_glyph_template = make_empty_bitmap_glyph(_empty_glyph_template);
		assert(_empty_glyph_template != NULL);
	}

	_sql_stmt_tls_index = create_tls_index();

	return true;
}

const FT_BitmapGlyph gdimm_glyph_cache::load_bmp_glyph(unsigned int font_trait, FT_UInt index, bool is_glyph_index)
{
	bool b_ret;
	int i_ret;
	FT_BitmapGlyph bmp_glyph = NULL;

	const uint64_t char_id = (((static_cast<uint64_t>(font_trait) << 1) | static_cast<char>(is_glyph_index)) << 31) | index;
	map<uint64_t, FT_BitmapGlyph>::const_iterator glyph_iter = _bmp_glyph_cache.find(char_id);
	if (glyph_iter != _bmp_glyph_cache.end())
		return glyph_iter->second;

	if (glyph_cache_db != NULL)
	{
		sqlite3_stmt *select_glyph_stmt = get_sql_stmt(SELECT_GLYPH_STMT);
		if (select_glyph_stmt == NULL)
		{
			b_ret = false;

			i_ret = sqlite3_prepare16_v2(glyph_cache_db, L"SELECT advance_x, advance_y, left, top, rows, width, pitch, buffer, num_grays, pixel_mode FROM bitmap_glyph WHERE char_id = ?", -1, &select_glyph_stmt, NULL);
			if (i_ret == SQLITE_OK)
				b_ret = set_sql_stmt(SELECT_GLYPH_STMT, select_glyph_stmt);

			if (!b_ret)
				return NULL;
		}


		i_ret = sqlite3_bind_int64(select_glyph_stmt, 1, char_id);
		assert(i_ret == SQLITE_OK);

		i_ret = sqlite3_step(select_glyph_stmt);
		if (i_ret == SQLITE_ROW)
		{
			bmp_glyph = reinterpret_cast<FT_BitmapGlyph>(calloc(1, sizeof(FT_BitmapGlyphRec)));
			bmp_glyph->root = *_empty_glyph_template;
			bmp_glyph->root.advance.x = sqlite3_column_int(select_glyph_stmt, 0);
			bmp_glyph->root.advance.y = sqlite3_column_int(select_glyph_stmt, 1);
			bmp_glyph->left = sqlite3_column_int(select_glyph_stmt, 2);
			bmp_glyph->top = sqlite3_column_int(select_glyph_stmt, 3);
			bmp_glyph->bitmap.rows = sqlite3_column_int(select_glyph_stmt, 4);
			bmp_glyph->bitmap.width = sqlite3_column_int(select_glyph_stmt, 5);
			bmp_glyph->bitmap.pitch = sqlite3_column_int(select_glyph_stmt, 6);
			const int glyph_buffer_size = sqlite3_column_bytes(select_glyph_stmt, 7);
			bmp_glyph->bitmap.buffer = reinterpret_cast<unsigned char *>(malloc(glyph_buffer_size));
			memcpy(bmp_glyph->bitmap.buffer, sqlite3_column_blob(select_glyph_stmt, 7), glyph_buffer_size);
			bmp_glyph->bitmap.num_grays = sqlite3_column_int(select_glyph_stmt, 8);
			bmp_glyph->bitmap.pixel_mode = sqlite3_column_int(select_glyph_stmt, 9);
			//bmp_glyph->bitmap.palette_mode = 27;
		}

		i_ret = sqlite3_reset(select_glyph_stmt);
		assert(i_ret == SQLITE_OK);

		_bmp_glyph_cache[char_id] = bmp_glyph;
	}

	return bmp_glyph;
}

bool gdimm_glyph_cache::store_bmp_glyph(unsigned int font_trait, FT_UInt index, bool is_glyph_index, const FT_BitmapGlyph bmp_glyph)
{
	bool b_ret;
	int i_ret;

	if (glyph_cache_db == NULL)
		return false;

	sqlite3_stmt *insert_glyph_stmt = get_sql_stmt(INSERT_GLYPH_STMT);
	if (insert_glyph_stmt == NULL)
	{
		b_ret = false;

		i_ret = sqlite3_prepare16_v2(glyph_cache_db, L"INSERT INTO bitmap_glyph VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &insert_glyph_stmt, NULL);
		if (i_ret == SQLITE_OK)
			b_ret = set_sql_stmt(INSERT_GLYPH_STMT, insert_glyph_stmt);

		if (!b_ret)
			return NULL;
	}

	const uint64_t char_id = (((static_cast<uint64_t>(font_trait) << 1) | static_cast<char>(is_glyph_index)) << 31) | index;

	i_ret = sqlite3_bind_int64(insert_glyph_stmt, 1, char_id);
	assert(i_ret == SQLITE_OK);

	i_ret = sqlite3_bind_int(insert_glyph_stmt, 2, bmp_glyph->root.advance.x);
	assert(i_ret == SQLITE_OK);

	i_ret = sqlite3_bind_int(insert_glyph_stmt, 3, bmp_glyph->root.advance.y);
	assert(i_ret == SQLITE_OK);

	i_ret = sqlite3_bind_int(insert_glyph_stmt, 4, bmp_glyph->left);
	assert(i_ret == SQLITE_OK);

	i_ret = sqlite3_bind_int(insert_glyph_stmt, 5, bmp_glyph->top);
	assert(i_ret == SQLITE_OK);

	i_ret = sqlite3_bind_int(insert_glyph_stmt, 6, bmp_glyph->bitmap.rows);
	assert(i_ret == SQLITE_OK);

	i_ret = sqlite3_bind_int(insert_glyph_stmt, 7, bmp_glyph->bitmap.width);
	assert(i_ret == SQLITE_OK);

	i_ret = sqlite3_bind_int(insert_glyph_stmt, 8, bmp_glyph->bitmap.pitch);
	assert(i_ret == SQLITE_OK);

	i_ret = sqlite3_bind_blob(insert_glyph_stmt, 9, bmp_glyph->bitmap.buffer, bmp_glyph->bitmap.pitch * bmp_glyph->bitmap.rows, SQLITE_TRANSIENT);
	assert(i_ret == SQLITE_OK);

	i_ret = sqlite3_bind_int(insert_glyph_stmt, 10, bmp_glyph->bitmap.num_grays);
	assert(i_ret == SQLITE_OK);

	i_ret = sqlite3_bind_int(insert_glyph_stmt, 11, bmp_glyph->bitmap.pixel_mode);
	assert(i_ret == SQLITE_OK);

	i_ret = sqlite3_step(insert_glyph_stmt);
	assert(i_ret == SQLITE_DONE);

	i_ret = sqlite3_reset(insert_glyph_stmt);
	assert(i_ret == SQLITE_OK);

	_bmp_glyph_cache[char_id] = bmp_glyph;

	return true;
}

bool gdimm_glyph_cache::lookup_glyph_run(unsigned int font_trait, uint64_t str_hash, glyph_run &a_glyph_run) const
{
	/*map<uint64_t, hash_to_run_map>::const_iterator trait_iter = _glyph_run_store.find(font_trait);
	if (trait_iter == _glyph_run_store.end())
		return false;

	hash_to_run_map::const_iterator hash_iter = trait_iter->second.find(str_hash);
	if (hash_iter == trait_iter->second.end())
		return false;

	a_glyph_run = hash_iter->second;*/

	return true;
}

bool gdimm_glyph_cache::store_glyph_run(unsigned int font_trait, uint64_t str_hash, const glyph_run &a_glyph_run) const
{
	/*const pair<map<uint64_t, hash_to_run_map>::iterator, bool> trait_insert_ret = _glyph_run_store.insert(pair<uint64_t, hash_to_run_map>(font_trait, hash_to_run_map()));
	const pair<hash_to_run_map::const_iterator, bool> hash_insert_ret = trait_insert_ret.first->second.insert(pair<uint64_t, glyph_run>(str_hash, a_glyph_run));

	return hash_insert_ret.second;*/

	return true;
}