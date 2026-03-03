#include "core.h"
#include <fstream>

Result Error(string file, int line, const char *format, ...) {
    static Result last_result = Success;

    // Already encountered an error?
    // Then skip.. only report the first error that occurs
    if (last_result == Failed)
        return Failed;

    printf("Error: ");
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n See %.*s, line %i\n", FORMAT_STRING(file), line);
    return (last_result = Failed);
}

string file_extension(string filename) {
    u64 start = filename.size();
    for (u64 i = 0; i < filename.size(); ++i) {
        u64 ri = filename.size() - i - 1;
        if (filename[ri] == '.') {
            start = ri;
            break;
        }
    }
    return filename.substr(start);
}

string parent_directory(string filepath) {
    u64 count = filepath.size();
    for (u64 i = 0; i < filepath.size(); ++i) {
        u64 ri = filepath.size() - i - 1;
        if (filepath[ri] == '/' || filepath[ri] == '\\') {
            count = ri;
            break;
        }
    }
    if (count == 0)
        return string(".");
    return filepath.substr(0, count);
}

string join_path(string root, string path, ScratchArena &arena) {
    u64 count = root.size() + path.size() + 1;
    u8 *data = arena.allocate(count);
    memcpy(data, root.data(), root.size());
    data[root.size()] = '/';
    memcpy(data + root.size() + 1, path.data(), path.size());
    return string((char*)(data), count);
}

optional<string> read_entire_file(string filename) {
	std::ifstream f(std::string(filename), std::ios::binary);
    if (!f.is_open()) return {};

	f.seekg(0, std::ios::end);
    size_t count = f.tellg();
	f.seekg(0, std::ios::beg);
    char *data = (char*)(malloc(count));
    assert(data != 0);
    f.read(data, count);
    if (f.bad() || f.fail()) {
        printf("Warning: Something went wrong when reading file \"%.*s\"!\n", FORMAT_STRING(filename));
    }
	f.close();
    return string(data, count);
}
