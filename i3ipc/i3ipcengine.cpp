#include "i3ipcengine.h"

#include <Plasma/DataContainer>
#include <i3/ipc.h>

I3IPCEngine::I3IPCEngine(QObject *parent, const QVariantList &args)
        : Plasma::DataEngine(parent, args),
          m_i3Socket(this)
{
    // We ignore any arguments - data engines do not have much use for them
    Q_UNUSED(args)

    // This prevents applets from setting an unnecessarily high
    // update interval and using too much CPU.
    // In the case of a clock that only has second precision,
    // a third of a second should be more than enough.
    setMinimumPollingInterval(333);

    connect(&m_i3Socket, SIGNAL(connected()),this, SLOT(connected()));
    connect(&m_i3Socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(&m_i3Socket, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));
    connect(&m_i3Socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
}

void I3IPCEngine::connected()
{
    qDebug() << "Connected!";
}

void I3IPCEngine::disconnected()
{
    qDebug() << "Disconnected!";
}

void I3IPCEngine::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written.";
}

void I3IPCEngine::readyRead()
{
    qDebug() << "reading...";
    uint magicLength = qstrlen(I3_IPC_MAGIC);

    QByteArray magicHeader = m_i3Socket.read(magicLength);
    if ((uint) magicHeader.length() != magicLength || qstrncmp(magicHeader.constData(), I3_IPC_MAGIC, magicLength))
    {
        qWarning() << "Invalid magic header: " << magicHeader;
        m_i3Socket.readAll(); // Flush the buffer
        return;
    };
    QDataStream stream(&m_i3Socket);
    stream.setByteOrder(QDataStream::LittleEndian);
    qint32 replyLength;
    stream >> replyLength;
    qint32 replyType;
    stream >> replyType;
    qDebug() << "Expecting " << replyLength << " bytes of type " << replyType;

    QByteArray reply = m_i3Socket.read(replyLength);
    qDebug() << "Received " << reply.length() << " bytes with content: " << reply;
    if (reply.length() < (int) replyLength) {
        qWarning("Expected more from reply! Waiting for more...");
        qWarning() << "Socket error: " << m_i3Socket.error();
        if (m_i3Socket.waitForReadyRead(1000)) {
            reply.append(m_i3Socket.read(replyLength - reply.length()));
            qDebug() << "Received " << reply.length() << " bytes with content: " << reply;
        } else {
            qDebug() << "Timeout when waiting for more content!";
        }
    }

    if (replyType == I3_IPC_EVENT_WORKSPACE) {
        qDebug() << "Update workspace!";
        m_i3Socket.sendMessage(I3_IPC_MESSAGE_TYPE_GET_WORKSPACES, "");
    } else {
        newLine(QString(reply));
    }
    if (m_i3Socket.bytesAvailable()) {
        emit m_i3Socket.readyRead();
    }
}

void I3IPCEngine::newLine(const QString &line)
{
    return setData("workspace", "state", line);
}

bool I3IPCEngine::sourceRequestEvent(const QString &name)
{
    char const *payload = "[\"workspace\"]";
    quint32 messageType = I3_IPC_MESSAGE_TYPE_SUBSCRIBE;

    m_i3Socket.sendMessage(messageType, payload);
    return updateSourceEvent(name);
}

bool I3IPCEngine::updateSourceEvent(const QString &name)
{
    setData(name, "state", "workspace");
    return true;
}

// This does the magic that allows Plasma to load
// this plugin.
K_EXPORT_PLASMA_DATAENGINE_WITH_JSON(i3ipcengine, I3IPCEngine, "plasma-dataengine-i3ipc.json")

#include "i3ipcengine.moc"
