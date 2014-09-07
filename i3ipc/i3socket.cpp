#include "i3socket.h"

#include <QtX11Extras/QX11Info>

#include <kwindowsystem.h>
#include <kservicetype.h>

#include <netwm.h>

#include <i3/ipc.h>

I3Socket::I3Socket(QObject *parent)
        : QLocalSocket(parent)
{
    QString socketPath = getSocketPath();
    qDebug() << "Found socket: " << socketPath;
    this->connectToServer(socketPath);
}

void I3Socket::sendMessage(quint32 messageType, const char *payload)
{
    QByteArray message;
    message.append(I3_IPC_MAGIC);
    quint32 messageLength = strlen(payload);
    message.append((const char *) &messageLength, sizeof(messageLength));
    message.append((const char *) &messageType, sizeof(messageType));
    message.append(payload);
    qint64 writtenLength = this->write(message);
    qDebug() << writtenLength << " bytes written.";
    this->flush();
}

QString const I3Socket::getSocketPath(void)
{
    // TODO: can this be done with kwindowsystem without any direct xcb_calls?
    xcb_intern_atom_reply_t *atomReply;
    char *socketPath;
    char *atomName = (char *) "I3_SOCKET_PATH";
    size_t contentMaxWords = 256;
    xcb_intern_atom_cookie_t atomCookie;
    xcb_window_t rootWindow;
    xcb_get_property_cookie_t propertyCookie;
    xcb_get_property_reply_t *propertyReply;

    xcb_connection_t *connection = QX11Info::connection();
    NETRootInfo info(connection, 0);
    rootWindow = info.rootWindow();

    atomCookie = xcb_intern_atom(connection, 0, strlen(atomName), atomName);
    atomReply = xcb_intern_atom_reply(connection, atomCookie, NULL);

    if (atomReply == NULL) {
        free(atomReply);
        qWarning() << "Socket path atom reply is NULL!";
        return Q_NULLPTR;
    }

    propertyCookie = xcb_get_property_unchecked(
            connection,                 // conn
            false,                      // _delete
            rootWindow,                 // window
            atomReply->atom,            // property
            XCB_GET_PROPERTY_TYPE_ANY,  // type
            0,                          // long_offset
            contentMaxWords             // long_length
    );

    propertyReply = xcb_get_property_reply(connection, propertyCookie, NULL);

    if (propertyReply == NULL) {
        free(atomReply);
        free(propertyReply);
        qWarning() << "Socket path property reply is NULL!";
        return Q_NULLPTR;
    }

    int propertyValueLength = xcb_get_property_value_length(propertyReply);
    socketPath = new char[propertyValueLength + 1]; // One extra for the \0

    strncpy(socketPath, (char *) xcb_get_property_value(propertyReply), propertyValueLength);
    socketPath[propertyValueLength] = '\0';

    free(atomReply);
    free(propertyReply);
    QString path(socketPath);
    delete[] socketPath;
    return path;
}

#include "i3socket.moc"
