#include "textEdit.hpp"
#include <cstdio>
#include <cmath>

TextEdit::TextEdit() 
{
	this->rect = Rectangle { 240, 560 };
	this->lines = std::vector<Line>{};
	//resetCursor();
	this->cursorPos = IntVec2 { 3, 1 };
	this->debounce = 0.05f;
	this->visibility = VisiblityInfo { 0, 0 };

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

ImVec2 TextEdit::getMouseWindowPos()
{
	ImVec2 mouseWindowPos = ImVec2 {
		ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x,
		ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y
	};
	return mouseWindowPos;
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

IntVec2 TextEdit::getPositionAtMouse()
{
	const ImVec2 calcText = ImGui::CalcTextSize("0000");
	const float lineNumWidth = calcText.x;
	const float lineHeight = calcText.y;

	ImVec2 mouseWindowPos = getMouseWindowPos();

	float offsetY = mouseWindowPos.y;
	int resultY = static_cast<int>(floor(offsetY / lineHeight));
	int resultX = 0;

	if (resultY >= lines.size() - 1) {
		int addedY = static_cast<int>(lines.size() - 1 - resultY);
		if (addedY < 0) {
			addedY = 1;
		}
		resultY -= addedY;
	}

	int counter = 0;
	std::string currentStr = "";
	std::string lineContent = lines.at(resultY).content;
	float mouseOffset = (mouseWindowPos.x - lineNumWidth);
	for (int i = 0; i < lineContent.size(); i++) {
		ImVec2 lineTextCalc = ImGui::CalcTextSize(currentStr.c_str());
		if (lineTextCalc.x > mouseOffset) {
			resultX = i;
			break;
		}
		currentStr = currentStr.append(TextFormat("%c", lineContent.at(i)));
	}

	ImVec2 lineTextCalc = ImGui::CalcTextSize(lineContent.c_str());
	if (mouseOffset > lineTextCalc.x) {
		resultX = lineContent.size() + 1;
	}

	IntVec2 result = IntVec2 { resultX - 1, resultY };
	return result;
}

void TextEdit::insertLine(int index, std::string content)
{
	lines.insert(lines.begin() + index, Line {
		content
	});
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
	const ImVec2 calcText = ImGui::CalcTextSize("0000");
	const float lineNumWidth = calcText.x;
	const float lineHeight = calcText.y;

	if (debounce < 0) {
		if (ImGui::IsKeyDown(ImGuiKey_LeftArrow)) {
			if (cursorPos.x > 0) {
				cursorPos.x--;
			} else {
				if (cursorPos.y > 0 && cursorPos.x == 0) {
					cursorPos.y--;
					cursorPos.x = getLastCharIndex();
				}
			}
		}

		if (ImGui::IsKeyDown(ImGuiKey_RightArrow)) {
			if (cursorPos.x < lines.at(cursorPos.y).content.size()) {
				cursorPos.x++;
			} else {
				if (cursorPos.y < (lines.size() - 1) && cursorPos.x == getLastCharIndex()) {
					cursorPos.y++;
					cursorPos.x = 0;
				}
			}
		}

		if (ImGui::IsKeyDown(ImGuiKey_UpArrow)) {
			if (cursorPos.y > 0) {
				cursorPos.y--;
				fitCursorInLine();

				if (cursorPos.y < visibility.firstVisibleLine) {
					ImGui::SetScrollY(lineHeight * cursorPos.y);
				}
			}
		}

		if (ImGui::IsKeyDown(ImGuiKey_DownArrow)) {
			if (cursorPos.y < (lines.size() - 1)) {
				cursorPos.y++;
				fitCursorInLine();

				if (cursorPos.y > visibility.lastVisibleLIne) {
					ImGui::SetScrollY(lineHeight * (visibility.firstVisibleLine + 1));
				}
			}
		}

		if (ImGui::IsKeyDown(ImGuiKey_Backspace)) {
			if (cursorPos.x > 0) {
				eraseChar(cursorPos.x - 1);
				cursorPos.x--;
			} else {
				if (cursorPos.y > 0) {
					eraseCurrentLine();
				}
			}
		}

		if (ImGui::IsKeyDown(ImGuiKey_Enter)) {
			std::string newLineContent = TextSubtext(lines.at(cursorPos.y).content.c_str(), cursorPos.x, 
				(lines.at(cursorPos.y).content.size() - cursorPos.x));

			lines.at(cursorPos.y).content.erase(cursorPos.x);
			insertLine(cursorPos.y + 1, newLineContent);
			cursorPos.y++;
			cursorPos.x = 0;
		}

		debounce = 0.05f;
	} else {
		debounce -= ImGui::GetIO().DeltaTime;
	}

	if (ImGui::IsKeyPressed(ImGuiKey_MouseLeft, false)) {
		ImVec2 mouseWindowPos = getMouseWindowPos();

		ImVec2 mousePos = ImVec2 {
			ImGui::GetMousePos().x - ImGui::GetWindowPos().x,
			ImGui::GetMousePos().y - ImGui::GetWindowPos().y
		};

		bool isInRect = mousePos.x > lineNumWidth && mousePos.y > 21 && 
		mousePos.x < ImGui::GetWindowWidth() && mousePos.y < ImGui::GetWindowHeight();

		if (isInRect) {
			IntVec2 newCursorPos = getPositionAtMouse();

			cursorPos.y = newCursorPos.y;
			cursorPos.x = newCursorPos.x;
		}
	}

	if (ImGui::IsKeyPressed(ImGuiKey_Tab, false)) {
		putChar('\t');
	}

	if (ImGui::IsKeyPressed(ImGuiKey_Keypad7, false)) {
		cursorPos.x = 0;
	}

	if (ImGui::IsKeyPressed(ImGuiKey_Keypad1, false)) {
		cursorPos.x = getLastCharIndex();
	}

	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_V)) {
		printf("paste cmd \n");
		int count = 0;
		char** clipLines = TextSplit(GetClipboardText(), '\n', &count);
		if (count > 0) {
			for (int i = 0; i < count; i++) {
				printf("%s \n", clipLines[i]);
			}

			std::string firstHalf = TextSubtext(lines.at(cursorPos.y).content.c_str(), 0, cursorPos.x);
			std::string secondHalf = TextSubtext(lines.at(cursorPos.y).content.c_str(), cursorPos.x, lines.at(cursorPos.y).content.size() - cursorPos.x);

			lines.at(cursorPos.y).content = firstHalf;
			lines.at(cursorPos.y).content.append(clipLines[0]);
			cursorPos.x += strlen(clipLines[0]);
			if (count == 2) {
				insertLine(cursorPos.y + 1, std::string(TextFormat("%s%s", clipLines[1], secondHalf.c_str())));
				cursorPos.x = strlen(clipLines[1]);
				cursorPos.y++;
			} else if (count >= 3) {
				for (int j = 0; j < count - 2; j++) {
					//printf("%i \n", j);
					insertLine(cursorPos.y + 1, std::string(clipLines[1 + j]));
					cursorPos.y++;
				}
				insertLine(cursorPos.y + 1, std::string(TextFormat("%s%s", clipLines[count - 1], secondHalf.c_str())));
				cursorPos.x = strlen(clipLines[count - 1]);
				cursorPos.y++;
			}
		}
	}
}

