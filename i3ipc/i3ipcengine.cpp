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
    QByteArray reply = m_i3Socket.readLine();
    quint32 offset = strlen(I3_IPC_MAGIC) + sizeof(quint32);

    qint32 replyType;
    memcpy(&replyType, reply.constData() + offset, sizeof(quint32));

    reply.remove(0, strlen(I3_IPC_MAGIC) + 2 * sizeof(quint32));
    qDebug() << "Received " << reply.length() << " bytes with content: " << reply;

    if (replyType == I3_IPC_EVENT_WORKSPACE) {
        qDebug() << "Update workspace!";
        m_i3Socket.sendMessage(I3_IPC_MESSAGE_TYPE_GET_WORKSPACES, "");
        return;
    }
    newLine(QString(reply));
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
