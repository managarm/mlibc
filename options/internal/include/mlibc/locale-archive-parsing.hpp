#pragma once

#include <stddef.h>
#include <stdint.h>

#pragma mark - locale-archive format description

namespace LocaleArchive {

constexpr uint32_t HEADER_MAGIC = 0xde020109;

struct Header {
	uint32_t magic;
	/* Serial number. */
	uint32_t serial;
	/* Name hash table. */
	uint32_t namehash_offset;
	uint32_t namehash_used;
	uint32_t namehash_size;
	/* String table. */
	uint32_t string_offset;
	uint32_t string_used;
	uint32_t string_size;
	/* Table with locale records. */
	uint32_t locrectab_offset;
	uint32_t locrectab_used;
	uint32_t locrectab_size;
	/* MD5 sum hash table. */
	uint32_t sumhash_offset;
	uint32_t sumhash_used;
	uint32_t sumhash_size;
};

struct NameHashEntry {
	/* Hash value of the name. */
	uint32_t hashval;
	/* Offset of the name in the string table. */
	uint32_t name_offset;
	/* Offset of the LocaleRecord. */
	uint32_t locrec_offset;
};

struct LocaleRecord {
	/* # of NameHashEntry records that point here */
	uint32_t refs;
	struct {
		uint32_t offset;
		uint32_t len;
	} record[LC_IDENTIFICATION + 1];
};

struct SumHashEntry {
	/* MD5 sum. */
	char sum[16];
	/* Offset of the file in the archive. */
	uint32_t file_offset;
};

} // namespace LocaleArchive

#pragma mark - hash and magic calculations
template <typename T>
constexpr T nameHashVal(const void *key, size_t keylen) {
	size_t cnt = 0;
	T hval = keylen;

	while (cnt < keylen) {
		hval = (hval << 9) | (hval >> (sizeof hval * CHAR_BIT - 9));
		hval += (T)((const unsigned char *)key)[cnt++];
	}
	return hval != 0 ? hval : ~((T)0);
}

constexpr uint32_t categoryMagic(int category) {
	if (category == LC_COLLATE)
		return 0x20051014 ^ category;
	else if (category == LC_CTYPE)
		return 0x20090720 ^ category;
	else
		return 0x20031115 ^ category;
}

