#include "settings.h"

Settings::Settings()
{

}

Settings::Settings(QString str)
{
    QString buf(str);
    QString s;
    bool flag;
    int n = 0;
    int n1 = 0;

    n = buf.indexOf(';');
    name = buf.mid(0, n);

    n1 = buf.indexOf(';', n+1);
    s = buf.mid(n+1, n1-n-1);
    if(s.indexOf('x') > -1)
        name_unit = s.toULongLong(&flag, 16);
    else
        name_unit = s.toULongLong();

    n = n1;
    n1 = buf.indexOf(';', n+1);
    s = buf.mid(n+1, n1-n-1);
    if(s.indexOf('x') > -1)
        mask = s.toULongLong(&flag, 16);
    else
        mask = s.toULongLong();

    n = n1;
    n1 = buf.indexOf(';', n+1);
    s = buf.mid(n+1, n1-n-1);
    if(s.indexOf('x') > -1)
        id_bootloader = s.toInt(&flag, 16);
    else
        id_bootloader = s.toInt();

    name_file = str.mid(n1+1, buf.length() - n1 - 1);
    name_file = name_file.trimmed();
}
