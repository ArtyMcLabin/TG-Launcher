#ifndef TG_SERVER_H
#define TG_SERVER_H

#include <QString>



struct tg_server
{
    static int serverCount; //common count variable

    int playersCurrent = 0;
    int playersMax = 0;
    QString title = "unknown_server";
    QString map = "unknown_map";
    QString mode = "unknown_mode";
    QString module = "native";
    int port;

    tg_server(QString title_, QString module_, int port_);
    ~tg_server();

};

#endif // TG_SERVER_H
