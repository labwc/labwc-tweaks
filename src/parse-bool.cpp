#include <cstring>
#include "log.h"
#include "parse-bool.h"

int parseBool(const char *str, int defaultValue)
{
    if (!str)
        goto error_not_a_boolean;
    else if (!strcasecmp(str, "yes"))
        return 1;
    else if (!strcasecmp(str, "true"))
        return 1;
    else if (!strcasecmp(str, "on"))
        return 1;
    else if (!strcmp(str, "1"))
        return 1;
    else if (!strcasecmp(str, "no"))
        return 0;
    else if (!strcasecmp(str, "false"))
        return 0;
    else if (!strcasecmp(str, "off"))
        return 0;
    else if (!strcmp(str, "0"))
        return 0;
error_not_a_boolean:
    warn("{} is not a boolean value", str);;
    return defaultValue;
}


