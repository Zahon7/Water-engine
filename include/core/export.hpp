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

	for(int index = 0; index < context.path.size() - 1; index++) {
		getter += "->Find(\"" + context.path[index] + "\")";
	}

	sections.INIT += "Scene::NewEmpty(\"" + context.name + "\", " + getter + ");\n";

	// TODO: Transfer properties from the real node

	std::string addToScript = "if($(SELF)->components.count(\"DrawableComponent\")) {\n"
							  "    Scene::DrawableComponent *component = dynamic_cast<Scene::DrawableComponent*>($(SELF)->components[\"DrawableComponent\"]);\n"
							  "    component->model = LoadModel(component->modelPath.c_str());\n"
							  "}\n";

	sections.INIT += StringUtils::StringReplace(addToScript, "$(SELF)", getter + "->Find(\"" + context.path.back() + "\")");
}

void ExportCpp(std::string name) {
	std::ifstream fileRead("engine/export.hpp");
	std::string content((std::istreambuf_iterator<char>(fileRead)),
	                     std::istreambuf_iterator<char>());

	Scene::scene->walk(AddContextToInit);

	content = StringUtils::StringReplace(content, "$(INIT)", sections.INIT);
	content = StringUtils::StringReplace(content, "$(UPDATE)", sections.UPDATE);

	content = StringUtils::StringReplace(content, "$(POSITION)", StringUtils::ToString(UiDef::camera.position));
	content = StringUtils::StringReplace(content, "$(TARGET)", StringUtils::ToString(UiDef::camera.target));
	content = StringUtils::StringReplace(content, "$(FOVY)", std::to_string(UiDef::camera.fovy) + "f");

	std::ofstream fileWrite("../" + name);
	fileWrite.write(content.c_str(), content.length());
	fileWrite.close();

	system(("g++ ../" + name + " -lraylib -o ../" + name + ".out").c_str());
	system(("rm " + name).c_str());
}

};
#endif
