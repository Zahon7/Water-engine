#include <vector>
#include <string>

class CodePointer {
public:
	int line = 0;
	int character = 0;

	std::vector<std::string> *lines;

	std::string CurrentLine() {
			return (*lines)[line];
	}

	bool InvalidPlace() {
			if(line > lines->size() - 1)
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

	void Insert(char newCharacter) {
		(*lines)[line].insert((*lines)[line].begin() + character, newCharacter);
		Forward(1);
	}

	void JoinLines() {
		(*lines)[line] += (*lines)[line + 1];
		(*lines).erase(lines->begin() + line + 1);
	}

	void Delete() {
		int oldLine = line;
		Backwards(1);
		if(oldLine != line) {
			JoinLines();
		} else {
			(*lines)[line].erase((*lines)[line].begin() + character);
		}
	}
};

#include <iostream>

int main() {
	CodePointer ptr;
	ptr.lines = new std::vector<std::string> {"abc cdef", "hijk lm", "nopqr"};
	
	{
		ptr.line = 2;
		ptr.character = 0;

		ptr.Delete();
	}


	system("clear");
	std::vector<std::string> linesCopy = (*ptr.lines);
	linesCopy[ptr.line].insert(linesCopy[ptr.line].begin() + ptr.character, '|');
	for(std::string s : linesCopy)
		std::cout << s << std::endl;

	return 0;
}
