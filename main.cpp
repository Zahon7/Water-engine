#include <raylib.h>

// Code auto-generated using the water engine

Camera3D camera;

#pragma region
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
	#include <map>

	namespace Scene {

	class BaseComponent {
	public:
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

		context.modelPath = name;

		at->childs.push_back(context);
	}

	void NewScript(std::string name, Context *at = scene) {
		Context scriptNode;
		scriptNode.name = name;
		scriptNode._depth = at->_depth + 1;
		
		scriptNode.path.reserve(scriptNode.path.size() + distance(at->path.begin(), at->path.end()));
		scriptNode.path.insert(scriptNode.path.end(), at->path.begin(), at->path.end());

		ScriptComponent *component = new ScriptComponent();
		component->content = "Init(Context &self) {\n"
							 "    // Write your init code here\n"
							 "}\n"
							 "\n"
							 "Update(Context& self) {\n"
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

	void DrawScene() {
		scene->walk(&DrawFunction);
	}

	int yPointer;

	Vector2 Vector2Move(Vector2 src, Vector2 dest, float srcMul = 1.f, float destMul = 1.f) {
		return Vector2 {(src.x * srcMul + dest.x * destMul) / (srcMul + destMul),
						(src.y * srcMul + dest.y * destMul) / (srcMul + destMul)};
	}

	};
	#endif
#pragma endregion

void DrawFunction(Scene::Context &node) {
	if(node.components.count("DrawableComponent")) {
		Scene::DrawableComponent *drawableComponent = dynamic_cast<Scene::DrawableComponent*>(node.components["DrawableComponent"]);
		drawableComponent->Draw();
	}
}

namespace Scene_Camera_Script {
void Init(Scene::Context *self) {
    // Write your init code here
}

void Update(Scene::Context *self) {
    // Write you update code here
}
}


int main() {
	InitWindow(1000, 600, "Water engine window");
	
	Scene::scene = new Scene::Context();
	Scene::scene->name = "Scene";
	
	Camera camera;

	camera.position = Vector3 {10.000000f, 10.000000f, 10.000000f};
	camera.target = Vector3 {0.000000f, 0.000000f, 0.000000f};
	camera.fovy = 60.000000f;
	camera.up = Vector3 {0.0f, 1.0f, 0.0f};
	camera.projection = CAMERA_PERSPECTIVE;

Scene::NewEmpty("Camera", Scene::scene);
Scene::scene->Find("Camera")->components["CameraComponent"] = new Scene::CameraComponent();
Scene::scene->Find("Camera")->components["DrawableComponent"] = new Scene::DrawableComponent();
Scene::scene->Find("Camera")->modelPath = "Camera";
Scene::DrawableComponent *drawableComponent_3086 = dynamic_cast<Scene::DrawableComponent*>(Scene::scene->Find("Camera")->components["DrawableComponent"]);
drawableComponent_3086->model = LoadModel(Scene::scene->Find("Camera")->modelPath.c_str());
drawableComponent_3086->tint = Color {255, 255, 255, 255};
drawableComponent_3086->position = Vector3 {10.000000f, 10.000000f, 10.000000f};
drawableComponent_3086->scale = Vector3 {1.000000f, 1.000000f, 1.000000f};
drawableComponent_3086->rotation = Vector3 {0.000000f, 0.000000f, 0.000000f};
drawableComponent_3086->transform = Matrix {1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000};
Scene::CameraComponent *cameraComponent_1025 = dynamic_cast<Scene::CameraComponent*>(Scene::scene->Find("Camera")->components["CameraComponent"]);
cameraComponent_1025->position = Vector3 {10.000000f, 10.000000f, 10.000000f};
cameraComponent_1025->target = Vector3 {0.000000f, 0.000000f, 0.000000f};
cameraComponent_1025->fovy = 60.000000f;
Scene::NewEmpty("Script", Scene::scene->Find("Camera"));
Scene::scene->Find("Camera")->Find("Script")->components["ScriptComponent"] = new Scene::ScriptComponent();
Scene::scene->Find("Camera")->Find("Script")->modelPath = "";
Scene::ScriptComponent *scriptComponent_2153 = dynamic_cast<Scene::ScriptComponent*>(Scene::scene->Find("Camera")->Find("Script")->components["ScriptComponent"]);
scriptComponent_2153->content = "void Init(Scene::Context *self) {\n    // Write your init code here\n}\n\nvoid Update(Scene::Context *self) {\n    // Write you update code here\n}\n";
Scene_Camera_Script::Init(Scene::scene->Find("Camera")->Find("Script")->parent);
Scene::NewEmpty("c", Scene::scene);
Scene::scene->Find("c")->components["DrawableComponent"] = new Scene::DrawableComponent();
Scene::scene->Find("c")->modelPath = "export/church.obj";
Scene::DrawableComponent *drawableComponent_1414 = dynamic_cast<Scene::DrawableComponent*>(Scene::scene->Find("c")->components["DrawableComponent"]);
drawableComponent_1414->model = LoadModel(Scene::scene->Find("c")->modelPath.c_str());
drawableComponent_1414->tint = Color {230, 41, 55, 255};
drawableComponent_1414->position = Vector3 {0.000000f, 0.000000f, 0.000000f};
drawableComponent_1414->scale = Vector3 {1.000000f, 1.000000f, 1.000000f};
drawableComponent_1414->rotation = Vector3 {0.000000f, 0.000000f, 0.000000f};
drawableComponent_1414->transform = Matrix {1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000};


	while(!WindowShouldClose()) {
		BeginDrawing();
			Scene::CameraComponent* cameraComponent = dynamic_cast<Scene::CameraComponent*>(Scene::scene->Find("Camera")->components["CameraComponent"]);
			if(cameraComponent != nullptr) {
				camera.position = cameraComponent->position;
				camera.target = cameraComponent->target;
				camera.fovy = cameraComponent->fovy;
			}

			ClearBackground(WHITE);
			BeginMode3D(camera);
				Scene::scene->walk(&DrawFunction);
			EndMode3D();
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
