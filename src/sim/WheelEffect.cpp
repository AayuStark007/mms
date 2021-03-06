#include "WheelEffect.h"

namespace mms {

WheelEffect::WheelEffect() :
    m_unitForwardEffect(Speed::MetersPerSecond(0)),
    m_unitSidewaysEffect(Speed::MetersPerSecond(0)),
    m_unitTurnEffect(AngularVelocity::RadiansPerSecond(0)) {
}

WheelEffect::WheelEffect(
        const Coordinate& initialTranslation,
        const Angle& initialRotation,
        const Wheel& wheel) {

    m_unitForwardEffect = Speed::MetersPerSecond(
        wheel.getRadius().getMeters() * 
        (initialRotation - wheel.getInitialDirection()).getCos()
    );

    m_unitSidewaysEffect = Speed::MetersPerSecond(
        wheel.getRadius().getMeters() * 
        (initialRotation - wheel.getInitialDirection()).getSin()
    );

    Coordinate wheelToCenter = initialTranslation - wheel.getInitialPosition();
    m_unitTurnEffect = AngularVelocity::RadiansPerSecond(
        wheel.getRadius().getMeters() * 
        (wheelToCenter.getTheta() - wheel.getInitialDirection()).getSin() *
        (1.0 / wheelToCenter.getRho().getMeters())
    );
}

std::tuple<Speed, Speed, AngularVelocity> WheelEffect::getEffects(
        const AngularVelocity& wheelSpeed) const {
    return std::make_tuple(
        m_unitForwardEffect * wheelSpeed.getRadiansPerSecond(),
        m_unitSidewaysEffect * wheelSpeed.getRadiansPerSecond(),
        m_unitTurnEffect * wheelSpeed.getRadiansPerSecond()
    );
}

} // namespace mms
