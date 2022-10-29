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

		void Init(
			std::string const& pModelPath,
			std::string const& pShaderPath);
		void LoadModel(std::string const& pPath);
		void ProcessNode(
			aiNode* const& pNode,
			const aiScene* const& pScene) noexcept;
		std::unique_ptr<Mesh> ProcessMesh(
			aiMesh* const& pMesh,
			const aiScene* const& pScene) noexcept;
		std::vector<Texture*> LoadMaterialTextures(
			aiMaterial* const& pMat,
			aiTextureType const& pType,
			TexType const& pTexType) noexcept;
		void LoadTexturesToShader() const noexcept;

	public:
		Model(
			std::string const& pModelPath,
			std::string const& pShaderPath,
			Camera* const& pCamera = nullptr,
			bool const& pLoadTextures = true);
		~Model();

		void Draw(const Camera* const& pCamera = nullptr) const noexcept;

		constexpr void SetCameraRef(Camera* const& pCamera) noexcept { m_cameraRef = pCamera; }
		constexpr void SetShaderRef(Shader* const& pShader) noexcept { m_shader = pShader; }

		constexpr Shader* const& GetShaderRef() const noexcept { return m_shader; }
		Mesh* GetMeshAt(uint16_t const& pPos) const noexcept;
	};
}
