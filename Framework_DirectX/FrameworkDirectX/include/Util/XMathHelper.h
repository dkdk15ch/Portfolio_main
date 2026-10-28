#pragma once
#include <DirectXMath.h>
#include <SimpleMath.h>

namespace XMathHelper {
    constexpr float EPSILON = 1e-6f;

    inline bool IsNearlyEqual(const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2, float e = EPSILON)
    {
        return DirectX::XMVector3NearEqual(v1, v2, DirectX::XMVectorReplicate(e));
    }

    inline bool IsNearlyEqual(const DirectX::SimpleMath::Quaternion& q1, const DirectX::SimpleMath::Quaternion& q2, float e = EPSILON)
    {
        return DirectX::XMVector4NearEqual(q1, q2, DirectX::XMVectorReplicate(e));
    }

    inline bool IsNearlyZero(const DirectX::SimpleMath::Vector3& v, float e = EPSILON) {
        return std::abs(v.LengthSquared()) < e;
    }

    inline bool IsNearlyZero(float v, float e = EPSILON) {
        return std::abs(v) < e;
    }

    struct EulerTRS {
        EulerTRS() 
            : translation(DirectX::SimpleMath::Vector3(0.f,0.f,0.f))
            , rot(DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f))
            , scale(DirectX::SimpleMath::Vector3(1.f, 1.f, 1.f))
        {}
        
        DirectX::SimpleMath::Vector3 translation;
        DirectX::SimpleMath::Vector3 rot; 
        DirectX::SimpleMath::Vector3 scale; 
    };

}