// SPDX-License-Identifier: GPL-2.0-only
#define _POSIX_C_SOURCE 200809L
#include <QDebug>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <cstring>

QString environment_get(const char *key)
{
    QString filename = qgetenv("HOME") + "/.config/labwc/environment";
    QFile inputFile(filename);
    inputFile.open(QIODevice::ReadOnly);
    if (!inputFile.isOpen()) {
        return nullptr;
    }

    QTextStream stream(&inputFile);
    QString line;
    while (stream.readLineInto(&line)) {
        if (line.isEmpty() || line.startsWith("#")) {
            continue;
        }

        QStringList elements = line.split('=');
        if (elements.count() != 2) {
            continue;
        }
        if (elements[0].trimmed() == QString(key)) {
            qDebug() << "fount it!" << elements;
            return elements[1].trimmed();
        }
    }
    return nullptr;
}

void environment_set(const char *key, const char *value)
{
    /* set cursor for labwc  - should cover 'replace' or 'append' */
    char xcur[4096] = { 0 };
    strcpy(xcur, key);
    strcat(xcur, "=");
    char filename[4096];
    char bufname[4096];
    char *home = getenv("HOME");
    snprintf(filename, sizeof(filename), "%s/%s", home, ".config/labwc/environment");
    snprintf(bufname, sizeof(bufname), "%s/%s", home, ".config/labwc/buf");
    FILE *fe = fopen(filename, "r");
    FILE *fw = fopen(bufname, "a");
    if ((fe == NULL) || (fw == NULL)) {
        perror("Unable to open file!");
        return;
    }
    char chunk[128];
    while (fgets(chunk, sizeof(chunk), fe) != NULL) {
        if (strstr(chunk, xcur) != NULL) {
            continue;
        } else {
            fprintf(fw, "%s", chunk);
        }
    }
    fclose(fe);
    if (value) {
        fprintf(fw, "%s\n", strcat(xcur, value));
    }
    fclose(fw);
    rename(bufname, filename);
}

void environment_set_num(const char *key, int value)
{
    char buffer[255];
    snprintf(buffer, 255, "%d", value);

    environment_set(key, buffer);
}
