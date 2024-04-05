#include <server.h>

Server::Server()
{
    nextBlockSize = 0;

    if (this->listen(QHostAddress::Any, 2323))
    {
        qDebug() << "Start server...";
    }

    else
    {
        qDebug() << "Error start server...";
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &Server::deleteLater);
    Sockets.push_back(socket);
    qDebug() << "Client is connected..." << socketDescriptor;
}

void Server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_2);

    if (in.status() == QDataStream::Ok)
    {
        for (;;)
        {
            if (nextBlockSize == 0)
            {
                qDebug() << "nextBlockSize = 0";

                if (socket->bytesAvailable() < 2)
                {
                    qDebug() << "Data < 2 bytes, break";
                    break;
                }
                in >> nextBlockSize;
                qDebug() << "nextBlockSize = " << nextBlockSize;
            }

            if (socket->bytesAvailable() < nextBlockSize)
            {
                qDebug() << "Data is not full, break";
                break;
            }

            QString mssng;
            in >> mssng;
            nextBlockSize = 0;
            qDebug() << mssng;
            SendToClient(mssng);
            break;
        }
    }

    else
    {
        qDebug() << "DataStream eror...";
    }
}

void Server::SendToClient(QString mssng)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << quint16(0) << mssng;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    for (int number = 0; number < Sockets.size(); number++)
    {
        Sockets[number]->write(Data);
    }
}
