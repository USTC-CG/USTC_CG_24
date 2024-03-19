// #define __GNUC__

#include "free_camera.h"

#include <algorithm>

#include "GCore/Components.h"
#include "imgui.h"
#include "pxr/base/gf/frustum.h"
#include "pxr/base/gf/matrix3f.h"
#include "pxr/base/gf/matrix4f.h"
#include "pxr/base/gf/quatf.h"
#include "pxr/base/gf/rotation.h"
#include "pxr/base/gf/vec2f.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

constexpr float PI_f = M_PI;
void ThirdPersonCamera::KeyboardUpdate(int key, int action)
{
    if (keyboardMap.find(key) == keyboardMap.end()) {
        return;
    }

    auto cameraKey = keyboardMap.at(key);
    if (action == ImGui::IsKeyPressed(ImGuiKey(key))) {
        keyboardState[cameraKey] = true;
    }
    else {
        keyboardState[cameraKey] = false;
    }
}

void ThirdPersonCamera::MousePosUpdate(double xpos, double ypos)
{
    m_MousePosPrev = m_MousePos;
    m_MousePos = pxr::GfVec2f(float(xpos), float(ypos));
}

void ThirdPersonCamera::MouseButtonUpdate(int button)
{
    const bool pressed = ImGui::IsMouseDown(button);

    switch (button) {
        case ImGuiMouseButton_Left: mouseButtonState[MouseButtons::Left] = pressed; break;
        case ImGuiMouseButton_Middle: mouseButtonState[MouseButtons::Middle] = pressed; break;
        case ImGuiMouseButton_Right: mouseButtonState[MouseButtons::Right] = pressed; break;
        default: break;
    }
}

void ThirdPersonCamera::MouseScrollUpdate(double offset)
{
    const float scrollFactor = 1.15f;
    m_Distance = std::clamp(
        m_Distance * (offset < 0 ? scrollFactor : 1.0f / scrollFactor),
        m_MinDistance,
        m_MaxDistance);
}

void ThirdPersonCamera::SetRotation(float yaw, float pitch)
{
    m_Yaw = yaw;
    m_Pitch = pitch;
}

void ThirdPersonCamera::AnimateOrbit(float deltaT)
{
    if (mouseButtonState[MouseButtons::Left]) {
        pxr::GfVec2f mouseMove = m_MousePos - m_MousePosPrev;
        float rotateSpeed = .005f;  // mouse sensitivity in radians/pixel

        m_Yaw -= rotateSpeed * mouseMove[0];
        m_Pitch += rotateSpeed * mouseMove[1];
    }

    const float ORBIT_SENSITIVITY = 1.5f;
    const float ZOOM_SENSITIVITY = 40.f;
    m_Distance += ZOOM_SENSITIVITY * deltaT * m_DeltaDistance;
    m_Yaw += ORBIT_SENSITIVITY * deltaT * m_DeltaYaw;
    m_Pitch += ORBIT_SENSITIVITY * deltaT * m_DeltaPitch;

    m_Distance = std::clamp(m_Distance, m_MinDistance, m_MaxDistance);

    m_Pitch = std::clamp(m_Pitch, PI_f * -0.5f, PI_f * 0.5f);

    m_DeltaDistance = 0;
    m_DeltaYaw = 0;
    m_DeltaPitch = 0;
}

