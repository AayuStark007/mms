#include "SimUtilities.h"

#include <QDateTime>
#include <QRegExp>
#include <QThread>
#include <QTime>

#include <limits>
#include <random>

#include "Assert.h"
#include "Param.h"

namespace mms {

void SimUtilities::quit() {
    // TODO: MACK - make a better API for exiting from each of the threads
    exit(1);
}

int SimUtilities::randomInt() {
    static std::mt19937 generator(P()->randomSeed());
    return generator();
}

int SimUtilities::randomNonNegativeInt(int max) {
    int value = randomInt();
    if (value < 0) {
        value -= std::numeric_limits<int>::min();
    }
    if (0 < max) {
        value %= max;
    }
    return value;
}

double SimUtilities::getRandom() {
    
    // The '- 1' ensures that the random number is never 1.
    // This matches the python implementation where random is [0,1).
    // This is particularly useful if you want to index into array like so
    // array[std::floor(random * <number of elements>)] without having to check
    // the condition if this function returns 1.
    
    static int max = std::mt19937().max();
    return std::abs(static_cast<double>(randomInt()) - 1) / static_cast<double>(max);
}

void SimUtilities::sleep(const Duration& duration) {
    ASSERT_LE(0, duration.getMicroseconds());
    QThread::usleep(duration.getMicroseconds());
}

double SimUtilities::getHighResTimestamp() {
    return QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0;
}

QString SimUtilities::formatDuration(const Duration& duration) {
    return QTime(0, 0, 0)
        .addMSecs(duration.getMilliseconds())
        .toString("mm:ss.zzz");
}

QStringList SimUtilities::splitLines(const QString& string) {
    return string.split(QRegExp("\n|\r\n|\r"));
}

bool SimUtilities::isBool(const QString& str) {
    return str == "true" || str == "false";
}

bool SimUtilities::isInt(const QString& str) {
    bool ok;
    str.toInt(&ok);
    return ok;
}

bool SimUtilities::isDouble(const QString& str) {
    bool ok;
    str.toDouble(&ok);
    return ok;
}

bool SimUtilities::strToBool(const QString& str) {
    ASSERT_TR(isBool(str));
    return str == "true";
}

char SimUtilities::strToChar(const QString& str) {
    ASSERT_EQ(str.size(), 1);
    return str.at(0).toLatin1();
}

int SimUtilities::strToInt(const QString& str) {
    ASSERT_TR(isInt(str));
    return str.toInt();
}

double SimUtilities::strToDouble(const QString& str) {
    ASSERT_TR(isDouble(str));
    return str.toDouble();
}

QString SimUtilities::boolToStr(bool value) {
    return value ? "true" : "false";
}

QVector<TriangleGraphic> SimUtilities::polygonToTriangleGraphics(
        const Polygon& polygon,
        Color color,
        double alpha) {
    QVector<Triangle> triangles = polygon.getTriangles();
    QVector<TriangleGraphic> triangleGraphics;
    RGB colorValues = COLOR_TO_RGB().value(color);
    for (Triangle triangle : triangles) {
        triangleGraphics.push_back({
            {triangle.p1.getX().getMeters(), triangle.p1.getY().getMeters(), colorValues, alpha},
            {triangle.p2.getX().getMeters(), triangle.p2.getY().getMeters(), colorValues, alpha},
            {triangle.p3.getX().getMeters(), triangle.p3.getY().getMeters(), colorValues, alpha}
        });
    }
    return triangleGraphics;
}

} // namespace mms
