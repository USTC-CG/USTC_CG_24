#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui_internal.h>

#include <map>
#include <string>

#include "GUI/node_system.h"
#include "imgui.h"
#include "imgui/blueprint-utilities/builders.h"
#include "imgui/blueprint-utilities/images.inl"
#include "imgui/blueprint-utilities/widgets.h"
#include "imgui/imgui-node-editor/imgui_node_editor.h"
#include "node_system_exec.h"

#define STB_IMAGE_IMPLEMENTATION
#include <Utils/Math/string_hash.h>

#include <fstream>
#include <iostream>

#include "Nodes/GlobalUsdStage.h"
#include "Nodes/id.hpp"
#include "Nodes/node.hpp"
#include "Nodes/node_exec_eager.hpp"
#include "Nodes/pin.hpp"
#include "Utils/json.hpp"
#include "imgui_impl_opengl3_loader.h"
#include "node_system_ui.h"
#include "stb_image.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
static inline ImRect ImGui_GetItemRect()
{
    return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
}

static inline ImRect ImRect_Expanded(const ImRect& rect, float x, float y)
{
    auto result = rect;
    result.Min.x -= x;
    result.Min.y -= y;
    result.Max.x += x;
    result.Max.y += y;
    return result;
}

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using namespace ax;

using ax::Widgets::IconType;

struct NodeIdLess {
    bool operator()(const NodeId& lhs, const NodeId& rhs) const
    {
        return lhs.AsPointer() < rhs.AsPointer();
    }
};

static bool Splitter(
    bool split_vertically,
    float thickness,
    float* size1,
    float* size2,
    float min_size1,
    float min_size2,
    float splitter_long_axis_size = -1.0f)
{
    using namespace ImGui;
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID("##Splitter");
    ImRect bb;
    bb.Min =
        window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
    bb.Max = bb.Min + CalcItemSize(
                          split_vertically ? ImVec2(thickness, splitter_long_axis_size)
                                           : ImVec2(splitter_long_axis_size, thickness),
                          0.0f,
                          0.0f);
    return SplitterBehavior(
        bb,
        id,
        split_vertically ? ImGuiAxis_X : ImGuiAxis_Y,
        size1,
        size2,
        min_size1,
        min_size2,
        0.0f);
}

struct NodeSystemImpl {
    friend class NodeSystem;
    explicit NodeSystemImpl(NodeSystemType type, const std::string& filename)
        : node_system_type(type),
          filename(filename)
    {
    }

    Node* SpawnComment();

    void OnStart();
    void OnStop();
    static bool draw_socket_controllers(NodeSocket* input);
    void OnFrame(float deltaTime);

    void ShowLeftPane(float paneWidth);

   protected:
    static void DrawPinIcon(const NodeSocket& pin, bool connected, int alpha);

   protected:
    size_t frame = 0;
    void TouchNode(NodeId id);
    float GetTouchProgress(NodeId id);
    void UpdateTouch();
    Node* FindNode(NodeId id);
    bool CanCreateLink(NodeSocket* a, NodeSocket* b);

    std::unique_ptr<NodeSystemExecution> node_system_execution_;
    static const int m_PinIconSize = 20;
    // std::vector<Node*> m_Nodes;
    ImTextureID m_HeaderBackground = nullptr;
    const float m_TouchTime = 1.0f;
    std::map<NodeId, float, NodeIdLess> m_NodeTouchTime;
    std::string filename;

    bool link_changed = true;

   private:
    ed::EditorContext* m_Editor = nullptr;
    const NodeSystemType node_system_type;

    ImTextureID LoadTexture(const unsigned char* data, size_t buffer_size);

    struct ImTexture {
        GLuint TextureID = 0;
        int Width = 0;
        int Height = 0;
    };

    ImVector<ImTexture> m_Textures;

    ImTextureID CreateTexture(const void* data, int width, int height);

    void DestroyTexture(ImTextureID texture);
    int GetTextureWidth(ImTextureID texture);
    int GetTextureHeight(ImTextureID texture);
    ImVector<ImTexture>::iterator FindTexture(ImTextureID texture);
};

Node* NodeSystemImpl::SpawnComment()
{
    auto& m_Nodes = node_system_execution_->get_nodes();
    m_Nodes.emplace_back(new Node(node_system_execution_->GetNextId(), "Test Comment"));
    m_Nodes.back()->Type = NodeType::Comment;
    m_Nodes.back()->Size[0] = 300;
    m_Nodes.back()->Size[1] = 200;

    return m_Nodes.back().get();
}

void NodeSystemImpl::OnStart()
{
    if (node_system_type == NodeSystemType::Geometry) {
        node_system_execution_ = std::make_unique<GeoNodeSystemExecution>();
    }
    else if (node_system_type == NodeSystemType::Render) {
        node_system_execution_ = std::make_unique<RenderNodeSystemExecution>();
        node_system_execution_->executor = CreateEagerNodeTreeExecutorRender();
    }
    else if (node_system_type == NodeSystemType::Composition) {
        node_system_execution_ = std::make_unique<CompositionNodeSystemExecution>();
    }

    ed::Config config;

    config.UserPointer = this;

    config.SaveSettings = [](const char* data,
                             size_t size,
                             NodeEditor::SaveReasonFlags reason,
                             void* userPointer) -> bool {
        auto ptr = static_cast<NodeSystemImpl*>(userPointer);

        std::ofstream file(ptr->filename);
        auto node_serialize = ptr->node_system_execution_->Serialize();

        node_serialize.erase(node_serialize.end() - 1);

        auto ui_json = std::string(data + 1);
        ui_json.erase(ui_json.end() - 1);

        node_serialize += "," + ui_json + '}';

        file << node_serialize;
        return true;
    };

    config.LoadSettings = [](char* d, void* userPointer) -> size_t {
        auto ptr = static_cast<NodeSystemImpl*>(userPointer);
        std::ifstream file(ptr->filename);
        if (!file) {
            return 0;
        }
        if (!d) {
            file.seekg(0, std::ios_base::end);
            return file.tellg();
        }

        std::string data;
        file.seekg(0, std::ios_base::end);
        auto size = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios_base::beg);

        data.reserve(size);
        data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

        if (data.size() > 0) {
            ptr->node_system_execution_->Deserialize(data);
        }

        memcpy(d, data.data(), data.size());

        return 0;
    };

    m_Editor = ed::CreateEditor(&config);

    m_HeaderBackground = LoadTexture(BlueprintBackground, sizeof(BlueprintBackground));
}

void NodeSystemImpl::OnStop()
{
    auto releaseTexture = [this](ImTextureID& id) {
        if (id) {
            DestroyTexture(id);
            id = nullptr;
        }
    };

    releaseTexture(m_HeaderBackground);

    if (m_Editor) {
        ed::DestroyEditor(m_Editor);
        m_Editor = nullptr;
    }
}

bool NodeSystemImpl::draw_socket_controllers(NodeSocket* input)
{
    bool changed = false;
    switch (input->type_info->type) {
        default:
            ImGui::TextUnformatted(input->ui_name);
            ImGui::Spring(0);
            break;
        case SocketType::Int:
            changed |= ImGui::SliderInt(
                (input->ui_name + ("##" + std::to_string(input->ID.Get()))).c_str(),
                (int*)default_value_storage(input),
                *(int*)default_value_min(input),
                *(int*)default_value_max(input));
            break;

        case SocketType::Float:
            changed |= ImGui::SliderFloat(
                (input->ui_name + ("##" + std::to_string(input->ID.Get()))).c_str(),
                (float*)default_value_storage(input),
                *(float*)default_value_min(input),
                *(float*)default_value_max(input));
            break;

        case SocketType::String:
            changed |= ImGui::InputText(
                (input->ui_name + ("##" + std::to_string(input->ID.Get()))).c_str(),
                (static_cast<std::string*>(default_value_storage(input)))->data(),
                255);
            break;
    }

    return changed;
}

