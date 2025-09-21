#include "textEdit.hpp"
#include <cstdio>

TextEdit::TextEdit() 
{
	this->rect = Rectangle { 240, 560 };
	this->lines = std::vector<Line>{};
	//resetCursor();
	this->cursorPos = IntVec2 { 3, 1 };
	this->debounce = 0.05f;

	pushLine("Hello1");
	pushLine("Hello2");
	pushLine("\tHello2");
	for (int i = 0; i < 50; i++) {
		pushLine("Test!");
	}
}

void TextEdit::resetCursor()
{
	this->cursorPos = IntVec2 { 0, 0 };
}

void TextEdit::pushLine(std::string content)
{
	this->lines.push_back(Line {
		content
	});
}

char TextEdit::getCharAt(int index)
{
	int currentLine = cursorPos.y;
	return lines.at(currentLine).content.at(index);
}

char TextEdit::getCurrentChar()
{
	return getCharAt(cursorPos.x);
}

void TextEdit::putChar(char c)
{
	int currentLine = cursorPos.y;
	lines.at(currentLine).content.insert(cursorPos.x, TextFormat("%c", c));
	cursorPos.x++;
}

void TextEdit::eraseChar(int index)
{
	int currentLine = cursorPos.y;
	lines.at(currentLine).content.erase(index, 1);
}

void TextEdit::eraseCurrentLine()
{
	int contentSize = lines.at(cursorPos.y).content.size(); 
	lines.at(cursorPos.y - 1).content.append(
		lines.at(cursorPos.y).content
	);
	lines.erase(lines.begin() + cursorPos.y);
	cursorPos.y--;
	cursorPos.x = getLastCharIndex() - contentSize;
}

void TextEdit::fitCursorInLine()
{
	int lineContentSize = lines.at(cursorPos.y).content.size();
	if (cursorPos.x > lineContentSize) {
		cursorPos.x = lineContentSize;
	}
}

int TextEdit::getLastCharIndex()
{
	return lines.at(cursorPos.y).content.size();
}

void TextEdit::handleCharInput()
{
	ImGuiIO& io = ImGui::GetIO();

	if (io.InputQueueCharacters.Size > 0) {
		for (int i = 0; i < io.InputQueueCharacters.Size; i++) {
			if (io.InputQueueCharacters[i] < 255) {
				putChar(static_cast<char>(io.InputQueueCharacters[i]));
			}
		}
	}
}

void TextEdit::handleKeyInput()
{
	if (ImGui::IsKeyDown(ImGuiKey_LeftArrow)) {
		if (debounce < 0) {
			if (cursorPos.x > 0) {
				cursorPos.x--;
			} else {
				if (cursorPos.y > 0 && cursorPos.x == 0) {
					cursorPos.y--;
					cursorPos.x = getLastCharIndex();
				}
			}
			debounce = 0.05f;
		} else {
			debounce -= ImGui::GetIO().DeltaTime;
		}
	}

	if (ImGui::IsKeyDown(ImGuiKey_RightArrow)) {
		if (debounce < 0) {
			if (cursorPos.x < lines.at(cursorPos.y).content.size()) {
				cursorPos.x++;
			} else {
				if (cursorPos.y < (lines.size() - 1) && cursorPos.x == getLastCharIndex()) {
					cursorPos.y++;
					cursorPos.x = 0;
				}
			}
			debounce = 0.05f;
		} else {
			debounce -= ImGui::GetIO().DeltaTime;
		}
	}

	if (ImGui::IsKeyDown(ImGuiKey_UpArrow)) {
		if (debounce < 0) {
			if (cursorPos.y > 0) {
				cursorPos.y--;
				fitCursorInLine();
			}
			debounce = 0.05f;
		} else {
			debounce -= ImGui::GetIO().DeltaTime;
		}
	}

	if (ImGui::IsKeyDown(ImGuiKey_DownArrow)) {
		if (debounce < 0) {
			if (cursorPos.y < (lines.size() - 1)) {
				cursorPos.y++;
				fitCursorInLine();
			}
			debounce = 0.05f;
		} else {
			debounce -= ImGui::GetIO().DeltaTime;
		}
	}

	if (ImGui::IsKeyDown(ImGuiKey_Backspace)) {
		if (debounce < 0) {
			if (cursorPos.x > 0) {
				eraseChar(cursorPos.x - 1);
				cursorPos.x--;
			} else {
				if (cursorPos.y > 0) {
					eraseCurrentLine();
				}
			}
			debounce = 0.05f;
		} else {
			debounce -= ImGui::GetIO().DeltaTime;
		}
	}
}

