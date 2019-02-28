#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore>

class Settings
{
public:
    QString name;
    quint64 name_unit;
    quint64 mask;
    quint8 id_bootloader;
    QString name_file;

    Settings();
    Settings(QString str);
};

#endif // SETTINGS_H
