#include "sceneViewer.hpp"

#include <raymath.h>
#include <cstdio>

SceneViewer::SceneViewer(Rectangle rect)
{
	this->rect = rect;
	this->renderRect = Rectangle {
		rect.x, rect.y + 21,
		rect.width, rect.height - 21
	};
	renderTexture = LoadRenderTexture(renderRect.width, renderRect.height);

	camera = Camera2D { {0} };
    camera.target = Vector2 { 0, 0 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    mouseInput = std::make_unique<MouseInputComponent>(Vector2 {rect.x, rect.y}, 
    	camera, Rectangle {rect.width, rect.height - 21}, 0);
}

Rectangle SceneViewer::getWindowRect()
{
	ImVec2 pos = ImGui::GetWindowPos();
	ImVec2 size = ImGui::GetWindowSize();
	Rectangle windowRect = Rectangle {
		pos.x, pos.y, size.x, size.y
	};
	return windowRect;
}

Rectangle SceneViewer::getRenderRect()
{
	Rectangle rec = getWindowRect();
	rec.y += 21;
	rec.height -= 21;
	return rec;
}

void SceneViewer::update()
{
	/*
	if (IsWindowResized())
	{
		UnloadRenderTexture(renderTexture);
		renderTexture = LoadRenderTexture(GetScreenWidth() / 2, GetScreenHeight() / 2);
	}
	*/
	
	mouseInput->update();

	BeginTextureMode(renderTexture);
	BeginMode2D(camera);

	ClearBackground(RAYWHITE);
	DrawRectangleRec(Rectangle { 20, 20, 100, 100 }, RED);

	if (mouseInput->isInRect()) {
		/*
		Vector2 worldMouse = mouseInput->getMousePos();
		DrawCircleV(worldMouse, 4.0f, MAROON);
		*/

		Vector2 mouse = mouseInput->getMouseWorldPos();
		DrawCircleV(mouse, 4.0f, GREEN);
	}

	EndMode2D();
	EndTextureMode();
}

void SceneViewer::draw()
{
	ImGui::SetNextWindowContentSize(ImVec2 {
		rect.width, rect.height - 21
	});
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("SceneViewer", &open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	Rectangle windowRect = getWindowRect();
	//this->rect = windowRect;
	this->renderRect = getRenderRect();

	if (renderRect.width != renderTexture.texture.width) {
		UnloadRenderTexture(renderTexture);
		renderTexture = LoadRenderTexture(renderRect.width, renderRect.height);
	}

	mouseInput->setCameraRect(renderRect);

	/*
	if (CheckCollisionPointRec(GetMousePosition(), windowRect)) {
		Vector2 windowPos = Vector2 { windowRect.x, windowRect.y };
		Vector2 mousePosRelative = Vector2Subtract(GetMousePosition(), windowPos);
		printf("[%f, %f]! \n", mousePosRelative.x, mousePosRelative.y);
	}
	*/

	//printf("[%f, %f, %f, %f] \n", cameraRect.x, cameraRect.y, cameraRect.width, cameraRect.height);

	//rlImGuiImageRenderTextureFit(&renderTexture, true);
	rlImGuiImageRenderTexture(&renderTexture);

	ImGui::End();

	ImGui::PopStyleVar();
}

void SceneViewer::clean()
{
	UnloadRenderTexture(renderTexture);
}