// si_targa.c
#include "si_targa.h"

/** Doxygen
 * @brief Local function that handles partial writes to a file.
 *
 * @param p_buffer Pointer to data to be written.
 * @param buffer_size Number of bytes to be written.
 * @param p_file Pointer to FILE to write to.
 *
 * @return Returns stdbool true on success. Returns false otherwise.
 */
static bool fwrite_all(uint8_t* const p_buffer, const size_t buffer_size,
	FILE* const p_file)
{
	bool result = false;
	if ((NULL == p_buffer) || (0u >= buffer_size) || (NULL == p_file))
	{
		goto END;
	}
	size_t amount_written = 0u;
	while (amount_written < buffer_size)
	{
		const size_t next_result = fwrite(
			&(p_buffer[amount_written]), sizeof(uint8_t),
			buffer_size - amount_written, p_file
		);
		const bool file_error = ferror(p_file);
		if ((0u >= next_result) || (true == file_error))
		{
			perror("si_targa file write has encountered an error.");
			goto END;
		}
		amount_written += next_result;
	}
	fflush(p_file);
	result = true;
END:
	return result;
}

/** Doxygen
 * @brief Local function that handles partial reads from file.
 *
 * @param p_buffer Pointer to byte buffer to hold file contents.
 * @param buffer_size Number of bytes that the buffer can hold.
 * @param p_file Pointer to FILE to read from.
 *
 * @return Returns stdbool true on success. Returns false otherwise.
 */
static bool fread_all(uint8_t* const p_buffer, const size_t buffer_size,
	FILE* const p_file)
{
	bool result = false;
	if ((NULL == p_buffer) || (0u >= buffer_size) || (NULL == p_file))
	{
		goto END;
	}
	size_t bytes_read = 0u;
	while(bytes_read < buffer_size)
	{
		const size_t next_read = fread(
			&(p_buffer[bytes_read]), sizeof(uint8_t),
			buffer_size - bytes_read, p_file
		);
		const bool file_error = ferror(p_file);
		if ((0u >= next_read) || (true == file_error))
		{
			perror("si_targa file read has encountered an error.");
			goto END;
		}
		bytes_read += next_read;
	}
	result = true;
END:
	return result;
}

/** Doxygen
 * @brief Allocates a buffer on the heap to hold an amount of data to read from
 *        file. If allocation fails will seek past the amount to be read.
 *
 * @param pp_buffer Pointer to the byte pointer to hold results.
 * @param buffer_size Number of bytes to read from file/allocate.
 * @param p_file Pointer to FILE to read the data from.
 *
 * @return Returns stdbool true on read or skip. Returns false on io error.
 */
static bool alloc_fread_all(
	uint8_t** const pp_buffer, const size_t buffer_size, FILE* const p_file)
{
	bool result = false;
	if ((NULL == pp_buffer) || (0u >= buffer_size) || (NULL == p_file))
	{
		goto END;
	}
	if (NULL != *pp_buffer)
	{
		free(*pp_buffer);
		*pp_buffer = NULL;
	}
	*pp_buffer = calloc(buffer_size, sizeof(uint8_t));
	if (NULL == *pp_buffer)
	{
		// Failed to allocate, skip it instead.
		const int skip_result = fseek(p_file, buffer_size, SEEK_CUR);
		result = true;
		if(0 != skip_result)
		{
			perror("si_targa failed to skip past data in file.");
			result = false;
		}
		goto END;
	}
	else
	{
		result = fread_all(*pp_buffer, buffer_size, p_file);
	}
END:
	return result;
}



