#ifndef EXPORT_HPP
#define EXPORT_HPP

#include <string>
#include <raylib.h>
#include <fstream>

#include "scene.hpp"
#include "stringutils.hpp"
#include "customui.hpp"
#include "uidef.hpp"
#include "erl.cpp"

namespace Export {

using Section = std::string;

struct Sections {
	Section SCRIPTS;
	Section INIT;
	Section UPDATE;
} sections;

std::string templateScript;

// Scene, Camera, Script
// -> Scene::scene + ->Find("Camera") + ->Find("Script")

void AddContextToInit(Scene::Context &context) {
	if(context.path.size() <= 1)
		return;

	std::string getter = "Scene::scene";

	for(int index = 1; index < context.path.size() - 1; index++) {
		getter += "->Find(\"" + context.path[index] + "\")";
	}

	sections.INIT += "Scene::NewEmpty(\"" + context.name + "\", " + getter + ");\n";

	getter += "->Find(\"" + context.path.back() + "\")";

	for(const auto& [name, component] : context.components) {
		sections.INIT += getter + "->components[\"" + StringUtils::EscapeString(name) +"\"] = new Scene::" + name + "();\n";

		// TODO: Transfer component properties
	}

	sections.INIT += getter + "->modelPath = \"" + StringUtils::EscapeString(context.modelPath) + "\";\n";
	// TODO: Transfer more properties from the real node

	if(context.components.count("DrawableComponent") && !(context.name == "Camera" && context.parent == nullptr)) {
		/*
			Model model;

			Color tint = WHITE;

			Vector3 position = {0.0f, 0.0f, 0.0f};
			Vector3 scale = {1.0f, 1.0f, 1.0f};
			Vector3 rotation = {0.0f, 0.0f, 0.0f};

			Matrix transform = MatrixIdentity();
		*/

		Scene::DrawableComponent *realComponent = dynamic_cast<Scene::DrawableComponent*>(context.components["DrawableComponent"]);

		std::string name = "drawableComponent_" + std::to_string(rand() % 4096);

		std::string addToScript = "Scene::DrawableComponent *" + name + " = dynamic_cast<Scene::DrawableComponent*>($(SELF)->components[\"DrawableComponent\"]);\n";
		
		addToScript += name + "->model = LoadModel(" + getter + "->modelPath.c_str());\n";
		addToScript += name + "->tint = " + StringUtils::ToString(realComponent->tint) + ";\n";
		addToScript += name + "->position = " + StringUtils::ToString(realComponent->position) + ";\n";
		addToScript += name + "->scale = " + StringUtils::ToString(realComponent->scale) + ";\n";
		addToScript += name + "->rotation = " + StringUtils::ToString(realComponent->rotation) + ";\n";
		addToScript += name + "->transform = " + StringUtils::ToString(realComponent->transform) + ";\n";

		sections.INIT += StringUtils::StringReplace(addToScript, "$(SELF)", getter);
	}

	if(context.components.count("CameraComponent")) {
		/*
			Vector3 position = {0.0f, 0.0f, 0.0f};
			Vector3 target = {0.0f, 0.0f, 1.0f};
			float fovy = 90;
		*/
		
		Scene::CameraComponent *realComponent = dynamic_cast<Scene::CameraComponent*>(context.components["CameraComponent"]);

		std::string name = "cameraComponent_" + std::to_string(rand() % 4096);

		std::string addToScript = "Scene::CameraComponent *" + name + " = dynamic_cast<Scene::CameraComponent*>($(SELF)->components[\"CameraComponent\"]);\n";
		
		addToScript += name + "->position = " + StringUtils::ToString(realComponent->position) + ";\n";
		addToScript += name + "->target = " + StringUtils::ToString(realComponent->target) + ";\n";
		addToScript += name + "->fovy = " + StringUtils::ToString(realComponent->fovy) + ";\n";

		sections.INIT += StringUtils::StringReplace(addToScript, "$(SELF)", getter);
	}

	if(context.components.count("ScriptComponent")) {
		/*
			std::string content;
		*/
		
		Scene::ScriptComponent *realComponent = dynamic_cast<Scene::ScriptComponent*>(context.components["ScriptComponent"]);

		std::string name = "scriptComponent_" + std::to_string(rand() % 4096);

		std::string addToScript = "Scene::ScriptComponent *" + name + " = dynamic_cast<Scene::ScriptComponent*>($(SELF)->components[\"ScriptComponent\"]);\n";
		
		addToScript += name + "->content = \"" + StringUtils::ToString(realComponent->content) + "\";\n";

		sections.INIT += StringUtils::StringReplace(addToScript, "$(SELF)", getter);
		sections.INIT += StringUtils::StringJoin(context.path, "_") + "::Init(" + getter + "->parent);\n";

		sections.SCRIPTS += "namespace " + StringUtils::StringJoin(context.path, "_") + " {\n";
		sections.SCRIPTS += realComponent->content;
		sections.SCRIPTS += "}\n";

		sections.UPDATE += StringUtils::StringJoin(context.path, "_") + "::Update(" + getter + "->parent);\n";
	}
}

void ExportCpp(std::string name) {
	std::ifstream fileRead("engine/export.hpp");
	std::string content((std::istreambuf_iterator<char>(fileRead)),
	                     std::istreambuf_iterator<char>());

	Scene::scene->walk(AddContextToInit);

	content = StringUtils::StringReplace(content, "$(SCRIPTS)", sections.SCRIPTS);
	content = StringUtils::StringReplace(content, "$(INIT)", sections.INIT);
	content = StringUtils::StringReplace(content, "$(UPDATE)", sections.UPDATE);

	content = StringUtils::StringReplace(content, "$(POSITION)", StringUtils::ToString(UiDef::camera.position));
	content = StringUtils::StringReplace(content, "$(TARGET)", StringUtils::ToString(UiDef::camera.target));
	content = StringUtils::StringReplace(content, "$(FOVY)", std::to_string(UiDef::camera.fovy) + "f");

	std::ofstream fileWrite("../" + name);
	fileWrite.write(content.c_str(), content.length());
	fileWrite.close();

	system(("g++ ../" + name + " -lraylib -o ../" + name + ".out").c_str());

	#ifndef DEBUG
		system(("rm ../" + name).c_str());
	#endif
}

};
#endif
