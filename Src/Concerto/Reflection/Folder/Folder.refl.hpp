#ifndef CONCERTO_REFLECTION_FOLDER_HPP
#define CONCERTO_REFLECTION_FOLDER_HPP

#include <string>
#include <string_view>

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	/// A reflectable directory-path type. Like File, but the wire format uses
	/// "$folder" so the UI renders a directory-only picker. The stored value is
	/// a virtual path of the form "<assetDirName>/<relativeSubPath>" pointing at
	/// a directory inside a registered asset directory.
	class CCT_REFL_CLASS() CCT_REFLECTION_API Folder : public Object
	{
	public:
		Folder() = default;
		explicit Folder(std::string_view value);
		explicit Folder(std::string value);
		~Folder() override = default;

		Folder(const Folder& other);
		Folder(Folder&& other) noexcept;
		Folder& operator=(const Folder& other);
		Folder& operator=(Folder&& other) noexcept;

		void Set(std::string_view value);
		void Set(std::string value);

		[[nodiscard]] const std::string& Get() const;
		[[nodiscard]] std::string& Get();

		operator std::string_view() const;

		Folder& operator=(std::string_view value);
		Folder& operator=(std::string value);

		[[nodiscard]] bool operator==(const Folder& other) const;
		[[nodiscard]] bool operator!=(const Folder& other) const;
		[[nodiscard]] bool operator==(std::string_view other) const;
		[[nodiscard]] bool operator!=(std::string_view other) const;

		[[nodiscard]] std::string ToString() const override;

		CCT_OBJECT(Folder);

	private:
		CCT_NATIVE_MEMBER()
		std::string m_value;
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_FOLDER_HPP
