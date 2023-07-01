# labwc-tweaks

This is a [WIP] configuration gui app for labwc without any real plan or
Acceptance Criteria. It was merely written to help change themes for the
purposes of testing labwc during development. Don't expect too much :smile:

<img src="https://user-images.githubusercontent.com/6955353/164969689-c2115c36-ad38-454c-b752-0356766156fb.png" />

<img src="https://user-images.githubusercontent.com/1019119/151689576-e9934712-f134-44a7-b5e7-3c05181b1a38.png" />

If you install labwc-gtktheme.py and set labwc-theme to GTK it'll
automatically sync with the selected GTK theme.

### build

```
meson build
ninja -C build
```

### install

```
ninja -C build install
```

This installs the binary to /usr/local/bin and data files to their respective locations.

If you find it a useful tool and want to expand its scope, feel free.

### packages

[![Packaging status](https://repology.org/badge/vertical-allrepos/labwc-tweaks.svg)](https://repology.org/project/labwc-tweaks/versions)
