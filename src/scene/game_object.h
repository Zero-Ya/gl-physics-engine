#pragma once
#include "component.h"

#include <string>
#include <vector>

class GameObject {
public:
	uint32_t id;
	std::string name;

	GameObject(uint32_t id, const std::string& name = "Entity")
		: id(id), name(name) {}

	~GameObject() {
		for (Component* comp : m_Components) {
			delete comp;
		}
		m_Components.clear();
	}

	template<typename T, typename... Args>
	T* addComponent(Args&&... args) {
		T* comp = new T(std::forward<Args>(args)...);
		comp->owner = this;
		comp->init();
		m_Components.push_back(comp);
		return comp;
	}

	template<typename T>
	T* getComponent() {
		for (Component* comp : m_Components) {
			if (typeid(*comp) == typeid(T)) {
				return static_cast<T*>(comp);
			}
		}
		return nullptr;
	}

private:
	std::vector<Component*> m_Components;
};