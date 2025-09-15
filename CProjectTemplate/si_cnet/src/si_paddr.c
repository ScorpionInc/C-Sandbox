//si_paddr.c

#include "si_paddr.h"

void convert_ipv4_addr_to_network(
	uint8_t p_address[INET_ADDRESS_SIZE],
	const uint8_t p_sub_msk[INET_ADDRESS_SIZE])
{
	if(NULL == p_address)
	{
		goto END;
	}
	for(size_t iii = 0u; iii < INET_ADDRESS_SIZE; iii++)
	{
		p_address[iii] = (p_address[iii] & p_sub_msk[iii]);
	}
END:
	return;
}

bool is_within_ipv4_network(
	const uint8_t p_network_address[INET_ADDRESS_SIZE],
	const uint8_t p_subnet_mask[INET_ADDRESS_SIZE],
	const uint8_t p_test_address[INET_ADDRESS_SIZE])
{
	// Assumes all are in the same byte order
	bool result = false;
	if((NULL == p_network_address) ||
	   (NULL == p_subnet_mask)     ||
	   (NULL == p_test_address))
	{
		goto END;
	}
	// Make closes of source data to be modified / tested
	uint8_t p_network[INET_ADDRESS_SIZE] = {0};
	uint8_t p_test[INET_ADDRESS_SIZE] = {0};
	memcpy(p_network, p_network_address, INET_ADDRESS_SIZE);
	memcpy(p_test, p_test_address, INET_ADDRESS_SIZE);
	convert_ipv4_addr_to_network(p_network, p_subnet_mask);
	convert_ipv4_addr_to_network(p_test, p_subnet_mask);
	for(size_t iii = 0u; iii < INET_ADDRESS_SIZE; iii++)
	{
		if(p_network[iii] != p_test[iii])
		{
			goto END;
		}
	}
	result = true;
END:
	return result;
}

bool is_localhost_address_v4(const struct sockaddr_in* const p_address)
{
	// Assumes address is in network order
	bool result = false;
	if(NULL == p_address)
	{
		goto END;
	}
	// IPv4 Localhost loopback address range: 127.0.0.0/8
	const uint32_t localhost = htonl(0x7F000001);
	const uint32_t loopback_mask = htonl(IN_CLASSA_NET);
	const uint32_t address = *((uint32_t*)&(p_address->sin_addr));
	result = is_within_ipv4_network(
		(const uint8_t*)&localhost,
		(const uint8_t*)&loopback_mask,
		(const uint8_t*)&address
	);
	if(true == result)
	{
		goto END;
	}
	// Technically the address of 0.0.0.0 is all of the local interfaces or
	// it is an unassigned address. Thus, is one of the loopback addresses but
	// would still be related to localhost so we will pass it. (For now)
	const uint8_t* const p_bytes = (uint8_t*)&(p_address->sin_addr);
	for(size_t iii = 0u; iii < INET_ADDRESS_SIZE; iii++)
	{
		if(0x00 != p_bytes[iii])
		{
			goto END;
		}
	}
END:
	return result;
}

bool is_localhost_address_v6(const struct sockaddr_in6* const p_address)
{
	// Assumes network is in network order
	bool result = false;
	if(NULL == p_address)
	{
		goto END;
	}
	uint8_t* const address_bytes = (uint8_t*)&(p_address->sin6_addr);
	for(size_t iii = 0u; iii < INET6_ADDRESS_SIZE - 1u; iii++)
	{
		if(0x00 != address_bytes[iii])
		{
			goto END;
		}
	}
	if( 0x00 == address_bytes[INET6_ADDRESS_SIZE - 1u] ||
		0x01 == address_bytes[INET6_ADDRESS_SIZE - 1u] )
	{
		// Technically the address of :: is all of the local interfaces or
		// it is an unassigned address. Thus, is not the loopback address but
		// would still be related to localhost so we will pass it. (For now)
		result = true;
	}
END:
	return result;
}

bool is_localhost_address(const struct sockaddr* const p_address)
{
	// Assumes address is in network order
	bool result = false;
	if(NULL == p_address)
	{
		goto END;
	}
	switch(p_address->sa_family)
	{
		case(AF_INET):
			result = is_localhost_address_v4((struct sockaddr_in*)p_address);
			break;
#ifdef AF_INET6
		case(AF_INET6):
			result = is_localhost_address_v6((struct sockaddr_in6*)p_address);
			break;
#endif//AF_INET6
		default:
			// Unknown/Unsupported socket family.
			break;
	}
END:
	return result;
}