void NodeSystemImpl::OnFrame(float deltaTime)
{
    // A very awkward workaround.
    if (frame == 0) {
        frame++;
        return;
    }
    frame++;

    auto& io = ImGui::GetIO();

    ed::SetCurrentEditor(m_Editor);

    static NodeId contextNodeId = 0;
    static LinkId contextLinkId = 0;
    static USTC_CG::SocketID contextPinId = 0;
    static bool createNewNode = false;
    static NodeSocket* newNodeLinkPin = nullptr;
    static NodeSocket* newLinkPin = nullptr;

    // ShowLeftPane(leftPaneWidth - 4.0f);

    // ImGui::SameLine(0.0f, 12.0f);

    if (ImGui::Button("Zoom to Content"))
        ed::NavigateToContent();
    ed::Begin(("Node editor" + filename).c_str());
    {
        auto cursorTopLeft = ImGui::GetCursorScreenPos();

        util::BlueprintNodeBuilder builder(
            m_HeaderBackground,
            GetTextureWidth(m_HeaderBackground),
            GetTextureHeight(m_HeaderBackground));

        for (auto&& node : node_system_execution_->get_nodes()) {
            if (node->Type != NodeType::Blueprint && node->Type != NodeType::Simple)
                continue;

            const auto isSimple = node->Type == NodeType::Simple;

            builder.Begin(node->ID);
            if (!isSimple) {
                ImColor color;
                memcpy(&color, node->Color, sizeof(ImColor));
                if (node->MISSING_INPUT) {
                    color = ImColor(255, 206, 69, 255);
                }
                if (!node->REQUIRED) {
                    color = ImColor(84, 57, 56, 255);
                }

                if (!node->execution_failed.empty()) {
                    color = ImColor(255, 0, 0, 255);
                }
                builder.Header(color);
                ImGui::Spring(0);
                ImGui::TextUnformatted(node->ui_name.c_str());
                if (!node->execution_failed.empty()) {
                    ImGui::TextUnformatted((": " + node->execution_failed).c_str());
                }
                ImGui::Spring(1);
                ImGui::Dummy(ImVec2(0, 28));
                ImGui::Spring(0);
                builder.EndHeader();
            }

            for (auto& input : node->inputs) {
                auto alpha = ImGui::GetStyle().Alpha;
                if (newLinkPin && !CanCreateLink(newLinkPin, input) && input != newLinkPin)
                    alpha = alpha * (48.0f / 255.0f);

                builder.Input(input->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

                DrawPinIcon(
                    *input, node_system_execution_->IsPinLinked(input->ID), (int)(alpha * 255));
                ImGui::Spring(0);

                if (node_system_execution_->IsPinLinked(input->ID)) {
                    ImGui::TextUnformatted(input->ui_name);
                    ImGui::Spring(0);
                }
                else {
                    ImGui::PushItemWidth(120.0f);
                    if (draw_socket_controllers(input))
                        node_system_execution_->MarkDirty();
                    ImGui::PopItemWidth();
                    ImGui::Spring(0);
                }

                ImGui::PopStyleVar();
                builder.EndInput();
            }

            if (isSimple) {
                builder.Middle();

                ImGui::Spring(1, 0);
                ImGui::TextUnformatted(node->ui_name.c_str());
                ImGui::Spring(1, 0);
            }

            for (auto& output : node->outputs) {
                auto alpha = ImGui::GetStyle().Alpha;
                if (newLinkPin && !CanCreateLink(newLinkPin, output) && output != newLinkPin)
                    alpha = alpha * (48.0f / 255.0f);

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                builder.Output(output->ID);

                ImGui::Spring(0);
                ImGui::TextUnformatted(output->ui_name);
                ImGui::Spring(0);
                DrawPinIcon(
                    *output, node_system_execution_->IsPinLinked(output->ID), (int)(alpha * 255));
                ImGui::PopStyleVar();

                builder.EndOutput();
            }

            builder.End();
        }

        for (auto&& node : node_system_execution_->get_nodes()) {
            if (node->Type != NodeType::Comment)
                continue;

            const float commentAlpha = 0.75f;

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
            ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
            ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
            ed::BeginNode(node->ID);
            ImGui::PushID(node->ID.AsPointer());
            ImGui::BeginVertical("content");
            ImGui::BeginHorizontal("horizontal");
            ImGui::Spring(1);
            ImGui::TextUnformatted(node->ui_name.c_str());
            ImGui::Spring(1);
            ImGui::EndHorizontal();
            ImVec2 size;
            memcpy(&size, node->Size, sizeof(ImVec2));
            ed::Group(size);
            ImGui::EndVertical();
            ImGui::PopID();
            ed::EndNode();
            ed::PopStyleColor(2);
            ImGui::PopStyleVar();

            if (ed::BeginGroupHint(node->ID)) {
                auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

                auto min = ed::GetGroupMin();
                ImGui::SetCursorScreenPos(
                    min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
                ImGui::BeginGroup();
                ImGui::TextUnformatted(node->ui_name.c_str());
                ImGui::EndGroup();

                auto drawList = ed::GetHintBackgroundDrawList();

                auto hintBounds = ImGui_GetItemRect();
                auto hintFrameBounds = ImRect_Expanded(hintBounds, 8, 4);

                drawList->AddRectFilled(
                    hintFrameBounds.GetTL(),
                    hintFrameBounds.GetBR(),
                    IM_COL32(255, 255, 255, 64 * bgAlpha / 255),
                    4.0f);

                drawList->AddRect(
                    hintFrameBounds.GetTL(),
                    hintFrameBounds.GetBR(),
                    IM_COL32(255, 255, 255, 128 * bgAlpha / 255),
                    4.0f);
            }
            ed::EndGroupHint();
        }

        for (auto& link : node_system_execution_->get_links()) {
            ImColor color = GetIconColor(link->fromsock->type_info->type);

            ed::Link(link->ID, link->StartPinID, link->EndPinID, color, 2.0f);
        }

        if (!createNewNode) {
            if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f)) {
                auto showLabel = [](const char* label, ImColor color) {
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
                    auto size = ImGui::CalcTextSize(label);

                    auto padding = ImGui::GetStyle().FramePadding;
                    auto spacing = ImGui::GetStyle().ItemSpacing;

                    ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

                    auto rectMin = ImGui::GetCursorScreenPos() - padding;
                    auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

                    auto drawList = ImGui::GetWindowDrawList();
                    drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
                    ImGui::TextUnformatted(label);
                };

                SocketID startPinId = 0, endPinId = 0;
                if (ed::QueryNewLink(&startPinId, &endPinId)) {
                    auto startPin = node_system_execution_->FindPin(startPinId);
                    auto endPin = node_system_execution_->FindPin(endPinId);

                    newLinkPin = startPin ? startPin : endPin;

                    if (startPin && endPin) {
                        if (node_system_execution_->CanCreateLink(startPin, endPin)) {
                            showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                            if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) {
                                node_system_execution_->CreateLink(startPinId, endPinId);
                            }
                        }
                    }
                }

                SocketID pinId = 0;
                if (ed::QueryNewNode(&pinId)) {
                    newLinkPin = node_system_execution_->FindPin(pinId);
                    if (newLinkPin)
                        showLabel("+ Create Node", ImColor(32, 45, 32, 180));

                    if (ed::AcceptNewItem()) {
                        createNewNode = true;
                        newNodeLinkPin = node_system_execution_->FindPin(pinId);
                        newLinkPin = nullptr;
                        ed::Suspend();
                        ImGui::OpenPopup("Create New Node");
                        ed::Resume();
                    }
                }
            }
            else
                newLinkPin = nullptr;

            ed::EndCreate();

            if (ed::BeginDelete()) {
                NodeId nodeId = 0;
                while (ed::QueryDeletedNode(&nodeId)) {
                    if (ed::AcceptDeletedItem()) {
                        auto id = std::find_if(
                            node_system_execution_->get_nodes().begin(),
                            node_system_execution_->get_nodes().end(),
                            [nodeId](auto& node) { return node->ID == nodeId; });
                        if (id != node_system_execution_->get_nodes().end())
                            node_system_execution_->delete_node(nodeId);
                    }
                }

                LinkId linkId = 0;
                while (ed::QueryDeletedLink(&linkId)) {
                    if (ed::AcceptDeletedItem()) {
                        node_system_execution_->RemoveLink(linkId);
                    }
                }

                node_system_execution_->trigger_refresh_topology();
            }
            ed::EndDelete();
        }

        ImGui::SetCursorScreenPos(cursorTopLeft);
    }

    auto openPopupPosition = ImGui::GetMousePos();
    ed::Suspend();
    if (ed::ShowNodeContextMenu(&contextNodeId))
        ImGui::OpenPopup("Node Context Menu");
    else if (ed::ShowPinContextMenu(&contextPinId))
        ImGui::OpenPopup("Pin Context Menu");
    else if (ed::ShowLinkContextMenu(&contextLinkId))
        ImGui::OpenPopup("Link Context Menu");
    else if (ed::ShowBackgroundContextMenu()) {
        ImGui::OpenPopup("Create New Node");
        newNodeLinkPin = nullptr;
    }
    ed::Resume();

    ed::Suspend();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("Node Context Menu")) {
        auto node = FindNode(contextNodeId);

        ImGui::TextUnformatted("Node Context Menu");
        ImGui::Separator();
        if (node) {
            ImGui::Text("ID: %p", node->ID.AsPointer());
            ImGui::Text("Type: %s", node->Type == NodeType::Blueprint ? "Blueprint" : "Comment");
            ImGui::Text("Inputs: %d", (int)node->inputs.size());
            ImGui::Text("Outputs: %d", (int)node->outputs.size());
        }
        else
            ImGui::Text("Unknown node: %p", contextNodeId.AsPointer());
        ImGui::Separator();
        if (ImGui::MenuItem("Delete"))
            ed::DeleteNode(contextNodeId);
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Pin Context Menu")) {
        auto pin = node_system_execution_->FindPin(contextPinId);

        ImGui::TextUnformatted("Pin Context Menu");
        ImGui::Separator();
        if (pin) {
            ImGui::Text("ID: %p", pin->ID.AsPointer());
            if (pin->Node)
                ImGui::Text("Node: %p", pin->Node->ID.AsPointer());
            else
                ImGui::Text("Node: %s", "<none>");
        }
        else
            ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Link Context Menu")) {
        auto link = node_system_execution_->FindLink(contextLinkId);

        ImGui::TextUnformatted("Link Context Menu");
        ImGui::Separator();
        if (link) {
            ImGui::Text("ID: %p", link->ID.AsPointer());
            ImGui::Text("From: %p", link->StartPinID.AsPointer());
            ImGui::Text("To: %p", link->EndPinID.AsPointer());
        }
        else
            ImGui::Text("Unknown link: %p", contextLinkId.AsPointer());
        ImGui::Separator();
        if (ImGui::MenuItem("Delete"))
            ed::DeleteLink(contextLinkId);
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Create New Node")) {
        auto newNodePostion = openPopupPosition;

        Node* node = node_system_execution_->create_node_menu();
        // ImGui::Separator();
        // if (ImGui::MenuItem("Comment"))
        //     node = SpawnComment();

        if (node) {
            createNewNode = false;

            ed::SetNodePosition(node->ID, newNodePostion);

            if (auto startPin = newNodeLinkPin) {
                auto& pins = startPin->in_out == PinKind::Input ? node->outputs : node->inputs;

                for (auto& pin : pins) {
                    if (CanCreateLink(startPin, pin)) {
                        auto endPin = pin;

                        node_system_execution_->CreateLink(startPin->ID, endPin->ID);

                        break;
                    }
                }
            }
        }

        ImGui::EndPopup();
    }
    else
        createNewNode = false;
    ImGui::PopStyleVar();
    ed::Resume();

    ed::End();

    auto editorMin = ImGui::GetItemRectMin();
    auto editorMax = ImGui::GetItemRectMax();

    if (node_system_type != NodeSystemType::Render) {
        node_system_execution_->try_execution();
    }
}

ImTextureID NodeSystemImpl::LoadTexture(const unsigned char* data, size_t buffer_size)
{
    int width = 0, height = 0, component = 0;
    if (auto loaded_data =
            stbi_load_from_memory(data, buffer_size, &width, &height, &component, 4)) {
        auto texture = CreateTexture(loaded_data, width, height);
        stbi_image_free(loaded_data);
        return texture;
    }
    else
        return nullptr;
}

ImTextureID NodeSystemImpl::CreateTexture(const void* data, int width, int height)
{
    m_Textures.resize(m_Textures.size() + 1);
    ImTexture& texture = m_Textures.back();

    // Upload texture to graphics system
    GLint last_texture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &texture.TextureID);
    glBindTexture(GL_TEXTURE_2D, texture.TextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, last_texture);

    texture.Width = width;
    texture.Height = height;

    return reinterpret_cast<ImTextureID>(static_cast<std::intptr_t>(texture.TextureID));
}

void NodeSystemImpl::DestroyTexture(ImTextureID texture)
{
    auto textureIt = FindTexture(texture);
    if (textureIt == m_Textures.end())
        return;

    glDeleteTextures(1, &textureIt->TextureID);

    m_Textures.erase(textureIt);
}

int NodeSystemImpl::GetTextureWidth(ImTextureID texture)
{
    auto textureIt = FindTexture(texture);
    if (textureIt != m_Textures.end())
        return textureIt->Width;
    return 0;
}

