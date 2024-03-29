#ifndef ERL_CPP
#define ERL_CPP

#include <string>
#include <vector>
#include <climits>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <cstring>
#include <sstream>

#include <iostream>
// Extended RayLib
namespace Erl {
	float desktopWindowWidth = 750 / 2.f;
	float desktopWindowHeight = 1280 / 2.f;

	void InitWindowEx(float width, float height, const char* title, unsigned int flagsNormal, unsigned int flagsAndroid = UINT_MAX) {
		desktopWindowWidth = width;
		desktopWindowHeight = height;

		// If not android flags specified
		if(flagsAndroid == UINT_MAX) {
			// Turn on the flagsNormal
	  		SetConfigFlags(flagsNormal);
	  	}

		#ifndef PLATFORM_ANDROID
			// If android flags specified
			if(flagsAndroid != UINT_MAX) {
				// Turn on the flagsNormal
		  		SetConfigFlags(flagsNormal);
		  	}
			
			// Init the window
			InitWindow(width, height, title);

			// Change directory to "assets/"
			ChangeDirectory("assets");
		#else
			// If android

			// If android flags specified
			if(flagsAndroid != UINT_MAX) {
				// Turn on the flagsAndroid
		  		SetConfigFlags(flagsAndroid);
		  	}

			// Size: 0, 0 (app is automaticly fullscreen)
			InitWindow(0, 0, title);
		#endif
	}

	/*
	// Calculate look at matrix (https://www.geertarien.com/blog/2017/07/30/breakdown-of-the-lookAt-function-in-OpenGL/)
	Matrix LookAt(Vector3 eye, Vector3 at, Vector3 up) {
	  Vector3 zaxis = Vector3Normalize(Vector3Subtract(at, eye));    
	  Vector3 xaxis = Vector3Normalize(Vector3CrossProduct(zaxis, up));
	  Vector3 yaxis = Vector3CrossProduct(xaxis, zaxis);

	  zaxis = Vector3Negate(zaxis);

	  Matrix viewMatrix = {
	    xaxis.x, xaxis.y, xaxis.z, -Vector3DotProduct(xaxis, eye),
	    yaxis.x, yaxis.y, yaxis.z, -Vector3DotProduct(yaxis, eye),
	    zaxis.x, zaxis.y, zaxis.z, -Vector3DotProduct(zaxis, eye),
	    0,       0,       0,       1
	  };

	  return viewMatrix;
	}
	*/

	// Expand a rectangle
	Rectangle RectangleExpanded(Rectangle rectangle, Vector2 amount) {
		return Rectangle {
			rectangle.x - amount.x / 2.f,
			rectangle.y - amount.y / 2.f,
			rectangle.width + amount.x,
			rectangle.height + amount.y
		};
	}

	// Scales the font size accordingly to the device size
	float FontSize(float fontSize) {
		return fontSize * ((GetScreenHeight() - desktopWindowHeight) / 512.f + 1.f);
	}

	// Draws text centered
	void DrawTextCentered(Font font, std::string text, Vector2 position, float fontSize, float spacing, Color tint) {
		Vector2 textSize = MeasureTextEx(font, text.c_str(), fontSize, spacing);

		DrawTextEx(font, text.c_str(), Vector2 {position.x - (textSize.x / 2.f), position.y - (textSize.y / 2.f)}, fontSize, spacing, tint);
	}

	// Draw smart text lines center
	void DrawLinesSmartC(Font font, std::vector<std::string> lines, Vector2 position, float fontSize, float spacing, Color tint) {
		float lineYSize = MeasureTextEx(font, lines[0].c_str(), fontSize, spacing).y;
		float ySize = lineYSize * lines.size();

		int index = 0;
		for(float y = position.y - (ySize / 2.f); y < position.y + (ySize / 2.f); y += lineYSize) {
			DrawTextCentered(font, lines[index], Vector2 {position.x, y}, fontSize, spacing, tint);
			index++;
		}
	}

	// Draw smart text lines left
	void DrawLinesSmartL(Font font, std::vector<std::string> lines, Vector2 position, float fontSize, float spacing, Color tint) {
		float lineYSize;
		if(lines.size() > 0)
			lineYSize = MeasureTextEx(font, lines[0].c_str(), fontSize, spacing).y;
		else
			lineYSize = 1.f;
		
		float ySize = lineYSize * lines.size();

		int index = 0;
		for(float y = position.y; y < position.y + ySize; y += lineYSize) {
			index = Clamp(index, 0, lines.size() - 1);
			DrawTextEx(font, lines[index].c_str(), Vector2 {position.x, y}, fontSize, spacing, tint);
			index++;
		}
	}

	// Measures only the first line, and calculates the `returned.y` by: `firstLine.y * lines.size()`
	// to high quality results please call hqMeasureTextExSmart
	Vector2 MeasureTextExSmart(Font font, std::vector<std::string> lines, float fontSize, float spacing) {
	    Vector2 lineMeasured = MeasureTextEx(font, lines[0].c_str(), fontSize, spacing);

		return Vector2 {lineMeasured.x, lineMeasured.y * lines.size()};
	}

	// Measures the entire text and picks the maximum width for `returned.x`
	Vector2 hqMeasureTextExSmart(Font font, std::vector<std::string> lines, float fontSize, float spacing) {
	    float maxWidth = 0.f;
		Vector2 lineMeasured;

		for(std::string line : lines) {
			lineMeasured = MeasureTextEx(font, line.c_str(), fontSize, spacing);
			if(maxWidth < lineMeasured.x)
				maxWidth = lineMeasured.x;
		}

		return Vector2 {maxWidth, lineMeasured.y * lines.size()};
	}

