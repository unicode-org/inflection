/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <iostream>
#include <fstream>
#include <vector>

// A flushless endl. The std::endl flushes when it's not needed.
template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>& endl(std::basic_ostream<CharT, Traits>& os)
{
    // Initialize the newline value once
    static const auto newline = os.widen('\n');
    return os.put(newline);
}

// Checks if a line is an empty line (0 or more whitespaces (' ' or '\t')) or a comment line,
// beyond check_index (check_index=0 will check the entire line)
static bool is_comment_or_empty(const std::string &str, const size_t check_index, const char comment_char) {
    if (check_index >= str.length() || str[check_index] == comment_char) {
        return true;
    }
    if (str[check_index] != ' ' && str[check_index] != '\t') {
        return false;
    }
    return is_comment_or_empty(str, check_index + 1, comment_char);
}

// Check if the character at escaped_char_index is escaped, by checking for an odd number
// of \ appearing before this index
// BE CAREFUL when using this function to check if \ is escaped (since \ appearing after this index
// can change the meaning of this \ at the end of the line
static bool is_escaped_char(const std::string &str, size_t escaped_char_index) {
    int32_t last_slash_count = 0;
    if (str.length() <= 0 || escaped_char_index <= 0 || escaped_char_index > str.length()) {
        return false;
    }
    for (int32_t i = int32_t(escaped_char_index - 1); i >= 0; i--) {
        char cur_char = str[i];
        if (cur_char == '\\') {
            last_slash_count++;
        } else {
            break;
        }
    }
    return ((last_slash_count % 2) != 0);
}

// Escapes any unescaped " characters in a string
static std::string escape(const std::string &str) {
    std::string ret;
    auto length = str.length();
    ret.reserve(length);
    for (size_t i = 0; i < length; i++) {
        char cur_char = str[i];
        if (cur_char == '\"' && !is_escaped_char(str, i)) {
            ret.push_back('\\');
        }
        ret.push_back(cur_char);
    }

    if (is_escaped_char(str, str.length())) {
        ret.pop_back();
    }

    return ret;
}

// Return the usage string for this tool
static const char USAGE_STRING[] =
        "Usage: text2cpp --infile INFILE --outfile OUTFILE --class_name CLASSNAME [--type TYPE] [--comment_char COMMENTCHAR]";

// Trim all whitespaces (' ' or '\t' at the beginning and end of the given string
// Return a new trimmed string
static std::string trim(const std::string &str) {
    std::string ret;
    std::string spaces;
    int32_t state = 0;
    for (char ch : str) {
        switch (state) {
            case 0:
                if (ch != ' ' && ch != '\t') {
                    ret += ch;
                    state = 1;
                    spaces = "";
                }
                break;
            case 1:
                if (ch == '\\') {
                    ret += spaces;
                    spaces = "";
                    ret += ch;
                    state = 2;
                } else if (ch == ' ' || ch == '\t') {
                    spaces += ch;
                } else {
                    ret += spaces;
                    spaces = "";
                    ret += ch;
                }
                break;
            case 2:
                ret += ch;
                state = 1;
                spaces = "";
                break;
        }
    }
    return ret;
}

// Split a line into key/value pair according to standards defined for properties in a properties file
// i.e., split on =, ' ' or :
// also, allow whitespaces (' ' and '\t') before and after the key-value delimiter
static std::pair<std::string, std::string> split_key_value(const std::string &str) {
    std::string key;
    std::string value;
    int32_t state = 0;
    for (char ch : str) {
        switch (state) {
            case 0:
                if (ch != ' ' && ch != '\t') {
                    key += ch;
                    state = 1;
                }
                break;
            case 1:
                if (ch == '\\') {
                    key += ch;
                    state = 2;
                } else if (ch == ' ' || ch == '\t') {
                    state = 3;
                } else if (ch == '=' || ch == ':') {
                    state = 4;
                } else {
                    key += ch;
                }
                break;
            case 2:
                key += ch;
                state = 1;
                break;
            case 3:
                if (ch == ' ' || ch == '\t') {
                    state = 3;
                } else if (ch == '=' || ch == ':') {
                    state = 4;
                } else {
                    value += ch;
                    state = 5;
                }
                break;
            case 4:
                if (ch == ' ' || ch == '\t') {
                    state = 4;
                } else {
                    value += ch;
                    state = 5;
                }
                break;
            case 5:
                value += ch;
                break;
        }
    }
    if (state < 4) {
        key = "";
        value = "";
    }
    return std::pair<std::string, std::string>(key, value);
}

