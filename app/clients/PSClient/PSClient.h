//
// Created by talik on 8/27/2025.
//

#ifndef POWERSHELL_CLIENT_H
#define POWERSHELL_CLIENT_H

#include <QObject>
#include <QTcpSocket>

class PSClient final : public QObject
{
    Q_OBJECT
public:
    explicit PSClient(QObject *parent = nullptr);
    void runScript(const QString &script);

    signals:
        void scriptResultReceived(const QVariant &result);

private slots:
    void onConnected();
    void onReadyRead();

private:
    QTcpSocket *m_socket;
};

#endif // POWERSHELL_CLIENT_H
