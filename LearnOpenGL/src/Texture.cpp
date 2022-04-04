#pragma region
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "Texture.hpp"
#ifdef _DEBUG
 #include <iostream>
 using std::cout;
 using std::endl;
#endif
#pragma endregion

namespace Engine
{
	unsigned int Texture::s_idTex[32];
	unsigned int Texture::s_numTex;

	Texture::Texture(const char* pPath, TexType pType) : m_type(pType)
	{
		m_id = LoadTexture(pPath);
	}

	void Texture::Destroy()
	{

	}

	// Static
	void Texture::UnloadAll(bool pValidate)
	{
		if (pValidate)
		{
			for (unsigned int i = 0; i < s_numTex; ++i)
			{
				glDeleteTextures(1, &s_idTex[i]);
			}
		}
	}

	uint8_t Texture::LoadTexture(const char* pPath)
	{
		#ifdef _DEBUG
		 if (s_numTex > 31)
		 {
			cout << "Texture ID has exceeded max possible (32): " << s_numTex << "\nfor file path: " << pPath << endl;
		 	return UINT8_MAX;
		 }
		#endif

		// Makes sure the images are oriented correctly when loaded
		stbi_set_flip_vertically_on_load(true);

		int texWidth = 0, texHeight = 0, numComponents = 0;
		unsigned char* imageData = stbi_load(pPath, &texWidth, &texHeight, &numComponents, 0);
		if (imageData)
		{
			float borderColour[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			// Generates a texture object in vram
			glActiveTexture(GL_TEXTURE0 + s_numTex);
			glGenTextures(1, &s_idTex[s_numTex]);
			// Remember this works like a pointer to the object using the ID
			glBindTexture(GL_TEXTURE_2D, s_idTex[s_numTex]);
			// Sets some parameters to the currently bound texture object
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColour);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			GLenum format;
			switch (numComponents)
			{
				case 1: format = GL_RED; break;
				case 3: format = GL_RGB; break;
				case 4: format = GL_RGBA; break;
				default: cout << "Failed to load texture " << pPath << endl; return UINT8_MAX;
			}

			/*Applies the image to the texture object and creates the mipmaps
			* p1: What object we are applying to
			* p2: Specifies which mipmap level we are applying to (0 for base)
			* p3: What format to store the texture as
			* p4/5: The width and height of the texture
			* p6: Border (legacy stuff, leave as 0)
			* p7: What format the image is
			* p8: The datatype being passed in (in this case a char)
			* p9: The image data being passed in
			*/
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, imageData);
			glGenerateMipmap(GL_TEXTURE_2D);

			// Frees the image memory
			stbi_image_free(imageData);

			// Assigns the id then increments the total number of textures loaded
			return s_idTex[s_numTex++];
		}
		#ifdef _DEBUG
		 else
		 {
		 	cout << "Failed to load texture " << pPath << endl;
			return UINT8_MAX;
		 }
		#endif
	}

	// Static
	unsigned int Texture::GetNumTex()
	{
		return s_numTex;
	}

	unsigned int Texture::GetId() const
	{
		return m_id;
	}

	string Texture::GetType() const
	{
		switch (m_type)
		{
			case TexType::diffuse: return string("texture_diffuse");
			case TexType::specular: return string("texture_specular");
			default: return string("ERROR");
		}
	}
}
