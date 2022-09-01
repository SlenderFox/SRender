#pragma region 
#pragma once
#include <string>

using std::string;
#pragma endregion

namespace Engine
{
	enum class TexType : uint8_t
	{
		diffuse,
		specular
	};

	class Texture
	{
		//friend class Model;
	// Static
	private:
		static unsigned int s_textureIds[32];	// List of all texture ids
		static unsigned int s_textureCount;		// How many textures have been loaded

	public:
		/**
		 * @brief Loads an image from a file into vram (I'm assuming) as a texture
		 * 
		 * @param pPath The location of the image file
		 * @return uint8_t The ID for the texture (max 32 textures so this will be more than enough)
		 */
		static uint8_t LoadTextureFromFile(const char* pPath);
		static void UnloadAll(bool pValidate);
		static unsigned int GetNumTex() { return s_textureCount; }

	// Member
	private:
		unsigned int m_id = 0;
		TexType m_type = TexType::diffuse;
		string m_file;

	public:
		Texture(string pPath, TexType pType);
		Texture(string pDirectory, string pPath, TexType pType);
		void Destroy();

		unsigned int GetId() const { return m_id; }
		string GetType() const;
		string GetFile() const { return m_file; }
	};
}
