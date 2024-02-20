#ifndef TOPNAV_HPP
#define TOPNAV_HPP

#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <raylib.h>

#include "erl.cpp"
#include "core/uidef.hpp"
#include "core/action.hpp"

namespace TopBar {

class Element {
public:
	std::vector<Element> childs = {};

	Element *parent = nullptr;
	std::string id;

	std::string name;

	bool disabled = false;

	bool expanded = true;

	int _depthY = 0;
	int _depthX = -1;

	bool _tabVisible = true;

	void walk(std::function<void(Element&)> f) {
    	f(*this);
        std::for_each(
            childs.begin(),
            childs.end(),
            std::bind(&Element::walk, std::placeholders::_1, f));
    }

    Element *Find(std::string name) {
    	for(Element &child : childs) {
    		if(child.name == name) {
    			return &child;
    		}
    	}
    	return nullptr;
    }
};

std::vector<Element> menus;

int NewMenu(std::string name) {
	Element newElement;
	newElement.name = name;
	newElement._depthX = menus.size();
	menus.push_back(newElement);

	return menus.size() - 1;
}

void AddElement(std::string name, Element *parent) {
	Element element;
	element.childs = {};

	Element *current = parent;
	std::vector<std::string> ids;
	while(true) {
		ids.push_back(current->name);
		if(current->parent == nullptr)
			break;
		current = current->parent;
	}

	std::reverse(ids.begin(), ids.end());

	std::string newName = "";
	for(std::string id : ids) {
		newName += id + ".";
	}
	element.id = newName + name; 

	element.name = name;

	element.parent = parent;

	if(parent->parent != nullptr) {
		element._depthX = parent->_depthX + 1;
		element._depthY = parent->childs.size() + parent->_depthY;
	} else {
		element._depthX = parent->_depthX;
		element._depthY = parent->childs.size() + 1;
	}

	parent->childs.push_back(element);
}

void _SetExpandedTrue(Element& node) {
	node._tabVisible = true;
}

void _SetExpandedFalse(Element& node) {
	node._tabVisible = false;
}

void SetExpanded(Element& context, bool status) {
	if(status) {
		for(Element &child : context.childs) {
			if(!status) {
				child.walk(&_SetExpandedTrue);
			} else {
				child._tabVisible = true;
			}
		}
	} else {
		for(Element &child : context.childs) {
			if(!status) {
				child.walk(&_SetExpandedFalse);
			} else {
				child._tabVisible = false;
			}
		}
	}

	context.expanded = status;
}

void Init() {
	int projectMenu = NewMenu("Project");
	int fileMenu = NewMenu("File");
	int newMenu = NewMenu("New");

	AddElement("New", &menus[projectMenu]);
	AddElement("Open", &menus[projectMenu]);

	AddElement("Export", &menus[fileMenu]);
	AddElement("Save", &menus[fileMenu]);
	menus[fileMenu].Find("Save")->disabled = true;

	AddElement("From model", &menus[newMenu]);

	AddElement("Basic", &menus[newMenu]);

	AddElement("Plane", menus[newMenu].Find("Basic"));
	AddElement("Cube", menus[newMenu].Find("Basic"));
	AddElement("Sphere", menus[newMenu].Find("Basic"));
	AddElement("Cylinder", menus[newMenu].Find("Basic"));
	AddElement("Cone", menus[newMenu].Find("Basic"));

	AddElement("Other", &menus[newMenu]);

	AddElement("HemiSphere", menus[newMenu].Find("Other"));
	AddElement("Torus", menus[newMenu].Find("Other"));
	AddElement("Knot", menus[newMenu].Find("Other"));
	AddElement("Heightmap", menus[newMenu].Find("Other"));
	AddElement("Cubicmap", menus[newMenu].Find("Other"));
	AddElement("Polygonal", menus[newMenu].Find("Other"));

	for(Element &menu : menus) {
		SetExpanded(menu, false);
	}
}
void DrawEach(Element &element) {
	if(!element._tabVisible)
		return;

	Rectangle rectangle {
		element._depthX * (UiDef::topBar.width / 10.f),
		element._depthY * UiDef::topBar.height,
		UiDef::topBar.width / 10.f,
		UiDef::topBar.height
	};

	DrawRectangleRounded(rectangle, 0.1f, 10, WHITE);
	DrawRectangleRoundedLines(Erl::RectangleExpanded(rectangle, {-2.f, -2.f}), 0.2f, 10, 1.f, CheckCollisionPointRec(GetMousePosition(), rectangle) ? GRAY : BLACK);

	std::string text = element.name + (element.childs.empty() ? "" : " >");

	Erl::DrawTextCentered(UiDef::font, text.c_str(), Vector2 {rectangle.x + rectangle.width / 2.f, rectangle.y + rectangle.height / 2.f}, (25.f + Erl::FontSize(100.f / text.length())) / 2.f, 1.0f, element.disabled ? GRAY : BLACK);
}

void CollideEach(Element &element) {
	if(!element._tabVisible)
		return;

	Rectangle rectangle {
		element._depthX * (UiDef::topBar.width / 10.f),
		element._depthY * UiDef::topBar.height,
		UiDef::topBar.width / 10.f,
		UiDef::topBar.height
	};

	if(CheckCollisionPointRec(GetMousePosition(), rectangle) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CustomUI::enabled && !element.disabled) {
		if(!element.childs.empty()) {
			if(!element.expanded) {
				if(element.parent != nullptr) {
					for(Element &child : element.parent->childs) {
						SetExpanded(child, false);
					}
				} else {
					for(Element &menu : menus) {
						SetExpanded(menu, false);
					}
					for(Element &child : element.childs) {
						SetExpanded(child, false);
					}
				}
				SetExpanded(element, true);
			} else {
				SetExpanded(element, false);
			}
		} else {
			Action::Activate(element.id);
		}
	}
}

void Draw() {
	for(Element &menu : menus) {
		menu.walk(&DrawEach);
	}
}

void Collide() {
	for(Element &menu : menus) {
		menu.walk(&CollideEach);
	}
}

};
#endif
