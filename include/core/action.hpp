#ifndef ACTION_HPP
#define ACTION_HPP

#include <string>
#include <raylib.h>

#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __cplusplus >= 201703L && __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
     namespace fs = boost::filesystem;
#  endif
#endif


#include "customui.hpp"
#include "scene.hpp"
#include "stringutils.hpp"
#include "export.hpp"
#include "project.hpp"

namespace Action {

void NewModel() {
	CustomUI::DrawWindow("Open model", Vector2 {GetScreenWidth() / 1.8f, GetScreenHeight() / 1.4f});

	CustomUI::DrawFilePicker("openModelPicker", Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f - (GetScreenHeight() / 22.f)}, Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f});
	
	CustomUI::DrawInput("name", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f + (GetScreenHeight() / 3.6f)});

	if(CustomUI::DrawButton("Ok", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f + (GetScreenHeight() / 3.6f)}) && !CustomUI::filePickers["openModelPicker"].input.empty()) {
		std::string modelPath = CustomUI::filePickers["openModelPicker"].input;
		std::string exportPath = "export/" + modelPath.substr(modelPath.find_last_of("/\\") + 1);
		std::string exportPathFull = UiDef::project + "/" + exportPath;

		fs::copy(modelPath, exportPathFull, fs::copy_options::overwrite_existing);

		Scene::NewObject(CustomUI::inputs["name"].input, LoadModel(modelPath.c_str()), Vector3Zero(), Vector3One(), Vector3Zero(), RED);
		Scene::scene->Find(CustomUI::inputs["name"].input)->modelPath = exportPath;
		CustomUI::Disable();
	}
}

void _ProcessNodeMesh(Scene::Context *node, Mesh mesh) {
	std::string numbers;
	for(int _ = 0; _ < 10; _ ++) {
		numbers += std::to_string(rand() % 10);
	}

	std::string path = UiDef::project + "/export/" + node->name + "-" + numbers + ".obj";

	ExportMesh(mesh, path.c_str());
	node->modelPath = path.substr(path.find_last_of("/\\") + 1);
}

void NewPlane() {
	CustomUI::DrawWindow("New plane");

	CustomUI::DrawInput("size", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f - (GetScreenHeight() / 10.5f)});

	CustomUI::DrawInput("resolution", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f - (GetScreenHeight() / 10.5f)});

	CustomUI::DrawInput("name", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f + (GetScreenHeight() / 10.5f)});

	if(CustomUI::DrawButton("Ok", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f + (GetScreenHeight() / 10.5f)})) {
		std::vector<float> scale = StringUtils::GetFloatVector(CustomUI::inputs["size"].input, 2);
		std::vector<float> resolution = StringUtils::GetFloatVector(CustomUI::inputs["resolution"].input, 2);
		Mesh mesh = GenMeshPlane(scale[0], scale[1], resolution[0], resolution[1]);
		Scene::NewObject(CustomUI::inputs["name"].input, LoadModelFromMesh(mesh), Vector3Zero(), Vector3One(), Vector3Zero(), WHITE);

		_ProcessNodeMesh(Scene::scene->Find(CustomUI::inputs["name"].input), mesh);

		CustomUI::Disable();
	}
}

void NewCube() {
	CustomUI::DrawWindow("New cube");

	CustomUI::DrawInput("size", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f - (GetScreenHeight() / 10.5f)});

	CustomUI::DrawInput("name", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f - (GetScreenHeight() / 10.5f)});

	if(CustomUI::DrawButton("Ok", Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f + (GetScreenHeight() / 10.5f)})) {
		std::vector<float> size = StringUtils::GetFloatVector(CustomUI::inputs["size"].input, 3);
		Mesh mesh = GenMeshCube(size[0], size[1], size[2]);
		Scene::NewObject(CustomUI::inputs["name"].input, LoadModelFromMesh(mesh), Vector3Zero(), Vector3One(), Vector3Zero(), WHITE);

		_ProcessNodeMesh(Scene::scene->Find(CustomUI::inputs["name"].input), mesh);

		CustomUI::Disable();
	}
}

