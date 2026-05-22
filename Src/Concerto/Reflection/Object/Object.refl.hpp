//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_OBJECT_HPP
#define CONCERTO_REFLECTION_OBJECT_HPP

#include <memory>
#include <string>
#include <string_view>

#include <Concerto/Core/EnumFlags/EnumFlags.hpp>
#include <Concerto/Core/Uuid/Uuid.hpp>

#include "Concerto/Core/Signal/Signal.hpp"
#include "Concerto/Reflection/Defines.hpp"

#define CCT_OBJECT(className)                       \
public:                                             \
	static const cct::refl::Class* GetClass()       \
	{                                               \
		return m_class;                             \
	}                                               \
                                                    \
private:                                            \
	inline static const cct::refl::Class* m_class;  \
	friend class Internal##className##Class;        \
	friend class Internal##className##GenericClass; \
	int PrivateReflInitClass##className = (this->InitReflection(m_class), 0)

struct CCT_REFL_PACKAGE("version = \"1.0.0\"", "description = \"Concerto Reflection Standard Package\"") ConcertoReflection
{
};

namespace cct::refl
{
	enum class ChangeType
	{
		PreChange,
		PostChange,
		Set,
		Insert,
		Delete
	};

	enum class ObjectFlags : cct::UInt8
	{
		None = 0,
		Constructing = 1 << 0, // suppresses all OnXXX signal emissions
	};
} // namespace cct::refl

CCT_ENABLE_ENUM_FLAGS(cct::refl::ChangeType)
CCT_ENABLE_ENUM_FLAGS(cct::refl::ObjectFlags)

namespace cct::refl
{
	class Class;
	class Registry;

	class CCT_REFL_CLASS() CCT_REFLECTION_API Object
	{
	public:
		Object();
		virtual ~Object() = default;

		/// Emitted whenever the value of this object changes.
		/// Connect with a lambda or a member function pointer:
		///   obj.OnValueChanged.Connect([](){ ... });
		///   obj.OnValueChanged.Connect(this, &MyClass::OnChanged);
		Signal<> OnValueChanged;

		Object(const Object& other);
		Object(Object&& other) noexcept;

		Object& operator=(const Object& other);
		Object& operator=(Object&& other) noexcept;

		static inline bool IsInstanceOf(const Class& klass);

		[[nodiscard]] inline const cct::refl::Object* GetMemberVariable(std::size_t index) const;
		[[nodiscard]] inline cct::refl::Object* GetMemberVariable(std::size_t index);

		[[nodiscard]] inline const cct::refl::Object* GetMemberVariable(std::string_view name) const;
		[[nodiscard]] inline cct::refl::Object* GetMemberVariable(std::string_view name);

		template<typename T>
		[[nodiscard]] T* GetMemberVariable(std::size_t index);
		template<typename T>
		[[nodiscard]] const T* GetMemberVariable(std::size_t index) const;

		template<typename T>
		[[nodiscard]] T* GetMemberVariable(std::string_view name);
		template<typename T>
		[[nodiscard]] const T* GetMemberVariable(std::string_view name) const;

		template<typename T>
		[[nodiscard]] T* GetNativeMemberVariable(std::size_t index);
		template<typename T>
		[[nodiscard]] const T* GetNativeMemberVariable(std::size_t index) const;

		template<typename T>
		[[nodiscard]] T* GetNativeMemberVariable(std::string_view name);
		template<typename T>
		[[nodiscard]] const T* GetNativeMemberVariable(std::string_view name) const;

		[[nodiscard]] virtual std::string ToString() const;

		[[nodiscard]] const cct::Uuid& GetUuid() const;
		void SetUuid(const cct::Uuid& uuid);
		void RegenerateUuid();

		[[nodiscard]] inline const cct::refl::Class* GetDynamicClass() const;
		inline void SetDynamicClass(const Class* klass);

		[[nodiscard]] bool HasRegistry() const;
		[[nodiscard]] Registry* GetRegistry();
		[[nodiscard]] const Registry* GetRegistry() const;

		inline void InitializeMemberVariables();

		inline void SetFlag(ObjectFlags flag);
		inline void ClearFlag(ObjectFlags flag);
		[[nodiscard]] inline bool HasFlag(ObjectFlags flag) const;
		[[nodiscard]] inline EnumFlags<ObjectFlags> GetFlags() const;

		CCT_OBJECT(Object);

	protected:
		const cct::refl::Class* m_dynamicClass;
		Registry* m_registry;
		EnumFlags<ObjectFlags> m_flags;

		CCT_NATIVE_MEMBER()
		cct::Uuid m_uuid;

		/*
		 * Default member initializer, evaluated by the compiler for every constructor of
		 * className (including make_shared<T>, new T, etc.), in member declaration order.
		 * Because CCT_OBJECT is placed last in the class body, this runs after all other
		 * members are initialized but before the constructor body. In an inheritance chain
		 * each class level calls InitReflection with its own m_class, so the most-derived
		 * call wins and m_dynamicClass ends up set to the concrete type.
		 */
		inline void InitReflection(const Class* cls) noexcept;
	};
	/// RAII guard that sets an ObjectFlag on construction and clears it on destruction.
	/// An optional callback is invoked just after the flag is cleared, allowing callers
	/// to fire a deferred notification (e.g. OnValueChanged) once construction is done.
	///
	/// Example:
	///   {
	///       ScopedObjectFlag guard(vec, ObjectFlags::Constructing,
	///           [&vec]() { vec.OnValueChanged.Emit(); });
	///       // ... bulk operations — signals suppressed ...
	///   } // flag cleared, then callback fires
	class CCT_REFLECTION_API ScopedObjectFlag
	{
	public:
		using Callback = std::function<void()>;

		ScopedObjectFlag(Object& obj, ObjectFlags flag, Callback onExit = {});
		~ScopedObjectFlag();

		ScopedObjectFlag(const ScopedObjectFlag&) = delete;
		ScopedObjectFlag& operator=(const ScopedObjectFlag&) = delete;
		ScopedObjectFlag(ScopedObjectFlag&&) = delete;
		ScopedObjectFlag& operator=(ScopedObjectFlag&&) = delete;

	private:
		Object& m_obj;
		ObjectFlags m_flag;
		Callback m_onExit;
	};
} // namespace cct::refl

#include "Concerto/Reflection/Object/Object.inl"
#endif // CONCERTO_REFLECTION_OBJECT_HPP