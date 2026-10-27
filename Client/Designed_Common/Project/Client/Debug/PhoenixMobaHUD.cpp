//----------------------------------------------------------------------
// PhoenixMobaHUD.cpp
//----------------------------------------------------------------------

#include "PhoenixMobaHUD.h"

#include "imgui.h"

#include <windows.h>
#include <d3d9.h>

#include <algorithm>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#include "../ClientDef.h"
#include "../Packet/Gpackets/GCMobaJoin.h"

namespace PhoenixImGui {

namespace {

struct MobaQueueState
{
    bool visible;
    unsigned char state;
    unsigned char joinedCount;
    unsigned char maxCount;
    unsigned short remainingSeconds;
    DWORD updateTick;

    MobaQueueState()
        : visible(false)
        , state(GCMobaJoin::QUEUE_STATE_CANCELLED)
        , joinedCount(0)
        , maxCount(10)
        , remainingSeconds(0)
        , updateTick(0)
    {
    }
};

struct HudTexture
{
    IDirect3DTexture9* texture;
    int width;
    int height;
    bool loadAttempted;
    bool loadFailedReported;

    HudTexture()
        : texture(NULL)
        , width(0)
        , height(0)
        , loadAttempted(false)
        , loadFailedReported(false)
    {
    }
};

MobaQueueState g_mobaQueueState;
HudTexture g_joinTexture;

const char* JOIN_SPKI_PATH = "data\\ui\\spk\\new\\moba\\mobajoin.spki";
const char* JOIN_SPK_PATH  = "data\\ui\\spk\\new\\moba\\mobajoin.spk";

unsigned short readLE16(const std::vector<unsigned char>& data, size_t offset)
{
    if (offset + 2 > data.size())
        return 0;
    return (unsigned short)(data[offset] | (data[offset + 1] << 8));
}

unsigned int readLE32(const std::vector<unsigned char>& data, size_t offset)
{
    if (offset + 4 > data.size())
        return 0;
    return (unsigned int)data[offset]
        | ((unsigned int)data[offset + 1] << 8)
        | ((unsigned int)data[offset + 2] << 16)
        | ((unsigned int)data[offset + 3] << 24);
}

bool readWholeFile(const char* path, std::vector<unsigned char>& out)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
        return false;

    file.seekg(0, std::ios::end);
    std::streamoff size = file.tellg();
    if (size <= 0)
        return false;

