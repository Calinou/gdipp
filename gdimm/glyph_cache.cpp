#include "stdafx.h"
#include "glyph_cache.h"
#include "gdimm.h"

gdimm_glyph_cache::gdimm_glyph_cache()
	: _empty_glyph_template(NULL)
{
}

size_t gdimm_glyph_cache::serialize(const FT_BitmapGlyph bmp_glyph, char *buffer)
{
	const int bitmap_buffer_size = bmp_glyph->bitmap.pitch * bmp_glyph->bitmap.rows;

	if (buffer == NULL)
		return sizeof(bmp_glyph->root.advance) + sizeof(bmp_glyph->left) + sizeof(bmp_glyph->top) + sizeof(bmp_glyph->bitmap) + bitmap_buffer_size;
	else
	{
		const char *orig_buffer_ptr = buffer;

		memcpy(buffer, &bmp_glyph->root.advance, sizeof(bmp_glyph->root.advance));
		buffer += sizeof(bmp_glyph->root.advance);

		memcpy(buffer, &bmp_glyph->left, sizeof(bmp_glyph->left));
		buffer += sizeof(bmp_glyph->left);

		memcpy(buffer, &bmp_glyph->top, sizeof(bmp_glyph->top));
		buffer += sizeof(bmp_glyph->top);

		memcpy(buffer, &bmp_glyph->bitmap, sizeof(bmp_glyph->bitmap));
		buffer += sizeof(bmp_glyph->bitmap);

		memcpy(buffer, bmp_glyph->bitmap.buffer, bitmap_buffer_size);
		buffer += bitmap_buffer_size;
		
		return buffer - orig_buffer_ptr;
	}
}

size_t gdimm_glyph_cache::deserialize(const char *buffer, int buffer_size, FT_BitmapGlyph bmp_glyph) const
{
	const char *orig_buffer_ptr = buffer;
	
	if (buffer != NULL)
	{
		bmp_glyph->root.advance = *reinterpret_cast<const FT_Vector *>(buffer);
		buffer += sizeof(bmp_glyph->root.advance);

		bmp_glyph->left = *reinterpret_cast<const FT_Int *>(buffer);
		buffer += sizeof(bmp_glyph->left);

		bmp_glyph->top = *reinterpret_cast<const FT_Int *>(buffer);
		buffer += sizeof(bmp_glyph->top);

		bmp_glyph->bitmap = *reinterpret_cast<const FT_Bitmap *>(buffer);
		buffer += sizeof(bmp_glyph->bitmap);

		const int bitmap_buffer_size = bmp_glyph->bitmap.pitch * bmp_glyph->bitmap.rows;
		assert(buffer_size - bitmap_buffer_size == buffer - orig_buffer_ptr);
		bmp_glyph->bitmap.buffer = new unsigned char[bitmap_buffer_size];
		memcpy(bmp_glyph->bitmap.buffer, buffer, bitmap_buffer_size);
		buffer += bitmap_buffer_size;
	}

	return buffer - orig_buffer_ptr;
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

	return true;
}

const FT_BitmapGlyph gdimm_glyph_cache::load_bmp_glyph(uint64_t font_trait, FT_UInt index, bool is_glyph_index)
{
	int i_ret;
	char *curr_sql;
	sqlite3_stmt *curr_stmt;
	FT_BitmapGlyph bmp_glyph = NULL;

	if (glyph_cache_db != NULL)
	{
		const FT_Int internal_index = index * (is_glyph_index ? 1 : -1);
		curr_sql = sqlite3_mprintf("SELECT glyph_data FROM 'gdipp_glyph' WHERE font_trait = %llu AND char_index = %d AND glyph_type = %d", font_trait, internal_index, 1);

		i_ret = sqlite3_prepare_v2(glyph_cache_db, curr_sql, -1, &curr_stmt, NULL);
		if (i_ret != SQLITE_OK)
			return bmp_glyph;

		i_ret = sqlite3_step(curr_stmt);
		
		if (i_ret == SQLITE_ROW)
		{
			bmp_glyph = new FT_BitmapGlyphRec;
			bmp_glyph->root = *_empty_glyph_template;
			const char *glyph_data_buffer = reinterpret_cast<const char *>(sqlite3_column_blob(curr_stmt, 0));
			deserialize(glyph_data_buffer, sqlite3_column_bytes(curr_stmt, 0), bmp_glyph);
		}

		i_ret = sqlite3_finalize(curr_stmt);
		assert(i_ret == SQLITE_OK);

		sqlite3_free(curr_sql);
	}

	return bmp_glyph;
}