// Replaces all non-alphanumeric character sequences in a character array with _
// and returns the corresponding std::string
static std::string is_alphanumeric_colon(std::string_view str) {
    std::string ret;
    ret.reserve(str.length());
    bool underscored = false;
    for (auto ch : str) {
        if (ch == ':' || ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') || ('0' <= ch && ch <= '9')) {
            ret += ch;
            underscored = false;
        } else if (!underscored) {
            ret += '_';
            underscored = true;
        }
    }
    return ret;
}

static std::string remove_suffix(const std::string& str, const std::string& suffix) {
    if (suffix.size() > str.size() || !std::equal(suffix.rbegin(), suffix.rend(), str.rbegin())) {
        return str;
    }
    return str.substr(0, str.length() - suffix.length());
}

static std::string string_after_last_occurrence(const std::string& str, const std::string& strToMatch) {
    size_t index = str.rfind(strToMatch);
    if (index == ::std::string::npos) {
        return str;
    }
    return str.substr(index + strToMatch.length(), str.length() - index - strToMatch.length());
}

static std::string convert_to_namespace(bool includeBetweenSeparators, const ::std::string& str, const std::string& strToMatch, const std::string& prefix, const std::string& suffix) {
    ::std::string result;
    size_t lastIndex = 0;
    size_t index;
    while ((index = str.find(strToMatch, lastIndex)) != ::std::string::npos) {
        result += prefix;
        if (includeBetweenSeparators) {
            result += str.substr(lastIndex, index - lastIndex);
        }
        result += suffix;
        lastIndex = index + strToMatch.length();
    }
    return result;
}

