#pragma region
#include "Renderer.hpp"
#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Debug.hpp"

using glm::vec3;
using glm::mat3;
using glm::mat4;
using std::string;
using std::vector;
using std::make_unique;
using std::unique_ptr;
#pragma endregion

namespace Engine
{
#	pragma region Variables
	Camera* Renderer::s_camera = nullptr;
	unique_ptr<vector<unique_ptr<Model>>> Renderer::s_models = nullptr;
	unique_ptr<vector<unique_ptr<Light>>> Renderer::s_lights = nullptr;
	const float Renderer::s_ambience = 0.15f;
#	pragma endregion

	bool Renderer::Init(float const& pAspect) noexcept
	{
		// Enables the use of the depth buffer
		glEnable(GL_DEPTH_TEST);

		//SetClearColour(Colour::CreateWithRGB(vec3(0.1f)));

		// Initialise camera
		s_camera = new Camera(pAspect, 75.0f);
		s_camera->SetPosition({ 0.0f, 0.0f, 6.0f });

		// Initialise arrays
		s_models = make_unique<vector<unique_ptr<Model>>>();
		s_lights = make_unique<vector<unique_ptr<Light>>>();

		return true;
	}

	void Renderer::Destroy() noexcept
	{
		if (s_models)
		{
			s_models.get()->clear();
			// Smart pointer needs to be manually released or it throws an error :|
			s_models.release();

			// Destroy all textures
			for (size_t i = 0; i < Model::s_loadedTextures.size(); ++i)
			{
				if (Model::s_loadedTextures[i])
					delete Model::s_loadedTextures[i];
			}
			// Unload all textures from memory once finished
			Texture::UnloadAll();
		}

		delete s_camera;
	}

	void Renderer::Draw() noexcept
	{
		// Clears to background colour
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (s_models.get()->size() > 0)
		{
			for (uint8_t i = 0; i < s_models.get()->size(); ++i)
				GetModelAt(i)->Draw();
		}
	}

	void Renderer::LoadLightsIntoShader(Shader const& pShader) noexcept
	{
		pShader.SetFloat("u_material.shininess", 32.0f);
		uint8_t numDirLights = 0;
		uint8_t numPointLights = 0;
		uint8_t numSpotLights = 0;

		for (uint8_t i = 0; i < s_lights.get()->size(); ++i)
		{
			Light* currentLight = GetLightAt(i);
			switch (currentLight->GetType())
			{
			case LightType::Directional:
				pShader.SetVec3(
					"u_dirLights[" + std::to_string(numDirLights) + "].colour.ambient",
					(vec3)currentLight->GetColour() * s_ambience);
				pShader.SetVec3(
					"u_dirLights[" + std::to_string(numDirLights) + "].colour.diffuse",
					currentLight->GetColour());
				pShader.SetVec3(
					"u_dirLights[" + std::to_string(numDirLights) + "].colour.specular",
					currentLight->GetColour());
				pShader.SetVec4(
					"u_dirLights[" + std::to_string(numDirLights) + "].direction",
					currentLight->GetDirection());
				++numDirLights; break;
			case LightType::Point:
				pShader.SetVec3(
					"u_pointLights[" + std::to_string(numPointLights) + "].colour.diffuse",
					currentLight->GetColour());
				pShader.SetVec3(
					"u_pointLights[" + std::to_string(numPointLights) + "].colour.specular",
					currentLight->GetColour());
				pShader.SetVec4(
					"u_pointLights[" + std::to_string(numPointLights) + "].position",
					currentLight->GetPosition());
				pShader.SetFloat(
					"u_pointLights[" + std::to_string(numPointLights) + "].linear",
					currentLight->GetLinear());
				pShader.SetFloat(
					"u_pointLights[" + std::to_string(numPointLights) + "].quadratic",
					currentLight->GetQuadratic());
				++numPointLights; break;
			case LightType::Spot:
				pShader.SetVec3(
					"u_spotLights[" + std::to_string(numSpotLights) + "].colour.diffuse",
					currentLight->GetColour());
				pShader.SetVec3(
					"u_spotLights[" + std::to_string(numSpotLights) + "].colour.specular",
					currentLight->GetColour());
				pShader.SetVec4(
					"u_spotLights[" + std::to_string(numSpotLights) + "].position",
					currentLight->GetPosition());
				pShader.SetVec4(
					"u_spotLights[" + std::to_string(numSpotLights) + "].direction",
					currentLight->GetDirection());
				pShader.SetFloat(
					"u_spotLights[" + std::to_string(numSpotLights) + "].linear",
					currentLight->GetLinear());
				pShader.SetFloat(
					"u_spotLights[" + std::to_string(numSpotLights) + "].quadratic",
					currentLight->GetQuadratic());
				pShader.SetFloat(
					"u_spotLights[" + std::to_string(numSpotLights) + "].cutoff",
					currentLight->GetAngle());
				pShader.SetFloat(
					"u_spotLights[" + std::to_string(numSpotLights) + "].blur",
					currentLight->GetBlur());
				++numSpotLights; break;
			default:
				Debug::Send("Incorrect light type"); return;
			}
		}
	}

