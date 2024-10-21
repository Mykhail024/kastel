#include <cstdio>

#include <QString>
#include <QProcess>

#include "Commands.h"

std::pair<int, QString> execCommand(const QString &command, const QStringList &args) {
    QProcess procces;
    procces.start(command, args);

    if(!procces.waitForStarted()) {
        return {-1, QLatin1String()};
    }

    if(!procces.waitForFinished()) {
        return {-1, QLatin1String()};
    }

    const QString output(QLatin1String(procces.readAllStandardOutput()));
    const int exitCode = procces.exitCode();

    return {exitCode, output};
}

bool checkIfPastelInstalled() {
    int exitCode = execCommand(QLatin1String("pastel"), {QLatin1String("--version")}).first;

    if(exitCode != 0) return false;
    return true;
}
