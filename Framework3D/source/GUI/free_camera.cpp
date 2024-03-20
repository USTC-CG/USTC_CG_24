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

FirstPersonCamera::FirstPersonCamera()
{
    // UpdateWorldToView();
    SetTransform(pxr::GfMatrix4d()
                     .SetLookAt(m_CameraPos, m_CameraPos + m_CameraDir, m_CameraUp)
                     .GetInverse());
}

void FirstPersonCamera::KeyboardUpdate()
{
    for (auto&& key : this->keyboardMap) {
        auto cameraKey = key.second;
        if (ImGui::IsKeyPressed(ImGuiKey(key.first)) || ImGui::IsKeyDown(ImGuiKey(key.first))) {
            keyboardState[cameraKey] = true;
        }
        else {
            keyboardState[cameraKey] = false;
        }
    }
}

void FirstPersonCamera::MousePosUpdate(double xpos, double ypos)
{
    mousePos = { float(xpos), float(ypos) };
}

void FirstPersonCamera::MouseButtonUpdate(int button)
{
    const bool pressed = ImGui::IsMouseDown(button);

    switch (button) {
        case ImGuiMouseButton_Left: mouseButtonState[MouseButtons::Left] = pressed; break;
        case ImGuiMouseButton_Middle: mouseButtonState[MouseButtons::Middle] = pressed; break;
        case ImGuiMouseButton_Right: mouseButtonState[MouseButtons::Right] = pressed; break;
        default: break;
    }
}

std::pair<bool, pxr::GfVec3f> FirstPersonCamera::AnimateTranslation(float deltaT)
{
    bool cameraDirty = false;
    float moveStep = deltaT * m_MoveSpeed;
    pxr::GfVec3f cameraMoveVec = pxr::GfVec3f(0.f);

    if (keyboardState[KeyboardControls::SpeedUp])
        moveStep *= 3.f;

    if (keyboardState[KeyboardControls::SlowDown])
        moveStep *= .1f;

    if (keyboardState[KeyboardControls::MoveForward]) {
        cameraDirty = true;
        cameraMoveVec += m_CameraDir * moveStep;
    }

    if (keyboardState[KeyboardControls::MoveBackward]) {
        cameraDirty = true;
        cameraMoveVec += -m_CameraDir * moveStep;
    }

    if (keyboardState[KeyboardControls::MoveLeft]) {
        cameraDirty = true;
        cameraMoveVec += -m_CameraRight * moveStep;
    }

    if (keyboardState[KeyboardControls::MoveRight]) {
        cameraDirty = true;
        cameraMoveVec += m_CameraRight * moveStep;
    }

    if (keyboardState[KeyboardControls::MoveUp]) {
        cameraDirty = true;
        cameraMoveVec += m_CameraUp * moveStep;
    }

    if (keyboardState[KeyboardControls::MoveDown]) {
        cameraDirty = true;
        cameraMoveVec += -m_CameraUp * moveStep;
    }
    return std::make_pair(cameraDirty, cameraMoveVec);
}

void FirstPersonCamera::UpdateCamera(pxr::GfVec3f cameraMoveVec, pxr::GfMatrix4f cameraRotation)
{
    m_CameraPos += cameraMoveVec;
    m_CameraDir = cameraRotation.TransformAffine(m_CameraDir).GetNormalized();
    m_CameraUp = cameraRotation.TransformAffine(m_CameraUp).GetNormalized();
    m_CameraRight = pxr::GfCross(m_CameraDir, m_CameraUp).GetNormalized();

    // UpdateWorldToView();
    SetTransform(pxr::GfMatrix4d()
                     .SetLookAt(m_CameraPos, m_CameraPos + m_CameraDir, m_CameraUp)
                     .GetInverse());
}

std::pair<bool, pxr::GfMatrix4f> FirstPersonCamera::AnimateRoll(pxr::GfMatrix4f initialRotation)
{
    bool cameraDirty = false;
    pxr::GfMatrix4f cameraRotation = initialRotation;
    if (keyboardState[KeyboardControls::RollLeft] || keyboardState[KeyboardControls::RollRight]) {
        float roll = float(keyboardState[KeyboardControls::RollLeft]) * -m_RotateSpeed * 2.0f +
                     float(keyboardState[KeyboardControls::RollRight]) * m_RotateSpeed * 2.0f;

        cameraRotation =
            cameraRotation * pxr::GfMatrix4f(pxr::GfRotation(m_CameraDir, roll), { 0, 0, 0 });
        cameraDirty = true;
    }
    return std::make_pair(cameraDirty, cameraRotation);
}