bool is_ipv6_a_mapped_ipv4(const uint8_t p_address[INET6_ADDRESS_SIZE])
{
	// TODO IN6_IS_ADDR_V4MAPPED from <netinet/in.h> might also work.
	bool result = false;
	if(NULL == p_address)
	{
		goto END;
	}
	const size_t PREFIX_SIZE = 2u;
	const size_t PADDING_END = (
		INET6_ADDRESS_SIZE - PREFIX_SIZE - INET_ADDRESS_SIZE
	);
	const size_t INET_START = (PADDING_END + PREFIX_SIZE);
	// Validate zero padding
	for(size_t iii = 0u; iii < PADDING_END; iii++)
	{
		if(0u != p_address[iii])
		{
			goto END;
		}
	}
	// Validate 0xFF prefix
	for(size_t iii = PADDING_END; iii < INET_START; iii++)
	{
		if(__UINT8_MAX__ != p_address[iii])
		{
			goto END;
		}
	}
	result = true;
END:
	return result;
}

uint32_t ipv4_from_ipv6(const uint8_t p_address[INET6_ADDRESS_SIZE])
{
	// Assumes in network order. Returns host order.
	uint32_t result = UINT32_MAX;
	const bool is_mapped = is_ipv6_a_mapped_ipv4(p_address);
	if(false == is_mapped)
	{
		goto END;
	}
	const size_t INET_START = (
		INET6_ADDRESS_SIZE - INET_ADDRESS_SIZE
	);
	result = ntohl(*((uint32_t*)&p_address[INET_START]));
END:
	return result;
}

bool does_ipv6_map_to_ipv4(const struct sockaddr_in6* const p_v6addr,
	const struct sockaddr_in* const p_v4addr, const bool ignore_ports)
{
	// Assumes both are in network order.
	bool result = false;
	if((NULL == p_v6addr) || (NULL == p_v4addr))
	{
		goto END;
	}
	const uint8_t* const p_v6_bytes = (uint8_t*)&(p_v6addr->sin6_addr);
	const uint32_t mapped_address = ipv4_from_ipv6(p_v6_bytes);
	if(UINT32_MAX == mapped_address)
	{
		goto END;
	}
	const uint32_t addr = ntohl(*((uint32_t*)&(p_v4addr->sin_addr)));
	result = (mapped_address == addr);
	if((true != result) || (true == ignore_ports))
	{
		goto END;
	}
	result = (p_v4addr->sin_port == p_v6addr->sin6_port);
END:
	return result;
}

size_t sockaddr_sizeof(const sa_family_t family)
{
	size_t result = 0u;
	switch(family)
	{
		case(AF_INET):
			result = sizeof(struct sockaddr_in);
			break;
#ifdef AF_INET6
		case(AF_INET6):
			result = sizeof(struct sockaddr_in6);
			break;
#endif//AF_INET6
		default:
			// Unsupported Address type.
			break;
	}
	return result;
}

struct sockaddr* sockaddr_new(const sa_family_t family)
{
	struct sockaddr* result = NULL;
	const size_t addr_size = sockaddr_sizeof(family);
	if(0u >= addr_size)
	{
		goto END;
	}

	// Create/Allocate new address struct
	result = calloc(1u, addr_size);
	if(NULL == result)
	{
		goto END;
	}
	memset(result, 0x00, addr_size);

	// Initialize address values
	switch(family)
	{
		case(AF_INET):
			((struct sockaddr_in*)result)->sin_family = AF_INET;
			break;
#ifdef AF_INET6
		case(AF_INET6):
			((struct sockaddr_in6*)result)->sin6_family = AF_INET6;
			break;
#endif//AF_INET6
		default:
			// Unsupported Address type.
			break;
	}
END:
	return result;
}

int sockaddr_in_cmp(const struct sockaddr_in* const p_left,
	const struct sockaddr_in* const p_right, const bool ignore_ports)
{
	int result = 0;
	if(p_left == p_right)
	{
		goto END;
	}
	result = -1;
	if(NULL == p_left)
	{
		goto END;
	}
	// Both pointers are not NULL
	result = memcmp(
		&(p_left->sin_family),
		&(p_right->sin_family),
		sizeof(sa_family_t)
	);
	if(0 != result)
	{
		// Doesn't handle mapped addresses
		goto END;
	}
	result = memcmp(
		&(p_left->sin_addr),
		&(p_right->sin_addr),
		sizeof(struct in_addr)
	);
	if((0 != result) || (true == ignore_ports))
	{
		goto END;
	}
	result = memcmp(
		&(p_left->sin_port),
		&(p_right->sin_port),
		sizeof(in_port_t)
	);
END:
	return result;
}

