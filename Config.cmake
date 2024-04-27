#===============================================================================
# Editable project configuration
#
# Essential, non translatable application information (except DESCRIPTION).
# Translatable strings are passed via code.
#===============================================================================
set(PROJECT_ID "labwc-tweaks")
list(APPEND PROJECT_CATEGORIES "Qt;Settings;DesktopSettings;") # Freedesktop menu categories
list(APPEND PROJECT_KEYWORDS   "labwc;wayland;compositor")
set(PROJECT_AUTHOR_NAME        "Labwc Team")
set(PROJECT_COPYRIGHT_YEAR     "2024")  # TODO: from git
set(PROJECT_DESCRIPTION        "Labwc Wayland compositor settings")
set(PROJECT_ORGANIZATION_NAME  "labwc")
set(PROJECT_ORGANIZATION_URL   "${PROJECT_ORGANIZATION_NAME}.github.io")
set(PROJECT_ORGANIZATION_ID    "io.github.${PROJECT_ORGANIZATION_NAME}")
set(PROJECT_REPOSITORY_URL     "https://github.com/${PROJECT_ORGANIZATION_NAME}/${PROJECT_ID}")
set(PROJECT_REPOSITORY_BRANCH  "master")
set(PROJECT_HOMEPAGE_URL       ${PROJECT_REPOSITORY_URL}) # TODO: "https://${PROJECT_ORGANIZATION_URL}/${PROJECT_ID}"
set(PROJECT_SPDX_ID            "GPL-2.0-only")
set(PROJECT_TRANSLATIONS_DIR   "${CMAKE_SOURCE_DIR}/data/translations")
set(PROJECT_SCREENSHOT_URL     "https://github-production-user-asset-6210df.s3.amazonaws.com/1019119/294060534-84ef3747-f336-444e-9e2c-9a417ebe67e5.png")
#===============================================================================
# Appstream
#===============================================================================
set(PROJECT_APPSTREAM_SPDX_ID  "CC0-1.0")
set(PROJECT_APPSTREAM_ID       "labwc_tweaks")
#===============================================================================
# Adapt to CMake variables
#===============================================================================
set(${PROJECT_NAME}_DESCRIPTION  "${PROJECT_DESCRIPTION}")
set(${PROJECT_NAME}_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
