#ifndef MAGMATIC_FULLENTITY_HPP
#define MAGMATIC_FULLENTITY_HPP

#include "ECS.hpp"

namespace magmatic::ecs
{
	class FullEntity
	{
	public:
		using EntityID = EntityManager::EntityID;

		FullEntity(EntityID id, ECS& ecs_handle) noexcept;

		template<typename ComponentType>
		ComponentType& get();

		template<typename ComponentType>
		const ComponentType& get() const;

		template<typename ComponentType, typename... Args>
		void set(Args&&... args);

		template<typename ComponentType>
		void remove() noexcept;

		template<typename ComponentType>
		[[nodiscard]] bool has() const;

		[[nodiscard]] bool valid() const noexcept;

	private:
		EntityID id_;
		ECS& ecs_handle_;
	};

	FullEntity::FullEntity(EntityID id, ECS& ecs_handle) noexcept
	: id_(id), ecs_handle_(ecs_handle)
	{}

	template<typename ComponentType>
	ComponentType& FullEntity::get()
	{
		assert(valid());
		return ecs_handle_.getComponent<ComponentType>(id_);
	}

	template<typename ComponentType>
	const ComponentType &FullEntity::get() const
	{
		assert(valid());
		return ecs_handle_.getComponent<ComponentType>(id_);
	}

	template<typename ComponentType, typename... Args>
	void FullEntity::set(Args &&... args)
	{
		assert(valid());
		ecs_handle_.setComponent<ComponentType>(id_, std::forward<Args>(args)...);
	}

	template<typename ComponentType>
	bool FullEntity::has() const
	{
		assert(valid());
		return ecs_handle_.hasComponent<ComponentType>(id_);
	}

	bool FullEntity::valid() const noexcept
	{
		return ecs_handle_.entityExists(id_);
	}

	template<typename ComponentType>
	void FullEntity::remove() noexcept
	{
		ecs_handle_.removeComponent<ComponentType>(id_);
	}
}

#endif //MAGMATIC_FULLENTITY_HPP