void ThirdPersonCamera::AnimateTranslation(const pxr::GfMatrix3f& viewMatrix)
{
    // If the view parameters have never been set, we can't translate
    if (m_ViewportSize[0] <= 0.f || m_ViewportSize[1] <= 0.f)
        return;

    if ((m_MousePos == m_MousePosPrev))
        return;

    if (mouseButtonState[MouseButtons::Right]) {
        pxr::GfVec4f oldClipPos = pxr::GfVec4f(0.f, 0.f, m_Distance, 1.f) * m_ProjectionMatrix;
        oldClipPos /= oldClipPos[3];
        oldClipPos[0] = 1.f - 2.f * m_MousePosPrev[0] / m_ViewportSize[0];
        oldClipPos[1] = 1.f - 2.f * m_MousePosPrev[1] / m_ViewportSize[1];
        pxr::GfVec4f newClipPos = oldClipPos;
        newClipPos[0] = 1.f - 2.f * m_MousePos[0] / m_ViewportSize[0];
        newClipPos[1] = 1.f - 2.f * m_MousePos[1] / m_ViewportSize[1];

        pxr::GfVec4f oldViewPos = oldClipPos * m_InverseProjectionMatrix;
        oldViewPos /= oldViewPos[3];
        pxr::GfVec4f newViewPos = newClipPos * m_InverseProjectionMatrix;
        newViewPos /= newViewPos[3];

        pxr::GfVec2f viewMotion =
            pxr::GfVec2f(oldViewPos[0], oldViewPos[1]) - pxr::GfVec2f(newViewPos[0], newViewPos[1]);

        m_TargetPos -= viewMotion[0] * viewMatrix.GetRow(0);

        if (keyboardState[KeyboardControls::HorizontalPan]) {
            pxr::GfVec3f horizontalForward =
                pxr::GfVec3f(viewMatrix.GetRow(2)[0], 0.f, viewMatrix.GetRow(2)[2]);
            float horizontalLength = horizontalForward.GetLength();
            if (horizontalLength == 0.f)
                horizontalForward =
                    pxr::GfVec3f(viewMatrix.GetRow(1)[0], 0.f, viewMatrix.GetRow(1)[2]);
            horizontalForward = horizontalForward.GetNormalized();
            m_TargetPos += viewMotion[1] * horizontalForward * 1.5f;
        }
        else
            m_TargetPos += viewMotion[1] * viewMatrix.GetRow(1);
    }
}

pxr::GfQuatf rotationQuat(const pxr::GfVec3f& euler)
{
    float sinHalfX = std::sin(0.5f * euler[0]);
    float cosHalfX = std::cos(0.5f * euler[0]);
    float sinHalfY = std::sin(0.5f * euler[1]);
    float cosHalfY = std::cos(0.5f * euler[1]);
    float sinHalfZ = std::sin(0.5f * euler[2]);
    float cosHalfZ = std::cos(0.5f * euler[2]);

    pxr::GfQuatf quatX = pxr::GfQuatf(cosHalfX, sinHalfX, 0, 0);
    pxr::GfQuatf quatY = pxr::GfQuatf(cosHalfY, 0, sinHalfY, 0);
    pxr::GfQuatf quatZ = pxr::GfQuatf(cosHalfZ, 0, 0, sinHalfZ);

    // Note: multiplication order for quats is like column-vector convention
    return quatZ * quatY * quatX;
}

void ThirdPersonCamera::Animate(float deltaT)
{
    SetPerspectiveFromAspectRatioAndFieldOfView(
        m_ViewportSize[0] / m_ViewportSize[1], 67, FOVHorizontal);
    m_ProjectionMatrix = pxr::GfMatrix4f(GetFrustum().ComputeProjectionMatrix());
    m_InverseProjectionMatrix = m_ProjectionMatrix.GetInverse();
    AnimateOrbit(deltaT);

    pxr::GfMatrix3f orbit = pxr::GfMatrix3f(rotationQuat(pxr::GfVec3f(m_Pitch, m_Yaw, 0.f)));

    const auto targetRotation = orbit;
    AnimateTranslation(orbit);

    const pxr::GfVec3f vectorToCamera = -m_Distance * targetRotation.GetRow(2);

    const pxr::GfVec3f camPos = m_TargetPos + vectorToCamera;

    m_CameraPos = camPos;
    m_CameraRight = -targetRotation.GetRow(0);
    m_CameraUp = targetRotation.GetRow(1);
    m_CameraDir = targetRotation.GetRow(2);
    // UpdateWorldToView();
    SetTransform(
        pxr::GfMatrix4d().SetLookAt(camPos, m_CameraPos + m_CameraDir, m_CameraUp).GetInverse());
    m_MousePosPrev = m_MousePos;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE