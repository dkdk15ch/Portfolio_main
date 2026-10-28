#pragma once
#include <utility>
#include <cstring>
#include <new>
#include <cstdint>

#ifdef _DEBUG
#include <source_location>
#include <iostream>
#include <format>
#include "FileUtil.h"
#include "StringHelper.h"
#endif // _DEBUG


namespace Delegate {

	template <typename Signature, std::size_t BufferSize = 16>
	class Singlecast;

	template <typename returnT, typename... Args, std::size_t BufferSize>
	class Singlecast<returnT(Args...), BufferSize>
	{
	public:
		Singlecast() = default;
		~Singlecast() { unBind(); }

#ifdef _DEBUG
	private:
		std::string fileName;
		std::string line;
	public:

		template<typename F>
		void bind(F&& f, std::source_location loc = std::source_location::current()) {
			fileName = RemoveDirectoryPath(loc.file_name());
			line = std::format("Line = {}", loc.line());
			std::cout << std::format("Bind: {}/{}\n", fileName, line);

			unBind();
			callable = new (buffer) Callable<F>(std::forward<F>(f));
		}

		returnT execute(Args... args) {
			std::cout << std::format("Execute: {}/{}", fileName, line);
			if (!callable) {
				std::cout << " - Callable is null\n";

				if constexpr (!std::is_void_v<returnT>) {
					return returnT();
				}
				else {
					return;
				}
			}
			std::cout << std::endl;
			return (*callable)(std::forward<Args>(args)...);
		}
#else
		template<typename F>
		void bind(F&& f) {
			unBind();
			callable = new (buffer) Callable<F>(std::forward<F>(f));
		}

		returnT execute(Args... args) {
			if (!callable) {
				if constexpr (!std::is_void_v<returnT>) {
					return returnT();
				}
				else {
					return;
				}
			}
			return (*callable)(std::forward<Args>(args)...);
		}

#endif // _DEBUG

		void unBind() {
			if (!callable) { return; }
			callable->~ICallable();
			callable = nullptr;
			std::memset(buffer, 0, BufferSize + sizeof(void*));
		}


	private:
		struct ICallable {
			virtual ~ICallable() = default;
			virtual returnT operator()(Args...) const = 0;
		};

		template <typename T>
		struct Callable : public ICallable
		{
			T functor;

			template<typename F>
			Callable(F&& f) : functor(std::forward<F>(f)) {}

			returnT operator()(Args... args) const override {
				return functor(std::forward<Args>(args)...);
			}
		};

		ICallable* callable = nullptr;
		uint8_t buffer[BufferSize + sizeof(void*)] = { 0 };

	};


	//template <typename Signature>
	//class Multicast;

	//template <typename... Args>
	//class Multicast<void(Args...)>
	//{
	//public:
	//	Multicast() = default;
	//	~Multicast() { Clear(); }

	//	template<typename F>
	//	void Add(F&& f) {
	//		callables.emplace_back(std::make_unique<Callable<F>>(std::forward<F>(f)));
	//	}
	//	//	マルチキャストは戻り値なし
	//	void Broadcast(Args... args) const {
	//		for (const auto& c : callables) {
	//			if (c) {
	//				(*c)(args...);
	//			}
	//		}
	//	}

	//	void Clear() {
	//		callables.clear();
	//	}

	//private:
	//	struct ICallable {
	//		virtual ~ICallable() = default;
	//		virtual void operator()(Args...) const = 0;
	//	};

	//	template <typename T>
	//	struct Callable : public ICallable {
	//		T functor;
	//		template<typename F>
	//		Callable(F&& f) : functor(std::forward<F>(f)) {}

	//		void operator()(Args... args) const override {
	//			functor(std::forward<Args>(args)...);
	//		}
	//	};

	//	std::vector<std::unique_ptr<ICallable>> callables;
	//};

};// namespace Delegate