void NewSphere() {
	CustomUI::DrawWindow("New sphere", Vector2 {GetScreenWidth() / 1.8f, GetScreenHeight() / 1.8f});

	CustomUI::DrawInput("radius", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f - (GetScreenHeight() / 6.5f)});

	CustomUI::DrawInput("rings", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f - (GetScreenHeight() / 6.5f)});

	CustomUI::DrawInput("slices", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f});

	CustomUI::DrawInput("name", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f});

	if(CustomUI::DrawButton("Ok", Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f + (GetScreenHeight() / 6.5f)})) {
		Mesh mesh = GenMeshSphere(std::stof(CustomUI::inputs["radius"].input), std::stof(CustomUI::inputs["rings"].input), std::stof(CustomUI::inputs["slices"].input));
		Scene::NewObject(CustomUI::inputs["name"].input, LoadModelFromMesh(mesh), Vector3Zero(), Vector3One(), Vector3Zero(), WHITE);

		_ProcessNodeMesh(Scene::scene->Find(CustomUI::inputs["name"].input), mesh);

		CustomUI::Disable();
	}
}

void NewCylinder() {
	CustomUI::DrawWindow("New cylinder", Vector2 {GetScreenWidth() / 1.8f, GetScreenHeight() / 1.8f});

	CustomUI::DrawInput("radius", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f - (GetScreenHeight() / 6.5f)});

	CustomUI::DrawInput("height", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f - (GetScreenHeight() / 6.5f)});

	CustomUI::DrawInput("slices", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f});

	CustomUI::DrawInput("name", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f});

	if(CustomUI::DrawButton("Ok", Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f + (GetScreenHeight() / 6.5f)})) {
		Mesh mesh = GenMeshCylinder(std::stof(CustomUI::inputs["radius"].input), std::stof(CustomUI::inputs["height"].input), std::stof(CustomUI::inputs["slices"].input));
		Scene::NewObject(CustomUI::inputs["name"].input, LoadModelFromMesh(mesh), Vector3Zero(), Vector3One(), Vector3Zero(), WHITE);

		_ProcessNodeMesh(Scene::scene->Find(CustomUI::inputs["name"].input), mesh);

		CustomUI::Disable();
	}
}

void NewCone() {
	CustomUI::DrawWindow("New sphere", Vector2 {GetScreenWidth() / 1.8f, GetScreenHeight() / 1.8f});

	CustomUI::DrawInput("radius", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f - (GetScreenHeight() / 6.5f)});

	CustomUI::DrawInput("height", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f - (GetScreenHeight() / 6.5f)});

	CustomUI::DrawInput("slices", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f});

	CustomUI::DrawInput("name", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f});

	if(CustomUI::DrawButton("Ok", Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f + (GetScreenHeight() / 6.5f)})) {
		Mesh mesh = GenMeshCone(std::stof(CustomUI::inputs["radius"].input), std::stof(CustomUI::inputs["height"].input), std::stof(CustomUI::inputs["slices"].input));
		Scene::NewObject(CustomUI::inputs["name"].input, LoadModelFromMesh(mesh), Vector3Zero(), Vector3One(), Vector3Zero(), WHITE);

		_ProcessNodeMesh(Scene::scene->Find(CustomUI::inputs["name"].input), mesh);

		CustomUI::Disable();
	}
}

