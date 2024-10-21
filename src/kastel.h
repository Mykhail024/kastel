#pragma once

#include <KRunner/AbstractRunner>
#include <KPluginMetaData>
#include <QObject>

class kastel : public KRunner::AbstractRunner
{
    Q_OBJECT
    public:
        kastel(QObject *parent, const KPluginMetaData &metaData);
        void match(KRunner::RunnerContext &context) override;
        void run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match) override;
        void reloadConfiguration() override;
};
