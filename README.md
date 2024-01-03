# labwc-tweaks

This is a [WIP] configuration gui app for labwc without any real plan or
Acceptance Criteria. It was merely written to help change themes for the
purposes of testing labwc during development. Don't expect too much :smile:

<img src="https://github-production-user-asset-6210df.s3.amazonaws.com/1019119/294060534-84ef3747-f336-444e-9e2c-9a417ebe67e5.png" />

If you install labwc-gtktheme.py and set labwc-theme to GTK it'll
automatically sync with the selected GTK theme.

### build

```
meson setup build
meson compile -C build
```

### install

```
meson install -C build
```

This installs the binary to /usr/local/bin and data files to their respective locations.

If you find it a useful tool and want to expand its scope, feel free.

### packages

[![Packaging status](https://repology.org/badge/vertical-allrepos/labwc-tweaks.svg)](https://repology.org/project/labwc-tweaks/versions)
