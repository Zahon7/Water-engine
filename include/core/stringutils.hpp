#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#include <string>
#include <vector>
#include <raylib.h>

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
};
#endif