	void Renderer::ModifyAllSpotlightAngles(float const& pValue) noexcept
	{
		for (uint8_t i = 0; i < s_lights.get()->size(); ++i)
		{
			if (GetLightAt(i)->GetType() == LightType::Spot)
			{
				Light* currentlLight = GetLightAt(i);
				float newValue = currentlLight->GetAngleRaw() + pValue;
				if (newValue <= 90.0f && newValue >= 0.0f)
				{
					currentlLight->SetAngle(newValue);
					for (uint8_t i = 0; i < s_models.get()->size(); ++i)
						GetModelAt(i)->m_shader->SetFloat("u_spotLights[0].cutoff", currentlLight->GetAngle());
				}
			}
		}
	}

	void Renderer::ModifyAllSpotlightBlurs(float const& pValue) noexcept
	{
		for (uint8_t i = 0; i < s_lights.get()->size(); ++i)
		{
			if (GetLightAt(i)->GetType() == LightType::Spot)
			{
				Light* currentlLight = GetLightAt(i);
				float newValue = currentlLight->GetBlurRaw() + pValue;
				if (newValue <= 1.0f && newValue > 0.0f)
				{
					currentlLight->SetBlur(newValue);
					for (uint8_t i = 0; i < s_models.get()->size(); ++i)
						GetModelAt(i)->m_shader->SetFloat("u_spotLights[0].blur", currentlLight->GetBlur());
				}
			}
		}
	}

	Model* Renderer::AddNewModel(
		uint8_t& id,
		string const& pModelPath,
		string const& pShaderPath,
		bool const& pLoadTextures) noexcept
	{
		// Caps at 255
		size_t currentAmount = s_models.get()->size();
		if (currentAmount > 255)
			return nullptr;

		id = (uint8_t)currentAmount;
		// Doesn't like paths being const refs
		s_models.get()->push_back(make_unique<Model>(pModelPath, pShaderPath, s_camera, pLoadTextures));
		return GetModelAt(id);
	}

	Light* Renderer::AddNewLight(
		uint8_t &id,
		LightType const& pType,
		Colour const& pColour) noexcept
	{
		// Caps at 255
		size_t currentAmount = s_lights.get()->size();
		if (currentAmount > 255)
			return nullptr;

		id = (uint8_t)currentAmount;
		s_lights.get()->push_back(make_unique<Light>(pType, pColour));
		return GetLightAt(id);
	}

	void Renderer::SetClearColour(Colour const& pColour) noexcept
	{
		vec3 col = pColour.RGBvec3();
		glClearColor(col.r, col.g, col.b, 1.0f);
	}

	void Renderer::SetRenderMode(Mode const& pMode) noexcept
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT + (int)pMode);
	}

	uint8_t Renderer::ModelCount() noexcept
	{
		return (uint8_t)s_models.get()->size();
	}

	uint8_t Renderer::LightCount() noexcept
	{
		return (uint8_t)s_lights.get()->size();
	}

	Model* Renderer::GetModelAt(uint8_t const& pPos) noexcept
	{
		if (!s_models.get())
			return nullptr;

		if (pPos > s_models.get()->size() - 1)
		{
			Debug::Send("Attempting to access model outside array size");
			return nullptr;
		}

		return (*s_models.get())[pPos].get();
	}

	Light* Renderer::GetLightAt(uint8_t const& pPos) noexcept
	{
		if (!s_lights.get())
			return nullptr;

		if (pPos > s_lights.get()->size() - 1)
		{
			Debug::Send("Attempting to access light outside array size");
			return nullptr;
		}

		return (*s_lights.get())[pPos].get();
	}
}
