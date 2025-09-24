/* si_targa.h - 20250923
 */

#include <endian.h> // BYTE_ORDER, BIG_ENDIAN, LITTLE_ENDIAN
#include <limits.h> // CHAR_BIT
#include <stdbool.h> // bool, false, true
#include <stdint.h> // uint8_t, uint16_t, uint32_t
#include <stdio.h> // FILE
#include <stdlib.h> // calloc(), free()
#include <string.h> // memset()

// Support packed structs if possible
#if defined(__GNUC__) || defined(__clang__)
// Uses an inline attribute
#define SI_PACKED_PRE
#define SI_PACKED_INLINE __attribute__((packed))
#define SI_PACKED_POST
#elif defined(_MSC_VER)
// Uses a settings stack
#define SI_PACKED_PRE #pragma pack(push, 1)
#define SI_PACKED_INLINE
#define SI_PACKED_POST #pragma pack(pop)
#else
// All are blank meaning no struct packing supported
#define SI_PACKED_PRE
#define SI_PACKED_INLINE
#define SI_PACKED_POST
#endif// End of compiler check(s)

#define SI_TARGA_HEADER_SIZE (18)

#define SI_TARGA_MAP_TYPE_OFFSET (1)
#define SI_TARGA_MAP_TYPE_NONE (0)
#define SI_TARGA_MAP_TYPE_PRESENT (1)

#define SI_TARGA_DATA_TYPE_OFFSET (2)
#define SI_TARGA_DATA_TYPE_NONE (0)
#define SI_TARGA_DATA_TYPE_UNCOMPRESSED_CMI (1)
#define SI_TARGA_DATA_TYPE_UNCOMPRESSED_RGB (2)
#define SI_TARGA_DATA_TYPE_UNCOMPRESSED_BWI (3)
#define SI_TARGA_DATA_TYPE_RLE_CMI (9)
#define SI_TARGA_DATA_TYPE_RLE_RGB (10)
#define SI_TARGA_DATA_TYPE_COMPRESSED_BWI (11)
#define SI_TARGA_DATA_TYPE_COMPRESSED_CMI (32)
#define SI_TARGA_DATA_TYPE_COMPRESSED_CMI_QT (33)
#define SI_TARGA_DEFAULT_DATA_TYPE (SI_TARGA_DATA_TYPE_UNCOMPRESSED_RGB)

#define SI_TARGA_MAP_START_OFFSET (3)
#define SI_TARGA_MAP_LEN_OFFSET (5)
#define SI_TARGA_MAP_DEPTH_OFFSET (7)

#define SI_TARGA_X_START_OFFSET (8)
#define SI_TARGA_Y_START_OFFSET (10)

#define SI_TARGA_WIDTH_OFFSET (12)
#define SI_TARGA_HEIGHT_OFFSET (14)

#define SI_TARGA_BITS_PER_PIXEL_OFFSET (16)
#define SI_TARGA_DEFAULT_CHANNEL_COUNT (4)

#define SI_TARGA_IMG_DESC_OFFSET (17)
// 0b11000000 0xC0 192
#define SI_TARGA_IMG_DESC_ZERO_MASK (0xC0)
// 0b00110000 0x30 48
#define SI_TARGA_IMG_DESC_ORIGIN_MASK (0x30)
// 0b00001111 0x0F 15
#define SI_TARGA_IMG_DESC_ALPHA_MASK (0x0F)
#define SI_TARGA_DEFAULT_IMG_DESC (0x20)

#ifndef SI_TARGA_H
#define SI_TARGA_H

SI_PACKED_PRE
typedef struct SI_PACKED_INLINE si_tga_header_t
{
	uint8_t  id_len;   // 1
	uint8_t  map_type; // 1
	uint8_t  data_type;// 1
	uint16_t map_start;// 2
	uint16_t map_len;  // 2
	uint8_t  map_depth;// 1
	uint16_t x_start;  // 2
	uint16_t y_start;  // 2
	uint16_t width;    // 2
	uint16_t height;   // 2
	uint8_t  bpp;      // 1
	uint8_t  img_desc; // 1
} si_tga_header_t;     //18
SI_PACKED_POST

void si_tga_header_init_4(si_tga_header_t* const p_header,
	const uint16_t width, const uint16_t height, const uint8_t channels);
void si_tga_header_init_3(si_tga_header_t* const p_header,
	const uint16_t width, const uint16_t height);
void si_tga_header_init(si_tga_header_t* const p_header);

bool si_tga_header_is_valid(const si_tga_header_t* const p_header);

uint8_t si_tga_header_channel_count(const si_tga_header_t* const p_header);

size_t si_tga_header_data_size(const si_tga_header_t* const p_header);

bool si_tga_header_fread(si_tga_header_t* const p_header, FILE* const p_file);

bool si_tga_header_fwrite(si_tga_header_t* const p_header, FILE* const p_file);

void si_tga_header_fprint(FILE* const p_file,
	const si_tga_header_t* const p_header);

// Typically 15, 16, 24 or 32 bits
typedef struct si_tga_cmap_t
{
	uint32_t data;
} si_tga_cmap_t;

typedef struct si_tga_t
{
	si_tga_header_t header;
	uint8_t*        p_img_id;
	si_tga_cmap_t*  p_colormaps;
	uint8_t*        p_data;
} si_tga_t;

void si_tga_init_4(si_tga_t* const p_tga,
	const uint16_t width, const uint16_t height, const uint8_t channels);
void si_tga_init_3(si_tga_t* const p_tga,
	const uint16_t width, const uint16_t height);
void si_tga_init(si_tga_t* const p_tga);

si_tga_t* si_tga_new();

bool si_tga_fread(si_tga_t* const p_tga, FILE* const p_file);

bool si_tga_fread_from(si_tga_t* const p_tga, const char* const p_path);

uint8_t* si_tga_pixel_at(const si_tga_t* const p_tga,
	const uint16_t x_pos, const uint16_t y_pos);

bool si_tga_fwrite(si_tga_t* const p_tga, FILE* const p_file);

bool si_tga_fwrite_to(si_tga_t* const p_tga, const char* const p_path);

void si_tga_free(si_tga_t* const p_tga);

void si_tga_destroy(si_tga_t** pp_tga);

#endif//SI_TARGA_H
