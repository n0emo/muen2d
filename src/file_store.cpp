#include <file_store.hpp>

#include <fstream>
#include <iterator>
#include <sstream>

#include <fmt/format.h>
#include <zip.h>
#include <spdlog/spdlog.h>

#include <defer.hpp>

namespace muen {

auto FilesystemStore::open(std::filesystem::path base_path) noexcept -> Result<FilesystemStore> {
    auto store = FilesystemStore {std::move(base_path)};
    return store;
}

auto FilesystemStore::read(const std::filesystem::path& file_path, std::ostream& stream) noexcept -> Result<> try {
    const auto path = _base_path / file_path;
    SPDLOG_TRACE("Reading file `{}` to stream", path.string());
    auto file = std::ifstream {path, std::ios::in | std::ios::binary};
    stream << file.rdbuf();
    if (!file) return err(fmt::format("Could not read {}: {}", path.string(), strerror(errno)));
    return {};
} catch (std::exception& e) {
    return err(e);
}

auto FilesystemStore::read_bytes(const std::filesystem::path& file_path) noexcept -> Result<std::vector<char>> try {
    const auto path = _base_path / file_path;
    SPDLOG_TRACE("Reading file `{}` to vector", path.string());
    auto file = std::ifstream {path, std::ios::in | std::ios::binary};
    auto eos = std::istreambuf_iterator<char>();
    auto buf = std::vector<char>(std::istreambuf_iterator<char>(file), eos);
    if (!file) return err(fmt::format("Could not read: {}", strerror(errno)));
    return buf;
} catch (std::exception& e) {
    return err(e);
}

auto FilesystemStore::read_string(const std::filesystem::path& file_path) noexcept -> Result<std::string> try {
    const auto path = _base_path / file_path;
    SPDLOG_TRACE("Reading file `{}` to string", path.string());
    auto file = std::ifstream {path, std::ios::in | std::ios::binary};
    auto buf = std::stringstream {};
    buf << file.rdbuf();
    if (!file) return err(fmt::format("Could not read: {}", strerror(errno)));
    return buf.str();
} catch (std::exception& e) {
    return err(e);
}

FilesystemStore::FilesystemStore(std::filesystem::path&& base_path) noexcept : _base_path(std::move(base_path)) {}

auto ZipStore::read(const std::filesystem::path& path, std::ostream& stream) noexcept -> Result<> try {
    auto file = zip_fopen(_zip, path.string().c_str(), 0);
    if (file == nullptr) return err(zip_strerror(_zip));
    defer(zip_fclose(file));

    auto buf = std::make_unique<std::array<char, 512ul * 1024ul>>();
    for (;;) {
        auto n = zip_fread(file, buf->data(), buf->size());
        if (n < 0) return err(zip_file_strerror(file));
        if (n == 0) break;

        stream.write(buf->data(), n);
    }

    return {};
} catch (std::exception& e) {
    // TODO: static zstring error
    return err(e);
}

auto ZipStore::read_bytes(const std::filesystem::path& path) noexcept -> Result<std::vector<char>> try {
    auto stats = zip_stat_t {};
    if (zip_stat(_zip, path.string().c_str(), 0, &stats) < 0) return err(zip_strerror(_zip));

    auto file = zip_fopen(_zip, stats.name, 0);
    if (file == nullptr) return err(zip_strerror(_zip));
    defer(zip_fclose(file));

    auto vec = std::vector<char> {};
    vec.reserve(stats.size);
    auto iter = std::back_inserter(vec);

    auto buf = std::make_unique<std::array<char, 512ul * 1024ul>>();
    for (;;) {
        auto n = zip_fread(file, buf->data(), buf->size());
        if (n < 0) return err(zip_file_strerror(file));
        if (n == 0) break;

        std::copy(buf->begin(), buf->begin() + n, iter);
    }

    return vec;
} catch (std::exception& e) {
    return err(e);
}

auto ZipStore::read_string(const std::filesystem::path& path) noexcept -> Result<std::string> try {
    auto file = zip_fopen(_zip, path.string().c_str(), 0);
    if (file == nullptr) return err(zip_strerror(_zip));
    defer(zip_fclose(file));

    auto str = std::string {};
    auto buf = std::make_unique<std::array<char, 512ul * 1024ul>>();
    for (;;) {
        auto n = zip_fread(file, buf->data(), buf->size());
        if (n < 0) return err(zip_file_strerror(file));
        if (n == 0) break;

        str.append(buf->begin(), buf->begin() + n);
    }

    return str;
} catch (std::exception& e) {
    return err(e);
}

// TODO: Open from self
auto ZipStore::open(const std::filesystem::path& path) noexcept -> Result<ZipStore> {
    auto ec = int {};
    auto zip = zip_open(path.string().c_str(), ZIP_RDONLY, &ec);
    if (zip == nullptr) {
        auto e = zip_error_t {};
        zip_error_init_with_code(&e, ec);
        defer(zip_error_fini(&e));
        return err(fmt::format("Could not open archive `{}`: {}", path.string(), zip_error_strerror(&e)));
    }
    return ZipStore(zip);
}

ZipStore::ZipStore(ZipStore&& other) noexcept : _zip(other._zip) {
    other._zip = nullptr;
};

auto ZipStore::operator=(ZipStore&& other) noexcept -> ZipStore& {
    // TODO: Free this->_zip?
    auto zip = other._zip;
    other._zip = nullptr;
    _zip = zip;
    return *this;
}

ZipStore::~ZipStore() {
    if (_zip == nullptr) return;
    zip_close(_zip);
}

ZipStore::ZipStore(zip_t *zip) : _zip(zip) {};

} // namespace muen
