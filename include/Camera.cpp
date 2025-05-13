#include "camera.hpp"
#include <cmath> // Para M_PI e funções trigonométricas

Camera::Camera()
    : distance(5.0f), height(2.0f), zoom(50.f), topDownView(false), sensitivity(0.2f), rotationX(0.0f), rotationY(0.0f), panSpeed(0.05f) {}

void Camera::applyView(const Player &player)
{
    if (topDownView)
    {
        gluLookAt(posX, zoom, posZ,
                  posX, 0.0f, posZ,
                  0.0f, 0.0f, -1.0f);
    }
    else
    {
        float angleRad = player.getRotY() * M_PI / 180.0f;
        float camX = player.getX() - distance * std::sin(angleRad + rotationY);
        float camZ = player.getZ() - distance * std::cos(angleRad + rotationY);

        gluLookAt(camX, player.getY() + height, camZ,
                  player.getX(), player.getY(), player.getZ(),
                  0.0f, 1.0f, 0.0f);
    }
}

void Camera::toggleTopDown(STATE_GAME &gameMode)
{
    gameMode = (gameMode == STATE_GAME::TOP_VIEW_MAP) ? STATE_GAME::PLAYING_EXPLORER : STATE_GAME::TOP_VIEW_MAP;
    topDownView = !topDownView;
}

void Camera::adjustHeight(int deltaY)
{
    height -= deltaY * sensitivity * 0.1f;
    if (height < 0.5f)
        height = 0.5f;
    if (height > 5.0f)
        height = 5.0f;
}

void Camera::adjustHeightByKey(bool up)
{
    if (up)
        height -= 0.1f;
    else
    {
        height += 0.1f;
        if (height < 0.5f)
            height = 0.5f;
    }
}

float Camera::getSensitivity() const
{
    return sensitivity;
}

void Camera::updatePanWithMouse(int deltaX, int deltaY)
{
    posX += deltaX * panSpeed;
    posZ += deltaY * panSpeed;

    float minX = -30.0f;
    float maxX = 30.0f;
    float minZ = -30.0f;
    float maxZ = 30.0f;

    if (posX < minX) posX = minX;
    else if (posX > maxX) posX = maxX;

    if (posZ < minZ) posZ = minZ;
    else if (posZ > maxZ) posZ = maxZ;
}

void Camera::updateRotation(float deltaX, float deltaY)
{
    rotationX += deltaY * sensitivity;
    rotationY += deltaX * sensitivity;

    if (rotationX > 89.0f) rotationX = 89.0f;
    if (rotationX < -89.0f) rotationX = -89.0f;
}

void Camera::updateZoom(int scrollDirection)
{
    const float zoomSpeed = 0.2f;
    const float minZoom = 2.0f;
    const float maxZoom = 20.0f;

    distance -= scrollDirection * zoomSpeed;

    if (distance < minZoom) distance = minZoom;
    if (distance > maxZoom) distance = maxZoom;
}

void Camera::updateTopDownZoom(int direction)
{
    const float zoomSpeed = 0.5f;
    const float minHeight = 20.0f;
    const float maxHeight = 60.0f;

    zoom -= direction * zoomSpeed;

    if (zoom < minHeight) zoom = minHeight;
    if (zoom > maxHeight) zoom = maxHeight;
}

// Implementação dos Getters e Setters

float Camera::getPosX() const { return posX; }
float Camera::getPosZ() const { return posZ; }
float Camera::getPanSpeed() const { return panSpeed; }
float Camera::getRotationX() const { return rotationX; }
float Camera::getRotationY() const { return rotationY; }
float Camera::getZoom() const { return zoom; }
float Camera::getDistance() const { return distance; }
float Camera::getHeight() const { return height; }
bool Camera::isTopDownView() const { return topDownView; }

void Camera::setPosX(float x) { posX = x; }
void Camera::setPosZ(float z) { posZ = z; }
void Camera::setPanSpeed(float speed) { panSpeed = speed; }
void Camera::setRotationX(float x) { rotationX = x; }
void Camera::setRotationY(float y) { rotationY = y; }
void Camera::setZoom(float zoomLevel) { zoom = zoomLevel; }
void Camera::setDistance(float dist) { distance = dist; }
void Camera::setHeight(float h) { height = h; }
void Camera::setTopDownView(bool view) { topDownView = view; }
void Camera::setSensitivity(float sens) { sensitivity = sens; }