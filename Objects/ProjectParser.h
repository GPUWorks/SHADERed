#pragma once
#include "../GUIManager.h"
#include "ShaderVariable.h"
#include "MessageStack.h"
#include "../Engine/Model.h"

#include <string>
#include <pugixml/src/pugixml.hpp>
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glew.h>
#if defined(__APPLE__)
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

namespace ed
{
	class PipelineManager;
	class RenderEngine;
	class ObjectManager;

	class ProjectParser
	{
	public:
		ProjectParser(PipelineManager* pipeline, ObjectManager* objects, RenderEngine* renderer, MessageStack* msgs, GUIManager* gui);
		~ProjectParser();

		void Open(const std::string& file);
		void OpenTemplate();
		inline void SetTemplate(const std::string& str) { m_template = str; }

		void Save();
		void SaveAs(const std::string& file, bool copyFiles = false);

		std::string LoadProjectFile(const std::string& file);
		char* LoadProjectFile(const std::string& file, size_t& len);
		eng::Model* LoadModel(const std::string& file);

		void SaveProjectFile(const std::string& file, const std::string& data);

		std::string GetRelativePath(const std::string& to);
		std::string GetProjectPath(const std::string& projectFile);
		bool FileExists(const std::string& file);

		void ResetProjectDirectory();
		inline void SetProjectDirectory(const std::string& path) { m_projectPath = path; }
		inline std::string GetProjectDirectory() { return m_projectPath; }

		inline std::string GetOpenedFile() { return m_file; }
		inline std::string GetTemplate() { return m_template; }

		inline void ModifyProject() { m_modified = true; }
		inline bool IsProjectModified() { return m_modified; }

	private:
		void m_parseV1(pugi::xml_node& projectNode); // old
		void m_parseV2(pugi::xml_node& projectNode); // current -> merge blend, rasterizer and depth states into one "render state" ||| remove input layout parsing ||| ignore shader entry property

		void m_parseVariableValue(pugi::xml_node& node, ShaderVariable* var);
		void m_exportVariableValue(pugi::xml_node& node, ShaderVariable* vars);
		void m_exportShaderVariables(pugi::xml_node& node, std::vector<ShaderVariable*>& vars);
		GLenum m_toBlend(const char* str);
		GLenum m_toBlendOp(const char* str);
		GLenum m_toComparisonFunc(const char* str);
		GLenum m_toStencilOp(const char* str);
		GLenum m_toCullMode(const char* str);

		bool m_modified;

		GUIManager* m_ui;
		PipelineManager* m_pipe;
		ObjectManager* m_objects;
		RenderEngine* m_renderer;
		MessageStack* m_msgs;
		std::string m_file;
		std::string m_projectPath;
		std::string m_template;

		std::vector<std::pair<std::string, eng::Model*>> m_models;
	};
}