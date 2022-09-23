#pragma region
#pragma once
#include "Mesh.hpp"

struct aiNode;
struct aiMesh;
struct aiScene;
struct aiMaterial;
enum aiTextureType;
#pragma endregion

namespace Engine
{
	class Model
	{
		friend class Renderer;
	private:
		static std::vector<Texture*> s_loadedTextures;	// Memory handled by renderer

		bool m_loadTextures;

		std::unique_ptr<std::vector<std::unique_ptr<Mesh>>> m_meshes = nullptr;
		std::vector<Texture*> m_textures;
		std::string m_directory;

		Shader* m_shader;
		Camera* m_cameraRef;

		void Init(char* pModelPath, char* pShaderPath);
		void LoadModel(std::string pPath);
		void ProcessNode(aiNode* pNode, const aiScene* pScene);
		std::unique_ptr<Mesh> ProcessMesh(aiMesh* pMesh, const aiScene* pScene);
		std::vector<Texture*> LoadMaterialTextures(aiMaterial* pMat, aiTextureType pType, TexType pTexType);
		void LoadTexturesToShader();

	public:
		Model(char* pModelPath, char* pShaderPath, Camera* pCamera = nullptr, bool pLoadTextures = true);
		~Model();

		void Draw(const Camera* const& pCamera = nullptr);

		void SetCameraRef(Camera* pCamera) { m_cameraRef = pCamera; }
		void SetShaderRef(Shader* pShader) { m_shader = pShader; }

		Shader* const& GetShaderRef() const { return m_shader; }
		Mesh* GetMeshAt(uint16_t pPos);
	};
}
