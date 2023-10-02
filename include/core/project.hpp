#ifndef PROJECT_HPP
#define PROJECT_HPP

#include <string>
#include <raylib.h>

#include <sys/stat.h>
#include <libgen.h>

#include "customui.hpp"
#include "uidef.hpp"

namespace Project {

void New(std::string name) {
	mkdir(("../projects/" + name).c_str(), 0777);
	UiDef::project = "../projects/" + name;
	
	SetWindowTitle(("Water engine - " + name).c_str());
}

void NewUI() {
	CustomUI::DrawWindow("New project");
	CustomUI::DrawInput("projectName", Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.3f});
	if(CustomUI::DrawButton("Ok", Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 1.7f})) {
		New(CustomUI::inputs["projectName"].input);
		CustomUI::Disable();
	}
}

void Open(std::string path) {
	UiDef::project = path;

	SetWindowTitle(TextFormat("Water engine - %s", basename((char*)path.c_str())));
}

void OpenUI() {
	CustomUI::DrawWindow("Open project", Vector2 {GetScreenWidth() / 2.2f, GetScreenHeight() / 1.7f});
	CustomUI::DrawFilePicker("openProjectFilePicker", Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.15f});
	
	if(CustomUI::DrawButton("Cancel", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 11.f), GetScreenHeight() / 1.375f})) CustomUI::Disable();

	if(CustomUI::DrawButton("Ok", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 11.f), GetScreenHeight() / 1.375f}) && CustomUI::filePickers["openProjectFilePicker"].input != "") {
		Open(CustomUI::filePickers["openProjectFilePicker"].input);
		CustomUI::Disable();
	}
}

};
#endif
