#include <array>
#include <unordered_map>

#include "USTC_CG.h"
#include "imgui.h"
#include "pxr/base/gf/camera.h"
#include "pxr/base/gf/matrix4f.h"
#include "pxr/base/gf/vec2f.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

class ThirdPersonCamera : public pxr::GfCamera {
   public:
    void KeyboardUpdate(int key, int action);
    void MousePosUpdate(double xpos, double ypos);
    void MouseButtonUpdate(int button);
    void MouseScrollUpdate(double offset);
    void Animate(float deltaT);

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
    pxr::GfVec2f m_ViewportSize = pxr::GfVec2f(0.0f);
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

    float m_MinDistance = 0.f;
    float m_MaxDistance = std::numeric_limits<float>::max();

    float m_Yaw = 90.f;
    float m_Pitch = 0.f;

    float m_DeltaYaw = 0.f;
    float m_DeltaPitch = 0.f;
    float m_DeltaDistance = 0.f;
    pxr::GfVec3f m_CameraPos;
    pxr::GfVec3f m_CameraRight;
    pxr::GfVec3f m_CameraUp;
    pxr::GfVec3f m_CameraDir;

    typedef enum {
        HorizontalPan,

        KeyboardControlCount,
    } KeyboardControls;

    const std::unordered_map<int, int> keyboardMap = {
        { ImGuiKey_LeftAlt, KeyboardControls::HorizontalPan },
    };

    typedef enum {
        Left,
        Middle,
        Right,

        MouseButtonCount
    } MouseButtons;

    std::array<bool, KeyboardControls::KeyboardControlCount> keyboardState = { false };
    std::array<bool, MouseButtons::MouseButtonCount> mouseButtonState = { false };
};

USTC_CG_NAMESPACE_CLOSE_SCOPE