    file.seekg(0, std::ios::beg);
    out.resize((size_t)size);
    file.read((char*)&out[0], size);
    return file.good();
}

unsigned int rgb565ToArgb(unsigned short c)
{
    unsigned int r = (c >> 11) & 0x1F;
    unsigned int g = (c >> 5) & 0x3F;
    unsigned int b = c & 0x1F;

    r = (r << 3) | (r >> 2);
    g = (g << 2) | (g >> 4);
    b = (b << 3) | (b >> 2);

    return 0xFF000000u | (r << 16) | (g << 8) | b;
}

bool decodeFirstJoinSprite(std::vector<unsigned int>& pixels, int& width, int& height)
{
    std::vector<unsigned char> indexData;
    std::vector<unsigned char> spriteData;
    if (!readWholeFile(JOIN_SPKI_PATH, indexData) || indexData.size() < 6)
        return false;
    if (!readWholeFile(JOIN_SPK_PATH, spriteData))
        return false;

    unsigned short count = readLE16(indexData, 0);
    if (count == 0)
        return false;

    size_t offset = readLE32(indexData, 2);
    if (offset + 4 > spriteData.size())
        return false;

    width = (int)readLE16(spriteData, offset);
    height = (int)readLE16(spriteData, offset + 2);
    if (width <= 0 || height <= 0 || width > 2048 || height > 2048)
        return false;

    pixels.assign((size_t)width * (size_t)height, 0x00000000u);

    size_t pos = offset + 4;
    for (int y = 0; y < height; ++y)
    {
        if (pos + 2 > spriteData.size())
            return false;

        unsigned short rowLen = readLE16(spriteData, pos);
        pos += 2;

        size_t rowEnd = pos + ((size_t)rowLen * 2);
        if (rowEnd > spriteData.size())
            return false;

        if (rowLen > 0)
        {
            size_t wordPos = pos;
            unsigned short pairCount = readLE16(spriteData, wordPos);
            wordPos += 2;

            int x = 0;
            for (unsigned short pair = 0; pair < pairCount && wordPos + 4 <= rowEnd; ++pair)
            {
                unsigned short transparentCount = readLE16(spriteData, wordPos);
                unsigned short colorCount = readLE16(spriteData, wordPos + 2);
                wordPos += 4;

                x += transparentCount;
                for (unsigned short i = 0; i < colorCount && wordPos + 2 <= rowEnd; ++i)
                {
                    unsigned short c = readLE16(spriteData, wordPos);
                    wordPos += 2;

                    if (0 <= x && x < width)
                        pixels[(size_t)y * (size_t)width + (size_t)x] = rgb565ToArgb(c);
                    ++x;
                }
            }
        }

        pos = rowEnd;
    }

    return true;
}

bool ensureJoinTexture(IDirect3DDevice9* device)
{
    if (g_joinTexture.texture != NULL)
        return true;
    if (device == NULL)
        return false;
    if (g_joinTexture.loadAttempted)
        return false;

    g_joinTexture.loadAttempted = true;

    std::vector<unsigned int> pixels;
    int width = 0;
    int height = 0;
    if (!decodeFirstJoinSprite(pixels, width, height))
    {
        if (!g_joinTexture.loadFailedReported)
        {
            OutputDebugStringA("[PhoenixMobaHUD] mobajoin.spk decode failed; using ImGui fallback.\n");
            g_joinTexture.loadFailedReported = true;
        }
        return false;
    }

    IDirect3DTexture9* texture = NULL;
    HRESULT hr = device->CreateTexture(
        (UINT)width,
        (UINT)height,
        1,
        0,
        D3DFMT_A8R8G8B8,
        D3DPOOL_MANAGED,
        &texture,
        NULL);

    if (FAILED(hr) || texture == NULL)
    {
        if (!g_joinTexture.loadFailedReported)
        {
            OutputDebugStringA("[PhoenixMobaHUD] mobajoin texture creation failed; using ImGui fallback.\n");
            g_joinTexture.loadFailedReported = true;
        }
        return false;
    }

    D3DLOCKED_RECT locked;
    hr = texture->LockRect(0, &locked, NULL, 0);
    if (FAILED(hr))
    {
        texture->Release();
        if (!g_joinTexture.loadFailedReported)
        {
            OutputDebugStringA("[PhoenixMobaHUD] mobajoin texture lock failed; using ImGui fallback.\n");
            g_joinTexture.loadFailedReported = true;
        }
        return false;
    }

    for (int y = 0; y < height; ++y)
    {
        unsigned char* dst = (unsigned char*)locked.pBits + (locked.Pitch * y);
        const unsigned int* src = &pixels[(size_t)y * (size_t)width];
        memcpy(dst, src, (size_t)width * sizeof(unsigned int));
    }

    texture->UnlockRect(0);

    g_joinTexture.texture = texture;
    g_joinTexture.width = width;
    g_joinTexture.height = height;
    return true;
}

unsigned short getDisplayedRemainingSeconds()
{
    if (!g_mobaQueueState.visible)
        return 0;

    DWORD now = GetTickCount();
    DWORD elapsedMs = (g_mobaQueueState.updateTick == 0) ? 0 : (now - g_mobaQueueState.updateTick);
    unsigned int elapsedSeconds = elapsedMs / 1000;
    if (elapsedSeconds >= g_mobaQueueState.remainingSeconds)
        return 0;

    return (unsigned short)(g_mobaQueueState.remainingSeconds - elapsedSeconds);
}

void drawFallbackBackground(const ImVec2& min, const ImVec2& max)
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(min, max, IM_COL32(3, 22, 24, 230), 4.0f);
    drawList->AddRect(min, max, IM_COL32(46, 185, 205, 255), 4.0f, 0, 1.0f);
}

} // namespace