int NodeSystemImpl::GetTextureHeight(ImTextureID texture)
{
    auto textureIt = FindTexture(texture);
    if (textureIt != m_Textures.end())
        return textureIt->Height;
    return 0;
}

ImVector<NodeSystemImpl::ImTexture>::iterator NodeSystemImpl::FindTexture(ImTextureID texture)
{
    auto textureID = static_cast<GLuint>(reinterpret_cast<std::intptr_t>(texture));

    return std::find_if(m_Textures.begin(), m_Textures.end(), [textureID](ImTexture& texture) {
        return texture.TextureID == textureID;
    });
}

void NodeSystemImpl::TouchNode(NodeId id)
{
    m_NodeTouchTime[id] = m_TouchTime;
}

float NodeSystemImpl::GetTouchProgress(NodeId id)
{
    auto it = m_NodeTouchTime.find(id);
    if (it != m_NodeTouchTime.end() && it->second > 0.0f)
        return (m_TouchTime - it->second) / m_TouchTime;
    else
        return 0.0f;
}

void NodeSystemImpl::UpdateTouch()
{
    const auto deltaTime = ImGui::GetIO().DeltaTime;
    for (auto& entry : m_NodeTouchTime) {
        if (entry.second > 0.0f)
            entry.second -= deltaTime;
    }
}

