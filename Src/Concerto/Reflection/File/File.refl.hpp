#ifndef CONCERTO_REFLECTION_FILE_HPP
#define CONCERTO_REFLECTION_FILE_HPP

#include <string>
#include <string_view>

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	/// A reflectable file-path type. Stores a virtual path of the form
	/// "<assetDirName>/<relativeSubPath>"; the host application resolves it
	/// against a registered asset directory. Wire format adds a "$file"
	/// discriminator so the UI knows to render a file picker instead of a
	/// plain text input. Behaves like a String otherwise (Get/Set, OnValueChanged).
	class CCT_REFL_CLASS() CCT_REFLECTION_API File : public Object
	{
	public:
		File() = default;
		explicit File(std::string_view value);
		explicit File(std::string value);
		~File() override = default;

		File(const File& other);
		File(File&& other) noexcept;
		File& operator=(const File& other);
		File& operator=(File&& other) noexcept;

		void Set(std::string_view value);
		void Set(std::string value);

		[[nodiscard]] const std::string& Get() const;
		[[nodiscard]] std::string& Get();

		operator std::string_view() const;

		File& operator=(std::string_view value);
		File& operator=(std::string value);

		[[nodiscard]] bool operator==(const File& other) const;
		[[nodiscard]] bool operator!=(const File& other) const;
		[[nodiscard]] bool operator==(std::string_view other) const;
		[[nodiscard]] bool operator!=(std::string_view other) const;

		[[nodiscard]] std::string ToString() const override;

		CCT_OBJECT(File);

	private:
		CCT_NATIVE_MEMBER()
		std::string m_value;
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_FILE_HPP
