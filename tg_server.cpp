#include "tg_server.h"



int tg_server::serverCount = 0;



tg_server::tg_server(QString title_, QString module_, int port_)
{
title = title_;
module = module_;
port = port_;

serverCount++;


}


tg_server::~tg_server()
{

}



