#pragma once

#include <QString>

std::pair<int, QString> execCommand(const QString &command, const QStringList &args);
bool checkIfPastelInstalled();
bool checkIfNotifySendInstalled();