void TextEdit::update()
{
}

void TextEdit::draw()
{
	ImGuiIO& io = ImGui::GetIO();

	const ImVec2 calcText = ImGui::CalcTextSize("0000");
	const float lineNumWidth = calcText.x;
	const float lineHeight = calcText.y;

	ImGui::SetNextWindowContentSize(ImVec2 { rect.width, rect.height });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 { 0, 0 });
	if (ImGui::Begin("DrawList")) {
		handleCharInput();
		handleKeyInput();

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		ImGui::BeginChild("content", ImVec2(windowSize.y, lines.size() * lineHeight), 0, ImGuiWindowFlags_HorizontalScrollbar);

		ImDrawList* draw = ImGui::GetWindowDrawList();
		ImVec2 cursor = ImGui::GetCursorScreenPos();
		drawBackground(draw, cursor);
		drawLines(draw, cursor);
		drawCursor(draw, cursor);

		ImGui::EndChild();

		ImGui::End();
	}
	ImGui::PopStyleVar();
}

void TextEdit::drawBackground(ImDrawList* draw, ImVec2 cursor)
{
	const ImVec2 calcText = ImGui::CalcTextSize("0000");
	const float lineNumWidth = calcText.x;
	const float lineHeight = calcText.y;

	draw->AddRectFilled(
		ImVec2 { cursor.x, cursor.y }, 
		ImVec2 { cursor.x + lineNumWidth, cursor.y + (lines.size() * lineHeight) }, 
		ImColor(0.060f, 0.060f, 0.060f, 0.940f));

	/*
	draw->AddRectFilled(
		ImVec2 { cursor.x + lineNumWidth, cursor.y + (lineHeight * cursorPos.y) + lineHeight }, 
		ImVec2 { cursor.x + lineNumWidth + (ImGui::GetWindowWidth()), cursorPos.y + (cursorPos.y * lineHeight) + lineHeight }, 
		ImColor(0.226f, 0.226f, 0.226f, 1.000f));
	*/
}

void TextEdit::drawLines(ImDrawList* draw, ImVec2 cursor)
{
	const ImVec2 calcText = ImGui::CalcTextSize("0000");
	const float lineNumWidth = calcText.x;
	const float lineHeight = calcText.y;

	int index = 0;
	for (Line line : lines) {
		draw->AddText(ImVec2 
			{ cursor.x + lineNumWidth, cursor.y + (index * lineHeight) },
			IM_COL32_WHITE, line.content.c_str());
		
		float numTextWidth = ImGui::CalcTextSize(TextFormat("%i", index + 1)).x;
		draw->AddText(ImVec2 
			{ cursor.x + (lineNumWidth - numTextWidth), cursor.y + (index * lineHeight) }, 
			IM_COL32_WHITE, TextFormat("%i", index + 1));
		index++;
	}
}

void TextEdit::drawCursor(ImDrawList* draw, ImVec2 cursor)
{
	const ImVec2 calcText = ImGui::CalcTextSize("0000");
	const float lineNumWidth = calcText.x;
	const float lineHeight = calcText.y;

	float cursorLineX = 0.0f;
	if (cursorPos.x > 0) {
		ImVec2 charCalc = ImGui::CalcTextSize(TextSubtext(lines.at(cursorPos.y).content.c_str(), 0, cursorPos.x));
		cursorLineX = static_cast<float>(cursor.x + lineNumWidth + (charCalc.x));
	} else {
		cursorLineX = static_cast<float>(cursor.x + lineNumWidth);
	}

	draw->AddLine(
		ImVec2 { cursorLineX, cursor.y + (lineHeight * cursorPos.y) }, 
		ImVec2 { cursorLineX, cursor.y + lineHeight + (lineHeight * cursorPos.y) }, 
		IM_COL32_WHITE);
}