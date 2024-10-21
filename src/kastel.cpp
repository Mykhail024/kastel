#include <QString>
#include <QClipboard>
#include <QApplication>
#include <QRegularExpression>
#include <QThreadPool>
#include <KRunner/QueryMatch>
#include <QStringList>
#include <QIcon>
#include <QPainter>
#include <QBrush>
#include <krunner/abstractrunner.h>
#include <qlogging.h>

#include "Commands.h"

#include "kastel.h"

const static QStringList formats({QLatin1String("hsl"), QLatin1String("cmyk"), QLatin1String("rgb"), QLatin1String("lab")});

static QIcon generateCircleIcon(const QColor& color, int size) {
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(QBrush(color));
    painter.setPen(Qt::NoPen);

    painter.drawEllipse(0, 0, size, size);

    return QIcon(pixmap);
}

kastel::kastel(QObject *parent, const KPluginMetaData &metaData)
    : KRunner::AbstractRunner(parent, metaData)
{}

void kastel::match(KRunner::RunnerContext &context)
{
    if(!context.isValid()) return;

    QString query = context.query();
    static QRegularExpression hex(QLatin1String("^#([A-Fa-f0-9]{3}|[A-Fa-f0-9]{6})$"));
    static QRegularExpression rgb(QLatin1String("^rg(?:b|ba)\\(\\s*(\\d{1,3})\\s*,\\s*(\\d{1,3})\\s*,\\s*(\\d{1,3})\\s*(,\\s*(0|1|0?\\.\\d+))?\\s*\\)$"));

    QString match_format;
    if(rgb.match(query).hasMatch()) match_format = QLatin1String("rgb");
    else if(hex.match(query).hasMatch()) match_format = QLatin1String("hex");
    else return;

    QIcon icon;
    if(checkIfPastelInstalled()) {
        if(match_format == QLatin1String("hex")) {
            icon = generateCircleIcon(QColor::fromString(query), 64);
        } else {
            QString out = execCommand(QLatin1String("pastel"), {QLatin1String("format"), QLatin1String("hex"), query}).second;
            icon = generateCircleIcon(QColor::fromString(out.mid(0, out.size()-1)), 64);

            KRunner::QueryMatch match(this);
            match.setText(out);
            match.setIcon(icon);
            match.setSubtext(QLatin1String("hex"));
            match.setRelevance(1.0);
            context.addMatch(match);
        }
    } else {
        KRunner::QueryMatch match(this);
        match.setText(QLatin1String("Pastel execution error."));
        match.setIconName(QLatin1String("dialog-warning"));
        context.addMatch(match);
        return;
    }

    for(const auto &format : formats) {
        if(match_format == format) continue;
        QThreadPool::globalInstance()->start([&, format, icon]() {
                KRunner::QueryMatch match(this);
                QString out = execCommand(QLatin1String("pastel"), {QLatin1String("format"), format, query}).second;

                match.setText(out);
                match.setIcon(icon);
                match.setSubtext(format);
                if(format == QLatin1String("rgb"))
                    match.setRelevance(1.0);

                context.addMatch(match);
                });
    }
    QThreadPool::globalInstance()->waitForDone();
}

void kastel::run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match)
{
    Q_UNUSED(context);
    QApplication::clipboard()->setText(match.text());
}

void kastel::reloadConfiguration() { }

K_PLUGIN_CLASS_WITH_JSON(kastel, "kastel.json")

#include "kastel.moc"
