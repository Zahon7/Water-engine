#ifndef CUSTOMUI_HPP
#define CUSTOMUI_HPP

#include <functional>
#include <map>

#include <raylib.h>
#include "core/uidef.hpp"
#include "erl.cpp"

#include <sys/stat.h>
#include <filesystem>
#include <regex>

namespace CustomUI {

using Function = std::function<void(void)>;

Function function;
bool enabled = false;


#define BACKGROUND_COLOR Color {255, 255, 255, 200}
#define WINDOW_BACKGROUND_COLOR Color {255, 255, 255, 220}

#define SELECTED_COLOR RED
#define OUTLINE_COLOR BLACK

#define TEXT_COLOR BLACK
#define TEXT_DISABLED_COLOR GRAY

void DrawWindow(std::string name, Vector2 size = Vector2 {GetScreenWidth() / 1.8f, GetScreenHeight() / 2.f}) {
	Rectangle rectangle = Erl::GetCenterRectangle(Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f}, size);

	DrawRectangleRec(rectangle, WINDOW_BACKGROUND_COLOR);
	DrawRectangleLinesEx(rectangle, 2.5f, OUTLINE_COLOR);

	DrawTextEx(UiDef::font, name.c_str(), Vector2 {rectangle.x + 5.f, rectangle.y + 5.f}, 25.f, 1.5f, BLACK);
}

struct InputInfo {
	std::string input;
	std::string defaultValue;
};

std::map<std::string, InputInfo> inputs;

void AddInput(std::string id, std::string defaultValue) {
	inputs[id] = InputInfo {.input = "", .defaultValue = defaultValue};
}

void DrawInput(std::string id, Vector2 center, Vector2 scale = {GetScreenWidth() / 4.f, GetScreenHeight() / 10.f}, float fontSize = 30.f) {
	Rectangle rectangle = Erl::GetCenterRectangle(center, scale);

	bool over = CheckCollisionPointRec(GetMousePosition(), rectangle);
	if(over) {
		char character = GetCharPressed();
		if(character > 0) {
			if(MeasureTextEx(UiDef::font, inputs[id].input.c_str(), fontSize, 1.5f).x < scale.x - GetScreenWidth() / 24.f)
				inputs[id].input += character;
		}

		if(IsKeyPressed(KEY_BACKSPACE)) {
			if(!inputs[id].input.empty())
				inputs[id].input.pop_back();
		}
	}
	
	DrawRectangleRec(rectangle, BACKGROUND_COLOR);
	DrawRectangleLinesEx(rectangle, 2.5f, over ? SELECTED_COLOR : OUTLINE_COLOR);

	if(over || inputs[id].input != "")
		DrawTextEx(UiDef::font, inputs[id].input.c_str(), Vector2 {center.x - scale.x / 2.f + 15.f, center.y - scale.y / 2.f + 15.f}, fontSize, 1.5f, TEXT_COLOR);
	else
		DrawTextEx(UiDef::font, inputs[id].defaultValue.c_str(), Vector2 {center.x - scale.x / 2.f + 15.f, center.y - scale.y / 2.f + 15.f}, fontSize, 1.5f, TEXT_DISABLED_COLOR);
}

bool DrawButton(std::string text, Vector2 center, Vector2 scale = {GetScreenWidth() / 6.f, GetScreenHeight() / 10.f}, float fontSize = 30.f) {
	Rectangle rectangle = Erl::GetCenterRectangle(center, scale);
	DrawRectangleRec(rectangle, BACKGROUND_COLOR);

	bool hover = CheckCollisionPointRec(GetMousePosition(), rectangle);
	DrawRectangleLinesEx(rectangle, 2.5f, hover ? SELECTED_COLOR : OUTLINE_COLOR);

	DrawTextEx(UiDef::font, text.c_str(), Vector2 {center.x - scale.x / 2.f + 15.f, center.y - scale.y / 2.f + 15.f}, Erl::FontSize(fontSize), 1.5f, TEXT_COLOR);

	return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

struct FilePickerInfo {
	std::string input;
	std::string path;
	std::string filter;
};

std::map<std::string, FilePickerInfo> filePickers;

void AddFilePicker(std::string id, std::string path, std::string filter) {
	filePickers[id] = FilePickerInfo {
		.input = "",
		.path = path,
		.filter = filter
	};
}

void DrawFilePicker(std::string id, Vector2 center, Vector2 size = {GetScreenWidth() / 2.5f, GetScreenHeight() / 2.5f}) {
	Rectangle rectangle = Erl::GetCenterRectangle(center, size);

	DrawRectangleRec(rectangle, BACKGROUND_COLOR);
	DrawRectangleLinesEx(rectangle, 2.5f, OUTLINE_COLOR);

	float yPointer = rectangle.y;
	for (const auto& entry : std::filesystem::directory_iterator(filePickers[id].path)) {
		std::regex regex(filePickers[id].filter);
		std::smatch match;
		std::string path = entry.path().u8string();
		if(!std::regex_search(path, match, regex)) continue;

		Rectangle pathRectangle = {
			rectangle.x,
			yPointer,
			rectangle.width,
			35.f
		};

        DrawRectangleRec(pathRectangle, BACKGROUND_COLOR);
        DrawRectangleLinesEx(pathRectangle, 2.5f, filePickers[id].input == entry.path() ? SELECTED_COLOR : OUTLINE_COLOR);

        Erl::DrawTextCentered(UiDef::font, entry.path().c_str(), Vector2 {pathRectangle.x + pathRectangle.width / 2.f, pathRectangle.y + pathRectangle.height / 2.f}, Erl::FontSize(512.f / entry.path().u8string().length()), 1.f, BLACK);

        if(CheckCollisionPointRec(GetMousePosition(), pathRectangle) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        	filePickers[id].input = entry.path();
        }

        yPointer += 35.f;
    }
}

bool Update() {
	if(enabled) {
		function();
	}
	return !enabled;
}

void Enable(Function newFunction) {
	function = newFunction;
	
	enabled = true;
}

void Disable() {
	enabled = false;
}

void InfoDialog(std::string text) {
	CustomUI::DrawWindow("Info", Vector2 {GetScreenWidth() / 2.8f, GetScreenHeight() / 2.4f});
	Erl::DrawTextCentered(UiDef::font, text, Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f - (GetScreenHeight() / 16.f)}, Erl::FontSize(550.f / text.length()), 1.5f, BLACK);
	if(CustomUI::DrawButton("Ok", Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f + (GetScreenHeight() / 16.f)})) CustomUI::Disable();
}

};
#endif
