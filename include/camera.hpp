#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glu.h>
#include "player.hpp"

class Camera
{
private:
    float distance;   
    float height;     
    bool topDownView;  
    float sensitivity; 
    float zoom;        

public:
    float posX = 0.0f;
    float posZ = 0.0f;
    float panSpeed = 0.1f;
    float rotationY = 0.0f;
    float rotationX = 0.0f;

    Camera();

    void applyView(const Player &player);
    void toggleTopDown(STATE_GAME &gameMode);
    void toggleDistance();
    void adjustHeight(int deltaY);
    void adjustHeightByKey(bool up);
    void updatePanWithMouse(int deltaX, int deltaY);
    void updateZoom(int scrollDirection);
    void updateTopDownZoom(int direction);
    void updateRotationJoystick(float joystickX, float joystickY);

    void updateRotation(float deltaX, float deltaY);

    float getPosX() const;
    float getPosZ() const;
    float getPanSpeed() const;
    float getRotationX() const;
    float getRotationY() const;
    float getZoom() const;
    float getDistance() const;
    float getHeight() const;
    bool isTopDownView() const;
    float getSensitivity() const;

    void setPosX(float x);
    void setPosZ(float z);
    void setPanSpeed(float speed);
    void setRotationX(float x);
    void setRotationY(float y);
    void setZoom(float zoomLevel);
    void setDistance(float distance);
    void setHeight(float height);
    void setTopDownView(bool view);
    void setSensitivity(float sensitivity);
};

#endif