#pragma once
#include <memory>
#include <vector>
#include <source_location>
#include <type_traits> 
#include <string>

#ifdef _DEBUG
#include <fstream>
#include <format>
#include <filesystem>
#include "FileLog.h"
#include "FileUtil.h"
#endif // _DEBUG

template <typename T>
struct CustomDeleter {
	CustomDeleter() noexcept = default;

	template<		//	型変換を許可
		typename U,
		typename std::enable_if<std::is_convertible<U*, T*>::value, std::nullptr_t>::type = nullptr	>
	CustomDeleter(const CustomDeleter<U>&) noexcept {}

	void operator()(T* ptr) {
#ifdef _DEBUG
		if constexpr (requires { T::ClassName; }) {
			// クラス名がある場合の処理
			FileLog::writePLog(std::format("D Unique {}\n", T::ClassName));
		}
		else {
			// ない場合の処理
			FileLog::writePLog(std::format("D Unique Unknown\n"));
		}
#endif
		delete ptr;
	}
};

template <typename T>
using LogPtr = std::unique_ptr<T, CustomDeleter<T>>;


template <typename T, typename... Args>
LogPtr<T> MAKE_UNIQUE_LOG(
	Args&&... args)
{
#ifdef _DEBUG
	const std::source_location location = std::source_location::current();

	auto filename = RemoveDirectoryPath(location.file_name());

	if constexpr (requires { T::ClassName; }) {
		// クラス名がある場合の処理
		FileLog::writePLog(std::format("M Unique {:<20} ---File:{}/Line:{}\n",
			T::ClassName,
			filename,
			location.line()
		));
	}
	else {
		FileLog::writePLog(std::format("M Unique {:<20} ---File:{}/Line:{}\n",
			"Unknown",
			filename,
			location.line()
		));
	}
#endif

	return LogPtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
std::vector<T*>
GetPointers(
	const std::vector<std::unique_ptr<T>>& unique)
{
	std::vector<T*> result;
	result.reserve(unique.size());

	for (const auto& node : unique) {
		result.push_back(node.get());
	}
	return result;
}

template <typename T>
std::vector<T*>
GetPointers(
	const std::vector<LogPtr<T>>& unique)
{
	std::vector<T*> result;
	result.reserve(unique.size());

	for (const auto& node : unique) {
		result.push_back(node.get());
	}
	return result;
}

template <typename To, typename From>
std::unique_ptr<To> DynamicCast_Unique(
	std::unique_ptr<From>&& ptr)
{
	To* result = dynamic_cast<To*>(ptr.get());
	if (result) {
		ptr.release();
		return std::unique_ptr<To>(result);
	}
	return nullptr;
}

template <typename To, typename From>
LogPtr<To> DynamicCast_Unique(
	LogPtr<From>&& ptr)
{
	if (!ptr) return nullptr;
	To* result = dynamic_cast<To*>(ptr.get());
	if (result) {
		ptr.release();
		return LogPtr<To>(result);
	}
	return nullptr;
}

template <typename To, typename From>
LogPtr<To> StaticCast_Unique(LogPtr<From>&& ptr) {
	if (!ptr) return nullptr;
	To* result = static_cast<To*>(ptr.get());
	if (result) {
		ptr.release();
		return LogPtr<To>(result);
	}
	return nullptr;
}