#define LANDSCAPE 0u
#define PORTRAIT  1u

#ifndef ORIENTATIONMANAGER_H
#define ORIENTATIONMANAGER_H

class OrientationManager
{
public:
    OrientationManager(float threshold);
    ~OrientationManager();

    void begin();
    unsigned int checkRotationChange();
    unsigned int checkRotationChange(float threshold);
    bool getOrientation();
    void setOrientation(bool isPortrait);
    short getRotation();
    void setRotation(short rotation);

private:
    class Impl;
    Impl *_pimpl;
};

#endif