bool gdimm_glyph_cache::store_bmp_glyph(uint64_t font_trait, FT_UInt index, bool is_glyph_index, const FT_BitmapGlyph bmp_glyph)
{
	int i_ret;
	char *curr_sql;
	sqlite3_stmt *curr_stmt;

	if (glyph_cache_db == NULL)
		return false;

	/*const FT_Int internal_index = index * (is_glyph_index ? 1 : -1);
	const size_t glyph_id_buffer_len = sizeof(font_trait) + sizeof(internal_index);
	char *glyph_id_buffer = new char[glyph_id_buffer_len];
	*reinterpret_cast<uint64_t *>(glyph_id_buffer) = font_trait;
	*reinterpret_cast<FT_Int *>(glyph_id_buffer + sizeof(font_trait)) = internal_index;

#ifdef _M_X64
	const uint64_t glyph_id = MurmurHash64A(glyph_id_buffer, glyph_id_buffer_len, 1);
#else
	const uint64_t glyph_id = MurmurHash64B(glyph_id_buffer, glyph_id_buffer_len, 1);
#endif // _M_X64

	delete [] glyph_id_buffer;*/

	curr_sql = sqlite3_mprintf("CREATE TABLE IF NOT EXISTS '%llu' ('char_index' INTEGER NOT NULL, 'glyph_type' INTEGER NOT NULL, 'glyph_data' BLOB NOT NULL, PRIMARY KEY ('char_index', 'glyph_type'))", font_trait);
	assert(curr_sql != NULL);
	i_ret = sqlite3_exec(glyph_cache_db, curr_sql, NULL, NULL, NULL);
	assert(i_ret == SQLITE_OK);
	sqlite3_free(curr_sql);

	const size_t glyph_data_size = serialize(bmp_glyph, NULL);
	char *glyph_data_buffer = new char[glyph_data_size];
	serialize(bmp_glyph, glyph_data_buffer);

	const FT_Int internal_index = index * (is_glyph_index ? 1 : -1);
	curr_sql = sqlite3_mprintf("INSERT INTO '%llu' VALUES (%d, %d, ?)", font_trait, internal_index, 1);
	assert(curr_sql != NULL);
	i_ret = sqlite3_prepare_v2(glyph_cache_db, curr_sql, -1, &curr_stmt, NULL);
	assert(i_ret == SQLITE_OK);
	
	i_ret = sqlite3_bind_blob(curr_stmt, 1, glyph_data_buffer, glyph_data_size, NULL);
	assert(i_ret == SQLITE_OK);

	i_ret = sqlite3_step(curr_stmt);
	//assert(i_ret == SQLITE_DONE);

	i_ret = sqlite3_finalize(curr_stmt);
	//assert(i_ret == SQLITE_OK);

	sqlite3_free(curr_sql);

	delete[] glyph_data_buffer;

	/*const size_t glyph_data_size = serialize(bmp_glyph, NULL);
	char *glyph_data_buffer = new char[glyph_data_size];
	serialize(bmp_glyph, glyph_data_buffer);

	FT_Int internal_index = index * (is_glyph_index ? 1 : -1);
	Dbt key(&font_trait, sizeof(font_trait)), value(glyph_data_buffer, glyph_data_size);
	db->put(NULL, &key, &value, DB_NOOVERWRITE);
	delete[] glyph_data_buffer;*/

	return true;
}

bool gdimm_glyph_cache::lookup_glyph_run(uint64_t font_trait, uint64_t str_hash, glyph_run &a_glyph_run)
{
	map<uint64_t, hash_to_run_map>::const_iterator trait_iter = _glyph_run_store.find(font_trait);
	if (trait_iter == _glyph_run_store.end())
		return false;

	hash_to_run_map::const_iterator hash_iter = trait_iter->second.find(str_hash);
	if (hash_iter == trait_iter->second.end())
		return false;

	a_glyph_run = hash_iter->second;

	return true;
}

bool gdimm_glyph_cache::store_glyph_run(uint64_t font_trait, uint64_t str_hash, const glyph_run &a_glyph_run)
{
	const pair<map<uint64_t, hash_to_run_map>::iterator, bool> trait_insert_ret = _glyph_run_store.insert(pair<uint64_t, hash_to_run_map>(font_trait, hash_to_run_map()));
	const pair<hash_to_run_map::const_iterator, bool> hash_insert_ret = trait_insert_ret.first->second.insert(pair<uint64_t, glyph_run>(str_hash, a_glyph_run));

	return hash_insert_ret.second;
}

bool gdimm_glyph_cache::erase_font_trait(uint64_t font_trait)
{
	// erase the glyph run from its store
	// then free and erase the contained glyphs
	
	const size_t del_count = _glyph_run_store.erase(font_trait);
	if (del_count == 0)
		return false;

	map<uint64_t, index_to_glyph_map>::iterator bmp_trait_iter = _glyph_store.find(font_trait);
	if (bmp_trait_iter != _glyph_store.end())
	{
		for (index_to_glyph_map::const_iterator index_iter = bmp_trait_iter->second.begin(); index_iter != bmp_trait_iter->second.end(); index_iter++)
			FT_Done_Glyph(index_iter->second);

		_glyph_store.erase(font_trait);
	}

	return true;
}