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
		BeginTextureMode(UiDef::mainRender); {
			ClearBackground(WHITE);

			if(sceneSelected && UiDef::mainType == UiDef::MainSceneType::SCENE)
				UpdateCamera(&UiDef::camera, CAMERA_FREE);
			
			BeginMode3D(UiDef::camera); {
				DrawGrid(10, 1);
				Scene::DrawScene();
			} EndMode3D();

			Scene::DrawSceneTags();
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

			float fontSize = Erl::FontSize(18.f);

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

			switch(UiDef::mainType) {
				case UiDef::MainSceneType::SCENE: {
					DrawTexturePro(UiDef::mainRender.texture, Rectangle {0, 0, (float)UiDef::mainRender.texture.width, (float)-UiDef::mainRender.texture.height},
						UiDef::main, Vector2 {0, 0}, 0.0f, WHITE);
					break;
				}
				case UiDef::MainSceneType::CODE: {
					DrawRectangleRec(UiDef::main, WHITE);

					Rectangle bar {
						UiDef::main.x,
						UiDef::main.y,
						UiDef::main.width,
						UiDef::main.height / 16.f
					};

					DrawRectangleRec(bar, Color {245, 245, 245, 245});
					DrawLineEx(Vector2 {bar.x, bar.y + bar.height}, Vector2 {bar.x + bar.width, bar.y + bar.height}, 1.5f, BLACK);

					Erl::DrawTextCentered(UiDef::font, UiDef::codeSrc.empty() ? "Unnamed file" : UiDef::codeSrc, Vector2 {bar.x + bar.width / 2.f, bar.y + bar.height / 2.f}, fontSize, 1.f, BLACK);

					Rectangle render {
						UiDef::main.x,
						bar.y + bar.height,
						UiDef::main.width,
						UiDef::main.height - bar.height
					};

					std::string textToDraw = UiDef::codeContent;
					if(updateTick > maxUpdateTick / 2.f) {
						textToDraw.insert(textToDraw.end() + UiDef::codePointer, '|');
					}

					Erl::DrawTextWrappedL(UiDef::font, textToDraw, Vector2 {render.x + 5.f, render.y + 5.f}, Erl::FontSize(35.f), 1.f, BLACK, render.width);
					
					char character = GetCharPressed();
					if(character > 0) {
						UiDef::codeContent.insert(UiDef::codeContent.end() + UiDef::codePointer, character);
					}

					if(IsKeyPressed(KEY_BACKSPACE) || (IsKeyPressedRepeat(KEY_BACKSPACE) && IsKeyDown(KEY_BACKSPACE))) {
						if(!UiDef::codeContent.empty())
							UiDef::codeContent.erase(UiDef::codeContent.length() - 1 + UiDef::codePointer, 1);
					}

					if(IsKeyPressed(KEY_ENTER)) {
						UiDef::codeContent.insert(UiDef::codeContent.end() + UiDef::codePointer, '\n');
					}

					if(IsKeyPressed(KEY_LEFT) || (IsKeyPressedRepeat(KEY_LEFT) && IsKeyDown(KEY_LEFT))) {
						if(UiDef::codeContent.length() + UiDef::codePointer > 0)
							UiDef::codePointer--;
					} else if(IsKeyPressed(KEY_RIGHT) || (IsKeyPressedRepeat(KEY_RIGHT) && IsKeyDown(KEY_RIGHT))) {
						if(UiDef::codePointer < 0)
							UiDef::codePointer++;
					}

					break;
				}
			}

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
