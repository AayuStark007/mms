#pragma once

#include <QString>
#include <QStringList>

namespace mms {

class SettingsMazeAlgos {

public:

    SettingsMazeAlgos() = delete;

    static QStringList names();
    static QString getDirPath(const QString& name);
    static QString getBuildCommand(const QString& name);
    static QString getRunCommand(const QString& name);

    static void add(
        const QString& name,
        const QString& dirPath,
        const QString& buildCommand,
        const QString& runCommand
    );
    static void update(
        const QString& name,
        const QString& newName,
        const QString& newDirPath,
        const QString& newBuildCommand,
        const QString& newRunCommand
    );
    static void remove(const QString& name);

private:

    static const QString GROUP_PREFIX;
    static const QString NAME_KEY;
    static const QString DIR_PATH_KEY;
    static const QString BUILD_COMMAND_KEY;
    static const QString RUN_COMMAND_KEY;

    static QString getValue(const QString& name, const QString& key);

};

} // namespace mms
