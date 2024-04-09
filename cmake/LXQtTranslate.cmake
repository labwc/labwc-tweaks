#=============================================================================
# Copyright 2014 Luís Pereira <luis.artur.pereira@gmail.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================
#
# funtion lxqt_translate_ts(qmFiles
#                           [UPDATE_TRANSLATIONS [Yes | No]]
#                           SOURCES <sources>
#                           [UPDATE_OPTIONS] update_options
#                           [TEMPLATE] translation_template
#                           [TRANSLATION_DIR] translation_directory
#                           [INSTALL_DIR] install_directory
#                           [COMPONENT] component
#                    )
#     Output:
#       qmFiles The generated compiled translations (.qm) files
#
#       UPDATE_TRANSLATIONS Optional flag. Setting it to Yes, extracts and
#                           compiles the translations. Setting it No, only
#                           compiles them.
#
#       UPDATE_OPTIONS Optional options to lupdate when UPDATE_TRANSLATIONS
#                       is True.
#
#       TEMPLATE Optional translations files base name. Defaults to
#                ${PROJECT_NAME}. An .ts extensions is added.
#
#       TRANSLATION_DIR Optional path to the directory with the .ts files,
#                        relative to the CMakeList.txt. Defaults to
#                        "translations".
#
#       INSTALL_DIR Optional destination of the file compiled files (qmFiles).
#                    If not present no installation is performed
#
#       COMPONENT Optional install component. Only effective if INSTALL_DIR
#                   present. Defaults to "Runtime".
#
function(lxqt_translate_ts qmFiles)
    set(oneValueArgs
        UPDATE_TRANSLATIONS
        TEMPLATE
        TRANSLATION_DIR
        INSTALL_DIR
        COMPONENT
    )
    set(multiValueArgs SOURCES UPDATE_OPTIONS)
    cmake_parse_arguments(TR "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT DEFINED TR_UPDATE_TRANSLATIONS)
        set(TR_UPDATE_TRANSLATIONS "No")
    endif()

    if (NOT DEFINED TR_UPDATE_OPTIONS)
        set(TR_UPDATE_OPTIONS "")
    endif()


    if(NOT DEFINED TR_TEMPLATE)
        set(TR_TEMPLATE "${PROJECT_NAME}")
    endif()

    if (NOT DEFINED TR_TRANSLATION_DIR)
        set(TR_TRANSLATION_DIR "translations")
    endif()
    get_filename_component(TR_TRANSLATION_DIR "${TR_TRANSLATION_DIR}" ABSOLUTE)

    if (EXISTS "${TR_TRANSLATION_DIR}")
        file(GLOB tsFiles "${TR_TRANSLATION_DIR}/${TR_TEMPLATE}_*.ts")
        set(templateFile "${TR_TRANSLATION_DIR}/${TR_TEMPLATE}.ts")
    endif ()

    if (TR_UPDATE_TRANSLATIONS)
        qt6_create_translation(QMS
            ${TR_SOURCES}
            ${templateFile}
            OPTIONS ${TR_UPDATE_OPTIONS}
        )
        qt6_create_translation(QM
            ${TR_SOURCES}
            ${tsFiles}
            OPTIONS ${TR_UPDATE_OPTIONS}
        )
    else()
        qt6_add_translation(QM ${tsFiles})
    endif()

    if(TR_UPDATE_TRANSLATIONS)
        add_custom_target("update_${TR_TEMPLATE}_ts" ALL DEPENDS ${QMS})
    endif()

    if(DEFINED TR_INSTALL_DIR)
        if(NOT DEFINED TR_COMPONENT)
            set(TR_COMPONENT "Runtime")
        endif()

        install(FILES ${QM}
            DESTINATION "${TR_INSTALL_DIR}"
            COMPONENT "${TR_COMPONENT}"
        )
    endif()

    set(${qmFiles} ${QM} PARENT_SCOPE)
endfunction()
#=============================================================================
# The lxqt_translate_desktop() function was copied from the
# LXQt LXQtTranslate.cmake
#
# Original Author: Alexander Sokolov <sokoloff.a@gmail.com>
#
# funtion lxqt_translate_desktop(_RESULT
#                           SOURCES <sources>
#                           [TRANSLATION_DIR] translation_directory
#                           [USE_YAML]
#                    )
#     Output:
#       _RESULT The generated .desktop (.desktop) files
#
#     Input:
#
#       SOURCES List of input desktop files (.destktop.in) to be translated
#               (merged), relative to the CMakeList.txt.
#
#       TRANSLATION_DIR Optional path to the directory with the .ts files,
#                        relative to the CMakeList.txt. Defaults to
#                        "translations".
#
#       USE_YAML Flag if *.desktop.yaml translation should be used.
#=============================================================================
find_package(Perl REQUIRED)

function(lxqt_translate_desktop _RESULT)
    # Parse arguments ***************************************
    set(options USE_YAML)
    set(oneValueArgs TRANSLATION_DIR)
    set(multiValueArgs SOURCES)

    cmake_parse_arguments(_ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # check for unknown arguments
    set(_UNPARSED_ARGS ${_ARGS_UNPARSED_ARGUMENTS})
    if (NOT ${_UNPARSED_ARGS} STREQUAL "")
        MESSAGE(FATAL_ERROR
          "Unknown arguments '${_UNPARSED_ARGS}'.\n"
          "See lxqt_translate_desktop() documentation for more information.\n"
        )
    endif()

    if (NOT DEFINED _ARGS_SOURCES)
        set(${_RESULT} "" PARENT_SCOPE)
        return()
    else()
        set(_sources ${_ARGS_SOURCES})
    endif()

    if (NOT DEFINED _ARGS_TRANSLATION_DIR)
        set(_translationDir "translations")
    else()
        set(_translationDir ${_ARGS_TRANSLATION_DIR})
    endif()

    get_filename_component (_translationDir ${_translationDir} ABSOLUTE)

    foreach (_inFile ${_sources})
        get_filename_component(_inFile   ${_inFile} ABSOLUTE)
        get_filename_component(_fileName ${_inFile} NAME_WE)
        #Extract the real extension ............
        get_filename_component(_fileExt  ${_inFile} EXT)
        string(REPLACE ".in" "" _fileExt ${_fileExt})
        string(REGEX REPLACE "^\\.([^.].*)$" "\\1" _fileExt ${_fileExt})
        #.......................................
        set(_outFile "${CMAKE_CURRENT_BINARY_DIR}/${_fileName}.${_fileExt}")

        if (_ARGS_USE_YAML)
            add_custom_command(OUTPUT ${_outFile}
                COMMAND ${PERL_EXECUTABLE} ${CMAKE_SOURCE_DIR}/cmake/LXQtTranslateDesktopYaml.pl ${_inFile} ${_fileName} ${_translationDir}/${_fileName}[_.]*${_fileExt}.yaml >> ${_outFile}
                VERBATIM
                COMMENT "Generating ${_fileName}.${_fileExt}"
            )
        else ()
            file(GLOB _translations
                ${_translationDir}/${_fileName}[_.]*${_fileExt}
            )
            list(SORT _translations)
            add_custom_command(OUTPUT ${_outFile}
                COMMAND grep -v -a "#TRANSLATIONS_DIR=" ${_inFile} > ${_outFile}
                VERBATIM
                COMMENT "Generating ${_fileName}.${_fileExt}"
            )
            if (_translations)
                add_custom_command(OUTPUT ${_outFile}
                    COMMAND grep -h -a "\\[.*]\\s*=" ${_translations} >> ${_outFile}
                    VERBATIM APPEND
                )
            endif ()
        endif ()

        set(__result ${__result} ${_outFile})
    endforeach()

    set(${_RESULT} ${__result} PARENT_SCOPE)
endfunction(lxqt_translate_desktop)
