#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#include "sceneViewer.hpp"
#include "textEdit.hpp"

#include <string>
#include <cstdio>

bool ImGuiDemo = true;

void DoMainMenu() 
{
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Exit")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window")) {
			ImGui::MenuItem("ImGui Demo", nullptr, &ImGuiDemo);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

int main() 
{
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
	InitWindow(1200, 720, "Test.");

	rlImGuiSetup(true);

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	io.Fonts->Clear();
	io.Fonts->AddFontFromFileTTF("./Karla-Regular.ttf", 15.0f);

	SceneViewer scene(Rectangle { 20, 20, static_cast<float>(GetScreenWidth() / 2), static_cast<float>(GetScreenHeight() / 2) });
	TextEdit textEdit;

	//state
	bool open = true;
	static char tx[128] = "";

	while (!WindowShouldClose()) {
		scene.update();
		textEdit.update();
        
		BeginDrawing();
		ClearBackground(RAYWHITE);
		rlImGuiBegin();

		DoMainMenu();

		if (ImGuiDemo) {
			ImGui::ShowDemoWindow(&open);
		}

		ImGui::Text("Hello World!");
		ImGui::InputText("My String", tx, IM_ARRAYSIZE(tx));
		if (ImGui::Button("Button!")) {
			std::string textStr = tx;
			printf("%s\n", textStr.c_str());
		}

		scene.draw();
		textEdit.draw();

		rlImGuiEnd();
		EndDrawing();
	}

	rlImGuiShutdown();
	scene.clean();
	CloseWindow();

	return 0;
}