int sockaddr_in6_cmp(const struct sockaddr_in6* const p_left,
	const struct sockaddr_in6* const p_right, const bool ignore_ports)
{
	int result = 0;
	if(p_left == p_right)
	{
		goto END;
	}
	result = -1;
	if(NULL == p_left)
	{
		goto END;
	}
	// Both pointers are not NULL
	result = memcmp(
		&p_left->sin6_family,
		&p_right->sin6_family,
		sizeof(sa_family_t)
	);
	if(0 != result)
	{
		// Doesn't handle mapped addresses
		goto END;
	}
	result = memcmp(
		&p_left->sin6_addr,
		&p_right->sin6_addr,
		sizeof(struct in6_addr)
	);
	if((0 != result) || (true == ignore_ports))
	{
		goto END;
	}
	result = memcmp(
		&p_left->sin6_port,
		&p_right->sin6_port,
		sizeof(in_port_t)
	);
END:
	return result;
}

int sockaddr_cmp(const struct sockaddr* const p_left,
	const struct sockaddr* const p_right, const bool ignore_ports)
{
	int result = 0;
	if((NULL == p_left) || (NULL == p_right))
	{
		goto END;
	}
	const sa_family_t left_family = p_left->sa_family;
	const sa_family_t right_family = p_right->sa_family;
	switch(left_family)
	{
		case(AF_INET):
			switch(right_family)
			{
				case(AF_INET):
					result = sockaddr_in_cmp(
						(struct sockaddr_in*)p_left,
						(struct sockaddr_in*)p_right,
						ignore_ports
					);
					break;
#ifdef AF_INET6
				case(AF_INET6):
					result = 1;
					const bool match = does_ipv6_map_to_ipv4(
						(const struct sockaddr_in6*)p_right,
						(const struct sockaddr_in*)p_left,
						ignore_ports
					);
					if(true == match)
					{
						result = 0;
					}
					break;
#endif//AF_INET6
				default:
					// Unsupported/Unknown family type
					break;
			}
			break;
#ifdef AF_INET6
		case(AF_INET6):
			switch(right_family)
			{
				case(AF_INET):
					result = -1;
					const bool match = does_ipv6_map_to_ipv4(
						(const struct sockaddr_in6*)p_left,
						(const struct sockaddr_in*)p_right,
						ignore_ports
					);
					if(true == match)
					{
						result = 0;
					}
					break;
				case(AF_INET6):
					result = sockaddr_in6_cmp(
						(struct sockaddr_in6*)p_left,
						(struct sockaddr_in6*)p_right,
						ignore_ports
					);
					break;
				default:
					// Unsupported/Unknown family type
					break;
			}
			break;
#endif//AF_INET6
		default:
			// Unknown/unsupported struct size.
			break;
	}
END:
	return result;
}

bool sockaddr_in_is_valid(struct sockaddr_in* const p_addr)
{
	bool result = false;
	if(NULL == p_addr)
	{
		goto END;
	}
	if((AF_INET != p_addr->sin_family) || (0 == p_addr->sin_port))
	{
		goto END;
	}
	// Doesn't validate sin_addr
	const size_t ZERO_SIZE = 8u;
	for(size_t iii = 0u; iii < ZERO_SIZE; iii++)
	{
		if(0 != p_addr->sin_zero[iii])
		{
			goto END;
		}
	}
	result = true;
END:
	return result;
}

bool sockaddr_in6_is_valid(struct sockaddr_in6* const p_addr)
{
	bool result = false;
	if(NULL == p_addr)
	{
		goto END;
	}
#ifdef AF_INET6
	if(AF_INET6 != p_addr->sin6_family)
	{
		goto END;
	}
#endif
	if(0 == p_addr->sin6_port)
	{
		goto END;
	}
	// Doesn't validate sin6_flowinfo
	// Doesn't validate sin6_addr
	// Doesn't validate sin6_scope_id
	result = true;
END:
	return result;
}

bool sockaddr_is_valid(struct sockaddr* const p_addr)
{
	bool result = false;
	if(NULL == p_addr)
	{
		goto END;
	}
	const sa_family_t family = p_addr->sa_family;
	switch(family)
	{
		case(AF_INET):
			result = sockaddr_in_is_valid((struct sockaddr_in*)p_addr);
			break;
#ifdef AF_INET6
		case(AF_INET6):
			result = sockaddr_in6_is_valid((struct sockaddr_in6*)p_addr);
			break;
#endif//AF_INET6
		default:
			// Unknown/unsupported struct size.
			break;
	}
END:
	return result;
}

