#include "CurveTurnFactorCalculator.h"

#include <QPair>

#include "units/Speed.h"

namespace mms {

CurveTurnFactorCalculator::CurveTurnFactorCalculator() :
    m_unitForwardComponent(0.0),
    m_unitTurnComponent(0.0) {
}

CurveTurnFactorCalculator::CurveTurnFactorCalculator(
        const QMap<QString, Wheel>& wheels,
        const QMap<QString, WheelEffect>& wheelEffects,
        const QMap<QString, QPair<double, double>>& wheelSpeedAdjustmentFactors) {

    // TODO: upforgrabs
    // Currently, this logic assumes that when the mouse has its wheel
    // speeds set for moving forward, that it does not turn at all, and vice
    // versa. This is not the case for mice where the wheels don't have
    // rotational symmetry. What we really care about is that the mouse moves
    // in the forward direction (relative to the initial direction of the
    // mouse) as much as intended. The sideways movement (relative to to the
    // initial direction of the mouse) can vary as needed.

    // Determine the total forward and turn rate of change from all wheels
    Speed totalForwardRateOfChange = Speed::MetersPerSecond(0);
    AngularVelocity totalRadialRateOfChange;
    for (const auto& pair : ContainerUtilities::items(wheels)) {

        // For each of the wheel speed adjustment factors, calculate the wheel's
        // contributions. Remember that each of these factors corresponds to
        // the fraction of the max wheel speed such that the mouse performs a
        // particular movement (moving forward or turning) most optimally.
        ASSERT_TR(wheelSpeedAdjustmentFactors.contains(pair.first));
        QPair<double, double> adjustmentFactors =
            wheelSpeedAdjustmentFactors.value(pair.first);

        for (double adjustmentFactor : {adjustmentFactors.first, adjustmentFactors.second}) {

            std::tuple<Speed, Speed, AngularVelocity> effects =
                wheelEffects.value(pair.first).getEffects(
                    pair.second.getMaxAngularVelocityMagnitude() * adjustmentFactor);

            totalForwardRateOfChange += std::get<0>(effects);
            totalRadialRateOfChange += std::get<2>(effects);
        }
    }

    // The main idea here is that, for a curve turn, we want the mouse to move
    // forward a distance equal to the length of the arc we'd like it to travel.
    // So we'd like to find a pair of factors, A and B, such that:
    //
    //  totalForwardRateOfChange * A   totalDistance
    //  ---------------------------- = -------------
    //  totalRadialRateOfChange  * B   totalRotation
    //
    // So we have that:
    //
    //      totalDistance   totalRadialRateOfChange
    //  A = ------------- * ------------------------ * B
    //      totalRotation   totalForwardRateOfChange
    //
    // Now if we take the radius of the arc to be 1m, we have that:
    //
    //      totalRadialRateOfChange
    //  A = ------------------------ * B
    //      totalForwardRateOfChange
    //
    // Then we can just choose B = 1.0 and solve for A

    double B = 1.0;
    double A = (
        totalRadialRateOfChange.getRadiansPerSecond() /
        totalForwardRateOfChange.getMetersPerSecond()
    );

    m_unitForwardComponent = A;
    m_unitTurnComponent = B;
}

QPair<double, double> CurveTurnFactorCalculator::getCurveTurnFactors(const Distance& radius) {
    return {m_unitForwardComponent * radius.getMeters(), m_unitTurnComponent};
}

} // namespace mms
