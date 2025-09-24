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

/** Doxygen
 * @brief Initializes an existing si_tga_header_t struct in memory.
 * 
 * @param p_header Pointer to the struct to be initialized.
 * @param width Initial width value of the header.
 * @param height Initial height value of the header.
 * @param channels Desired number of 8-bit color channels from 0->4.
 */
void si_tga_header_init_4(si_tga_header_t* const p_header,
	const uint16_t width, const uint16_t height, const uint8_t channels);
void si_tga_header_init_3(si_tga_header_t* const p_header,
	const uint16_t width, const uint16_t height);
void si_tga_header_init(si_tga_header_t* const p_header);

/** Doxygen
 * @brief Determines if the pointed at tga header contains valid values.
 * 
 * @param p_header Pointer to si_tga_header_t struct to be validated.
 * 
 * @return Returns stdbool true if valid. Returns false otherwise.
 */
bool si_tga_header_is_valid(const si_tga_header_t* const p_header);

/** Doxygen
 * @brief Determines the number of 8-bit color channels in a tga header.
 * 
 * @param p_header Pointer to si_tga_header_t struct to read from.
 * 
 * @return Returns channel count on success. Otherwise returns 0u.
 */
uint8_t si_tga_header_channel_count(const si_tga_header_t* const p_header);

/** Doxygen
 * @brief Returns the pixel data size based upon the values of the tga header.
 * 
 * @param p_header Pointer to si_tga_header_t struct to read from.
 * 
 * @return Returns size on known fixed size/success. Otherwise returns 0u.
 */
size_t si_tga_header_data_size(const si_tga_header_t* const p_header);

/** Doxygen
 * @brief Reads a tga formatted file's header into header struct from FILE*
 * 
 * @param p_header Pointer to si_tga_header_t struct to read header into.
 * @param p_file FILE pointer to read the binary data from. (mode:'rb')
 * 
 * @return Returns a stdbool true on success. Returns false otherwise.
 */
bool si_tga_header_fread(si_tga_header_t* const p_header, FILE* const p_file);

/** Doxygen
 * @brief Writes a tga header from memory to a FILE*
 * 
 * @param p_header Pointer to si_tga_header_t to read data from.
 * @param p_file FILE pointer to write header values to.
 * 
 * @return Returns a stdbool true on success. Otherwise returns a false.
 */
bool si_tga_header_fwrite(si_tga_header_t* const p_header, FILE* const p_file);

/** Doxygen
 * @brief Pretty prints the values of an in-memory tga_header to FILE*
 * 
 * @param p_file FILE pointer to be written to.
 * @param p_header Pointer to si_tga_header_t struct to read values from.
 */
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

/** Doxygen
 * @brief Initializes the values inside a si_tga_t struct.
 * 
 * @param p_tga Pointer to si_tga_t struct to be initialized.
 * @param width Width of the image to be initialized in pixels.
 * @param height Height of the image to be initialized in pixels.
 * @param channels Number of 8-bit color channels in the initial struct.
 */
void si_tga_init_4(si_tga_t* const p_tga,
	const uint16_t width, const uint16_t height, const uint8_t channels);
void si_tga_init_3(si_tga_t* const p_tga,
	const uint16_t width, const uint16_t height);
void si_tga_init(si_tga_t* const p_tga);

/** Doxygen
 * @brief Heap allocates and initializes a new si_tga_t struct.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
si_tga_t* si_tga_new();

/** Doxygen
 * @brief Reads a tga image into memory from a FILE*
 * 
 * @param p_tga Pointer to si_tga_t struct to hold values in.
 * @param p_file FILE pointer to read image data from.
 * 
 * @return Returns a stdbool true on success. Returns false otherwise.
 */
bool si_tga_fread(si_tga_t* const p_tga, FILE* const p_file);

/** Doxygen
 * @brief Reads a tga image into memory from a file path.
 * 
 * @param p_tga Pointer to the si_tga_t struct to hold values in.
 * @param p_path C string file path to open and read tga data from.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_tga_fread_from(si_tga_t* const p_tga, const char* const p_path);

/** Doxygen
 * @brief Finds the location in memory of a pixel by its image position.
 * 
 * @param p_tga Pointer to the si_tga_t struct that holds the pixels.
 * @param x_pos Position of the pixel within the image that is to be found.
 * @param y_pos Position of the pixel within the image that is to be found.
 * 
 * @return Returns pointer to first byte of the pixel value at position or NULL
 */
uint8_t* si_tga_pixel_at(const si_tga_t* const p_tga,
	const uint16_t x_pos, const uint16_t y_pos);

/** Doxygen
 * @brief Resizes the pixel data of a si_tga_t on the heap.
 * 
 * @param p_tga Pointer to the si_tga_t struct to be resized/modifed.
 * @param width Desired new horizontal size of the tga being pointed at.
 * @param height Desired new vertical size of the tga being pointer at.
 * 
 * @return Returns a stdbool true on success. Returns a false otherwise.
 */
bool si_tga_resize(si_tga_t* const p_tga,
	const uint16_t width, const uint16_t height);
	
/** Doxygen
 * @brief Writes a si_tga_t image from memory into a FILE*
 * 
 * @param p_tga Pointer to the si_tga_t struct to read the data from.
 * @param p_file FILE pointer to write the data to.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_tga_fwrite(si_tga_t* const p_tga, FILE* const p_file);

/** Doxygen
 * @brief Writes a si_tga_t image from memory into a FILE at a file path.
 * 
 * @param p_tga Pointer to the si_tga_t struct to read the data from.
 * @param p_path C string pointer of the file path to write to.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_tga_fwrite_to(si_tga_t* const p_tga, const char* const p_path);

/** Doxygen
 * @brief Frees allocated data elements within an existing si_tga_t struct.
 * 
 * @param p_tga Pointer to the si_tga_t struct to free the contents of.
 */
void si_tga_free(si_tga_t* const p_tga);

/** Doxygen
 * @brief Frees an allocated heap si_tga_t by it's pointer.
 * 
 * @param pp_tga Pointer to si_tga_t heap pointer to be freed.
 */
void si_tga_destroy(si_tga_t** pp_tga);

#endif//SI_TARGA_H