void si_tga_header_init_4(si_tga_header_t* const p_header,
	const uint16_t width, const uint16_t height, const uint8_t channels)
{
	if (NULL == p_header)
	{
		goto END;
	}
	memset(p_header, 0x00, sizeof(si_tga_header_t));
	p_header->data_type = SI_TARGA_DEFAULT_DATA_TYPE;
	p_header->width = width;
	p_header->height = height;
	p_header->bpp = (CHAR_BIT * channels);
	p_header->img_desc = SI_TARGA_DEFAULT_IMG_DESC;
END:
	return;
}
inline void si_tga_header_init_3(si_tga_header_t* const p_header,
	const uint16_t width, const uint16_t height)
{
	// Default value of channels if SI_TARGA_DEFAULT_CHANNEL_COUNT(4)
	si_tga_header_init_4(
		p_header, width, height, SI_TARGA_DEFAULT_CHANNEL_COUNT
	);
}
inline void si_tga_header_init(si_tga_header_t* const p_header)
{
	// Default image x,y dimesions are (0,0)
	si_tga_header_init_3(p_header, 0u, 0u);
}

/** Doxygen
 * @brief Local function determines if a data type is a valid value.
 *
 * @param data_type Targa data type byte value.
 *
 * @return Returns stdbool true if valid. Returns false otherwise.
 */
static bool si_tga_header_is_data_type_valid(const uint8_t data_type)
{
	bool result = false;
	if (0u >= data_type)
	{
		goto END;
	}
	if ((SI_TARGA_DATA_TYPE_UNCOMPRESSED_BWI < data_type) &&
	    (SI_TARGA_DATA_TYPE_RLE_CMI > data_type))
	{
		goto END;
	}
	if ((SI_TARGA_DATA_TYPE_COMPRESSED_BWI < data_type) &&
	    (SI_TARGA_DATA_TYPE_COMPRESSED_CMI > data_type))
	{
		goto END;
	}
	if (SI_TARGA_DATA_TYPE_COMPRESSED_CMI_QT < data_type)
	{
		goto END;
	}
	result = true;
END:
	return result;
}

bool si_tga_header_is_valid(const si_tga_header_t* const p_header)
{
	bool result = false;
	if (NULL == p_header)
	{
		goto END;
	}
	if (SI_TARGA_MAP_TYPE_PRESENT < p_header->map_type)
	{
		goto END;
	}
	const bool is_valid_data_type = si_tga_header_is_data_type_valid(
		p_header->data_type
	);
	if(true != is_valid_data_type)
	{
		goto END;
	}
	if (SI_TARGA_MAP_TYPE_NONE == p_header->map_type)
	{
		if ((0u != p_header->map_start) ||
		    (0u != p_header->map_len  ) ||
		    (0u != p_header->map_depth))
		{
			goto END;
		}
	}
	if ((0u >= p_header->width) || (0u >= p_header->height))
	{
		goto END;
	}
	const unsigned int mod_result = (p_header->bpp % CHAR_BIT);
	if (0u != mod_result)
	{
		goto END;
	}
	const uint8_t zero = (SI_TARGA_IMG_DESC_ZERO_MASK & p_header->img_desc);
	if(0u != zero)
	{
		goto END;
	}
	// TODO Temporary shows all mapped images as invalid until implimented.
	if (SI_TARGA_MAP_TYPE_PRESENT == p_header->map_type)
	{
		fprintf(stderr, "[ERROR]: si_targa Doesnt yet impliment color maps\n");
		goto END;
	}
	result = true;
END:
	return result;
}

uint8_t si_tga_header_channel_count(const si_tga_header_t* const p_header)
{
	uint8_t result = 0u;
	if (NULL == p_header)
	{
		goto END;
	}
	const bool is_valid = si_tga_header_is_valid(p_header);
	if (false == is_valid)
	{
		goto END;
	}
	result = (p_header->bpp / CHAR_BIT);
END:
	return result;
}

