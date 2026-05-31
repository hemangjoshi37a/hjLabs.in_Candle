// This file is a part of "Candle" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich

#include <QApplication>
#include <QDebug>
#include <QGLWidget>
#include <QLocale>
#include <QTranslator>
#include <QFile>
#include <QStyleFactory>
#include <QFontDatabase>
#include <QMessageBox>
#include <QDir>

#include "parser/gcodepreprocessorutils.h"
#include "parser/gcodeparser.h"
#include "parser/gcodeviewparse.h"
#include "logging/fileloghandler.h"
#include "versionconfig.h"

#include "frmmain.h"

void loadTranslationsForLocale(const QString &locale, QCoreApplication &app)
{
    auto translationsFolder = qApp->applicationDirPath() + "/translations/";
    QDir dir(translationsFolder);

    if (!dir.exists())
        return;

    for (const QString &fileName : dir.entryList(QStringList{ "*_" + locale + ".qm" }, QDir::Files))
    {
        auto tr = new QTranslator(&app);

        if (tr->load(dir.absoluteFilePath(fileName)))
            app.installTranslator(tr);
        else
            delete tr;
    }
}

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
#ifdef Q_OS_LINUX
    QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
#endif
    QApplication a(argc, argv);

    a.setOrganizationName(APP_NAME);
    a.setApplicationName(APP_NAME);
    a.setApplicationDisplayName(APP_NAME);
    a.setApplicationVersion(APP_VERSION);

    installFileLogHandler();

    QSettings set;
    QString locale = set.value("General/language", "en").toString();

    loadTranslationsForLocale(locale, a);

    a.setStyleSheet(a.styleSheet() + "QWidget {font-size: 10pt}");

    // Sane initial window stylesheet load. frmMain's constructor calls applyTheme()
    // (which reads the persisted "theme" setting) and is the source of truth — it
    // runs after this initial set below since the stylesheet is applied to the
    // window only once the object exists. We pick a reasonable default here for the
    // brief moment before the window is shown.
#if defined(Q_OS_MAC)
    QFile styles(":/styles/frmmainmacos.qss");
#else
    QFile styles(":/styles/frmmaindefault.qss");
#endif

    QString initialStyles;
    if (styles.open(QFile::ReadOnly))
        initialStyles = styles.readAll();

    frmMain w;

    // Only apply the initial default if the window's constructor didn't already set
    // a stylesheet via applyTheme() (it always does, so this is a no-op fallback for
    // safety on platforms without the dark/light resources).
    if (w.styleSheet().isEmpty() && !initialStyles.isEmpty())
        w.setStyleSheet(initialStyles);

    w.show();

    return a.exec();
}