#pragma once
#include <unordered_map>
#include <string>   
#include <memory>

class AnimationResource
{
public:

private:
    //std::unordered_map<std::wstring, std::unique_ptr<SceneLoader>> _loaderMap;
    //std::unordered_map<std::wstring, std::unique_ptr<Shader>> _shaderMap;
    std::unordered_map<std::wstring, std::unique_ptr<Mesh>> _meshMap;
    std::unordered_map<std::wstring, std::unique_ptr<Model>> _modelMap;
    std::unordered_map<std::wstring, std::unique_ptr<Texture>> _textureMap;
    std::unordered_map<std::wstring, std::unique_ptr<Animation>> _animationMap;

};

