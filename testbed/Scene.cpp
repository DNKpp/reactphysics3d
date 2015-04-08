/********************************************************************************
* ReactPhysics3D physics library, http://www.reactphysics3d.com                 *
* Copyright (c) 2010-2015 Daniel Chappuis                                       *
*********************************************************************************
*                                                                               *
* This software is provided 'as-is', without any express or implied warranty.   *
* In no event will the authors be held liable for any damages arising from the  *
* use of this software.                                                         *
*                                                                               *
* Permission is granted to anyone to use this software for any purpose,         *
* including commercial applications, and to alter it and redistribute it        *
* freely, subject to the following restrictions:                                *
*                                                                               *
* 1. The origin of this software must not be misrepresented; you must not claim *
*    that you wrote the original software. If you use this software in a        *
*    product, an acknowledgment in the product documentation would be           *
*    appreciated but is not required.                                           *
*                                                                               *
* 2. Altered source versions must be plainly marked as such, and must not be    *
*    misrepresented as being the original software.                             *
*                                                                               *
* 3. This notice may not be removed or altered from any source distribution.    *
*                                                                               *
********************************************************************************/

// Libraries
#include "Scene.h"

using namespace openglframework;

// Constructor
Scene::Scene(const std::string& name) : mName(name) {

}

// Set the scene position (where the camera needs to look at)
void Scene::setScenePosition(const openglframework::Vector3& position, float sceneRadius) {

    // Set the position and radius of the scene
    mCenterScene = position;
    mCamera.setSceneRadius(sceneRadius);

    // Reset the camera position and zoom in order to view all the scene
    resetCameraToViewAll();
}

// Set the camera so that we can view the whole scene
void Scene::resetCameraToViewAll() {

    // Move the camera to the origin of the scene
    mCamera.translateWorld(-mCamera.getOrigin());

    // Move the camera to the center of the scene
    mCamera.translateWorld(mCenterScene);

    // Set the zoom of the camera so that the scene center is
    // in negative view direction of the camera
    mCamera.setZoom(1.0);
}

// Map the mouse x,y coordinates to a point on a sphere
bool Scene::mapMouseCoordinatesToSphere(double xMouse, double yMouse, Vector3& spherePoint) const {

    // Get the window dimension
    int width, height;
    glfwGetWindowSize(mWindow, &width, &height);

    if ((xMouse >= 0) && (xMouse <= width) && (yMouse >= 0) && (yMouse <= height)) {
        float x = float(xMouse - 0.5f * width) / float(width);
        float y = float(0.5f * height - yMouse) / float(height);
        float sinx = sin(PI * x * 0.5f);
        float siny = sin(PI * y * 0.5f);
        float sinx2siny2 = sinx * sinx + siny * siny;

        // Compute the point on the sphere
        spherePoint.x = sinx;
        spherePoint.y = siny;
        spherePoint.z = (sinx2siny2 < 1.0) ? sqrt(1.0f - sinx2siny2) : 0.0f;

        return true;
    }

    return false;
}

// Called when a mouse button event occurs
void Scene::mouseButtonEvent(int button, int action, int mods) {

    // Get the mouse cursor position
    double x, y;
    glfwGetCursorPos(mWindow, &x, &y);

    // If the mouse button is pressed
    if (action == GLFW_PRESS) {
        mLastMouseX = x;
        mLastMouseY = y;
        mIsLastPointOnSphereValid = mapMouseCoordinatesToSphere(x, y, mLastPointOnSphere);
    }
    else {  // If the mouse button is released
        mIsLastPointOnSphereValid = false;
    }
}

// Called when a mouse motion event occurs
void Scene::mouseMotionEvent(double xMouse, double yMouse) {

    int leftButtonState = glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_LEFT);
    int rightButtonState = glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_RIGHT);
    int middleButtonState = glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_MIDDLE);
    int altKeyState = glfwGetKey(mWindow, GLFW_KEY_LEFT_ALT);

    // Zoom
    if (leftButtonState == GLFW_PRESS && altKeyState == GLFW_PRESS) {

        // Get the window dimension
        int width, height;
        glfwGetWindowSize(mWindow, &width, &height);

        float dy = static_cast<float>(yMouse - mLastMouseY);
        float h = static_cast<float>(height);

        // Zoom the camera
        zoom(-dy / h);
    }
    // Translation
    else if (middleButtonState == GLFW_PRESS || rightButtonState == GLFW_PRESS ||
             (leftButtonState == GLFW_PRESS && altKeyState == GLFW_PRESS)) {
        translate(xMouse, yMouse);
    }
    // Rotation
    else if (leftButtonState == GLFW_PRESS) {
        rotate(xMouse, yMouse);
    }

    // Remember the mouse position
    mLastMouseX = xMouse;
    mLastMouseY = yMouse;
    mIsLastPointOnSphereValid = mapMouseCoordinatesToSphere(xMouse, yMouse, mLastPointOnSphere);
}

// Called when a scrolling event occurs
void Scene::scrollingEvent(float xAxis, float yAxis) {
    zoom(yAxis * SCROLL_SENSITIVITY);
}

// Zoom the camera
void Scene::zoom(float zoomDiff) {

    // Zoom the camera
    mCamera.setZoom(zoomDiff);
}

// Translate the camera
void Scene::translate(int xMouse, int yMouse) {
   float dx = static_cast<float>(xMouse - mLastMouseX);
   float dy = static_cast<float>(yMouse - mLastMouseY);

   // Translate the camera
   mCamera.translateCamera(-dx / float(mCamera.getWidth()),
                           -dy / float(mCamera.getHeight()), mCenterScene);
}

// Rotate the camera
void Scene::rotate(int xMouse, int yMouse) {

    if (mIsLastPointOnSphereValid) {

        Vector3 newPoint3D;
        bool isNewPointOK = mapMouseCoordinatesToSphere(xMouse, yMouse, newPoint3D);

        if (isNewPointOK) {
            Vector3 axis = mLastPointOnSphere.cross(newPoint3D);
            float cosAngle = mLastPointOnSphere.dot(newPoint3D);

            float epsilon = std::numeric_limits<float>::epsilon();
            if (fabs(cosAngle) < 1.0f && axis.length() > epsilon) {
                axis.normalize();
                float angle = 2.0f * acos(cosAngle);

                // Rotate the camera around the center of the scene
                mCamera.rotateAroundLocalPoint(axis, -angle, mCenterScene);
            }
        }
    }
}