void UpdateMobaQueueState(
    unsigned char state,
    unsigned char joinedCount,
    unsigned char maxCount,
    unsigned short remainingSeconds)
{
    g_mobaQueueState.state = state;
    g_mobaQueueState.joinedCount = joinedCount;
    g_mobaQueueState.maxCount = (maxCount > 0) ? maxCount : 10;
    g_mobaQueueState.remainingSeconds = remainingSeconds;
    g_mobaQueueState.updateTick = GetTickCount();

    g_mobaQueueState.visible = (state == GCMobaJoin::QUEUE_STATE_WAITING);
}

void DrawMobaQueueHUD(IDirect3DDevice9* device)
{
    if (!g_mobaQueueState.visible)
        return;
    if (g_Mode != MODE_GAME)
        return;

    const bool hasTexture = ensureJoinTexture(device);
    const float textureW = hasTexture ? (float)g_joinTexture.width : 240.0f;
    const float textureH = hasTexture ? (float)g_joinTexture.height : 50.0f;

    const float windowW = textureW + 20.0f;
    const float windowH = textureH + 34.0f;

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 defaultPos((io.DisplaySize.x - windowW) * 0.5f, 95.0f);
    ImGui::SetNextWindowPos(defaultPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(windowW, windowH), ImGuiCond_FirstUseEver);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.02f, 0.04f, 0.05f, 0.82f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.18f, 0.75f, 0.85f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.02f, 0.14f, 0.18f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.04f, 0.22f, 0.28f, 1.00f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 8.0f));

    const ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    if (ImGui::Begin("AOS Battle Queue", NULL, flags))
    {
        ImVec2 bgMin = ImGui::GetCursorScreenPos();
        ImVec2 bgMax(bgMin.x + textureW, bgMin.y + textureH);

        if (hasTexture)
        {
            ImTextureID textureId = (ImTextureID)(uintptr_t)g_joinTexture.texture;
            ImGui::GetWindowDrawList()->AddImage(textureId, bgMin, bgMax);
        }
        else
            drawFallbackBackground(bgMin, bgMax);

        ImGui::InvisibleButton("##moba_queue_bg", ImVec2(textureW, textureH));

        unsigned short remain = getDisplayedRemainingSeconds();
        unsigned int minutes = remain / 60;
        unsigned int seconds = remain % 60;
        unsigned int maxCount = (g_mobaQueueState.maxCount > 0) ? g_mobaQueueState.maxCount : 10;

        char line1[64];
        char line2[64];
        std::snprintf(line1, sizeof(line1), "Waiting PVP List: %u / %u",
            (unsigned int)g_mobaQueueState.joinedCount,
            maxCount);
        std::snprintf(line2, sizeof(line2), "Queue Cancel Time: %02u:%02u", minutes, seconds);

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImU32 shadow = IM_COL32(0, 0, 0, 220);
        ImU32 text = IM_COL32(115, 245, 235, 255);
        ImVec2 p1(bgMin.x + 14.0f, bgMin.y + 9.0f);
        ImVec2 p2(bgMin.x + 14.0f, bgMin.y + 28.0f);
        drawList->AddText(ImVec2(p1.x + 1.0f, p1.y + 1.0f), shadow, line1);
        drawList->AddText(p1, text, line1);
        drawList->AddText(ImVec2(p2.x + 1.0f, p2.y + 1.0f), shadow, line2);
        drawList->AddText(p2, text, line2);
    }
    ImGui::End();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(4);
}

void ShutdownMobaQueueHUD()
{
    if (g_joinTexture.texture != NULL)
    {
        g_joinTexture.texture->Release();
        g_joinTexture.texture = NULL;
    }

    g_joinTexture.width = 0;
    g_joinTexture.height = 0;
    g_joinTexture.loadAttempted = false;
    g_joinTexture.loadFailedReported = false;
}

} // namespace PhoenixImGui
