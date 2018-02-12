#include <main.h>
#include <kernel/pit.h>

/**
 * Convert int to char array
 * @param  value  Input integer
 * @param  result Output buffer
 * @param  base   Number base (ex: base 10)
 * @return        Char array of number
 */
char* itoa(uint32_t value, char* result, int base) {
		// check that the base if valid
	if (base < 2 || base > 36) { *result = '\0'; return result; }

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );

		// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

char* utoa(uint32_t value, char* result, int base) {
		// check that the base if valid
	if (base < 2 || base > 36) { *result = '\0'; return result; }

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );

		// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len]) { len++; }
	return len;
}

// Sleep for the specified number of milliseconds.
void sleep(uint32_t ms)
{
	// 1 tick = 1 ms.
	uint64_t startTick = pit_ticks();
	uint64_t endTick = startTick + ms;
	while (pit_ticks() < endTick);
}
