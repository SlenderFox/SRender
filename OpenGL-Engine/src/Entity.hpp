#pragma once
#include "Colour.hpp"

namespace Engine
{
	class Entity: public Transform
	{
	public:
		static Entity* LoadModel(Model* pModelOut, std::string pModelPath,
					   Shader* pShaderOut, std::string pShaderPath,
					   bool pLoadTextures = true);

	private:
		Entity* m_parent = nullptr;
		std::vector<Entity*> m_children;

		Model* m_model = nullptr;
		Shader* m_shader = nullptr;	// TODO: Remove this

		void UpdateModel() const;

	public:
		Entity();
		Entity(Entity* pParent);
		~Entity();

		void AddChild(Entity* pChild);
		void RemoveChild(Entity* pChild);

		#pragma region Setters
		void SetParent(Entity* pParent);
		void SetTransform(glm::mat4 pValue) override;
		void Translate(glm::vec3 pValue) override;
		void SingleColour(bool pState);
		void SetScale(glm::vec3 pValue);
		void JustColour(Colour pCol);
		#pragma endregion

		Entity& GetParent() const { return *m_parent; }
		virtual std::vector<Entity*> GetChildren() const { return m_children; }
	};

	/// @brief Root is a special, static entity that only has children
	class Root: public Entity
	{
	public:
		static Root* GetRoot()
		{
			static Root* s_root = new Root();
			return s_root;
		}

		~Root() {}
		
	private:
		std::vector<Entity*> m_children = std::vector<Entity*>();

		#pragma region Constructors
		Root() = default;
		// Delete copy/move so extra instances can't be created/moved.
		Root(const Root&) = delete;
		Root& operator=(const Root&) = delete;
		Root(Root&&) = delete;
		Root& operator=(Root&&) = delete;
		#pragma endregion

	public:
		std::vector<Entity*> GetChildren() const override { return m_children; }
	};
}
