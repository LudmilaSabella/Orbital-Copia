/**
 * Orbital UI renderer for ImGui.
 *
 * Copyright 2017-2021. Orbital project.
 * Released under MIT license. Read LICENSE for more details.
 *
 * Authors:
 * - Alexandro Sanchez Bach <alexandro@phi.nz>
 */

#include "ui_orbital.h"
#include <orbital/hardware/ps4.h>

#include <orbital/core.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#define IMGUI_IMPL_API
#include <imgui.h>

void OrbitalUI::init() {
    auto& io = ImGui::GetIO();
    auto& style = ImGui::GetStyle();

    // Create dockspace
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Calculate DPI scaling
    float dpi_scale = 1.0;
    float dpi_diagonal;
    if (SDL_GetDisplayDPI(0, &dpi_diagonal, nullptr, nullptr) == 0)
        dpi_scale = dpi_diagonal / 96.0f;

    // Create new fonts
    font_text_data = fs::read_bin("fonts/Roboto-Medium.ttf");
    font_code_data = fs::read_bin("fonts/RobotoMono-Medium.ttf");

    ImFontConfig font_config{};
    font_config.FontDataOwnedByAtlas = false;
    font_text = io.Fonts->AddFontFromMemoryTTF(font_text_data.data(), static_cast<S32>(font_text_data.size()),
        16.f * dpi_scale, &font_config, io.Fonts->GetGlyphRangesDefault());
    font_code = io.Fonts->AddFontFromMemoryTTF(font_code_data.data(), static_cast<S32>(font_code_data.size()),
        16.f * dpi_scale, &font_config, io.Fonts->GetGlyphRangesDefault());
    font_default = io.Fonts->AddFontDefault();

    tab_cpu.set_font_code(font_code);
    tab_gpu.set_font_code(font_code);

    // Initialize style
    style.Colors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]               = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_Header]                = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_Separator]             = style.Colors[ImGuiCol_Border];
    style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_Tab]                   = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
    style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
    style.Colors[ImGuiCol_TabActive]             = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused]          = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_DockingPreview]        = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
    style.Colors[ImGuiCol_DockingEmptyBg]        = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget]        = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void OrbitalUI::render(PS4Machine& ps4) {
    ImGui::PushFont(font_text);

    render_menus(ps4);

    // Main space
    ImGuiWindowFlags window_flags = 0
        | ImGuiWindowFlags_MenuBar
        | ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoNavFocus;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.7f));

    if (ImGui::Begin("mainspace", nullptr, window_flags)) {
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(3);

        // Tabs
        if (ImGui::BeginTabBar("tabspace")) {
            if (ImGui::BeginTabItem("CPU")) {
                tab_cpu.render(ps4);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("GPU")) {
                tab_gpu.render(ps4);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("PCI")) {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("ICC")) {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("SBL")) {
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        // Tools
        if (show_uart1) {
            tool_uart1.Draw("UART #1");
        }
        if (show_uart0) {
            tool_uart0.Draw("UART #0");
        }
        if (show_mem_gpa) {
            if (ImGui::Begin("Memory (GPA)")) {
                const auto addr = 0x600000;
                const auto size = 0x1000;
                Buffer buf(size);
                ps4.mem()->read(addr, size, &buf[0]);
                me_mem_gpa.DrawContents(buf.data(), buf.size(), addr);
            }
            ImGui::End();
        }

        ImGui::End();
    }

    ImGui::PopFont();
}

CharHost* OrbitalUI::get_uart0_backend() {
    return tool_uart0.backend();
}

CharHost* OrbitalUI::get_uart1_backend() {
    return tool_uart1.backend();
}

void OrbitalUI::render_menus(PS4Machine& ps4) {
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("File", true)) {
        if (ImGui::MenuItem("Exit", NULL, false, true)) {
            SDL_Event quit;
            quit.type = SDL_QUIT;
            SDL_PushEvent(&quit);
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Machine", true)) {
        const bool running = ps4.is_running();
        if (ImGui::MenuItem("Resume", nullptr, false, !running)) {
            ps4.request_resume();
        }
        if (ImGui::MenuItem("Pause", nullptr, false, running)) {
            ps4.request_pause();
        }
        if (ImGui::MenuItem("Reset")) {
            ps4.reset();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Load state", nullptr, false, false)) {
            // TODO
        }
        if (ImGui::MenuItem("Save state", nullptr, false, false)) {
            // TODO
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Configuration...", nullptr, false, false)) {
            // TODO
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Tools", true)) {
        ImGui::MenuItem("Statistics", "Alt+1", &show_stats, false);
        ImGui::MenuItem("UART #0", "Alt+2", &show_uart0, true);
        ImGui::MenuItem("UART #1", "Alt+3", &show_uart1, true);
        ImGui::MenuItem("Executing Processes", "Alt+4", &show_executing_processes, false);
        ImGui::MenuItem("Process List", "Alt+5", &show_process_list, false);
        ImGui::Separator();
        ImGui::MenuItem("CP Commands", "Alt+6", &show_trace_cp, false);
        ImGui::MenuItem("ICC Commands", "Alt+7", &show_trace_icc, false);
        ImGui::MenuItem("SAMU Commands", "Alt+8", &show_trace_samu, false);
        ImGui::Separator();
        ImGui::MenuItem("Memory Editor (GPA)", "Ctrl+1", &show_mem_gpa, true);
        ImGui::MenuItem("Memory Editor (GVA)", "Ctrl+2", &show_mem_gva, false);
        ImGui::MenuItem("Memory Editor (GART)", "Ctrl+3", &show_mem_gart, false);
        ImGui::MenuItem("Memory Editor (IOMMU)", "Ctrl+4", &show_mem_iommu, false);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help", true)) {
        if (ImGui::MenuItem("About...", nullptr, false, false)) {
            // TODO
        }
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}