Node* NodeSystemImpl::FindNode(NodeId id)
{
    for (auto&& node : node_system_execution_->get_nodes())
        if (node->ID == id)
            return node.get();

    return nullptr;
}

bool NodeSystemImpl::CanCreateLink(NodeSocket* a, NodeSocket* b)
{
    return node_system_execution_->CanCreateLink(a, b);
}

inline ImGuiWindowFlags GetWindowFlags()
{
    return ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;
}

NodeSystem::NodeSystem(
    NodeSystemType type,
    const std::string& file_name,
    const std::string& window_name)
    : node_system_type(type),
      window_name(window_name)
{
    impl_ = std::make_unique<NodeSystemImpl>(type, file_name);
    impl_->OnStart();
}

NodeSystem::~NodeSystem()
{
    impl_->OnStop();
}

void NodeSystem::draw_imgui()
{
    auto delta_time = ImGui::GetIO().DeltaTime;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3.0f, 3.0f));
    ImGui::Begin(window_name.c_str(), nullptr, GetWindowFlags());
    ImGui::PopStyleVar(1);
    impl_->OnFrame(delta_time);
    ImGui::End();
}

NodeTree* NodeSystem::get_tree()
{
    return impl_->node_system_execution_->node_tree.get();
}

NodeTreeExecutor* NodeSystem::get_executor() const
{
    return impl_->node_system_execution_->executor.get();
}