size_t si_tga_header_data_size(const si_tga_header_t* const p_header)
{
	size_t data_size = 0u;
	if (NULL == p_header)
	{
		goto END;
	}
	const uint8_t channel_count = si_tga_header_channel_count(p_header);
	if (0u >= channel_count)
	{
		goto END;
	}

	const size_t stage_a = (p_header->width * p_header->height);
	// Check for overflows
	if ((stage_a / p_header->height) != p_header->width)
	{
		goto END;
	}

	const size_t stage_b = (stage_a * channel_count);
	// Check for overflows
	if ((stage_b / channel_count) != stage_a)
	{
		goto END;
	}

	data_size = stage_b;
END:
	return data_size;
}

bool si_tga_header_fread(si_tga_header_t* const p_header, FILE* const p_file)
{
	bool result = false;
	if ((NULL == p_header) || (NULL == p_file))
	{
		goto END;
	}
	const size_t struct_size = sizeof(si_tga_header_t);
	// Utilize a packed struct
	if (SI_TARGA_HEADER_SIZE == struct_size)
	{
		result = fread_all(
			(uint8_t*)p_header, SI_TARGA_HEADER_SIZE, p_file
		);
		goto ENDIAN;
	}
	// Utilize a stack array buffer
	uint8_t file_buffer[SI_TARGA_HEADER_SIZE] = {0};
	result = fread_all(
		(uint8_t*)&file_buffer[0u], SI_TARGA_HEADER_SIZE, p_file
	);
	if (false == result)
	{
		goto END;
	}
	memcpy(&p_header->id_len,
		&file_buffer[0u], sizeof(uint8_t));
	memcpy(&p_header->map_type,
		&file_buffer[SI_TARGA_MAP_TYPE_OFFSET], sizeof(uint8_t));
	memcpy(&p_header->data_type,
		&file_buffer[SI_TARGA_DATA_TYPE_OFFSET], sizeof(uint8_t));

	memcpy(&p_header->map_start,
		&file_buffer[SI_TARGA_MAP_START_OFFSET], sizeof(uint16_t));
	memcpy(&p_header->map_len,
		&file_buffer[SI_TARGA_MAP_LEN_OFFSET], sizeof(uint16_t));
	memcpy(&p_header->map_depth,
		&file_buffer[SI_TARGA_MAP_DEPTH_OFFSET], sizeof(uint8_t));

	memcpy(&p_header->x_start,
		&file_buffer[SI_TARGA_X_START_OFFSET], sizeof(uint16_t));
	memcpy(&p_header->y_start,
		&file_buffer[SI_TARGA_Y_START_OFFSET], sizeof(uint16_t));

	memcpy(&p_header->width,
		&file_buffer[SI_TARGA_WIDTH_OFFSET], sizeof(uint16_t));
	memcpy(&p_header->height,
		&file_buffer[SI_TARGA_HEIGHT_OFFSET], sizeof(uint16_t));

	memcpy(&p_header->bpp,
		&file_buffer[SI_TARGA_BITS_PER_PIXEL_OFFSET], sizeof(uint8_t));
	memcpy(&p_header->img_desc,
		&file_buffer[SI_TARGA_IMG_DESC_OFFSET], sizeof(uint8_t));
ENDIAN:
	if (false == result)
	{
		goto END;
	}
	// TODO Handle endianess
END:
	return result;
}

