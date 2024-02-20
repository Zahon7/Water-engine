#ifndef UIDEF_HPP
#define UIDEF_HPP

#include <raylib.h>
#include <vector>
#include <string>
#include "stringutils.hpp"

namespace UiDef {
	enum MainSceneType {SCENE = 0, CODE, _LAST};
	MainSceneType mainType = MainSceneType::SCENE;

	std::string project;

	Camera camera;

	Font font;

	Rectangle console;
	Rectangle components;

	Rectangle mainBar;
	Rectangle main;
	Vector2 codeOffset;

	Rectangle nodeExplorer;
	Rectangle topBar;

	std::vector<std::pair<Vector2, Vector2>> splitLines;

	RenderTexture mainRender;

	std::string consoleContent;

	std::vector<std::string> codeLines = {""};
	StringUtils::CodePointer codePointer; // .x = line, .y = character

	std::string codeSrc = "";

	void Recalculate() {
		/* -------- FONT ------------ */
		font = LoadFont("engine/font.ttf");
		SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

		/* -------- CLEARING -------- */

		splitLines.clear();

		/* --------- SETTING UP ---------- */
		codePointer.lines = codeLines;

		/* -------- SCENE RECTANGLES -------- */

		topBar = Rectangle {
			0, 0,
			(float)GetScreenWidth(),
			GetScreenHeight() / 25.f
		};

		console = Rectangle {
			0.0f, topBar.height,
			GetScreenWidth() / 4.0f, (float)GetScreenHeight()
		};

		mainBar = Rectangle {
			console.width,
			topBar.height,
			GetScreenWidth() - (GetScreenWidth() / 2.0f),
			GetScreenHeight() / 24.f
		};

		main = Rectangle {
			console.width,
			mainBar.y + mainBar.height,
			GetScreenWidth() - (GetScreenWidth() / 2.0f),
			GetScreenHeight() - (GetScreenHeight() / 2.5f + mainBar.height)
		};

		codeOffset = Vector2 {0.f, 0.f};

		components = Rectangle {
			console.width,
			main.y + main.height,
			GetScreenWidth() - (GetScreenWidth() / 2.0f),
			GetScreenHeight() - main.height
		};

		nodeExplorer = Rectangle {
			main.x + main.width,
			topBar.height,
			GetScreenWidth() - (main.x + main.width),
			(float)GetScreenHeight()
		};

		/* ----- SPLIT LINES ----- */

		splitLines.push_back(std::make_pair(
			Vector2 {console.width, topBar.height},
			Vector2 {console.width, console.height}
		));

		splitLines.push_back(std::make_pair(
			Vector2 {console.width, components.y},
			Vector2 {console.width + components.width, components.y}
		));

		splitLines.push_back(std::make_pair(
			Vector2 {nodeExplorer.x, topBar.height},
			Vector2 {nodeExplorer.x, nodeExplorer.height}
		));

		splitLines.push_back(std::make_pair(
			Vector2 {0, topBar.height},
			Vector2 {topBar.width, topBar.height}
		));
	}

	void Calculate() {
		Recalculate();

		/* ----- INITIALIZING ------ */
		mainRender = LoadRenderTexture(main.width, main.height);
	}
};

#endif
