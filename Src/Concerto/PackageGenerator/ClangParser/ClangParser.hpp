//
// Clang LibTooling based parser for Concerto Package Generator
//

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "Concerto/PackageGenerator/Defines.hpp"
#include <clang/AST/DeclCXX.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/Frontend/ASTUnit.h>

namespace cct
{
	class ClangParser
	{
	public:
		Package* Parse(const std::vector<std::string>& includeDirs,
					   const std::vector<std::string>& defines,
					   const std::vector<std::string>& sources,
					   const std::string& resourceDir,
					   const std::string& sdk);

		void ProcessDeclaration(const clang::Decl* declaration);
		void ProcessNamespace(const clang::NamespaceDecl* namespaceDeclaration);
		void ProcessRecord(const clang::CXXRecordDecl* recordDeclaration);
		void ProcessTemplateRecord(const clang::ClassTemplateDecl* templateDeclaration);
		void ProcessTemplateSpecialization(const clang::ClassTemplateSpecializationDecl* specializationDeclaration);
		void ProcessEnum(const clang::EnumDecl* enumDeclaration);

	private:
		Package m_package;
		clang::SourceManager* m_sourceManager = nullptr;
		const clang::LangOptions* m_langOptions = nullptr;
		clang::ASTContext* m_astContext = nullptr;

		void RemoveEmptyNamespaces(std::vector<Namespace>& namespaces);
	};
} // namespace cct