bool si_tga_header_fwrite(si_tga_header_t* const p_header, FILE* const p_file)
{
	bool result = false;
	if ((NULL == p_header) || (NULL == p_file))
	{
		goto END;
	}
	uint8_t header_buffer[SI_TARGA_HEADER_SIZE] = {0};
	const size_t struct_size = sizeof(si_tga_header_t);
	// Utilize packed struct
	if (SI_TARGA_HEADER_SIZE == struct_size)
	{
		memcpy(&header_buffer, p_header, SI_TARGA_HEADER_SIZE);
		goto ENDIAN;
	}
	// Set buffer values from struct.
	memcpy(&header_buffer[0u],
		&p_header->id_len, sizeof(uint8_t));
	memcpy(&header_buffer[SI_TARGA_MAP_TYPE_OFFSET],
		&p_header->map_type, sizeof(uint8_t));
	memcpy(&header_buffer[SI_TARGA_DATA_TYPE_OFFSET],
		&p_header->data_type, sizeof(uint8_t));

	memcpy(&header_buffer[SI_TARGA_MAP_START_OFFSET],
		&p_header->map_start, sizeof(uint16_t));
	memcpy(&header_buffer[SI_TARGA_MAP_LEN_OFFSET],
		&p_header->map_len, sizeof(uint16_t));
	memcpy(&header_buffer[SI_TARGA_MAP_DEPTH_OFFSET],
		&p_header->map_depth, sizeof(uint8_t));

	memcpy(&header_buffer[SI_TARGA_X_START_OFFSET],
		&p_header->x_start, sizeof(uint16_t));
	memcpy(&header_buffer[SI_TARGA_Y_START_OFFSET],
		&p_header->y_start, sizeof(uint16_t));

	memcpy(&header_buffer[SI_TARGA_WIDTH_OFFSET],
		&p_header->width, sizeof(uint16_t));
	memcpy(&header_buffer[SI_TARGA_HEIGHT_OFFSET],
		&p_header->height, sizeof(uint16_t));

	memcpy(&header_buffer[SI_TARGA_BITS_PER_PIXEL_OFFSET],
		&p_header->bpp, sizeof(uint8_t));
	memcpy(&header_buffer[SI_TARGA_IMG_DESC_OFFSET],
		&p_header->img_desc, sizeof(uint8_t));
ENDIAN:
	// TODO Handle endianess
	// Write to FILE
	result = fwrite_all(header_buffer, SI_TARGA_HEADER_SIZE, p_file);
END:
	return result;
}

/** Doxygen
 * @brief Writes Image descriptor information to FILE
 *
 * @param p_file Pointer to the FILE to write to.
 * @param img_desc Byte value of image descriptor.
 */
static void si_tga_header_fprint_img_desc(FILE* const p_file,
	const uint8_t img_desc)
{
	if (NULL == p_file)
	{
		goto END;
	}
	const uint8_t zero = (img_desc & SI_TARGA_IMG_DESC_ZERO_MASK);
	if (0u != zero)
	{
		fprintf(p_file, "Invalid");
		goto END;
	}
	const uint8_t order = (img_desc & SI_TARGA_IMG_DESC_ORIGIN_MASK);
	const uint8_t alpha = (img_desc & SI_TARGA_IMG_DESC_ALPHA_MASK);
	switch (order)
	{
		case(0u):
			fprintf(p_file, "bottom-left");
		break;
		case(16u):
			fprintf(p_file, "bottom-right");
		break;
		case(32u):
			fprintf(p_file, "top-left");
		break;
		case(48u):
			fprintf(p_file, "top-right");
		break;
		default:
			fprintf(p_file, "Invalid");
		goto END;
	}
	fprintf(p_file, " %02x", alpha);
END:
	return;
}

void si_tga_header_fprint(FILE* const p_file,
    const si_tga_header_t* const p_header)
{
	if ((NULL == p_file) || (NULL == p_header))
	{
		goto END;
	}
	fprintf(p_file, "si_tga_header@%p\n{\n", p_header);
	fprintf(p_file, "\tID Length     : %hhu\n", p_header->id_len);
	fprintf(p_file, "\tMap Type      : %hhu\n", p_header->map_type);
	fprintf(p_file, "\tData Type     : %hhu\n", p_header->data_type);
	fprintf(p_file, "\tMap Start     : %hu\n" , p_header->map_start);
	fprintf(p_file, "\tMap Length    : %hu\n" , p_header->map_len);
	fprintf(p_file, "\tMap Depth     : %hhu\n", p_header->map_depth);
	fprintf(p_file, "\tX Start       : %hu\n" , p_header->x_start);
	fprintf(p_file, "\tY Start       : %hu\n" , p_header->y_start);
	fprintf(p_file, "\tWidth         : %hu\n" , p_header->width);
	fprintf(p_file, "\tHeight        : %hu\n" , p_header->height);
	fprintf(p_file, "\tBits per pixel: %hhu\n", p_header->bpp);
	fprintf(p_file, "\tImg Descriptor: ");
	si_tga_header_fprint_img_desc(p_file, p_header->img_desc);
	fprintf(p_file, "\n}\n");
	fflush(p_file);
END:
	return;
}


