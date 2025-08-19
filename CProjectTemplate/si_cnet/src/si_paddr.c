//si_paddr.c

#include "si_paddr.h"

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
		if(p_address[iii] != 0u)
		{
			printf("is_ipv6_mapped_ipv4() False padding[%lu] not zero: %x.\n", iii, p_address[iii]);//Debugging
			goto END;
		}
	}
	// Validate 0xFF prefix
	for(size_t iii = PADDING_END; iii < INET_START; iii++)
	{
		if(p_address[iii] != __UINT8_MAX__)
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
	uint32_t result = UINT32_MAX;
	if(false == is_ipv6_a_mapped_ipv4(p_address))
	{
		goto END;
	}
	const size_t INET_START = (
		INET6_ADDRESS_SIZE - INET_ADDRESS_SIZE
	);
	result = *((uint32_t*)&p_address[INET_START]);
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
	const struct sockaddr_in* const p_right)
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
	if(0 != result)
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
	const struct sockaddr_in6* const p_right)
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
		goto END;
	}
	result = memcmp(
		&p_left->sin6_addr,
		&p_right->sin6_addr,
		sizeof(struct in6_addr)
	);
	if(0 != result)
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
	const struct sockaddr* const p_right, const sa_family_t family)
{
	int result = 0;
	switch(family)
	{
		case(AF_INET):
			result = sockaddr_in_cmp(
				(struct sockaddr_in*)p_left,
				(struct sockaddr_in*)p_right
			);
			break;
#ifdef AF_INET6
		case(AF_INET6):
			result = sockaddr_in6_cmp(
				(struct sockaddr_in6*)p_left,
				(struct sockaddr_in6*)p_right
			);
			break;
#endif//AF_INET6
		default:
			// Unknown/unsupported struct size.
			break;
	}
	return result;
}

bool sockaddr_in_is_valid(struct sockaddr_in* const p_addr)
{
	bool result = false;
	if(NULL == p_addr)
	{
		goto END;
	}
	if(AF_INET != p_addr->sin_family)
	{
		goto END;
	}
	if(0 == p_addr->sin_port)
	{
		goto END;
	}
	// Doesn't validate sin_addr
	const size_t ZERO_SIZE = 8u;
	for(size_t i = 0u; i < ZERO_SIZE; i++)
	{
		if(0 != p_addr->sin_zero[i])
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
	if(AF_INET6 != p_addr->sin6_family)
	{
		goto END;
	}
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

bool sockaddr_is_valid(struct sockaddr* const p_addr,
	const sa_family_t family)
{
	bool result = false;
	if(NULL == p_addr)
	{
		goto END;
	}
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
	if((NULL == p_file) || (NULL == p_address))
	{
		goto END;
	}
	for(size_t i = 0u; i < INET_ADDRESS_SIZE; i++)
	{
		fprintf(p_file, "%u", p_address[i]);
		if(i < (INET_ADDRESS_SIZE - 1u))
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
	if((NULL == p_file) || (NULL == p_address))
	{
		goto END;
	}
	// Uses IPv6 expanded address format
	for(size_t i = 0u; i < INET6_ADDRESS_SIZE; i++)
	{
		fprintf(p_file, "%02x", p_address[i]);
		if((i < (INET6_ADDRESS_SIZE - 1u)) && (1u == (i % 2u)))
		{
			fprintf(p_file, ":");
		}
	}
	// Handle IPv4 mapped addresses
	if(is_ipv6_a_mapped_ipv4(p_address))
	{
		fprintf(p_file, "(");
		uint32_t v4_addr = ipv4_from_ipv6(p_address);
		sin_addr_fprint(p_file, (uint8_t*)&v4_addr);
		fprintf(p_file, ")");
	}
	fflush(p_file);
END:
	return;
}

void sockaddr_fprint(FILE* const p_file, const struct sockaddr* const p_addr,
	const socklen_t sock_len)
{
	switch(sock_len)
	{
		case(sizeof(struct sockaddr_in)):
			struct sockaddr_in* p_sai = (struct sockaddr_in*)p_addr;
			sin_addr_fprint(p_file, (uint8_t*)&p_sai->sin_addr);
			fprintf(p_file, ":%hu", p_sai->sin_port);
			break;
#ifdef AF_INET6
		case(sizeof(struct sockaddr_in6)):
			struct sockaddr_in6* p_sai6 = (struct sockaddr_in6*)p_addr;
			sin6_addr_fprint(p_file, (uint8_t*)&p_sai6->sin6_addr);
			fprintf(p_file, ":%hu", p_sai6->sin6_port);
			break;
#endif//AF_INET6
		default:
			// Unknown/Unsupported family type.
			break;
	}
}
