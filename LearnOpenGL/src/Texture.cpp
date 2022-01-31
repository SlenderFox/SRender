#include "Texture.hpp"
#include <glad/glad.h> // Include glad to get all the required OpenGL headers
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#ifdef _DEBUG
 #include <iostream>
 using std::cout;
 using std::endl;
#endif

namespace Engine
{
	void Texture::LoadImages()
	{
		// Generates two texture objects
		glGenTextures(1, &m_idTEX0);
		glGenTextures(1, &m_idTEX1);
		glGenTextures(1, &m_idTEX2);

		// Makes sure the images are oriented correctly when loaded
		stbi_set_flip_vertically_on_load(true);

		glActiveTexture(GL_TEXTURE0);
		LoadTexture(&m_idTEX0, "../Assets/textures/greybox.png", false);
		glActiveTexture(GL_TEXTURE1);
		LoadTexture(&m_idTEX1, "../Assets/textures/container.jpg", false);
		glActiveTexture(GL_TEXTURE2);
		LoadTexture(&m_idTEX2, "../Assets/textures/awesomeface.png", true);

//        glActiveTexture(GL_TEXTURE0);
//        // Rmember this works like a pointer to the object using the ID
//        glBindTexture(GL_TEXTURE_2D, m_idTEX0);
//        // Sets some parameters to the currently bound texture object
//        float borderColour[] = { 0.0f, 0.0f, 0.0f, 0.0f };
//        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColour);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//        // Loads the first texture into memory
//        m_imageData = stbi_load("../Assets/textures/container.jpg", &m_texWidth, &m_texHeight, &m_texColChannels, 0);
//        if (m_imageData)
//        {
//            /*Applies the image to the texture object and creates the mipmaps
//            * p1: What object we are applying to
//            * p2: Specifies which mipmap level we are applying to (0 for base)
//            * p3: What format to store the texture as
//            * p4/5: The width and height of the texture
//            * p6: Border (legacy stuff, leave as 0)
//            * p7: What format the image is
//            * p8: The datatype being passed in (in this case a char)
//            * p9: The actual image being passed in
//            */
//            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texWidth, m_texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, m_imageData);
//            glGenerateMipmap(GL_TEXTURE_2D);
//        }
//#ifdef _DEBUG
//        else
//        {
//            cout << "Failed to load texture0" << endl;
//        }
//#endif
//
//        // Frees the image memory
//        stbi_image_free(m_imageData);
//
//        glActiveTexture(GL_TEXTURE1);
//        // Binds the second texture
//        glBindTexture(GL_TEXTURE_2D, m_idTEX1);
//        // Sets some parameters to the currently bound texture object
//        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColour);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//        // Loads the seconds texture into memory
//        m_imageData = stbi_load("../Assets/textures/awesomeface.png", &m_texWidth, &m_texHeight, &m_texColChannels, 0);
//        if (m_imageData)
//        {
//            // Applies the image to the texture object and creates the mipmaps
//            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texWidth, m_texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_imageData);
//            glGenerateMipmap(GL_TEXTURE_2D);
//        }
//#ifdef _DEBUG
//        else
//        {
//            cout << "Failed to load texture1" << endl;
//        }
//#endif
//        // Frees the image memory
//        stbi_image_free(m_imageData);
//
//        //glActiveTexture(GL_TEXTURE0);
//        //glBindTexture(GL_TEXTURE_2D, m_idTEX0);
//        //glActiveTexture(GL_TEXTURE1);
//        //glBindTexture(GL_TEXTURE_2D, m_idTEX1);
	}

	void Texture::LoadTexture(unsigned int* texID, const char* location, bool transparent)
	{
		float borderColour[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		// Rmember this works like a pointer to the object using the ID
		glBindTexture(GL_TEXTURE_2D, *texID);
		// Sets some parameters to the currently bound texture object
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColour);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Loads the first texture into memory
		m_imageData = stbi_load(location, &m_texWidth, &m_texHeight, &m_texColChannels, 0);
		if (m_imageData)
		{
			/*Applies the image to the texture object and creates the mipmaps
			* p1: What object we are applying to
			* p2: Specifies which mipmap level we are applying to (0 for base)
			* p3: What format to store the texture as
			* p4/5: The width and height of the texture
			* p6: Border (legacy stuff, leave as 0)
			* p7: What format the image is
			* p8: The datatype being passed in (in this case a char)
			* p9: The actual image being passed in
			*/
			if (transparent)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texWidth, m_texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_imageData);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texWidth, m_texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, m_imageData);

			glGenerateMipmap(GL_TEXTURE_2D);
		}
#ifdef _DEBUG
		else
		{
			cout << "Failed to load texture " << texID << endl;
		}
#endif

		// Frees the image memory
		stbi_image_free(m_imageData);
	}
}
