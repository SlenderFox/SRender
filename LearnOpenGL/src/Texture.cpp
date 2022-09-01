#pragma region
#include "Texture.hpp"
#include "glad/glad.h" // Include glad to get all the required OpenGL headers
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#ifdef _DEBUG
 #include <iostream>
 using std::cout;
 using std::endl;
#endif
#pragma endregion

namespace Engine
{
	// Static
	unsigned int Texture::s_textureIds[32];
	unsigned int Texture::s_textureCount;

	void Texture::UnloadAll(bool pValidate)
	{
		if (pValidate)
		{
			glDeleteTextures(s_textureCount, s_textureIds);
		}
	}

	uint8_t Texture::LoadTextureFromFile(const char* pPath)
	{
		#ifdef _DEBUG
		 cout << "Loading texture " << s_textureCount << ": \"" << pPath << "\"";
		#endif

		if (s_textureCount > 31)
		{
			#ifdef _DEBUG
			 cout << "\nFailed to load texture: Exceeded max texture id " << s_textureCount << endl;
			#endif
			return UINT8_MAX;
		}

		// Makes sure the images are oriented correctly when loaded
		stbi_set_flip_vertically_on_load(true);

		int texWidth = 0, texHeight = 0, numComponents = 0;
		unsigned char* imageData = stbi_load(pPath, &texWidth, &texHeight, &numComponents, 0);
		if (imageData)
		{
			float borderColour[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			// Generates a texture object in vram
			glActiveTexture(GL_TEXTURE0 + s_textureCount);
			glGenTextures(1, &s_textureIds[s_textureCount]);
			// Remember this works like a pointer to the object using the ID
			glBindTexture(GL_TEXTURE_2D, s_textureIds[s_textureCount]);
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
				default: 
				#ifdef _DEBUG
				 cout << "\nFailed to load texture: Too many components" << endl;
				#endif
				return UINT8_MAX;
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

			#ifdef _DEBUG
			 cout << "...Success!" << endl;
			#endif

			// Assigns the id then increments the total number of textures loaded
			return s_textureIds[s_textureCount++];
		}
		#ifdef _DEBUG
		 else
		 {
		 	cout << "\nFailed to load texture: No file found" << endl;
			return UINT8_MAX;
		 }
		#endif
	}

	// Member
	Texture::Texture(string pPath, TexType pType = TexType::diffuse) : m_file(pPath), m_type(pType)
	{
		m_id = LoadTextureFromFile(pPath.c_str());
	}

	Texture::Texture(string pDirectory, string pPath, TexType pType = TexType::diffuse) : m_file(pPath), m_type(pType)
	{
		m_id = LoadTextureFromFile((pDirectory + '/' + pPath).c_str());
	}

	void Texture::Destroy()
	{
		// This is currently bad as it leaves a gap in the loaded textures
		glDeleteTextures(1, &s_textureIds[m_id]);
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