void NodeSystemImpl::ShowLeftPane(float paneWidth)
{
    auto& io = ImGui::GetIO();
    ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

    ImGui::BeginChild("Selection", ImVec2(paneWidth, 0));

    paneWidth = ImGui::GetContentRegionAvail().x;

    ImGui::BeginHorizontal("Style Editor", ImVec2(paneWidth, 0));
    ImGui::Spring(0.0f, 0.0f);
    if (ImGui::Button("Zoom to Content"))
        ed::NavigateToContent();
    ImGui::Spring(0.0f);
    if (ImGui::Button("Show Flow")) {
        for (auto& link : node_system_execution_->get_links())
            ed::Flow(link->ID);
    }
    ImGui::Spring();
    if (ImGui::Button("Save"))
        ;
    ImGui::EndHorizontal();

    std::vector<NodeId> selectedNodes;
    std::vector<LinkId> selectedLinks;
    selectedNodes.resize(ed::GetSelectedObjectCount());
    selectedLinks.resize(ed::GetSelectedObjectCount());

    int nodeCount =
        ed::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
    int linkCount =
        ed::GetSelectedLinks(selectedLinks.data(), static_cast<int>(selectedLinks.size()));

    selectedNodes.resize(nodeCount);
    selectedLinks.resize(linkCount);

    ImGui::GetWindowDrawList()->AddRectFilled(
        ImGui::GetCursorScreenPos(),
        ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
        ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]),
        ImGui::GetTextLineHeight() * 0.25f);
    ImGui::Spacing();
    ImGui::SameLine();
    ImGui::TextUnformatted("Nodes");
    ImGui::Indent();
    for (auto&& node : node_system_execution_->get_nodes()) {
        ImGui::PushID(node->ID.AsPointer());
        auto start = ImGui::GetCursorScreenPos();

        if (const auto progress = GetTouchProgress(node->ID)) {
            ImGui::GetWindowDrawList()->AddLine(
                start + ImVec2(-8, 0),
                start + ImVec2(-8, ImGui::GetTextLineHeight()),
                IM_COL32(255, 0, 0, 255 - (int)(255 * progress)),
                4.0f);
        }

        bool isSelected =
            std::find(selectedNodes.begin(), selectedNodes.end(), node->ID) != selectedNodes.end();
        ImGui::SetNextItemAllowOverlap();
        if (ImGui::Selectable(
                (node->ui_name + "##" +
                 std::to_string(reinterpret_cast<uintptr_t>(node->ID.AsPointer())))
                    .c_str(),
                &isSelected)) {
            if (io.KeyCtrl) {
                if (isSelected)
                    ed::SelectNode(node->ID, true);
                else
                    ed::DeselectNode(node->ID);
            }
            else
                ed::SelectNode(node->ID, false);

            ed::NavigateToSelection();
        }
        ImGui::PopID();
    }
    ImGui::Unindent();

    static int changeCount = 0;

    ImGui::GetWindowDrawList()->AddRectFilled(
        ImGui::GetCursorScreenPos(),
        ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
        ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]),
        ImGui::GetTextLineHeight() * 0.25f);
    ImGui::Spacing();
    ImGui::SameLine();
    ImGui::TextUnformatted("Selection");

    ImGui::Indent();
    for (int i = 0; i < nodeCount; ++i) {
        ImGui::Text("Node (%p)", selectedNodes[i].AsPointer());
        auto node = FindNode(selectedNodes[i]);
        if (node->override_left_pane_info)
            node->override_left_pane_info();
    }

    for (int i = 0; i < linkCount; ++i)
        ImGui::Text("Link (%p)", selectedLinks[i].AsPointer());
    ImGui::Unindent();

    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
        for (auto& link : node_system_execution_->get_links())
            ed::Flow(link->ID);

    if (ed::HasSelectionChanged())
        ++changeCount;

    ImGui::GetWindowDrawList()->AddRectFilled(
        ImGui::GetCursorScreenPos(),
        ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
        ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]),
        ImGui::GetTextLineHeight() * 0.25f);
    ImGui::Spacing();
    ImGui::SameLine();
    ImGui::TextUnformatted("Node Tree Info");

    node_system_execution_->show_debug_info();

    ImGui::EndChild();
}

void NodeSystemImpl::DrawPinIcon(const NodeSocket& pin, bool connected, int alpha)
{
    IconType iconType;
    ImColor color = GetIconColor(pin.type_info->type);
    color.Value.w = alpha / 255.0f;
    switch (pin.type_info->type) {
        case SocketType::Geometry: iconType = IconType::Circle; break;
        default: iconType = IconType::Circle;
    }

    Widgets::Icon(
        ImVec2(static_cast<float>(m_PinIconSize), static_cast<float>(m_PinIconSize)),
        iconType,
        connected,
        color,
        ImColor(32, 32, 32, alpha));
}

ImColor GetIconColor(SocketType type)
{
    int hashValue_r = hash_str_to_uint32(std::string("r") + get_socket_typename(type));
    hashValue_r = std::abs(hashValue_r);
    int hashValue_g = hash_str_to_uint32(std::string("g") + get_socket_typename(type));
    hashValue_g = std::abs(hashValue_g);

    int hashValue_b = hash_str_to_uint32(std::string("b") + get_socket_typename(type));
    hashValue_b = std::abs(hashValue_b);

    switch (type) {
        case SocketType::Geometry: return ImColor(0, 214, 163);
        case SocketType::Int: return ImColor(68, 201, 156);
        case SocketType::Float: return ImColor(147, 226, 74);
        case SocketType::String:
            return ImColor(6, 95, 146);

            // case PinType::Bool: return ImColor(220, 48, 48);

            // case PinType::String: return ImColor(124, 21, 153);
            // case PinType::Object: return ImColor(51, 150, 215);
            // case PinType::Function: return ImColor(218, 0, 183);
            // case PinType::Delegate: return ImColor(255, 48, 48);
            // default: return ImColor(255, 0, 255);
    }
    return ImColor(hashValue_r % 255, hashValue_g % 255, hashValue_b % 255);
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
