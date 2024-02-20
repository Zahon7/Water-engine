#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#include <string>
#include <vector>
#include <raylib.h>
#include <algorithm>

namespace StringUtils {
	std::string StringReplace(std::string str, const std::string& from, const std::string& to) {
	    size_t start_pos = 0;
	    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
	        str.replace(start_pos, from.length(), to);
	        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	    }
	    return str;
	}

	std::vector<std::string> StringSplit(std::string s, std::string delimiter) {
	    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	    std::string token;
	    std::vector<std::string> res;

	    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
	        token = s.substr (pos_start, pos_end - pos_start);
	        pos_start = pos_end + delim_len;
	        res.push_back (token);
	    }

	    res.push_back (s.substr (pos_start));
	    return res;
	}

	std::string StringJoin(const std::vector<std::string> &lst, const std::string &delim) {
	    std::string ret;
	    for(const auto &s : lst) {
	        if(!ret.empty())
	            ret += delim;
	        ret += s;
	    }
	    return ret;
	}

	std::vector<float> GetFloatVector(std::string string, int expectedLength) {
		std::vector<std::string> strings = StringSplit(string, ", ");
		std::vector<float> vector;
		for(int index = 0; index < expectedLength; index++) {
			if(index > strings.size() - 1)
				vector.push_back(0.0f);
			else
				vector.push_back(std::stof(strings[index]));
		}
		return vector;
	}

	std::string ToString(Vector2 position) {
		return std::string(TextFormat("Vector2 {%ff, %ff}", position.x, position.y));
	}

	std::string ToString(Vector3 position) {
		return std::string(TextFormat("Vector3 {%ff, %ff, %ff}", position.x, position.y, position.z));
	}

	std::string ToString(Vector4 position) {
		return std::string(TextFormat("Vector4 {%ff, %ff, %ff, %ff}", position.x, position.y, position.z, position.w));
	}

	std::string ToString(float value) {
		return std::string(TextFormat("%ff", value));
	}

	std::string ToString(Color color) {
		return std::string(TextFormat("Color {%u, %u, %u, %u}", color.r, color.g, color.b, color.a));
	}

	std::string ToString(Matrix matrix) {
		return std::string(TextFormat("Matrix {%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f}",
			matrix.m0, matrix.m4, matrix.m8, matrix.m12,
			matrix.m1, matrix.m5, matrix.m9, matrix.m13,
			matrix.m2, matrix.m6, matrix.m10, matrix.m14,
			matrix.m3, matrix.m7, matrix.m11, matrix.m15
		));
	}

	std::string EscapeString(std::string string) {
		std::string output = string;

		output = StringReplace(output, "\"", "\\\"");
		output = StringReplace(output, "'", "\\'");

		output = StringReplace(output, "\n", "\\n");
		output = StringReplace(output, "\t", "\\t");

		return output;
	}

	std::string ToString(std::string string) {
		return EscapeString(string);
	}

	struct CodeColors {
		std::string red;
		std::string green;
		std::string blue;

		std::string white;
	};

	struct CodeColorsLines {
		std::vector<std::string> red;
		std::vector<std::string> green;
		std::vector<std::string> blue;

		std::vector<std::string> white;
	};

	enum CodeColor {CC_RED, CC_GREEN, CC_BLUE, CC_WHITE};

	char _StringSafeIndex(std::string string, int index) {
		if(index >= string.size() && index < 0)
			return ' ';
		else
			return string[index];
	}

	bool _StringCheck(std::string string, int index, std::string substring) {
		std::string out;
		for(int charIndex = index; charIndex < index + substring.length(); charIndex++) {
			out += _StringSafeIndex(string, charIndex);
		}
		return out == substring;
	}

	bool _StringCheckNegative(std::string string, int index, std::string substring) {
		std::string out;
		for(int charIndex = index; charIndex > index - (int)substring.length(); charIndex--) {
			out += _StringSafeIndex(string, charIndex);
		}
		std::reverse(out.begin(), out.end());
		return out == substring;
	}

	struct CodeKeywords {
		std::vector<std::string> redKeywords;
		std::vector<std::string> greenKeywords;
		std::vector<std::string> blueKeywords;
	};

	CodeColors CppColorify(std::string code, CodeKeywords keywords) {
		CodeColors out;
		CodeColor current = CC_WHITE;
		std::string currentWord = "";
		bool comment = false;

		for(int index = 0; index < code.length(); index++) {
			if(!comment) {
				for(std::string redKeyword : keywords.redKeywords) {
					if(_StringCheck(code, index, redKeyword)) {
						current = CC_RED;
						currentWord = redKeyword;
						break;
					}
				}

				for(std::string greenKeyword : keywords.greenKeywords) {
					if(_StringCheck(code, index, greenKeyword)) {
						current = CC_GREEN;
						currentWord = greenKeyword;
						break;
					}
				}

				for(std::string blueKeyword : keywords.blueKeywords) {
					if(_StringCheck(code, index, blueKeyword)) {
						current = CC_BLUE;
						currentWord = blueKeyword;
						break;
					}
				}
			}

			if(_StringCheck(code, index, "//")) {
				comment = true;
				current = CC_GREEN;
			}
			
			if(!currentWord.empty()) {
				if(_StringCheckNegative(code, index - 1, currentWord)) {
					current = CC_WHITE;
				}
			}

			out.red += " ";
			out.green += " ";
			out.blue += " ";
			out.white += " ";

			switch(current) {
				case CC_RED: {
					out.red.back() = code[index];
					break;
				}
				
				case CC_GREEN: {
					out.green.back() = code[index];
					break;
				}

				case CC_BLUE: {
					out.blue.back() = code[index];
					break;
				}

				case CC_WHITE: {
					out.white.back() = code[index];
					break;
				} 
			}
		}
		return out;
	}

	CodeColorsLines CppColorify(std::vector<std::string> code, CodeKeywords keywords) {
		CodeColorsLines out;
		for(std::string line : code) {
			CodeColors currentColors = CppColorify(line, keywords);
			out.red.push_back(currentColors.red);
			out.green.push_back(currentColors.green);
			out.blue.push_back(currentColors.blue);
			out.white.push_back(currentColors.white);
		}
		return out;
	}

	/*
	int GetLineIndex(std::string text, int index) {
		int pointer = index - 1;
		int counter = 0;
		while(true) {
			if(text[pointer] == '\n')
				counter++;

			pointer--;
			if(pointer < 0)
				break;
		}

		return counter;
	}

	enum JumpType {JUMP_UP, JUMP_DOWN};

	// Input: text, index
	// Returns: From index get the current line length
	
	// Example:
	// Input: text = "abc\ncdef\nxyz", index = 2
	//                  ^
	// Returns: 3
	// "abc\ncdef\nxyz"
	//  ^^^ (length 3)
	int GetLineLength(std::string text, int index, JumpType type) {
		int currentIndex = index;
		for(;;) {
			currentIndex--;
			if(currentIndex == -1 || text[currentIndex] == '\n')
				break;
		}

		currentIndex++;

		int length = 0;
		for(;;) {
			if(text[currentIndex] == '\n' || currentIndex > text.size() - 1)
				break;
			currentIndex++;
			length++;
		}

		length++;

		std::cout << length << std::endl;

		int currentLine = GetLineIndex(text, index);
		while(GetLineIndex(text, index + length) != currentLine + 1) {
			length--;
			if(index + length == text.length() - 1)
				break;
		}

		return length;
	}
	*/

	class CodePointer {
	public:
		int line = 0;
		int character = 0;

		std::vector<std::string> lines;

		std::string CurrentLine() {
				return lines[line];
		}

		bool InvalidPlace() {
				if(line > lines.size() - 1)
						true;

				if(character > CurrentLine().size())
					return true;

				if(line < 0 || character < 0)
					return true;

				return false;
		}

		bool Forward(int places) {
				int oldLine = line;
				int oldCharacter = character;

				for(int i = 0; i < places; i++) {
						character++;
						if(character > CurrentLine().size()) {
								character = 0;
								line++;
						}
				}

				if(InvalidPlace()) {
						line = oldLine;
						character = oldCharacter;
						return false;
				}
				return true;
		}

		bool Backwards(int places) {
				int oldLine = line;
				int oldCharacter = character;

				for(int i = 0; i < places; i++) {
						character--;
						if(character < 0) {
							line--;
							if(line < 0) {
								line = oldLine;
								character = oldCharacter;
								return false;
							}
							character = CurrentLine().size();
						}
				}

				if(InvalidPlace()) {
					line = oldLine;
					character = oldCharacter;
					return false;
				}
				return true;
		}

		bool JumpDown() {
			line++;
			if(line >= lines.size()) {
				line--;
				return false;
			} else {
				character = Clamp(character, 0, lines[line].size());
			}

			return true;
		}

		bool JumpUp() {
			line--;
			if(line < 0) {
				line++;
				return false;
			} else {
				character = Clamp(character, 0, lines[line].size());
			}

			return true;
		}

		void NewLine() {
			std::pair<std::string, std::string> parts {lines[line].substr(0, character), lines[line].substr(character, lines[line].length() - character)};
			lines[line] = parts.first;
			lines.insert(lines.begin() + line + 1, parts.second);
			
			line++;
			character = 0;
		}

		void Insert(char newCharacter) {
			if(newCharacter == '\n')
				NewLine();
			else {
				lines[line].insert(lines[line].begin() + character, newCharacter);
				Forward(1);
			}
		}

		void JoinLines() {
			lines[line] += lines[line + 1];
			lines.erase(lines.begin() + line + 1);
		}

		void Delete() {
			int oldLine = line;
			Backwards(1);
			if(oldLine != line) {
				JoinLines();
			} else {
				lines[line].erase(lines[line].begin() + character);
			}
		}

		void LoadFrom(std::string string) {
			lines = StringSplit(string, "\n");
		}

		std::string SaveTo() {
			return StringJoin(lines, "\n");
		}
	};
};
#endif
