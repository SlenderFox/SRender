#include "model.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "glm/gtc/matrix_transform.hpp"
#include "assert.h"
#include "debug.hpp"
#include "exception.hpp"
#include "graphics.hpp"

using glm::vec2;
using glm::vec3;
using std::string;
using std::vector;

namespace srender
{
// Forward declaration
class application { public: _NODISCARD static std::string getAppLocation() noexcept; };

void makePathAbsolute(string *_path)
{
	if (*_path == "")
	{
		_path = nullptr;
		return;
	}

	*_path = application::getAppLocation() + *_path;
}

void model::processNode(
	const aiNode *_node,
	const aiScene *_scene,
	const string _directory,
	const bool _loadTextures
)
{
	// Process all the node's meshes (if any)
	for (uint32_t i = 0; i < _node->mNumMeshes; ++i)
	{
		aiMesh *mesh = _scene->mMeshes[_node->mMeshes[i]];
		m_meshes.push_back(processMesh(mesh, _scene, _directory, _loadTextures));
	}

	// Then do the same for each of it's children
	for (uint32_t i = 0; i < _node->mNumChildren; ++i)
	{	processNode(_node->mChildren[i], _scene, _directory, _loadTextures); }
}

mesh *model::processMesh(
	const aiMesh *_mesh,
	const aiScene *_scene,
	const string _directory,
	const bool _loadTextures
)
{
	// Process vertex positions, normals, and texture coordinates
	vector<mesh::vertex> *vertices = new vector<mesh::vertex>();
	for (uint32_t i = 0; i < _mesh->mNumVertices; ++i)
	{
		mesh::vertex vertex;
		vec3 vector;
		// Position
		vector.x = _mesh->mVertices[i].x;
		vector.y = _mesh->mVertices[i].y;
		vector.z = _mesh->mVertices[i].z;
		vertex.position = vector;
		// Normal
		if (_mesh->HasNormals())
		{
			vector.x = _mesh->mNormals[i].x;
			vector.y = _mesh->mNormals[i].y;
			vector.z = _mesh->mNormals[i].z;
			vertex.normal = vector;
		}
		// TexCoords
		if (_mesh->mTextureCoords[0])	// Does the mesh have texture coords
		{
			vertex.texCoords.x = _mesh->mTextureCoords[0][i].x;
			vertex.texCoords.y = _mesh->mTextureCoords[0][i].y;
		}

		vertices->push_back(vertex);
	}

	// Process indices
	vector<uint32_t> *indices = new vector<uint32_t>();
	for (uint32_t i = 0; i < _mesh->mNumFaces; ++i)
	{
		aiFace face = _mesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; ++j)
		{	indices->push_back(face.mIndices[j]); }
	}

