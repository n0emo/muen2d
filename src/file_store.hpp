#pragma once

#include <filesystem>
#include <ostream>
#include <string>
#include <vector>

#include <error.hpp>

using zip_t = struct zip;

namespace muen {

class IFileStore {
  public:
    /// Read file to stream
    virtual auto read(const std::filesystem::path& path, std::ostream& stream) noexcept -> Result<> = 0;

    /// Read entire file and return bytes
    virtual auto read_bytes(const std::filesystem::path& path) noexcept -> Result<std::vector<char>> = 0;

    /// Read entire file and return bytes
    virtual auto read_string(const std::filesystem::path& path) noexcept -> Result<std::string> = 0;

    virtual ~IFileStore() = default;
    IFileStore(const IFileStore&) = default;
    IFileStore(IFileStore&&) = default;
    auto operator=(const IFileStore&) -> IFileStore& = default;
    auto operator=(IFileStore&&) -> IFileStore& = default;

  protected:
    IFileStore() = default;
};

class FilesystemStore final: public IFileStore {
  private:
    std::filesystem::path _base_path {};

  public:
    static auto open(std::filesystem::path base_path) noexcept -> Result<FilesystemStore>;

    auto read(const std::filesystem::path& path, std::ostream& stream) noexcept -> Result<> override;
    auto read_bytes(const std::filesystem::path& path) noexcept -> Result<std::vector<char>> override;
    auto read_string(const std::filesystem::path& path) noexcept -> Result<std::string> override;

  private:
    FilesystemStore(std::filesystem::path&& base_path) noexcept;
};

class ZipStore final: public IFileStore {
  private:
    zip_t *_zip;

  public:
    static auto open(const std::filesystem::path& path) noexcept -> Result<ZipStore>;

    auto read(const std::filesystem::path& path, std::ostream& stream) noexcept -> Result<> override;
    auto read_bytes(const std::filesystem::path& path) noexcept -> Result<std::vector<char>> override;
    auto read_string(const std::filesystem::path& path) noexcept -> Result<std::string> override;

    ZipStore(const ZipStore&) = delete;
    ZipStore(ZipStore&& other) noexcept;
    auto operator=(const ZipStore&) -> ZipStore& = delete;
    auto operator=(ZipStore&& other) noexcept -> ZipStore&;
    ~ZipStore() override;

  private:
    ZipStore(zip_t *zip);
};

} // namespace muen
