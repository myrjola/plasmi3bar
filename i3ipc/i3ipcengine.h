#ifndef I3IPCENGINE_H
#define I3IPCENGINE_H

#include <Plasma/DataEngine>

#include "i3socket.h"

class I3IPCEngine : public Plasma::DataEngine
{
    // required since Plasma::DataEngine inherits QObject
Q_OBJECT

public:
    // every engine needs a constructor with these arguments
    I3IPCEngine(QObject *parent, const QVariantList &args);

protected:
    // this virtual function is called when a new source is requested
    bool sourceRequestEvent(const QString &name);

    // this virtual function is called when an automatic update
    // is triggered for an existing source (ie: when a valid update
    // interval is set when requesting a source)
    bool updateSourceEvent(const QString &source);


public slots:

    void connected();

    void disconnected();

    void bytesWritten(qint64 bytes);

    void readyRead();

    void newLine(const QString &line);

private:
    I3Socket m_i3Socket;
};

#endif // I3IPCRENGINE_H
