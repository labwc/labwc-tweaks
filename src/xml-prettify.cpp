// SPDX-License-Identifier: GPL-2.0-only
#define _POSIX_C_SOURCE 200809L
#include "xml-prettify.h"
#include <iostream>
#include <fstream>
#include <string>

void xml_prettify(const char *filename)
{
    char const *noPrettify = getenv("LABWC_TWEAKS_NO_PRETTIFY");
    if (noPrettify) {
        return;
    }

    std::string line, output;
    bool firstLine = true;

    std::ifstream in(filename);
    if (!in) {
        std::cerr << "warn: no file '" << filename << "'" << std::endl;
    }
    while (getline(in, line)) {
        // Remove the <?xml version="1.0"?> line at the start
        if (firstLine) {
            firstLine = false;
            continue;
        }

        // Convert <foo/> to <foo />
        for (size_t i = 0; i < line.size(); ++i) {
            if (line[i] == '/' && line[i + 1] == '>') {
                output += " />";
                i += 1;
                continue;
            }
            output += line[i];
        }
        output += '\n';
    }
    in.close();

    // Overwrite
    std::ofstream out(filename, std::ios::trunc);
    out << output;
    out.close();
}