void si_tga_init_4(si_tga_t* const p_tga,
	const uint16_t width, const uint16_t height, const uint8_t channels)
{
	if (NULL == p_tga)
	{
		goto END;
	}
	si_tga_header_init_4(&p_tga->header, width, height, channels);
	p_tga->p_img_id = NULL;
	p_tga->p_colormaps = NULL;
	p_tga->p_data = NULL;
	const size_t pixel_data_size = si_tga_header_data_size(&p_tga->header);
	if (0u >= pixel_data_size)
	{
		goto END;
	}
	p_tga->p_data = calloc(pixel_data_size, sizeof(uint8_t));
	if (NULL == p_tga->p_data)
	{
		goto END;
	}
	memset(p_tga->p_data, 0x00, pixel_data_size * sizeof(uint8_t));
END:
	return;
}
inline void si_tga_init_3(si_tga_t* const p_tga,
	const uint16_t width, const uint16_t height)
{
	// Default value of channels is SI_TARGA_DEFAULT_CHANNEL_COUNT(4)
	si_tga_init_4(p_tga, width, height, SI_TARGA_DEFAULT_CHANNEL_COUNT);
}
inline void si_tga_init(si_tga_t* const p_tga)
{
	// Default image dimensions are (0,0)
	si_tga_init_3(p_tga, 0u, 0u);
}

si_tga_t* si_tga_new()
{
	si_tga_t* p_result = NULL;
	p_result = calloc(1u, sizeof(si_tga_t));
	if (NULL == p_result)
	{
		goto END;
	}
	si_tga_init(p_result);
END:
	return p_result;
}

bool si_tga_fread(si_tga_t* const p_tga, FILE* const p_file)
{
	bool result = false;
	if ((NULL == p_tga) || (NULL == p_file))
	{
		goto END;
	}
	const bool header_result = si_tga_header_fread(&p_tga->header, p_file);
	if (true != header_result)
	{
		goto END;
	}
	const size_t pixel_data_size = si_tga_header_data_size(&p_tga->header);
	if (0u >= pixel_data_size)
	{
		goto END;
	}

	// Reads the image ID
	if (0u < p_tga->header.id_len)
	{
		const bool io_good = alloc_fread_all(
			&(p_tga->p_img_id), p_tga->header.id_len, p_file
		);
		if (true != io_good)
		{
			goto END;
		}
	}

	// TODO Reads color maps

	// Reads pixel data
	const bool io_still_good = alloc_fread_all(
		&(p_tga->p_data), pixel_data_size, p_file
	);
	if (true != io_still_good)
	{
		si_tga_free(p_tga);
		goto END;
	}

	// TODO Reads Developer Area

	// TODO Reads Extension Area

	// TODO Reads File Footer

	result = true;
END:
	return result;
}

bool si_tga_fread_from(si_tga_t* const p_tga, const char* const p_path)
{
	bool result = false;
	if ((NULL == p_tga) || (NULL == p_path))
	{
		goto END;
	}
	FILE* p_file = fopen(p_path, "rb");
	if (NULL == p_file)
	{
		perror("si_tga_fread_from failed to open file for reading");
		goto END;
	}
	result = si_tga_fread(p_tga, p_file);
	(void)fclose(p_file);
	p_file = NULL;
END:
	return result;
}