	// Process material
	if (_loadTextures && _mesh->mMaterialIndex >= 0U)
	{
		aiMaterial *material = _scene->mMaterials[_mesh->mMaterialIndex];
		vector<texture*> diffuseMaps = loadMaterialTextures(
			material,
			texture::type::diffuse,
			_directory,
			_loadTextures
		);
		m_textures.insert(m_textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		vector<texture*> specularMaps = loadMaterialTextures(
			material,
			texture::type::specular,
			_directory,
			_loadTextures
		);
		m_textures.insert(m_textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return new mesh(vertices, indices);
}

vector<texture*> model::loadMaterialTextures(
	const aiMaterial *_material,
	const texture::type _texType,
	const string _directory,
	const bool _loadTextures
) const
{
	// Textures from this specific node being output
	vector<texture*> texturesOut;

	aiTextureType aitextype = aiTextureType_DIFFUSE;
	switch (_texType)
	{
	case texture::type::diffuse:
		aitextype = aiTextureType_DIFFUSE; break;
	case texture::type::specular:
		aitextype = aiTextureType_SPECULAR; break;
	default:
		assert(false && "Unknown texture type specified");
	};

	for (uint32_t i = 0; i < _material->GetTextureCount(aitextype); ++i)
	{
		aiString file;
		_material->GetTexture(aitextype, i, &file);
		string path = _directory + '/' + file.C_Str();

		// First we check if the texture has already been loaded into memory
		bool loadNewTexture = true;
		for (uint8_t j = 0; j < texture::size(); ++j)
		{
			// Skip if not the same texture or not loaded
			if (!texture::at(j)->getLoaded() || *texture::at(j) != path)
			{	continue; }

			// Texture has already been loaded into memory
			loadNewTexture = false;

			// Then check if it has been loaded into this model
			bool reuseExistingTexture = true;
			for (size_t k = 0; k < m_textures.size(); ++k)
			{
				if (*m_textures[k] == path)
				{
					reuseExistingTexture = false;
					break;
				}
			}

			if (reuseExistingTexture)
			{
				#ifdef _VERBOSE
					debug::send(
						"Reusing texture "
						+ std::to_string(texture::at(j)->getLocation())
						+ ": "
						+ texture::at(j)->getFilePath().c_str(),
						debug::type::note,
						debug::impact::small,
						debug::stage::mid
					);
				#endif
				texturesOut.push_back(texture::at(j));
			}

			break;
		}

		// If texture has not been loaded before, load it for the first time
		if (loadNewTexture)
		{
			try
			{
				texture *tex = texture::loadNew(path, _texType);
				texturesOut.push_back(tex);
			}
			catch (textureException &e)
			{
				string reason = e.what();
				debug::send("Error loading texture at \"" + path +  "\" with reason: \"" + reason + "\"");
			}
		}
	}
	return texturesOut;
}

void model::loadTexturesToShader() const
{
	bool loaded = false;
	uint8_t diffuseNr = 0;
	uint8_t specularNr = 0;
	for (size_t i = 0; i < m_textures.size(); ++i)
	{
		// Retrieve texture number (the N in diffuse_textureN)
		string name;
		string number;
		switch (m_textures[i]->getType())
		{
		case texture::type::diffuse:
			name = "texture_diffuse";
			number = std::to_string(diffuseNr++);
			loaded = true;
			break;
		case texture::type::specular:
			name = "texture_specular";
			number = std::to_string(specularNr++);
			loaded = true;
			break;
		default:
			return;
		}

		string location = "u_material." + name + number;
		string errorMsg;
		if (m_shader->setInt(location.c_str(), (int32_t)m_textures[i]->getLocation(), errorMsg))
		{
			#ifdef _VERBOSE
				string msg = {
					"Setting "
					+ location
					+ " to "
					+ std::to_string(m_textures[i]->getLocation())
				};
				debug::send(
					msg,
					debug::type::note,
					debug::impact::small,
					debug::stage::mid
				);
			#endif
		}
		else
		{
			#ifdef _VERBOSE
				debug::send(
					errorMsg,
					debug::type::note,
					debug::impact::large,
					debug::stage::mid
				);
			#endif
		}
	}

	if (loaded)
	{ useTextures(true); }
}

model::model()
{}

model::model(
	string _modelPath,
	string _shaderPath,
	const bool _loadTextures
)
{
	makePathAbsolute(&_modelPath);
	makePathAbsolute(&_shaderPath);
	loadFromFile(&_modelPath, _loadTextures);
	addShader(new shader(&_shaderPath));
	// Needs to be done after shader is loaded
	if (_loadTextures)
	{	loadTexturesToShader(); }

	#ifdef _VERBOSE
		debug::send("Done!", debug::type::note, debug::impact::small, debug::stage::end);
	#endif
}

model::~model()
{
	for (unsigned int i = 0; i < m_meshes.size(); ++i)
	{	delete m_meshes[i]; }

	delete m_shader;
}

void model::loadFromFile(const string *_path, const bool _loadTextures)
{
	assert(_path && m_meshes.empty() && m_textures.empty());

	#ifdef _VERBOSE
		debug::send(
			"Loading model \"" + *_path + "\"",
			debug::type::process, debug::impact::large, debug::stage::begin
		);

		if (!_loadTextures)
		{
			debug::send(
				"Ignoring textures",
				debug::type::note, debug::impact::small, debug::stage::mid
			);
		}
	#endif

	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(*_path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		#ifdef _VERBOSE
			debug::send(
				"ERROR::ASSIMP::" + string(importer.GetErrorString()),
				debug::type::note, debug::impact::large, debug::stage::mid
			);
		#endif
		return;
	}

	size_t last_slash = (*_path).find_last_of("\\/");
	string directory = (*_path).substr(0, last_slash);
	processNode(scene->mRootNode, scene, directory, _loadTextures);
}

void model::draw() const noexcept
{
	for (uint16_t i = 0; i < m_meshes.size(); ++i)
	{	getMeshAt(i)->draw(); }
}

void model::addShader(shader *_shader)
{
	if (m_shader)
	{	delete m_shader; }
	m_shader = _shader;
	graphics::loadLightsIntoShader(m_shader);
}

void model::clearMeshes()
{
	for (auto mesh : m_meshes)
	{	delete mesh; }
	m_meshes = vector<mesh*>();
}

void model::addMesh(mesh *_mesh)
{	m_meshes.push_back(_mesh); }

void model::setMesh(mesh *_mesh)
{
	clearMeshes();
	addMesh(_mesh);
}

void model::useTextures(const bool _state) const
{
	//> Throw exception if no shader
	string errorMsg;
	m_shader->setBool("u_useTextures", _state, errorMsg);
}

void model::fullbright(const bool _state) const
{
	//> Throw exception if no shader
	string errorMsg;
	m_shader->setBool("u_fullbright", _state, errorMsg);
}

void model::sentTint(const colour _colour) const
{
	//> Throw exception if no shader
	string errorMsg;
	m_shader->setFloat3("u_colour", _colour.rgb(), errorMsg);
}

shader *model::getShaderRef() const noexcept
{
	//> Throw exception if no shader
	return m_shader;
}

mesh *model::getMeshAt(const uint16_t _pos) const  noexcept
{
	if (_pos > m_meshes.size() - 1)
	{
		debug::send("Attempting to access mesh outside array size");
		return nullptr;
	}

	return m_meshes[_pos];
}

texture *model::getTextureAt(const uint16_t _pos) const  noexcept
{
	if (_pos > m_textures.size() - 1)
	{
		debug::send("Attempting to access texture outside array size");
		return nullptr;
	}

	return m_textures[_pos];
}
}