void NewHemiSphere() {
	CustomUI::DrawWindow("New hemi-sphere", Vector2 {GetScreenWidth() / 1.8f, GetScreenHeight() / 1.8f});

	CustomUI::DrawInput("radius", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f - (GetScreenHeight() / 6.5f)});

	CustomUI::DrawInput("rings", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f - (GetScreenHeight() / 6.5f)});

	CustomUI::DrawInput("slices", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f});

	CustomUI::DrawInput("name", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f});

	if(CustomUI::DrawButton("Ok", Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f + (GetScreenHeight() / 6.5f)})) {
		Mesh mesh = GenMeshHemiSphere(std::stof(CustomUI::inputs["radius"].input), std::stof(CustomUI::inputs["rings"].input), std::stof(CustomUI::inputs["slices"].input));
		Scene::NewObject(CustomUI::inputs["name"].input, LoadModelFromMesh(mesh), Vector3Zero(), Vector3One(), Vector3Zero(), WHITE);
		
		_ProcessNodeMesh(Scene::scene->Find(CustomUI::inputs["name"].input), mesh);

		CustomUI::Disable();
	}
}

void NewTorus() {
	CustomUI::DrawWindow("New torus", Vector2 {GetScreenWidth() / 1.8f, GetScreenHeight() / 1.8f});

	CustomUI::DrawInput("radius", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f - (GetScreenHeight() / 6.5f)});

	CustomUI::DrawInput("size", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f - (GetScreenHeight() / 6.5f)});

	CustomUI::DrawInput("radSeg", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f});

	CustomUI::DrawInput("sides", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f});

	CustomUI::DrawInput("name", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f + (GetScreenHeight() / 6.5f)});

	if(CustomUI::DrawButton("Ok", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f + (GetScreenHeight() / 6.5f)})) {
		Mesh mesh = GenMeshTorus(std::stof(CustomUI::inputs["radius"].input), std::stof(CustomUI::inputs["size"].input), std::stof(CustomUI::inputs["radSeg"].input), std::stof(CustomUI::inputs["sides"].input));
		Scene::NewObject(CustomUI::inputs["name"].input, LoadModelFromMesh(mesh), Vector3Zero(), Vector3One(), Vector3Zero(), WHITE);
		
		_ProcessNodeMesh(Scene::scene->Find(CustomUI::inputs["name"].input), mesh);

		CustomUI::Disable();
	}
}

void NewKnot() {
	CustomUI::DrawWindow("New knot", Vector2 {GetScreenWidth() / 1.8f, GetScreenHeight() / 1.8f});

	CustomUI::DrawInput("radius", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f - (GetScreenHeight() / 6.5f)});

	CustomUI::DrawInput("size", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f - (GetScreenHeight() / 6.5f)});

	CustomUI::DrawInput("radSeg", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f});

	CustomUI::DrawInput("sides", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f});

	CustomUI::DrawInput("name", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f + (GetScreenHeight() / 6.5f)});

	if(CustomUI::DrawButton("Ok", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f + (GetScreenHeight() / 6.5f)})) {
		Mesh mesh = GenMeshKnot(std::stof(CustomUI::inputs["radius"].input), std::stof(CustomUI::inputs["size"].input), std::stof(CustomUI::inputs["radSeg"].input), std::stof(CustomUI::inputs["sides"].input));
		Scene::NewObject(CustomUI::inputs["name"].input, LoadModelFromMesh(mesh), Vector3Zero(), Vector3One(), Vector3Zero(), WHITE);
		
		_ProcessNodeMesh(Scene::scene->Find(CustomUI::inputs["name"].input), mesh);

		CustomUI::Disable();
	}
}

void NewHeightmap() {
	CustomUI::DrawWindow("New heightmap", Vector2 {GetScreenWidth() / 1.8f, GetScreenHeight() / 1.2f});
	CustomUI::DrawFilePicker("imagePicker", Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f - (GetScreenHeight() / 12.f)}, Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f});
	
	CustomUI::DrawInput("size", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f + (GetScreenHeight() / 4.2f)});
	CustomUI::DrawInput("name", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f + (GetScreenHeight() / 4.2f)});

	if(CustomUI::DrawButton("Ok", Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f + (GetScreenHeight() / 2.85f)}) && !CustomUI::filePickers["imagePicker"].input.empty()) {
		std::vector<float> size = StringUtils::GetFloatVector(CustomUI::inputs["size"].input, 3);
		Mesh mesh = GenMeshHeightmap(LoadImage(CustomUI::filePickers["imagePicker"].input.c_str()), Vector3 {size[0], size[1], size[2]});

		Scene::NewObject(CustomUI::inputs["name"].input, LoadModelFromMesh(mesh), Vector3Zero(), Vector3One(), Vector3Zero(), WHITE);
		
		_ProcessNodeMesh(Scene::scene->Find(CustomUI::inputs["name"].input), mesh);

		CustomUI::Disable();
	}
}