void TextEdit::update()
{
}

void TextEdit::updateVisibilityInfo()
{
	const ImVec2 calcText = ImGui::CalcTextSize("0000");
	const float lineHeight = calcText.y;

	float scrollY = ImGui::GetScrollY();
	visibility.firstVisibleLine = static_cast<int>((scrollY / lineHeight));
	visibility.lastVisibleLIne = static_cast<int>(((scrollY + ImGui::GetWindowHeight() - 20) / lineHeight) - 1);
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
		/*
		std::string clipContent = GetClipboardText();
		if (!clipContent.empty()) {
			printf("%s \n", clipContent.c_str());
		}
		*/

		handleCharInput();
		handleKeyInput();

		updateVisibilityInfo();

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		ImGui::BeginChild("content", ImVec2(windowSize.x, lines.size() * lineHeight), 0, ImGuiWindowFlags_HorizontalScrollbar);

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

		if (cursorPos.y == index) {
			draw->AddText(ImVec2 
				{ cursor.x + (lineNumWidth - numTextWidth), cursor.y + (index * lineHeight) }, 
				ImColor(0.650f, 0.650f, 0.650f, 1.000f), TextFormat("%i", index + 1));
		} else {
			draw->AddText(ImVec2 
				{ cursor.x + (lineNumWidth - numTextWidth), cursor.y + (index * lineHeight) }, 
				IM_COL32_WHITE, TextFormat("%i", index + 1));
		}
		
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