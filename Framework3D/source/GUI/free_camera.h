#include <array>
#include <unordered_map>

#include "USTC_CG.h"
#include "imgui.h"
#include "pxr/base/gf/camera.h"
#include "pxr/base/gf/matrix4f.h"
#include "pxr/base/gf/vec2f.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

class FreeCamera : public pxr::GfCamera {
   public:
    virtual void MousePosUpdate(double xpos, double ypos) = 0;
    virtual void KeyboardUpdate() = 0;
    virtual void MouseButtonUpdate(int button) = 0;
    virtual void Animate(float deltaT) = 0;
    virtual void MouseScrollUpdate(double offset){};

    enum MouseButtons {
        Left,
        Middle,
        Right,

        MouseButtonCount
    };
    pxr::GfVec2f m_ViewportSize = pxr::GfVec2f(0.0f);

   protected:
    pxr::GfVec3f m_CameraPos = { -10, 0, 0 };
    pxr::GfVec3f m_CameraRight = { 0, 1, 0 };
    pxr::GfVec3f m_CameraUp = { 0, 0, 1 };
    pxr::GfVec3f m_CameraDir = { 1, 0, 0 };
    float m_MoveSpeed = 5.f;      // movement speed in units/second
    float m_RotateSpeed = .03f;  // mouse sensitivity in radians/pixel
};

class FirstPersonCamera : public FreeCamera {
   public:
    FirstPersonCamera();
    void KeyboardUpdate() override;
    void MousePosUpdate(double xpos, double ypos) override;
    void MouseButtonUpdate(int button) override;
    void Animate(float deltaT) override;
    void AnimateSmooth(float deltaT);
    void MouseScrollUpdate(double offset) override;

private:
    std::pair<bool, pxr::GfMatrix4f> AnimateRoll(pxr::GfMatrix4f initialRotation);
    std::pair<bool, pxr::GfVec3f> AnimateTranslation(float deltaT);
    void UpdateCamera(pxr::GfVec3f cameraMoveVec, pxr::GfMatrix4f cameraRotation);

    pxr::GfVec2f mousePos;
    pxr::GfVec2f mousePosPrev;
    // fields used only for AnimateSmooth()
    pxr::GfVec2f mousePosDamp;
    bool isMoving = false;

    typedef enum {
        MoveUp,
        MoveDown,
        MoveLeft,
        MoveRight,
        MoveForward,
        MoveBackward,

        YawRight,
        YawLeft,
        PitchUp,
        PitchDown,
        RollLeft,
        RollRight,

        SpeedUp,
        SlowDown,

        KeyboardControlCount,
    } KeyboardControls;

    const std::unordered_map<int, int> keyboardMap = {
        { ImGuiKey_Q, KeyboardControls::MoveDown },
        { ImGuiKey_E, KeyboardControls::MoveUp },
        { ImGuiKey_A, KeyboardControls::MoveLeft },
        { ImGuiKey_D, KeyboardControls::MoveRight },
        { ImGuiKey_W, KeyboardControls::MoveForward },
        { ImGuiKey_S, KeyboardControls::MoveBackward },
        { ImGuiKey_LeftArrow, KeyboardControls::YawLeft },
        { ImGuiKey_RightArrow, KeyboardControls::YawRight },
        { ImGuiKey_UpArrow, KeyboardControls::PitchUp },
        { ImGuiKey_DownArrow, KeyboardControls::PitchDown },
        { ImGuiKey_Z, KeyboardControls::RollLeft },
        { ImGuiKey_C, KeyboardControls::RollRight },
        { ImGuiKey_LeftShift, KeyboardControls::SpeedUp },
        { ImGuiKey_LeftCtrl, KeyboardControls::SlowDown },
    };

    const std::unordered_map<int, int> mouseButtonMap = {
        { ImGuiMouseButton_Left, MouseButtons::Left },
        { ImGuiMouseButton_Middle, MouseButtons::Middle },
        { ImGuiMouseButton_Right, MouseButtons::Right },
    };

    std::array<bool, KeyboardControls::KeyboardControlCount> keyboardState = { false };
    std::array<bool, MouseButtons::MouseButtonCount> mouseButtonState = { false };
};

class ThirdPersonCamera : public FreeCamera {
   public:
    void KeyboardUpdate() override;
    void MousePosUpdate(double xpos, double ypos) override;
    void MouseButtonUpdate(int button) override;
    void MouseScrollUpdate(double offset);
    void Animate(float deltaT) override;

    void SetTargetPosition(pxr::GfVec3f position)
    {
        m_TargetPos = position;
    }
    void SetDistance(float distance)
    {
        m_Distance = distance;
    }
    void SetRotation(float yaw, float pitch);
    void SetMinDistance(float value)
    {
        m_MinDistance = value;
    }
    void SetMaxDistance(float value)
    {
        m_MaxDistance = value;
    }

   private:
    void AnimateOrbit(float deltaT);
    void AnimateTranslation(const pxr::GfMatrix3f& viewMatrix);

    // View parameters to derive translation amounts
    pxr::GfMatrix4f m_ProjectionMatrix = pxr::GfMatrix4f(1.0f);
    pxr::GfMatrix4f m_InverseProjectionMatrix = pxr::GfMatrix4f(1.0f);

    pxr::GfVec2f m_MousePos = pxr::GfVec2f(0.f);
    pxr::GfVec2f m_MousePosPrev = pxr::GfVec2f(0.f);

    pxr::GfVec3f m_TargetPos = pxr::GfVec3f(0.f);
    float m_Distance = 10.f;

    float m_MinDistance = 0.01f;
    float m_MaxDistance = std::numeric_limits<float>::max();

    float m_Yaw = 90.f;
    float m_Pitch = 0.f;

    float m_DeltaYaw = 0.f;
    float m_DeltaPitch = 0.f;
    float m_DeltaDistance = 0.f;

    typedef enum {
        HorizontalPan,

        KeyboardControlCount,
    } KeyboardControls;

    const std::unordered_map<int, int> keyboardMap = {
        { ImGuiKey_LeftAlt, KeyboardControls::HorizontalPan },
    };

    std::array<bool, KeyboardControls::KeyboardControlCount> keyboardState = { false };
    std::array<bool, MouseButtons::MouseButtonCount> mouseButtonState = { false };
};

USTC_CG_NAMESPACE_CLOSE_SCOPE