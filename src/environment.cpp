// SPDX-License-Identifier: GPL-2.0-only
#define _POSIX_C_SOURCE 200809L
#include <iostream>
#include <fstream>
#include <QDebug>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <cstring>
#include <vector>
#include <memory>
#include "log.h"

class Line
{
public:
    Line();
    ~Line();

    QString data;
    bool isKeyValuePair;
    QString key;
    QString value;
};

static std::vector<std::unique_ptr<Line>> lines;

Line::Line(void)
{
    isKeyValuePair = false;
}

Line::~Line(void) { };

QString environmentGet(QString key)
{
    for (auto &line : lines) {
        if (!line->isKeyValuePair)
            continue;
        if (line->key == key)
            return line->value;
    }
    return nullptr;
}

int environmentGetInt(QString key)
{
    for (auto &line : lines) {
        if (!line->isKeyValuePair) {
            continue;
        }
        if (line->key == key) {
            // TODO: Not ideal
            return atoi(line->value.toStdString().c_str());
        }
    }
    return -1;
}

void environmentSet(QString key, QString value)
{
    if (key.isEmpty() || value.isEmpty()) {
        return;
    }
    for (auto &line : lines) {
        if (!line->isKeyValuePair) {
            continue;
        }
        if (line->key == key) {
            // Modify existing key=value pair
            line->value = value;
            return;
        }
    }

    // Append
    lines.push_back(std::make_unique<Line>());
    lines.back()->isKeyValuePair = true;
    lines.back()->key = key;
    lines.back()->value = value;
}

void environmentSetInt(QString key, int value)
{
    char buffer[255];
    snprintf(buffer, 255, "%d", value);

    environmentSet(key, QString(buffer));
}

static void processLine(QString line)
{
    lines.push_back(std::make_unique<Line>());
    lines.back()->data = line;
    if (line.isEmpty() || line.startsWith("#") || !line.contains("=")) {
        return;
    }
    lines.back()->isKeyValuePair = true;
    QStringList elements = line.split('=');
    lines.back()->key = elements[0].trimmed();
    lines.back()->value = elements[1].trimmed();
}

void environmentInit(std::string filename)
{
    if (access(filename.c_str(), F_OK)) {
        info("environment file not found '{}'", filename);
        return;
    }

    std::string line;
    std::ifstream stream(filename);
    while (getline(stream, line)) {
        processLine(QString(line.c_str()));
    }
    stream.close();
}

void environmentSave(std::string filename)
{
    std::ofstream ofs(filename);
    for (auto &line : lines) {
        if (!line->isKeyValuePair) {
            ofs << line->data.toStdString() << std::endl;
        } else {
            ofs << line->key.toStdString() << "=" << line->value.toStdString() << std::endl;
        }
    }
    ofs.close();
}
