#pragma once

#include <lib.hpp>
#include <nn.hpp>

#include <string>
#include <span>
#include <array>

namespace util {

    class FileHandle {
        private:
        nn::fs::FileHandle m_Handle;

        public:
        inline FileHandle(const char* path, int mode) {
            R_ABORT_UNLESS(nn::fs::OpenFile(&m_Handle, path, mode));
        }

        inline FileHandle(std::string path, int mode) : FileHandle(path.c_str(), mode) {}

        /* nn shims. */
        inline Result Read(long position, void* buffer, ulong size) const {
            return nn::fs::ReadFile(m_Handle, position, buffer, size);
        }
        inline Result Read(long position, void* buffer, const nn::fs::ReadOption& option) const {
            return nn::fs::ReadFile(m_Handle, position, buffer, option);
        }
        inline Result Read(ulong* bytesRead, long position, void* buffer) const {
            return nn::fs::ReadFile(bytesRead, m_Handle, position, buffer);
        }
        inline Result Read(ulong* bytesRead, long position, void* buffer, const nn::fs::ReadOption& option) const {
            return nn::fs::ReadFile(bytesRead, m_Handle, position, buffer, option);
        }
        inline Result Write(long position, const void* buffer, ulong size, nn::fs::WriteOption const& option) {
            return nn::fs::WriteFile(m_Handle, position, buffer, size, option);
        }
        inline Result Write(const void* buffer, ulong size, nn::fs::WriteOption const& option) {
            return Write(0, buffer, size, option);
        }
        inline Result Write(long position, const void* buffer, ulong size) {
            return Write(position, buffer, size, nn::fs::WriteOption::CreateOption(0));
        }
        inline Result Write(const void* buffer, ulong size) {
            return Write(0, buffer, size);
        }
        
        /* std::span helpers. */
        template<typename T>
        inline Result Read(long position, std::span<T> out) const {
            return Read(position, (void*) out.data(), sizeof(T) * out.size());
        }
        template<typename T>
        inline Result Read(std::span<T> out) const {
            return Read(0, out);
        }
        template<typename T>
        inline Result Write(long position, std::span<T> in) {
            return Write(position, (void*) in.data(), sizeof(T) * in.size());
        }
        template<typename T>
        inline Result Write(const std::span<T> in) {
            return Write((const void*) in.data(), sizeof(T) * in.size());
        }

        /* std::array helpers. */
        template<size_t Size>
        inline Result Read(long position, std::array<u8, Size>* out) const {
            return Read(position, (void*) out->data(), Size);
        }
        template<size_t Size>
        inline Result Read(std::array<u8, Size>* out) const {
            return Read(0, out);
        }
        template<size_t Size>
        inline Result Write(long position, std::array<u8, Size>* out) {
            return Write(position, (void*) out->data(), Size);
        }
        template<size_t Size>
        inline Result Write(std::array<u8, Size>* out) {
            return Write((void*) out->data(), Size);
        }

        /* Generic helpers. */
        template<typename T>
        inline Result Read(long position, T* out) const {
            return Read(position, (void*) out, sizeof(T));
        }
        template<typename T>
        inline Result Read(T* out) const {
            return Read(0, out);
        }
        template<typename T>
        inline Result Write(long position, const T* out) {
            return Write(position, (const void*) out, sizeof(T));
        }
        template<typename T>
        inline Result Write(const T* out) {
            return Write((const void*) out->data(), sizeof(T));
        }

        inline size_t GetSize() const {
            long size;
            R_ABORT_UNLESS(nn::fs::GetFileSize(&size, m_Handle));
            return size;
        }

        inline Result Flush() const {
            return nn::fs::FlushFile(m_Handle);
        }

        inline ~FileHandle() {
            nn::fs::CloseFile(m_Handle);
        }
        
    };
}