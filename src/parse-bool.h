
#pragma once

/**
 * parseBool() - Parse boolean value of string.
 * @string: String to interpret. This check is case-insensitive.
 * @default_value: Default value to use if string is not a recognised boolean.
 *                 Use -1 to avoid setting a default value.
 *
 * Return: 0 for false; 1 for true; -1 for non-boolean
 */
int parseBool(const char *str, int defaultValue);
