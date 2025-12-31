// SPDX-License-Identifier: GPL-2.0-only
#include "nodename.h"
#include <algorithm>
#include <cassert>
#include <iterator>
#include <ranges>
#include <string>
#include <vector>

static std::vector<std::string> splitIgnoringEmptyParts(const std::string &s, char delim)
{
    auto has_content = [](auto const &s) { return s.size() > 0; };
    auto parts = s | std::views::split(delim) | std::views::filter(has_content)
            | std::ranges::to<std::vector<std::string>>();
    return parts;
}

std::string nodenameFromXPath(std::string xpath)
{
    auto parts = splitIgnoringEmptyParts(xpath, '/');
    std::reverse(parts.begin(), parts.end());
    std::string nodename;
    for (auto part : parts) {
        nodename.append(part);
        nodename.append(".");
    }
    if (!nodename.empty() && nodename.back() == '.') {
        nodename.pop_back();
    }
    return nodename;
}