void NewCubicmap() {
	CustomUI::DrawWindow("New cubicmap", Vector2 {GetScreenWidth() / 1.8f, GetScreenHeight() / 1.2f});
	CustomUI::DrawFilePicker("imagePicker", Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f - (GetScreenHeight() / 12.f)}, Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f});
	
	CustomUI::DrawInput("size", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f + (GetScreenHeight() / 4.2f)});
	CustomUI::DrawInput("name", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f + (GetScreenHeight() / 4.2f)});

	if(CustomUI::DrawButton("Ok", Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f + (GetScreenHeight() / 2.85f)}) && !CustomUI::filePickers["imagePicker"].input.empty()) {
		std::vector<float> size = StringUtils::GetFloatVector(CustomUI::inputs["size"].input, 3);
		Mesh mesh = GenMeshCubicmap(LoadImage(CustomUI::filePickers["imagePicker"].input.c_str()), Vector3 {size[0], size[1], size[2]});

		Scene::NewObject(CustomUI::inputs["name"].input, LoadModelFromMesh(mesh), Vector3Zero(), Vector3One(), Vector3Zero(), WHITE);
		
		_ProcessNodeMesh(Scene::scene->Find(CustomUI::inputs["name"].input), mesh);

		CustomUI::Disable();
	}
}

void NewPolygonal() {
	CustomUI::DrawWindow("New polygonal");

	CustomUI::DrawInput("sides", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f - (GetScreenHeight() / 10.5f)});

	CustomUI::DrawInput("radius", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f - (GetScreenHeight() / 10.5f)});

	CustomUI::DrawInput("name", Vector2 {GetScreenWidth() / 2.f - (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f + (GetScreenHeight() / 10.5f)});

	if(CustomUI::DrawButton("Ok", Vector2 {GetScreenWidth() / 2.f + (GetScreenWidth() / 7.5f), GetScreenHeight() / 2.f + (GetScreenHeight() / 10.5f)})) {
		Mesh mesh = GenMeshPoly(std::stof(CustomUI::inputs["sides"].input), std::stof(CustomUI::inputs["radius"].input));
		
		Scene::NewObject(CustomUI::inputs["name"].input, LoadModelFromMesh(mesh), Vector3Zero(), Vector3One(), Vector3Zero(), WHITE);
		
		_ProcessNodeMesh(Scene::scene->Find(CustomUI::inputs["name"].input), mesh);

		CustomUI::Disable();
	}
}

