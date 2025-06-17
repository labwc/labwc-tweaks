# labwc-tweaks

[![CI]](https://github.com/labwc/labwc-tweaks/actions/workflows/build.yml)

This is a GUI 'Settings' application for labwc.

<img src="https://private-user-images.githubusercontent.com/6955353/456192696-a8070566-5676-4932-a187-9e984943dfdd.png?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NTAxOTE0NDcsIm5iZiI6MTc1MDE5MTE0NywicGF0aCI6Ii82OTU1MzUzLzQ1NjE5MjY5Ni1hODA3MDU2Ni01Njc2LTQ5MzItYTE4Ny05ZTk4NDk0M2RmZGQucG5nP1gtQW16LUFsZ29yaXRobT1BV1M0LUhNQUMtU0hBMjU2JlgtQW16LUNyZWRlbnRpYWw9QUtJQVZDT0RZTFNBNTNQUUs0WkElMkYyMDI1MDYxNyUyRnVzLWVhc3QtMSUyRnMzJTJGYXdzNF9yZXF1ZXN0JlgtQW16LURhdGU9MjAyNTA2MTdUMjAxMjI3WiZYLUFtei1FeHBpcmVzPTMwMCZYLUFtei1TaWduYXR1cmU9N2M3NjgzYjY0NzY1NDc4OTNlMGZkZDMzNWIyZDIwNTVjNzY2MTcwMGFlMWUzZTA1OWI3OTZjMmY0MjViMGNmOSZYLUFtei1TaWduZWRIZWFkZXJzPWhvc3QifQ.kjAiUZyN771SQ3T7_MNY2rTFPFzV9ABiBzImu9MCyfI"/>

### dependencies

Runtime:

- Qt6 base
- libxml2
- glib2

Build:

- CMake
- Qt Linguist Tools
- Git (optional, to pull latest VCS checkouts)

### build

`CMAKE_BUILD_TYPE` is usually set to `Release`, though `None` might be a valid [alternative].<br>
`CMAKE_INSTALL_PREFIX` has to be set to `/usr` on most operating systems.

```bash
cmake -B build -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr -W no-dev
cmake --build build --verbose
```

### test

```bash
ctest --verbose --force-new-ctest-process --test-dir build
```

### install

Using `sudo make install` is discouraged, instead use the system package manager where possible.

In this packaging simulation, CMake installs the binary to /usr/bin
and data files to their respective locations in a "package" directory.

```bash
DESTDIR="$(pwd)/package" cmake --install build
```

If you find it a useful tool and want to expand its scope, feel free.

### packages

[![Packaging status]](https://repology.org/project/labwc-tweaks/versions)

### translations

For contributing translations the [LXQt Weblate] platform can be used.

[![Translation status]](https://translate.lxqt-project.org/widgets/labwc/)

### licenses

- labwc-tweaks is licensed under the [GPL-2.0-only] license
- LXQt build tools cmake modules are licensed under the [BSD-3-Clause] license.


[alternative]:          https://wiki.archlinux.org/title/CMake_package_guidelines#Fixing_the_automatic_optimization_flag_override
[BSD-3-Clause]:         BSD-3-Clause
[CI]:                   https://github.com/labwc/labwc-tweaks/actions/workflows/build.yml/badge.svg
[GPL-2.0-only]:         LICENSE
[LXQt Weblate]:         https://translate.lxqt-project.org/projects/labwc/labwc-tweaks/
[Packaging status]:     https://repology.org/badge/vertical-allrepos/labwc-tweaks.svg
[Translation status]:   https://translate.lxqt-project.org/widgets/labwc/-/labwc-tweaks/multi-auto.svg
