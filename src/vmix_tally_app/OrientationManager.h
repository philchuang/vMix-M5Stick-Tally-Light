#define LANDSCAPE 0
#define PORTRAIT  1

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
    byte getRotation();
    void setRotation(byte rotation);

private:
    class Impl;
    Impl *_pimpl;
};

#endif