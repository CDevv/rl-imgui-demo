#include "textEdit.hpp"

TextEdit::TextEdit() 
{
	this->rect = Rectangle { 240, 560 };
	this->lines = std::vector<Line>{};
	//resetCursor();
	this->cursorPos = IntVec2 { 3, 1 };

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

void TextEdit::update()
{

}

void TextEdit::draw()
{
	const ImVec2 calcText = ImGui::CalcTextSize("0000");
	const float lineNumWidth = calcText.x;
	const float lineHeight = calcText.y;

	ImGui::SetNextWindowContentSize(ImVec2 { rect.width, rect.height });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 { 0, 0 });
	if (ImGui::Begin("DrawList")) {
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