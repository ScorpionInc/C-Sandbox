//si_realloc_settings.c

#include "si_realloc_settings.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


void si_resize_mode_fprint(FILE* p_file, const si_resize_mode resize_mode)
{
	// Validate parameter
	if(NULL == p_file)
	{
		goto END;
	}
	// Begin
	switch(resize_mode)
	{
		case(NEVER):
			fprintf(p_file, "%s", "NEVER");
			break;
		case(LINEAR):
			fprintf(p_file, "%s", "LINEAR");
			break;
		case(SCALAR):
			fprintf(p_file, "%s", "SCALAR");
			break;
		case(EXPONENTIAL):
			fprintf(p_file, "%s", "EXPONENTIAL");
			break;
		default:
			fprintf(p_file, "%s", "UNKNOWN");
			break;
	}
	// End
END:
	return;
}


void si_realloc_settings_new(si_realloc_settings* p_settings)
{
	// Validate parameter
	if (NULL == p_settings)
	{
		goto END;
	}
	// Begin
	p_settings->grow_mode = SI_DEFAULT_RESIZE_MODE;
	p_settings->grow_value = SI_DEFAULT_RESIZE_VALUE;
	p_settings->shrink_mode = SI_DEFAULT_RESIZE_MODE;
	p_settings->shrink_value = SI_DEFAULT_RESIZE_VALUE;
	// End
END:
	return;
}

size_t si_realloc_next_grow_capacity(const si_realloc_settings* p_settings,
	const size_t current_capacity)
{
	size_t new_capacity = current_capacity;
	if (NULL == p_settings)
	{
		goto END;
	}
	// Begin
	switch (p_settings->grow_mode)
	{
		case NEVER:
			break;
		case LINEAR:
			// Prevent Overflows
			if (SIZE_MAX - p_settings->grow_value < new_capacity)
			{
				// Overflow detected
				new_capacity = SIZE_MAX;
				break;
			}
			new_capacity += p_settings->grow_value;
			break;
		case SCALAR:
			// Prevent Overflows
			if (p_settings->grow_value != 0.0f)
			{
				// Don't divide by zero.
				if ((new_capacity * p_settings->grow_value) /
						p_settings->grow_value != new_capacity )
				{
					// Overflow detected
					new_capacity = SIZE_MAX;
					break;
				}
			}
			new_capacity *= p_settings->grow_value;
			break;
		case EXPONENTIAL:
			// Prevent Overflows
			size_t calculated_result =
				pow(new_capacity, p_settings->grow_value);
			if (calculated_result < new_capacity)
			{
				// Wrap-around detected Overflow
				calculated_result = SIZE_MAX;
			}
			new_capacity = calculated_result;
			break;
		default:
			// Unknown grow mode
			//goto END;
			break;
	}
	// End
END:
	return new_capacity;
}

size_t si_realloc_next_shrink_capacity(const si_realloc_settings* p_settings,
	const size_t current_capacity)
{
	size_t new_capacity = current_capacity;
	if (NULL == p_settings)
	{
		goto END;
	}
	// Begin
	switch (p_settings->shrink_mode)
	{
		case NEVER:
			//goto END;
			break;
		case LINEAR:
			// Prevent Underflow
			if (p_settings->shrink_value > new_capacity)
			{
				// Underflow detected
				new_capacity = 0u;
				break;
			}
			new_capacity -= p_settings->shrink_value;
			break;
		case SCALAR:
			// Prevent Underflow
			if (p_settings->shrink_value != 0.0f)
			{
				// Don't divide by zero.
				if ((new_capacity / p_settings->shrink_value) *
						p_settings->shrink_value != new_capacity)
				{
					// Underflow detected
					new_capacity = 0u;
					break;
				}
			}
			new_capacity /= p_settings->shrink_value;
			break;
		case EXPONENTIAL:
			// Prevent Underflow
			size_t calculated_result = new_capacity;
			if (p_settings->shrink_value != 0.0f)
			{
				// Don't divide by zero.
				calculated_result = pow(new_capacity,
					1.0f / p_settings->shrink_value);
				if (calculated_result > new_capacity)
				{
					// Wrap-around detected Underflow
					calculated_result = 0u;
				}
			}
			new_capacity = calculated_result;
			break;
		default:
			// Unknown grow mode
			//goto END;
			break;
	}
	// End
END:
	return new_capacity;
}

void si_realloc_settings_fprint(FILE* p_file,
	const si_realloc_settings* p_settings)
{
	// Validate parameters
	if((NULL == p_file) || (NULL == p_settings))
	{
		goto END;
	}
	// Begin
	fprintf(p_file, "{Grow : ");
	si_resize_mode_fprint(p_file, p_settings->grow_mode);
	fprintf(p_file, " %f; ", p_settings->grow_value);
	fprintf(p_file, "Shrink: ");
	si_resize_mode_fprint(p_file, p_settings->shrink_mode);
	fprintf(p_file, " %f}", p_settings->shrink_value);
	// End
END:
	return;
}


#ifdef __cplusplus
}
#endif //__cplusplus