uint8_t* si_tga_pixel_at(const si_tga_t* const p_tga,
	const uint16_t x_pos, const uint16_t y_pos)
{
	uint8_t* p_pixel = NULL;
	if (NULL == p_tga)
	{
		goto END;
	}
	if ((p_tga->header.width <= x_pos) || (p_tga->header.height <= y_pos))
	{
		goto END;
	}
	if (NULL == p_tga->p_data)
	{
		goto END;
	}
	const uint8_t channel_count = si_tga_header_channel_count(
		&(p_tga->header)
	);
	if (0u >= channel_count)
	{
		goto END;
	}
	switch (p_tga->header.data_type)
	{
		case(SI_TARGA_DATA_TYPE_UNCOMPRESSED_RGB):
		break;
		default:
			fprintf(stderr, "si_tga_pixel_at() Unsupported data_type.\n");
		goto END;
	}
	switch (SI_TARGA_IMG_DESC_ORIGIN_MASK & p_tga->header.img_desc)
	{
		case(SI_TARGA_DEFAULT_IMG_DESC):
		break;
		default:
			fprintf(stderr, "si_tga_pixel_at() Unsupported Origin.\n");
		goto END;
	}
	const size_t index = (y_pos * p_tga->header.width) + x_pos;
	const size_t offset = (index * channel_count);
	p_pixel = &(p_tga->p_data[offset]);
END:
	return p_pixel;
}

bool si_tga_fwrite(si_tga_t* const p_tga, FILE* const p_file)
{
	bool result = false;
	if ((NULL == p_tga) || (NULL == p_file))
	{
		goto END;
	}
	const size_t pixel_data_size = si_tga_header_data_size(&p_tga->header);
	if ((0u >= pixel_data_size) || (NULL == p_tga->p_data))
	{
		fprintf(stderr, "si_tga_fwrite() Failed to determine data size.\n");
		goto END;
	}

	// Write Header
	const bool header_result = si_tga_header_fwrite(&p_tga->header, p_file);
	if (true != header_result)
	{
		goto END;
	}

	// Write Image ID
	if ((0u < p_tga->header.id_len) && (NULL != p_tga->p_img_id))
	{
		const bool wrote_id = fwrite_all(
			p_tga->p_img_id, p_tga->header.id_len, p_file
		);
		if(true != wrote_id)
		{
			goto END;
		}
	}

	//!TODO Write Colormaps

	// Write Pixel Data
	const bool wrote_data = fwrite_all(p_tga->p_data, pixel_data_size, p_file);
	if (true != wrote_data)
	{
		goto END;
	}

	// TODO Write Developer Area

	// TODO Write Extension Area

	// TODO Write File Footer
	result = true;
END:
	return result;
}

bool si_tga_fwrite_to(si_tga_t* const p_tga, const char* const p_path)
{
	bool result = false;
	if ((NULL == p_tga) || (NULL == p_path))
	{
		goto END;
	}
	FILE* p_file = fopen(p_path, "wb");
	if (NULL == p_file)
	{
		perror("si_tga_fread_from failed to open file for writing");
		goto END;
	}
	result = si_tga_fwrite(p_tga, p_file);
	(void)fclose(p_file);
	p_file = NULL;
END:
	return result;
}

void si_tga_free(si_tga_t* const p_tga)
{
	if (NULL == p_tga)
	{
		goto END;
	}
	if (NULL != p_tga->p_img_id)
	{
		free(p_tga->p_img_id);
		p_tga->p_img_id = NULL;
	}
	if (NULL != p_tga->p_colormaps)
	{
		free(p_tga->p_colormaps);
		p_tga->p_colormaps = NULL;
	}
	if (NULL != p_tga->p_data)
	{
		free(p_tga->p_data);
		p_tga->p_data = NULL;
	}
END:
	return;
}

void si_tga_destroy(si_tga_t** pp_tga)
{
	if (NULL == pp_tga)
	{
		goto END;
	}
	if (NULL == *pp_tga)
	{
		// Already freed
		goto END;
	}
	si_tga_free(*pp_tga);
	free(*pp_tga);
	*pp_tga = NULL;
END:
	return;
}
