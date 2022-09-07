#pragma region
#include "Light.hpp"
#include <math.h>
using glm::cos;
using glm::radians;
#pragma endregion

namespace Engine
{
	#pragma region Constructors
	Light::Light(LightType pType) : m_type(pType)
	{
		m_angle = cos(radians(20.0f));
	}

	Light::Light(LightType pType, vec3 pColour) : m_type(pType), m_lightColour(pColour)
	{
		m_angle = cos(radians(20.0f));
	}

	Light::Light(LightType pType, mat4 pTransform) : m_type(pType)
	{
		SetTransform(pTransform);
		m_angle = cos(radians(20.0f));
	}

	Light::Light(LightType pType, mat4 pTransform, vec3 pColour) : m_type(pType), m_lightColour(pColour)
	{
		SetTransform(pTransform);
		m_angle = cos(radians(20.0f));
	}

	//// Directional
	//Light::Light(LightType pType, vec3 pDirection)
	// : m_type(pType), m_angle(17.0f), m_blur(0.2f)
	//{
	//	//m_transform = mat4(1);
	//	SetForward(pDirection);
	//	m_lightColour = vec3(1);
	//	m_angle = glm::cos(glm::radians(20.0f));
	//}
	//
	//Light::Light(LightType pType, vec3 pDirection, vec3 pColour)
	// : m_type(pType), m_lightColour(pColour), m_angle(17.0f), m_blur(0.2f)
	//{
	//	//m_transform = mat4(1);
	//	SetForward(pDirection);
	//	m_angle = glm::cos(glm::radians(20.0f));
	//}
	//
	//// Point
	//Light::Light(LightType pType, vec4 pPosition)
	// : m_type(pType), m_angle(17.0f), m_blur(0.2f)
	//{
	//	//m_transform = mat4(1);
	//	SetPosition(pPosition);
	//	m_lightColour = vec3(1);
	//	m_angle = glm::cos(glm::radians(20.0f));
	//}
	//
	//Light::Light(LightType pType, vec4 pPosition, vec3 pColour)
	// : m_type(pType), m_lightColour(pColour), m_angle(17.0f), m_blur(0.2f)
	//{
	//	//m_transform = mat4(1);
	//	SetPosition(pPosition);
	//	m_angle = glm::cos(glm::radians(20.0f));
	//}
	//
	//// Spot
	//Light::Light(LightType pType, mat4 pTransform, float pAngle, float pBlur)
	// : m_type(pType), m_angle(pAngle), m_blur(pBlur)
	//{
	//	SetTransform(pTransform);
	//	m_lightColour = vec3(1);
	//}
	//
	//Light::Light(LightType pType, mat4 pTransform, vec3 pColour, float pAngle, float pBlur)
	// : m_type(pType), m_lightColour(pColour), m_angle(pAngle), m_blur(pBlur)
	//{
	//	SetTransform(pTransform);
	//}
	//
	//Light::Light(LightType pType, vec4 pPosition, vec3 pDirection, vec3 pColour, float pAngle, float pBlur)
	// : m_type(pType), m_lightColour(pColour), m_angle(pAngle), m_blur(pBlur)
	//{
	//	//m_transform = mat4(1);
	//	SetPosition(pPosition);
	//	SetForward(pDirection);
	//}
	#pragma endregion
	#pragma region Setters
	Light* Light::SetType(LightType pType)
	{
		m_type = pType;
		return this;
	}

	Light* Light::SetCamPosition(vec3 pPosition)
	{
		SetPosition(pPosition);
		return this;
	}

	Light* Light::SetDirection(vec3 pDirection)
	{
		SetForward(pDirection);
		return this;
	}

	Light* Light::SetColour(vec3 pColour)
	{
		m_lightColour = pColour;
		return this;
	}

	Light* Light::SetLinear(float pValue)
	{
		m_linear = pValue;
		return this;
	}

	Light* Light::SetQuadratic(float pValue)
	{
		m_quadratic = pValue;
		return this;
	}

	Light* Light::SetAngle(float pValue)
	{
		m_angle = pValue;
		return this;
	}

	Light* Light::SetBlur(float pValue)
	{
		m_blur = pValue;
		return this;
	}
	#pragma endregion
	#pragma region Getters
	vec4 Light::GetDirection() const
	{
		return vec4(GetForward(), 0);
	}
	
	float Light::GetAngle()
	{
		return std::cosf(glm::radians(m_angle));
	}
	
	float Light::GetBlur()
	{
		return std::sinf(glm::radians(90 * m_blur));
	}
	#pragma endregion
}
