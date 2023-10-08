#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <string>

#include <list>
#include <algorithm>
#include <functional>

#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>

#include <iostream>

#include "core/uidef.hpp"
#include "core/assets.hpp"
#include "core/customui.hpp"
#include "core/stringutils.hpp"

namespace Scene {

class BaseComponent {
public:
	virtual std::string ToString(std::string parent) {return 0;};
	virtual ~BaseComponent() {};
};

class CameraComponent : public BaseComponent {
public:
	Vector3 position = {0.0f, 0.0f, 0.0f};
	Vector3 target = {0.0f, 0.0f, 1.0f};
	float fovy = 90;
};

class ScriptComponent : public BaseComponent {
public:
	std::string content;
};

class DrawableComponent : public BaseComponent {
public:
	Model model;

	Color tint = WHITE;

	Vector3 position = {0.0f, 0.0f, 0.0f};
	Vector3 scale = {1.0f, 1.0f, 1.0f};
	Vector3 rotation = {0.0f, 0.0f, 0.0f};

	Matrix transform = MatrixIdentity();

	void FromModel(Model model) {
		// boundaries = BoundaryInfo{model};

		this->model = model;
	}

	void LookAt(Vector3 target) {
  		model.transform = MatrixInvert(MatrixLookAt(Vector3 {position.x, position.y, position.z}, target, (Vector3) {0.0f,1.0f,0.0f}));
	}

	void ApplyTransform() {
		model.transform = MatrixMultiply(model.transform, MatrixTranslate(position.x, position.y, position.z));

  		model.transform = MatrixMultiply(model.transform, MatrixRotate(Vector3 {1, 0, 0}, rotation.x));
  		model.transform = MatrixMultiply(model.transform, MatrixRotate(Vector3 {0, 1, 0}, rotation.y));
  		model.transform = MatrixMultiply(model.transform, MatrixRotate(Vector3 {0, 0, 1}, rotation.z));
	}

	void Draw() {
		DrawModel(model, Vector3Zero(), 1.0f, tint);
	}
};

struct Context {
	std::vector<Context> childs = {};
	Context *parent = nullptr;

	std::string name;
	std::vector<std::string> path;

	std::map<std::string, BaseComponent*> components;

	std::string modelPath;

	bool drawable = false;
	bool expanded = false;

	int _depth = 0;
	bool _nodeExplorerVisible = true;

	void DrawTag() {
		DrawableComponent *drawable = dynamic_cast<DrawableComponent*>(components["DrawableComponent"]);

		if(drawable != nullptr) {
			Vector2 position = GetWorldToScreenEx(Vector3 {drawable->position.x, drawable->position.y + 1.f, drawable->position.z}, UiDef::camera, UiDef::main.width, UiDef::main.height);

			float distance = Vector3DistanceSqr(UiDef::camera.position, drawable->position);
			if(distance < 30.f) {
				DrawTextEx(UiDef::font, name.c_str(), position, Remap(distance, 0, 30.f, 30.f, 0), 1.5f, BLACK);
			}
		}
	}

    void walk(std::function<void(Context&)> f) {
    	f(*this);
        std::for_each(
            childs.begin(),
            childs.end(),
            std::bind(&Context::walk, std::placeholders::_1, f));
    }

