#include "IMouseAlgorithm.h"

std::string IMouseAlgorithm::mouseFile() const {
    return "default.xml";
}

std::string IMouseAlgorithm::interfaceType() const {
    return "DISCRETE";
}

std::string IMouseAlgorithm::initialDirection() const {
    return "NORTH";
}

bool IMouseAlgorithm::automaticallyClearFog() const {
    return true;
}

bool IMouseAlgorithm::declareWallOnRead() const {
    return false;
}

bool IMouseAlgorithm::declareBothWallHalves() const {
    return false;
}

bool IMouseAlgorithm::setTileTextWhenDistanceDeclared() const {
    return true;
}

bool IMouseAlgorithm::setTileBaseColorWhenDistanceDeclaredCorrectly() const {
    return false;
}

double IMouseAlgorithm::wheelSpeedFraction() const {
    return 1.0; 
}

int IMouseAlgorithm::tileTextNumberOfRows() const {
    return 2;
}

int IMouseAlgorithm::tileTextNumberOfCols() const {
    return 3;
}