void Activate(std::string actionString) {
	if(actionString == "New.Basic.Plane") {
		CustomUI::AddInput("size", "Size (XZ)");
		CustomUI::AddInput("resolution", "Resolution");
		CustomUI::AddInput("name", "Name");
		CustomUI::Enable(NewPlane);
	} else if(actionString == "New.Basic.Cube") {
		CustomUI::AddInput("size", "Size (XYZ)");
		CustomUI::AddInput("name", "Name");
		CustomUI::Enable(NewCube);
	} else if(actionString == "New.Basic.Sphere") {
		CustomUI::AddInput("radius", "Radius");
		CustomUI::AddInput("rings", "Rings");
		CustomUI::AddInput("slices", "Slices");
		CustomUI::AddInput("name", "Name");
		CustomUI::Enable(NewSphere);
	} else if(actionString == "New.Basic.Cylinder") {
		CustomUI::AddInput("radius", "Radius");
		CustomUI::AddInput("height", "Height");
		CustomUI::AddInput("slices", "Slices");
		CustomUI::AddInput("name", "Name");
		CustomUI::Enable(NewCylinder);
	} else if(actionString == "New.Basic.Cone") {
		CustomUI::AddInput("radius", "Radius");
		CustomUI::AddInput("height", "Height");
		CustomUI::AddInput("slices", "Slices");
		CustomUI::AddInput("name", "Name");
		CustomUI::Enable(NewCone);
	} else if(actionString == "New.Other.HemiSphere") {
		CustomUI::AddInput("radius", "Radius");
		CustomUI::AddInput("rings", "Rings");
		CustomUI::AddInput("slices", "Slices");
		CustomUI::AddInput("name", "Name");
		CustomUI::Enable(NewHemiSphere);
	} else if(actionString == "New.Other.Torus") {
		CustomUI::AddInput("radius", "Radius");
		CustomUI::AddInput("size", "Size");
		// GenMeshTorus(float radius, float size, int radSeg, int sides)
		// WTF is radSeg for? Guessing radius segmentation...
		CustomUI::AddInput("radSeg", "Radius segmentation");
		CustomUI::AddInput("sides", "Sides");
		CustomUI::AddInput("name", "Name");
		CustomUI::Enable(NewTorus);
	} else if(actionString == "New.Other.Knot") {
		CustomUI::AddInput("radius", "Radius");
		CustomUI::AddInput("size", "Size");
		CustomUI::AddInput("radSeg", "Radius segmentation");
		CustomUI::AddInput("sides", "Sides");
		CustomUI::AddInput("name", "Name");
		CustomUI::Enable(NewKnot);
	} else if(actionString == "New.Other.Heightmap") {
		if(!UiDef::project.empty()) {
			CustomUI::AddFilePicker("imagePicker", UiDef::project, R"""(.*\.(png|bmp|tga|jpg|gif|pic|ppm|pgm|psd)$)""");
			CustomUI::AddInput("size", "Size (XYZ)");
			CustomUI::AddInput("name", "Name");
			CustomUI::Enable(NewHeightmap);
		} else {
			CustomUI::Enable(std::bind(CustomUI::InfoDialog, "Error: no project loaded"));
		}
	} else if(actionString == "New.Other.Cubicmap") {
		if(!UiDef::project.empty()) {
			CustomUI::AddFilePicker("imagePicker", UiDef::project, R"""(.*\.(png|bmp|tga|jpg|gif|pic|ppm|pgm|psd)$)""");
			CustomUI::AddInput("size", "Size (XYZ)");
			CustomUI::AddInput("name", "Name");
			CustomUI::Enable(NewCubicmap);
		} else {
			CustomUI::Enable(std::bind(CustomUI::InfoDialog, "Error: no project loaded"));
		}
	} else if(actionString == "New.Other.Polygonal") {
		CustomUI::AddInput("sides", "Sides");
		CustomUI::AddInput("radius", "Radius");
		CustomUI::AddInput("name", "Name");
		CustomUI::Enable(NewPolygonal);
	} else if(actionString == "File.Export") {
		Export::ExportCpp("main.cpp");
	} else if(actionString == "Project.New") {
		CustomUI::AddInput("projectName", "Project name");
		CustomUI::Enable(Project::NewUI);
	} else if(actionString == "Project.Open") {
		CustomUI::AddFilePicker("openProjectFilePicker", "../projects/", R"""([A-z]+)""");
		CustomUI::Enable(Project::OpenUI);
	} else if(actionString == "New.From model") {
		if(!UiDef::project.empty()) {
			CustomUI::AddFilePicker("openModelPicker", UiDef::project, R"""(.*\.(obj|gltf|iqm|vox|m3d)$)""");
			CustomUI::AddInput("name", "Name");
			CustomUI::Enable(NewModel);
		} else {
			CustomUI::Enable(std::bind(CustomUI::InfoDialog, "Error: no project loaded"));
		}
	}
}

};
#endif
