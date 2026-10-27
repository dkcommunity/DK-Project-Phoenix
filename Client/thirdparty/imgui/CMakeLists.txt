# ==========================================================================
# Dear ImGui v1.91.4 — Phoenix static lib
# ==========================================================================
# Vendored from https://github.com/ocornut/imgui.git (.git silindi, dosyalar
# repo icine entegre).
#
# Phoenix kullanim: Hibrit Admin Menu — VS_UI native menu dokunulmaz, ImGui
# ek debug paneli (DebugConsole). Keybind CTRL+SHIFT+F12, authentication
# attrOperator.GetValue() != 0 gate.
# ==========================================================================

cmake_minimum_required(VERSION 3.15)

set(IMGUI_SOURCES
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui_demo.cpp
    backends/imgui_impl_win32.cpp
    backends/imgui_impl_dx9.cpp
    # 2026-08-31 Legacy DX11 gecis P0 — D3D11Backend modu icin DX11 impl
    backends/imgui_impl_dx11.cpp
)

set(IMGUI_HEADERS
    imgui.h
    imgui_internal.h
    imconfig.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
    backends/imgui_impl_win32.h
    backends/imgui_impl_dx9.h
    backends/imgui_impl_dx11.h
)

add_library(imgui_lib STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})

target_include_directories(imgui_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/backends
)

# Phoenix x86 Win32 build — Phoenix CMakeLists.txt'i ile uyumlu compile flag'ler
# inherit edilecek (parent project _UNICODE / _MBCS, runtime lib, etc.).
target_compile_definitions(imgui_lib PRIVATE
    WIN32
    _WINDOWS
    _CRT_SECURE_NO_WARNINGS    # _vsnprintf vb. legacy CRT API'ler icin
)

# D3D9 backend gereksinimleri: d3d9.lib (Phoenix Designed_Chs zaten d3d9
# kullaniyor — link Client target asamasinda saglanir, burada PUBLIC link
# duplicate dependency riskine sokmamak icin koyulmadi).
# Win32 backend: dwmapi (Vista+) cok modern kullanim ama Phoenix XP-compat
# istemiyor — IMGUI default win32 hooks zaten dwmapi'yi opsiyonel kullaniyor.
target_link_libraries(imgui_lib PUBLIC
    d3d9
    # 2026-08-31 DX11 P0 — imgui_impl_dx11 gereksinimi
    d3d11
    dwmapi
)

# Phoenix LNK4006 (562 duplicate def warning'i) maruziyetini azaltmak icin
# inline'in tutarli emit edilmesi gerek. ImGui zaten kendi static linkage
# pattern'i kullaniyor (no inline expansion conflict beklenmez).
set_target_properties(imgui_lib PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    FOLDER "thirdparty"
)

message(STATUS "Phoenix: ImGui v1.91.4 static lib (imgui_lib) configured")