void FirstPersonCamera::Animate(float deltaT)
{
    // track mouse delta
    pxr::GfVec2f mouseMove = mousePos - mousePosPrev;
    mousePosPrev = mousePos;

    bool cameraDirty = false;
    pxr::GfMatrix4f cameraRotation;
    cameraRotation.SetIdentity();

    // handle mouse rotation first
    // this will affect the movement vectors in the world matrix, which we use below
    if (mouseButtonState[MouseButtons::Left] && (mouseMove[0] != 0 || mouseMove[1] != 0)) {
        float yaw = m_RotateSpeed * mouseMove[0];
        float pitch = m_RotateSpeed * mouseMove[1];

        cameraRotation =
            pxr::GfMatrix4f(pxr::GfRotation(pxr::GfVec3f(0.f, 0.f, 1.f), -yaw), { 0, 0, 0 });
        cameraRotation =
            cameraRotation * pxr::GfMatrix4f(pxr::GfRotation(m_CameraRight, pitch), { 0, 0, 0 });

        cameraDirty = true;
    }

    // handle keyboard roll next
    auto rollResult = AnimateRoll(cameraRotation);
    cameraDirty |= rollResult.first;
    cameraRotation = rollResult.second;

    // handle translation
    auto translateResult = AnimateTranslation(deltaT);
    cameraDirty |= translateResult.first;
    const pxr::GfVec3f& cameraMoveVec = translateResult.second;

    if (cameraDirty) {
        UpdateCamera(cameraMoveVec, cameraRotation);
    }
}

void FirstPersonCamera::AnimateSmooth(float deltaT)
{
    const float c_DampeningRate = 7.5f;
    float dampenWeight = exp(-c_DampeningRate * deltaT);

    pxr::GfVec2f mouseMove{ 0, 0 };
    if (mouseButtonState[MouseButtons::Left]) {
        if (!isMoving) {
            isMoving = true;
            mousePosPrev = mousePos;
        }

        mousePosDamp[0] = pxr::GfLerp(mousePos[0], mousePosPrev[0], dampenWeight);
        mousePosDamp[1] = pxr::GfLerp(mousePos[1], mousePosPrev[1], dampenWeight);

        // track mouse delta
        mouseMove = mousePosDamp - mousePosPrev;
        mousePosPrev = mousePosDamp;
    }
    else {
        isMoving = false;
    }

    bool cameraDirty = false;
    pxr::GfMatrix4f cameraRotation;
    cameraRotation.SetIdentity();

    // handle mouse rotation first
    // this will affect the movement vectors in the world matrix, which we use below
    if (mouseMove[0] || mouseMove[1]) {
        float yaw = m_RotateSpeed * mouseMove[0];
        float pitch = m_RotateSpeed * mouseMove[1];

        cameraRotation =
            pxr::GfMatrix4f(pxr::GfRotation(pxr::GfVec3f(0.f, 0.f, 1.f), -yaw), { 0, 0, 0 });
        cameraRotation =
            cameraRotation * pxr::GfMatrix4f(pxr::GfRotation(m_CameraRight, pitch), { 0, 0, 0 });

        cameraDirty = true;
    }

    // handle keyboard roll next
    auto rollResult = AnimateRoll(cameraRotation);
    cameraDirty |= rollResult.first;
    cameraRotation = rollResult.second;

    // handle translation
    auto translateResult = AnimateTranslation(deltaT);
    cameraDirty |= translateResult.first;
    const pxr::GfVec3f& cameraMoveVec = translateResult.second;

    if (cameraDirty) {
        UpdateCamera(cameraMoveVec, cameraRotation);
    }
}

void FirstPersonCamera::MouseScrollUpdate(double offset)
{
    float scrollFactor = 1.15f;
    m_MoveSpeed *= (offset > 0 ? scrollFactor : 1.0f / scrollFactor);
}

void ThirdPersonCamera::KeyboardUpdate()
{
    for (auto&& key : this->keyboardMap) {
        auto cameraKey = key.second;
        if (ImGui::IsKeyPressed(ImGuiKey(key.first))) {
            keyboardState[cameraKey] = true;
        }
        else {
            keyboardState[cameraKey] = false;
        }
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