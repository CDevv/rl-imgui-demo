#ifndef _TEXTEDIT_H
#define _TEXTEDIT_H

#include <raylib.h>
#include <imgui.h>
#include <vector>
#include <string>

struct IntVec2 {
	int x;
	int y;
};

struct Line {
	std::string content;
};

class TextEdit {
private:
	Rectangle rect;
	std::vector<Line> lines;
	IntVec2 cursorPos;
	float debounce;
	void pushLine(std::string content);
	char getCharAt(int index);
	char getCurrentChar();
	void putChar(char c);
	void eraseChar(int index);
	void eraseCurrentLine();
	void fitCursorInLine();
	int getLastCharIndex();
	void handleCharInput();
	void handleKeyInput();
	void drawBackground(ImDrawList* draw, ImVec2 cursor);
	void drawLines(ImDrawList* draw, ImVec2 cursor);
	void drawCursor(ImDrawList* draw, ImVec2 cursor);
public:
	TextEdit();
	void resetCursor();
	void update();
	void draw();
};

#endif