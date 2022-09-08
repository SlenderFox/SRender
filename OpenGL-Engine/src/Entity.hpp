#pragma once
#include "Model.hpp"

namespace Engine
{
	class Entity : public Transform
	{
	// Static
		Entity* s_root;

	// Member
	private:
		Entity* m_parent = nullptr;
		std::unique_ptr<std::vector<Entity*>> m_children = nullptr;

	public:
		Entity();
		Entity(Entity* pParent);

		Entity* GetParent() const;
		std::vector<Entity*>* GetChildren() const;
	};
}
