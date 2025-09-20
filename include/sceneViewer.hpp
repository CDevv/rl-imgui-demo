#ifndef _SCENEVIEWER_H
#define _SCENEVIEWER_H

#include <raylib.h>
#include "imgui.h"
#include "rlImGui.h"

#include <memory>
#include "mouseInput.hpp"

class SceneViewer {
private:
	Rectangle rect;
	Rectangle renderRect;
	RenderTexture renderTexture;
	Camera2D camera;
	bool open;
	std::unique_ptr<MouseInputComponent> mouseInput;
	Rectangle getWindowRect();
	Rectangle getRenderRect();
public:
	SceneViewer(Rectangle rect);
	void update();
	void draw();
	void clean();
};

#endif