    Context *Find(std::string name) {
    	for(Context &child : childs) {
    		if(child.name == name) {
    			return &child;
    		}
    	}
    	return nullptr;
    }
};

void _SetExpandedTrue(Context& node) {
	node._nodeExplorerVisible = false;
}

void _SetExpandedFalse(Context& node) {
	node._nodeExplorerVisible = true;
}

void SetExpanded(Context& context, bool status) {
	if(status) {
		for(Context &child : context.childs)
			child.walk(&_SetExpandedTrue);
	} else {
		for(Context &child : context.childs)
			child.walk(&_SetExpandedFalse);
	}

	context.expanded = status;
}

void _SetVisibleFalse(Context& node) {
	node.drawable = false;
}

void _SetVisibleTrue(Context& node) {
	node.drawable = false;
}

void SetVisible(Context& context, bool status) {
	if(status) {
		for(Context &child : context.childs)
			child.walk(&_SetVisibleTrue);
	} else {
		for(Context &child : context.childs)
			child.walk(&_SetVisibleFalse);
	}
}

Context *scene;

void NewObject(std::string name, Model model, Vector3 position, Vector3 scale, Vector3 rotation, Color tint, Context *at = scene) {
	Context context;
	context.name = name;
	context.parent = at;

	context.drawable = true;

	DrawableComponent *info = new DrawableComponent();

	info->FromModel(model);
	info->tint = tint;

	info->position = position;
	info->scale = scale;
	info->rotation = rotation;
	
	context.components["DrawableComponent"] = info;

	context._depth = at->_depth + 1;

	context.path.reserve(context.path.size() + distance(at->path.begin(), at->path.end()));
	context.path.insert(context.path.end(), at->path.begin(), at->path.end());
	context.path.push_back(name);

	context.modelPath = name;

	at->childs.push_back(context);
}

void NewScript(std::string name, Context *at = scene) {
	Context scriptNode;
	scriptNode.name = name;
	scriptNode._depth = at->_depth + 1;
	
	scriptNode.path.reserve(scriptNode.path.size() + distance(at->path.begin(), at->path.end()));
	scriptNode.path.insert(scriptNode.path.end(), at->path.begin(), at->path.end());
	scriptNode.path.push_back(name);

	ScriptComponent *component = new ScriptComponent();
	component->content = "void Init(Scene::Context *self) {\n"
						 "    // Write your init code here\n"
						 "}\n"
						 "\n"
						 "void Update(Scene::Context *self) {\n"
						 "    // Write you update code here\n"
						 "}\n";
	scriptNode.components["ScriptComponent"] = component;

	at->childs.push_back(scriptNode);
}

void NewEmpty(std::string name, Context *at = scene) {
	Context scriptNode;
	scriptNode.name = name;
	scriptNode._depth = at->_depth + 1;
	
	scriptNode.path.reserve(scriptNode.path.size() + distance(at->path.begin(), at->path.end()));
	scriptNode.path.insert(scriptNode.path.end(), at->path.begin(), at->path.end());

	at->childs.push_back(scriptNode);
}

void DrawFunction(Context& node) {
	if(node.drawable) {
		if(node.components.count("DrawableComponent")) {
			DrawableComponent *drawable = dynamic_cast<DrawableComponent*>(node.components["DrawableComponent"]);
			if(drawable != nullptr) {
				drawable->Draw();
			}
		}
	}
}

void DrawTagFunction(Context& node) {
	if(node.drawable) {
		node.DrawTag();
	}
}

void DrawScene() {
	scene->walk(&DrawFunction);
}

void DrawSceneTags() {
	scene->walk(&DrawTagFunction);
}

int yPointer;

Vector2 Vector2Move(Vector2 src, Vector2 dest, float srcMul = 1.f, float destMul = 1.f) {
	return Vector2 {(src.x * srcMul + dest.x * destMul) / (srcMul + destMul),
					(src.y * srcMul + dest.y * destMul) / (srcMul + destMul)};
}

void DrawSceneNode(Context &node) {
	if(!node._nodeExplorerVisible)
		return;

	int nodeDepth = node._depth * 25;

	float y = (GetScreenHeight() + GetScreenWidth()) / 64.f;

	Rectangle rectangle {
		UiDef::nodeExplorer.x + nodeDepth, yPointer * y + UiDef::nodeExplorer.y, UiDef::nodeExplorer.width - nodeDepth, y
	};

	if(node.components.count("DrawableComponent")) {
		DrawableComponent *drawable = dynamic_cast<DrawableComponent*>(node.components["DrawableComponent"]);
		if(drawable != nullptr) {
			DrawRectangleRec(rectangle, Color {(unsigned char)((drawable->tint.r + 255) / 2), (unsigned char)((drawable->tint.g + 255) / 2),
										       (unsigned char)((drawable->tint.b + 255) / 2), (unsigned char)((drawable->tint.a + 255) / 2)});
		}
	} else if(node.components.count("ScriptComponent")) {
		DrawRectangleRec(rectangle, GREEN);
	}

	DrawRectangleLinesEx(rectangle, 1.5f, BLACK);

	DrawTextEx(UiDef::font, node.name.c_str(), Vector2 {rectangle.x + 3.f, rectangle.y + 1.f}, 25.f, 1.0, BLACK);

	if(!node.childs.empty()) {
		Rectangle arrowRectangle {
			rectangle.x + rectangle.width - rectangle.height, rectangle.y,
			rectangle.height, rectangle.height
		};

		Texture *currentTexture = node.expanded ? &AssetManager::arrowNormal : &AssetManager::arrowExpanded;

		DrawTexturePro((*currentTexture), Rectangle {0, 0, (float)currentTexture->width, (float)currentTexture->height}, arrowRectangle, Vector2Zero(), 0.f, WHITE);

		if(CheckCollisionPointRec(GetMousePosition(), arrowRectangle) &&
				IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CustomUI::enabled) {
			SetExpanded(node, !node.expanded);
		}
	}

	if(!CustomUI::enabled) {
		int gesture = GetGestureDetected();
		if(node.components.count("ScriptComponent") && CheckCollisionPointRec(GetMousePosition(), rectangle) && gesture == GESTURE_DOUBLETAP) {
			ScriptComponent *script = dynamic_cast<ScriptComponent*>(node.components["ScriptComponent"]);
			UiDef::codeContent = script->content;
			UiDef::codePointer = 0;
			UiDef::codeSrc = StringUtils::StringJoin(node.path, "->");

			UiDef::mainType = UiDef::MainSceneType::CODE;
		}
	}

	yPointer++;
}

void DrawSceneNodes() {
	yPointer = 0;
	scene->walk(&DrawSceneNode);
}

};
#endif
