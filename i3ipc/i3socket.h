#ifndef I3SOCKET_H
#define I3SOCKET_H

#include <QtNetwork/QLocalSocket>

class I3Socket : public QLocalSocket
{
Q_OBJECT

public:
    I3Socket(QObject *parent);

    void sendMessage(quint32 messageType, const char *payload);

private:
    QString const getSocketPath(void);
};

#endif /* I3SOCKET_H */
