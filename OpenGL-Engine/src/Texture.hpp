#pragma region 
#pragma once
#include <string>
#pragma endregion

namespace Engine
{
	enum class TexType: uint8_t
	{
		diffuse,
		specular
	};

	class Texture
	{
		//friend class Model;
	private:
		static uint32_t s_textureIds[32];	// List of all texture ids
		static uint16_t s_textureCount;		// How many textures have been loaded

	public:
		static int32_t LoadTextureFromFile(const char* pPath);
		static void UnloadAll(bool pValidate);
		static uint16_t GetNumTex() { return s_textureCount; }

	private:
		uint16_t m_id = 0;
		TexType m_type = TexType::diffuse;
		std::string m_file;

	public:
		Texture(std::string pPath, TexType pType = TexType::diffuse);
		void Destroy();

		uint16_t GetId() const { return m_id; }
		TexType GetType() const { return m_type; }
		std::string GetFile() const { return m_file; }
	};
}
