#include <raylib.h>

// Include extended raylib
#include "erl.cpp"

// Include core files
#include "core/assets.hpp"
#include "core/uidef.hpp"
#include "core/scene.hpp"
#include "core/topbar.hpp"
#include "core/customui.hpp"

const int maxUpdateTick = 30;

bool sceneSelected = false;

int main() {
	Erl::InitWindowEx(1000, 600, "Water engine", FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);

	TopBar::Init();
	AssetManager::LoadAssets();

	Scene::scene = new Scene::Context();
	Scene::scene->name = "Scene";
	Scene::scene->path = {"Scene"};
	
	Vector2 oldResolution = {1000.0f, 600.0f}; 

	UiDef::Calculate();

	int updateTick = 0;

	UiDef::camera.position = Vector3 {10, 10, 10};
	UiDef::camera.target = Vector3 {0, 0, 0};
	UiDef::camera.up = Vector3 {0, 1, 0};
	UiDef::camera.fovy = 60;
	UiDef::camera.projection = CAMERA_PERSPECTIVE;

    /* Camera object */ {
		Scene::NewObject("Camera", LoadModel("engine/camera.obj"), UiDef::camera.position, Vector3One(), Vector3Zero(), WHITE);
		Scene::Context *cameraContext = Scene::scene->Find("Camera");

		Scene::NewScript("Script", Scene::scene->Find("Camera"));

		Scene::CameraComponent* cameraComponent = new Scene::CameraComponent();
		cameraComponent->position = UiDef::camera.position;
		cameraComponent->target = UiDef::camera.target;
		cameraComponent->fovy = UiDef::camera.fovy;
		cameraContext->components["CameraComponent"] = cameraComponent;
	}

	SetTargetFPS(30);
	SetExitKey(0);

	while(!WindowShouldClose()) {
		float fontSize = Erl::FontSize(18.f);

		BeginTextureMode(UiDef::mainRender); {
			switch(UiDef::mainType) {
				case UiDef::MainSceneType::SCENE: {
					ClearBackground(WHITE);

					if(sceneSelected && UiDef::mainType == UiDef::MainSceneType::SCENE)
						UpdateCamera(&UiDef::camera, CAMERA_FREE);
					
					BeginMode3D(UiDef::camera); {
						DrawGrid(10, 1);
						Scene::DrawScene();
					} EndMode3D();

					Scene::DrawSceneTags();
					break;
				}

				case UiDef::MainSceneType::CODE: {
					ClearBackground(WHITE);

					DrawRectangleRec(UiDef::main, WHITE);

					Rectangle bar {
						0.f,
						0.f,
						UiDef::main.width,
						UiDef::main.height / 16.f
					};

					Rectangle downBar {
						0.f,
						UiDef::main.height - UiDef::main.height / 16.f,
						UiDef::main.width - UiDef::main.height / 16.f,
						UiDef::main.height / 16.f
					};

					Rectangle rightBar {
						UiDef::main.width - UiDef::main.height / 16.f,
						bar.height,
						UiDef::main.height / 16.f,
						UiDef::main.height - UiDef::main.height / 16.f - bar.height
					};

					Rectangle render {
						0.f,
						bar.y + bar.height,
						UiDef::main.width,
						UiDef::main.height - bar.height
					};

					std::vector<std::string> wrapped = UiDef::codePointer.lines;

					std::vector<std::string> pointerStringWrapped = wrapped;

					int index = 0;
					int i = 0;
					for(std::string &line : pointerStringWrapped) {
						for(int j = 0; j < line.length() + 1; j++) {
							line[j] = ' ';
							if(i >= UiDef::codePointer.line && j >= UiDef::codePointer.character) {
								goto quit;
							}
							
							index++;
						}
						i++;
					}

					quit: {
						
					}

					std::string joined = StringUtils::StringJoin(pointerStringWrapped, "\n");

					for(; joined.length() != index ;) {
						joined.erase(joined.begin() + index);
					}
					pointerStringWrapped = StringUtils::StringSplit(joined, "\n");

					if(updateTick > maxUpdateTick / 2.f)
						pointerStringWrapped.back() += "|";

					Erl::DrawLinesSmartL(UiDef::font, pointerStringWrapped, Vector2 {render.x - 1.f + UiDef::codeOffset.x, render.y + 5.f + UiDef::codeOffset.y}, Erl::FontSize(25.f), 1.f, BLACK);

					StringUtils::CodeColorsLines colorLines = StringUtils::CppColorify(wrapped, StringUtils::CodeKeywords {{"void"}, {}, {"Scene::Context", "self->"}});
					
					Erl::DrawLinesSmartL(UiDef::font, colorLines.white, Vector2 {render.x + 5.f + UiDef::codeOffset.x, render.y + 5.f + UiDef::codeOffset.y}, Erl::FontSize(25.f), 1.f, BLACK);
					Erl::DrawLinesSmartL(UiDef::font, colorLines.red, Vector2 {render.x + 5.f + UiDef::codeOffset.x, render.y + 5.f + UiDef::codeOffset.y}, Erl::FontSize(25.f), 1.f, RED);
					Erl::DrawLinesSmartL(UiDef::font, colorLines.green, Vector2 {render.x + 5.f + UiDef::codeOffset.x, render.y + 5.f + UiDef::codeOffset.y}, Erl::FontSize(25.f), 1.f, DARKGREEN);
					Erl::DrawLinesSmartL(UiDef::font, colorLines.blue, Vector2 {render.x + 5.f + UiDef::codeOffset.x, render.y + 5.f + UiDef::codeOffset.y}, Erl::FontSize(25.f), 1.f, BLUE);

					Vector2 measured = Erl::hqMeasureTextExSmart(UiDef::font, wrapped, Erl::FontSize(25.f), 1.f);

					char character = GetCharPressed();

					if(IsKeyPressed(KEY_BACKSPACE) || (IsKeyPressedRepeat(KEY_BACKSPACE) && IsKeyDown(KEY_BACKSPACE))) {
						UiDef::codePointer.Delete();
					} else if(character > 0) {
						UiDef::codePointer.Insert(character);
					}

					if(IsKeyPressed(KEY_ENTER)) {
						UiDef::codePointer.Insert('\n');
					}

					if(IsKeyPressed(KEY_LEFT) || (IsKeyPressedRepeat(KEY_LEFT) && IsKeyDown(KEY_LEFT))) {
						UiDef::codePointer.Backwards(1);
					} else if(IsKeyPressed(KEY_RIGHT) || (IsKeyPressedRepeat(KEY_RIGHT) && IsKeyDown(KEY_RIGHT))) {
						UiDef::codePointer.Forward(1);
					} else if(IsKeyPressed(KEY_UP) || (IsKeyPressedRepeat(KEY_UP) && IsKeyDown(KEY_UP))) {
						UiDef::codePointer.JumpUp();
					} else if(IsKeyPressed(KEY_DOWN) || (IsKeyPressedRepeat(KEY_DOWN) && IsKeyDown(KEY_DOWN))) {
						UiDef::codePointer.JumpDown();
					}

					DrawRectangleRec(bar, Color {245, 245, 245, 245});
					DrawLineEx(Vector2 {bar.x, bar.y + bar.height}, Vector2 {bar.x + bar.width, bar.y + bar.height}, 1.5f, BLACK);

					Erl::DrawTextCentered(UiDef::font, UiDef::codeSrc.empty() ? "Unnamed file" : UiDef::codeSrc, Vector2 {bar.x + bar.width / 2.f, bar.y + bar.height / 2.f}, fontSize, 1.f, BLACK);
					
					if(IsKeyDown(KEY_LEFT_SHIFT)) {
						UiDef::codeOffset.x += GetMouseWheelMove() * 10.f;
					} else {
						UiDef::codeOffset.y += GetMouseWheelMove() * 10.f;
					}

					if(UiDef::codeOffset.x > 0) UiDef::codeOffset.x = 0.f;
					if(UiDef::codeOffset.y > 0) UiDef::codeOffset.y = 0.f;

					if(UiDef::codeOffset.y < -measured.y) UiDef::codeOffset.y = -measured.y;
					if(UiDef::codeOffset.x < -measured.x) UiDef::codeOffset.x = -measured.x;

					DrawRectangleRec(downBar, Color {245, 245, 245, 245});
					DrawRectangleLinesEx(downBar, 1.5f, BLACK);

					Rectangle downPointer = {downBar.x - Remap(UiDef::codeOffset.x, 0.f, measured.x, 0.f, downBar.width - downBar.height), downBar.y, downBar.height, downBar.height};
					Vector2 mousePosition = Vector2Subtract(GetMousePosition(), {UiDef::main.x, UiDef::main.y});

					if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePosition, downBar)) {
						UiDef::codeOffset.x = -Remap(mousePosition.x, downBar.x, downBar.x + downBar.width, -downPointer.width / 2.f, downBar.width + downBar.height / 2.f - downPointer.width / 2.f);
					}

					DrawRectangleRec(downPointer, BLACK);

					DrawRectangleRec(rightBar, Color {245, 245, 245, 245});
					DrawRectangleLinesEx(rightBar, 1.5f, BLACK);

					Rectangle rightPointer = {rightBar.x, rightBar.y - Remap(UiDef::codeOffset.y, 0.f, measured.y, 0.f, rightBar.height - rightBar.width), rightBar.width, rightBar.width};
					if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePosition, rightBar)) {
						UiDef::codeOffset.y = -Remap(mousePosition.y, rightBar.y, rightBar.y + rightBar.height, -rightPointer.width / 2.f, rightBar.height - rightBar.width * 3.75f - rightPointer.width / 2.f);
					}
					
					DrawRectangleRec(rightPointer, BLACK);

					break;
				}
			}
		} EndTextureMode();

		BeginDrawing(); {
			ClearBackground(Color {250, 250, 255, 255});

			// Check the collision for the top bar
			TopBar::Collide();

			if(IsKeyPressed(KEY_ESCAPE) && UiDef::mainType == UiDef::MainSceneType::SCENE) {
				if(sceneSelected) {
					EnableCursor();
					sceneSelected = false;
				}
			}

			Vector2 mousePoint = GetMousePosition();

			if(CheckCollisionPointRec(mousePoint, UiDef::main) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !sceneSelected && !CustomUI::enabled && UiDef::mainType == UiDef::MainSceneType::SCENE) {
				sceneSelected = true;
				DisableCursor();
			}

			DrawLineEx(Vector2 {UiDef::main.x, UiDef::main.y}, Vector2 {UiDef::main.x + UiDef::main.width, UiDef::main.y}, 2.0f, BLACK);

			for(int index = 0; index < UiDef::_LAST; index++) {
				float size = GetScreenWidth() / 16.f;

				Rectangle tabRectangle = Erl::RectangleExpanded(Rectangle {
					UiDef::mainBar.x + (index * (size * 1.2f)) + 10.f,
					UiDef::mainBar.y + 1.5f,
					size,
					UiDef::mainBar.height * 2.f
				}, Vector2 {-5, -5});

				if((int)UiDef::mainType == index) {
					DrawRectangleRounded(tabRectangle, 0.25f, 10, WHITE);
					DrawRectangleRoundedLines(tabRectangle, 0.25f, 10, 1.5f, BLACK);
				} else {
					DrawRectangleRounded(tabRectangle, 0.25f, 10, WHITE);
					DrawRectangleRoundedLines(tabRectangle, 0.25f, 10, 1.5f, BLACK);
					DrawLineEx(Vector2 {tabRectangle.x, UiDef::mainBar.y + UiDef::mainBar.height}, Vector2 {tabRectangle.x + tabRectangle.width, UiDef::mainBar.y + UiDef::mainBar.height}, 2.0f, BLACK);
				}

				switch((UiDef::MainSceneType)index) {
					case UiDef::MainSceneType::SCENE: {
						Erl::DrawTextCentered(UiDef::font, "Scene", Vector2 {tabRectangle.x + tabRectangle.width / 2.f, tabRectangle.y + tabRectangle.height / 4.f}, fontSize, 1.f, BLACK);
						break;
					}
					case UiDef::MainSceneType::CODE: {
						Erl::DrawTextCentered(UiDef::font, "Code", Vector2 {tabRectangle.x + tabRectangle.width / 2.f, tabRectangle.y + tabRectangle.height / 4.f}, fontSize, 1.f, BLACK);
						break;
					}
				}

				if(!CustomUI::enabled && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), Rectangle {tabRectangle.x, tabRectangle.y, tabRectangle.width, tabRectangle.height / 2.f})) {
					UiDef::mainType = (UiDef::MainSceneType)index;
				}
			}

			DrawTexturePro(UiDef::mainRender.texture, Rectangle {0, 0, (float)UiDef::mainRender.texture.width, (float)-UiDef::mainRender.texture.height},
						   UiDef::main, Vector2 {0, 0}, 0.0f, WHITE);

			Erl::DrawTextWrappedL(UiDef::font, UiDef::consoleContent, Vector2Zero(), Erl::FontSize(25.f), 1.0f, BLACK, 25.f + UiDef::main.width + UiDef::nodeExplorer.width);

			Scene::DrawSceneNodes();

			// After drawing everything, draw splitting lines
			for(std::pair<Vector2, Vector2> line : UiDef::splitLines) {
				DrawLineEx(line.first, line.second, 1.5, BLACK);
			}

			// ... and the bar
			TopBar::Draw();

			CustomUI::Update();
		} EndDrawing();

		if(updateTick >= maxUpdateTick) {
			if((int)oldResolution.x != GetScreenWidth() || (int)oldResolution.y != GetScreenHeight()) {
				UiDef::Recalculate();
			}

			Scene::DrawableComponent* drawableComponent = dynamic_cast<Scene::DrawableComponent*>(Scene::scene->Find("Camera")->components["DrawableComponent"]);
			Scene::CameraComponent* cameraComponent = dynamic_cast<Scene::CameraComponent*>(Scene::scene->Find("Camera")->components["CameraComponent"]);
			if(drawableComponent != nullptr && cameraComponent != nullptr) {
				drawableComponent->LookAt(cameraComponent->target);
			}

			updateTick = 0;
		
			oldResolution = Vector2 {(float)GetScreenWidth(), (float)GetScreenHeight()};
		}

		updateTick++;
	}

	CloseWindow();
}