int main (int argc, char *argv[]) {
    // If no command line arguments, print usage and exit
    if (argc < 2) {
        std::cout << USAGE_STRING << std::endl;
    }

    // Get the command line arguments:
    // in_file_name <-- input file to be read
    // out_file_name <-- output file to be written
    // type <-- can be "properties", "dictionary", "string"
    // comment_char <-- For "dictionary" only, specify the comment character
    // out_class_name <-- the name of the C++ class to create
    std::string in_file_name;
    int32_t in_file_count = 0;
    std::string out_file_name;
    int32_t out_file_count = 0;
    std::string type = "string";
    int32_t type_count = 0;
    std::string comment_char_string;
    char comment_char = 0;
    int32_t comment_char_count = 0;
    std::string out_class_name;
    std::string out_namespaceless_class_name;
    int32_t out_class_name_count = 0;
    std::string out_namespace_start;
    std::string out_namespace_end;
    std::string basename;
    for (int32_t i = 1; i < argc; i++) {
        if (std::string("--infile") == argv[i]) {
            if (in_file_count > 0) {
                std::cerr << "Multiple --infile parameters defined" << std::endl;
                std::cerr << USAGE_STRING << std::endl;
                return -1;
            }
            if (i >= argc) {
                std::cerr << "Need a file path after --infile" << std::endl;
                std::cerr << USAGE_STRING << std::endl;
            }
            in_file_name = argv[++i];
            in_file_count++;
        } else if (std::string("--outfile") == argv[i]) {
            if (out_file_count > 0) {
                std::cerr << "Multiple --outfile parameters defined" << std::endl;
                std::cerr << USAGE_STRING << std::endl;
                return -1;
            }
            if (i >= argc) {
                std::cerr << "Need a file path after --outfile" << std::endl;
                std::cerr << USAGE_STRING << std::endl;
            }
            out_file_name = remove_suffix(remove_suffix(argv[++i], ".cpp"), ".hpp");
            basename = string_after_last_occurrence(out_file_name, "/");
            out_file_count++;
        } else if (std::string("--class_name") == argv[i]) {
            if (out_class_name_count > 0) {
                std::cerr << "Multiple --class_name parameters defined" << std::endl;
                std::cerr << USAGE_STRING << std::endl;
                return -1;
            }
            if (i >= argc) {
                std::cerr << "Need a class name after --class_name" << std::endl;
                std::cerr << USAGE_STRING << std::endl;
            }
            out_class_name = is_alphanumeric_colon(argv[++i]);
            out_namespace_start = convert_to_namespace(true, out_class_name, "::", "namespace ", " { ");
            out_namespace_end = convert_to_namespace(false, out_class_name, "::", "} ", "");
            out_namespaceless_class_name = string_after_last_occurrence(out_class_name, "::");
            out_class_name_count++;
        } else if (std::string("--type") == argv[i]) {
            if (type_count > 0) {
                std::cerr << "Multiple --type parameters defined" << std::endl;
                std::cerr << USAGE_STRING << std::endl;
                return -1;
            }
            if (i >= argc) {
                std::cerr << "Need a type name after --type" << std::endl;
                std::cerr << USAGE_STRING << std::endl;
            }
            type = argv[++i];
            type_count++;
        } else if (std::string("--comment_char") == argv[i]) {
            if (comment_char_count > 0) {
                std::cerr << "Multiple --comment_char parameters defined" << std::endl;
                std::cerr << USAGE_STRING << std::endl;
                return -1;
            }
            if (i >= argc) {
                std::cerr << "Need a comment character after --comment_char" << std::endl;
                std::cerr << USAGE_STRING << std::endl;
            }
            comment_char_string = argv[++i];
            comment_char_count++;
        } else {
            std::cerr << "Unknown argument: " << argv[i] << std::endl;
            std::cerr << USAGE_STRING << std::endl;
            return -1;
        }
    }
    if (in_file_count != 1) {
        std::cerr << "Missing required parameter --infile " << std::endl;
        std::cerr << USAGE_STRING << std::endl;
        return -1;
    }
    if (out_file_count != 1) {
        std::cerr << "Missing required parameter --outfile " << std::endl;
        std::cerr << USAGE_STRING << std::endl;
        return -1;
    }
    if (out_class_name_count != 1) {
        std::cerr << "Missing required parameter --class_name " << std::endl;
        std::cerr << USAGE_STRING << std::endl;
        return -1;
    }
    if (comment_char_string.length() > 1) {
        std::cerr << "Comment character can only be a single character." << std::endl;
        return -1;
    } else if (comment_char_string.length() == 1) {
        comment_char = comment_char_string[0];
    }

    // Open the file for reading...
    std::ifstream in_handle(in_file_name);
    if (!in_handle) {
        std::cerr << "Unable to open input file: " << in_file_name << std::endl;
        return -1;
    }

    // Open the output file for writing
    std::ofstream out_handle(out_file_name + ".hpp");
    if (!out_handle) {
        std::cerr << "Unable to open output file: " << out_file_name << ".hpp" << std::endl;
        return -1;
    }

    // --- Processing a "properties" file
    if (type == "properties") {
        std::string line;

        // A comment char cannot be specified by user, since the file standard defines these
        // for properties files
        if (comment_char != 0) {
            std::cerr << "Comment character does not apply to --type properties" << std::endl;
            std::cerr << "Properties file standard defines # and ! as comment characters" << std::endl;
            return -1;
        }

        // Read the file line by line, and populate the two parallel vectors with keys and values
        std::vector<std::string> keys;
        std::vector<std::string> values;
        int32_t state = 0;
        while (std::getline(in_handle, line)) {

            // state keeps track of "continuation" from previous line (if previous line ends in \ )
            // state == 0 is a regular (non-continuation line), state == 1 is a continuation from previous line
            if (state == 0) {

                // Check for empty or comment line (and ignore it)
                if (line.length() != 0 && !is_comment_or_empty(line, 0, '#') && !is_comment_or_empty(line, 0, '!')) {

                    // Split key-value pair according to delimiter
                    std::pair<std::string, std::string> key_value = split_key_value(trim(line));

                    // If key is empty, something is wrong with the file format
                    if (key_value.first.empty()) {
                        std::cerr << "File format error: " << line << std::endl;
                        return -1;
                    }

                    // Save the key in key-vector
                    keys.push_back(key_value.first);

                    // Check if the value indicates continuation on next line
                    // adjust state accordingly, remove \ at end if it exists
                    // and save in value-vector
                    std::string value = key_value.second;
                    if (value.length() > 0 && is_escaped_char(value, value.length())) {
                        value.pop_back();
                        state = 1;
                    }
                    values.push_back(value);
                }
            // state == 1 indicates this line a continuation from previous line
            } else {
                // Set state to appropriate setting, and append line to last value in value-vector
                state = 0;
                std::string value = trim(line);
                if (value.length() > 0 && is_escaped_char(value, value.length())) {
                    value.pop_back();
                    state = 1;
                }
                values.back() += value;
            }
        }

        // Sanity check
        if (keys.size() != values.size()) {
            std::cerr << "Internal Error: Number of keys does not match number of values" << std::endl;
            return -1;
        }

        // Print out a class -- returns a map of the properties
        out_handle << "#pragma once" << endl;
        out_handle << "#include <map>" << endl;
        out_handle << "#include <string_view>" << endl;
        out_handle << out_namespace_start << endl;
        out_handle << "struct " << out_namespaceless_class_name << " final {" << endl;
        out_handle << "    static ::std::map<::std::u16string_view, const char16_t*> getContent() {" << endl;
        out_handle << "        return {";

        bool firstLine = true;
        for (size_t i = 0; i < keys.size(); i++) {
            if (firstLine) {
                firstLine = false;
                out_handle << endl;
            }
            else {
                out_handle << "," << endl;
            }
            out_handle << "        { u\"" << escape(keys[i]) << "\", u\"" << escape(values[i]) << "\" }";
        }
        out_handle << endl;
        out_handle << "        };" << endl;
        out_handle << "    }" << std::endl;
        out_handle << "};" << endl;
        out_handle << out_namespace_end << std::endl;
    } else if (type == "string") {
        // Handling any file as a string...

        // Comment char does not apply, since we're reading in everything, as is
        if (comment_char != 0) {
            std::cerr << "Comment character does not apply to --type string" << std::endl;
            return -1;
        }

        // Print out a class that returns a char16_t array
        out_handle << "#pragma once" << endl;
        out_handle << out_namespace_start << endl;
        out_handle << "struct " << out_namespaceless_class_name << " final {" << endl;
        out_handle << "    static constexpr char16_t content[] = " << endl;

        std::string line;
        line.reserve(128); // This is a good buffer size to start off with
        while (std::getline(in_handle, line)) {
            if (is_escaped_char(line, line.length())) {
                // An escape character causes trouble for the output string
                // We will escape it
                line += '\\';
            }
            out_handle << endl << "    u\"" << escape(line) << "\\n\"";
        }
        out_handle << ";" << std::endl;
        out_handle << "};" << endl;
        out_handle << out_namespace_end << std::endl;
    } else {
        // An unknown type was specified on the command line
        std::cerr << "Unknown type " << type << std::endl;
        std::cerr << "Options are properties, dictionary or string" << std::endl;
        return -1;
    }

    if (!in_handle.eof()) {
        std::cerr << "Incomplete read of: " << in_file_name << std::endl;
        return -1;
    }

    in_handle.close();
    out_handle.close();

    if (out_handle.fail()) {
        std::cerr << "Unable to close file: " << out_file_name << ".cpp" << std::endl;
        return -1;
    }
    return 0;
}
