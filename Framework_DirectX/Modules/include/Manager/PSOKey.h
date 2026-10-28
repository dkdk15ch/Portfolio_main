#pragma once
#include <string>
class GPSOkey {
public:
	inline static std::string LAMBERT = "Lambert";
	inline static std::string SIMPLE = "Simple";
	inline static std::string SIMPLE_ALPHA = "SimpleAlpha";
	inline static std::string MODEL = "Model";
	inline static std::string WIRE = "Wireframe";

private:
	GPSOkey() = default;
	~GPSOkey()  = default;
};

class CPSOkey {
public:
	inline static std::string SEPIA_FILTER = "SepiaFilter";
	inline static std::string LUMINA_FILTER = "HighLuminanceFilter";

private:
	CPSOkey() = default;
	~CPSOkey() = default;
};