void sin_addr_fprint(FILE* const p_file,
	const uint8_t p_address[INET_ADDRESS_SIZE])
{
	// Assumes address is in network order
	if((NULL == p_file) || (NULL == p_address))
	{
		goto END;
	}
	for(size_t iii = 0u; iii < INET_ADDRESS_SIZE; iii++)
	{
		fprintf(p_file, "%u", p_address[iii]);
		if(iii < (INET_ADDRESS_SIZE - 1u))
		{
			fprintf(p_file, ".");
		}
	}
	fflush(p_file);
END:
	return;
}

void sin6_addr_fprint(FILE* const p_file,
	const uint8_t p_address[INET6_ADDRESS_SIZE])
{
	// Assumes address is in network order
	if((NULL == p_file) || (NULL == p_address))
	{
		goto END;
	}
	// Uses IPv6 expanded address format
	for(size_t iii = 0u; iii < INET6_ADDRESS_SIZE; iii++)
	{
		fprintf(p_file, "%02x", p_address[iii]);
		if((iii < (INET6_ADDRESS_SIZE - 1u)) && (1u == (iii % 2u)))
		{
			fprintf(p_file, ":");
		}
	}
	// Handle IPv4 mapped addresses
	const bool is_mapped = is_ipv6_a_mapped_ipv4(p_address);
	if(true == is_mapped)
	{
		fprintf(p_file, "(");
		const uint32_t h_v4_addr = ipv4_from_ipv6(p_address);
		const uint32_t n_v4_addr = htonl(h_v4_addr);
		sin_addr_fprint(p_file, (const uint8_t*)&n_v4_addr);
		fprintf(p_file, ")");
	}
	fflush(p_file);
END:
	return;
}

void sockaddr_fprint(FILE* const p_file, const struct sockaddr* const p_addr)
{
	// Assumes address is in network order
	const sa_family_t family = p_addr->sa_family;
	switch(family)
	{
		case(AF_INET):
		{
			struct sockaddr_in* p_sai = (struct sockaddr_in*)p_addr;
			sin_addr_fprint(p_file, (uint8_t*)&p_sai->sin_addr);
			fprintf(p_file, ":%hu", p_sai->sin_port);
			break;
		}
#ifdef AF_INET6
		case(AF_INET6):
		{
			struct sockaddr_in6* p_sai6 = (struct sockaddr_in6*)p_addr;
			sin6_addr_fprint(p_file, (uint8_t*)&p_sai6->sin6_addr);
			fprintf(p_file, ":%hu", p_sai6->sin6_port);
			break;
		}
#endif//AF_INET6
		default:
			// Unknown/Unsupported family type.
			break;
	}
}

char* sockaddr_as_str(const struct sockaddr* const p_addr)
{
	char* p_result = NULL;
	if(NULL == p_addr)
	{
		goto END;
	}
	// Defined value of INET6_ADDRSTRLEN is incorrect
	const size_t INET6_ADDRSTR_LEN = (3u * INET6_ADDRESS_SIZE) - 1u;
	// Defined value of INET_ADDRSTRLEN is correct but lacks formatting.
	const size_t INET_ADDRSTR_LEN = (INET_ADDRSTRLEN - 1u) + 2u;
	const size_t PORT_STRLEN = (5u + 1u);// ":65535"
	const size_t MAX_STR_LEN = (
		INET6_ADDRSTR_LEN + INET_ADDRSTR_LEN + PORT_STRLEN + 1u
	);
	{
		char p_buffer[MAX_STR_LEN];
		memset(p_buffer, 0x00, MAX_STR_LEN);
		FILE* p_stream = fmemopen(p_buffer, MAX_STR_LEN, "r+");
		if(NULL == p_stream)
		{
			// Failed to open buffer as file.
			goto END;
		}
		sockaddr_fprint(p_stream, p_addr);
		fclose(p_stream);
		p_buffer[MAX_STR_LEN - 1u] = '\0';
		const size_t str_len = strnlen(p_buffer, MAX_STR_LEN - 1u);
		p_result = calloc(str_len + 1u, sizeof(char));
		if(NULL == p_result)
		{
			goto END;
		}
		memcpy(p_result, p_buffer, str_len);
		p_result[str_len] = '\0';
	}
END:
	return p_result;
}