	Vector2 MeasureTextExSmart(Font font, std::string text, float fontSize, float spacing) {
		std::vector<std::string> lines;

		std::string word = "";
	    for (auto x : text) {
	        if (x == '\n') {
	            lines.push_back(word);
	            word = "";
	        } else {
	            word = word + x;
	        }
	    }

	    Vector2 lineMeasured = MeasureTextEx(font, lines[0].c_str(), fontSize, spacing);

		return Vector2 {lineMeasured.x, lineMeasured.y * lines.size()};
	}

	/*
	// Word wrap
	// INFO: Bad word wrap ._.
	std::vector<std::string> WordWrap(std::string text, Font font, float fontSize, float max = -1) {
		if(!max)
			max = GetScreenWidth();

		std::vector<std::string> output = {};
		std::string current;
		int index = 0;
		for(char character : text) {
			if(character == '\n' || index == text.length() - 1) {
				output.push_back(current);
				current = "";
				continue;
			}

			if(MeasureTextEx(font, (current + character + ' ').c_str(), fontSize, 1.f).x >= max) {
				std::string suffix = current.substr(current.find_last_of(' '));
				output.push_back(current.substr(0, current.length() - suffix.length()));
				current = suffix;
			}

			current += character;
		}
		if(output.empty()) output.emplace_back();
		return output;
	}
	*/

	// https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
	bool _HasEnding(std::string const &fullString, std::string const &ending) {
    		if (fullString.length() >= ending.length()) {
        		return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    		} else {
        		return false;
    		}
	}



	// Metrics to pixels
	float Pixels(std::string string) {
		int index = 0;
		while(isdigit(string[index])) {
			++ index;
		}
		int value = std::stoi(string.substr(0, index));

		if(_HasEnding(string, "mm")) {
			#ifdef PLATFORM_ANDROID
				#warning "Pixels('... mm') doesn't work on android"
			#else
				return ((float)GetMonitorWidth(GetCurrentMonitor()) / (float)GetMonitorPhysicalWidth(GetCurrentMonitor())) * value;
			#endif
		} else if(_HasEnding(string, "% h")) {
			return GetScreenHeight() * (value / 100.f);
		} else if(_HasEnding(string, "% w")) {
			return GetScreenWidth() * (value / 100.f);
		} else {
			return value;
		}
	}

	// https://sentry.io/answers/split-string-in-cpp/
	std::vector<std::string> _StringSplit(std::string s, std::string delim) {
	    std::vector<std::string> res;
	    std::string token = "";
	    for (int i = 0; i < s.size(); i++) {
	        bool flag = true;
	        for (int j = 0; j < delim.size(); j++) {
	            if (s[i + j] != delim[j]) flag = false;
	        }
	        if (flag) {
	            if (token.size() > 0) {
	                res.push_back(token);
	                token = "";
	                i += delim.size() - 1;
	            }
	        } else {
	            token += s[i];
	        }
	    }
	    res.push_back(token);
	    return res;
	}

	// Pixel vector 2
	Vector2 PixelsVector2(std::string input) {
		std::vector<std::string> vector = _StringSplit(input, ", ");

		Vector2 output = {0, 0};

		int index = 0;
		for(std::string string : vector) {
			int charIndex = 0;
			while(isdigit(string[charIndex])) {
				++ charIndex;
			}
			int value = std::stoi(string.substr(0, charIndex));

			float pixels;
			if(_HasEnding(string, "mm")) {
				#ifdef PLATFORM_ANDROID
					#warning "Pixels('... mm') doesn't work on android"
				#else
					pixels = ((float)GetMonitorWidth(GetCurrentMonitor()) / (float)GetMonitorPhysicalWidth(GetCurrentMonitor())) * value;
				#endif
			} else if(_HasEnding(string, "%")) {
				if(index == 0) pixels = GetScreenWidth() * (value / 100.f);
				else if(index == 1) pixels = GetScreenHeight() * (value / 100.f);
			} else {
				pixels = value;
			}

			if(index == 0) output.x = pixels;
			else if(index == 1) output.y = pixels;
			index++;
		}

		return output;
	}

	Rectangle GetCenterRectangle(Vector2 position, Vector2 scale) {
		return Rectangle {
			position.x - scale.x / 2.f,
			position.y - scale.y / 2.f,
			scale.x,
			scale.y
		};
	}

	std::vector<std::string> WordWrap(const std::string& inputString, Font font, float fontSize, float max = -1) {
		if(!max)
			max = GetScreenWidth();

		Vector2 charSize = MeasureTextEx(font, "A", fontSize, 1.f);

		std::istringstream iss(inputString);

		std::vector<std::string> outputString;
		std::string line;

		do {
			std::string word;
			iss >> word;

			std::cout << word << std::endl;
			if ((line.length() + word.length()) * charSize.x > max)
			{
				outputString.push_back(line);
				line.clear();
			}
			line += word + " ";

		} while (iss);

		if (!line.empty())
		{
			outputString.push_back(line);
		}

		return outputString;
	}

	// Draws text word wrapped center
	void DrawTextWrappedC(Font font, std::string text, Vector2 position, float fontSize, float spacing, Color tint, float margin = 0) {
		DrawLinesSmartC(font, WordWrap(text, font, fontSize, margin), position, fontSize, spacing, tint);
	}

	// Draws text word wrapped left
	void DrawTextWrappedL(Font font, std::string text, Vector2 position, float fontSize, float spacing, Color tint, float margin = 0) {
		DrawLinesSmartL(font, WordWrap(text, font, fontSize, margin), position, fontSize, spacing, tint);
	}
}

#endif
