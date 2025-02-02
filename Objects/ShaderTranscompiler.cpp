#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>

#include "Logger.h"
#include "Settings.h"
#include "HLSLFileIncluder.h"
#include "ShaderTranscompiler.h"
#include <glslang/glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/StandAlone/DirStackFileIncluder.h>
#include <SPIRVCross/spirv_glsl.hpp>
#include <SPIRVCross/spirv_cross_util.hpp>
#include "../Engine/GLUtils.h"


const TBuiltInResource DefaultTBuiltInResource = {
	/* .MaxLights = */ 32,
	/* .MaxClipPlanes = */ 6,
	/* .MaxTextureUnits = */ 32,
	/* .MaxTextureCoords = */ 32,
	/* .MaxVertexAttribs = */ 64,
	/* .MaxVertexUniformComponents = */ 4096,
	/* .MaxVaryingFloats = */ 64,
	/* .MaxVertexTextureImageUnits = */ 32,
	/* .MaxCombinedTextureImageUnits = */ 80,
	/* .MaxTextureImageUnits = */ 32,
	/* .MaxFragmentUniformComponents = */ 4096,
	/* .MaxDrawBuffers = */ 32,
	/* .MaxVertexUniformVectors = */ 128,
	/* .MaxVaryingVectors = */ 8,
	/* .MaxFragmentUniformVectors = */ 16,
	/* .MaxVertexOutputVectors = */ 16,
	/* .MaxFragmentInputVectors = */ 15,
	/* .MinProgramTexelOffset = */ -8,
	/* .MaxProgramTexelOffset = */ 7,
	/* .MaxClipDistances = */ 8,
	/* .MaxComputeWorkGroupCountX = */ 65535,
	/* .MaxComputeWorkGroupCountY = */ 65535,
	/* .MaxComputeWorkGroupCountZ = */ 65535,
	/* .MaxComputeWorkGroupSizeX = */ 1024,
	/* .MaxComputeWorkGroupSizeY = */ 1024,
	/* .MaxComputeWorkGroupSizeZ = */ 64,
	/* .MaxComputeUniformComponents = */ 1024,
	/* .MaxComputeTextureImageUnits = */ 16,
	/* .MaxComputeImageUniforms = */ 8,
	/* .MaxComputeAtomicCounters = */ 8,
	/* .MaxComputeAtomicCounterBuffers = */ 1,
	/* .MaxVaryingComponents = */ 60,
	/* .MaxVertexOutputComponents = */ 64,
	/* .MaxGeometryInputComponents = */ 64,
	/* .MaxGeometryOutputComponents = */ 128,
	/* .MaxFragmentInputComponents = */ 128,
	/* .MaxImageUnits = */ 8,
	/* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
	/* .MaxCombinedShaderOutputResources = */ 8,
	/* .MaxImageSamples = */ 0,
	/* .MaxVertexImageUniforms = */ 0,
	/* .MaxTessControlImageUniforms = */ 0,
	/* .MaxTessEvaluationImageUniforms = */ 0,
	/* .MaxGeometryImageUniforms = */ 0,
	/* .MaxFragmentImageUniforms = */ 8,
	/* .MaxCombinedImageUniforms = */ 8,
	/* .MaxGeometryTextureImageUnits = */ 16,
	/* .MaxGeometryOutputVertices = */ 256,
	/* .MaxGeometryTotalOutputComponents = */ 1024,
	/* .MaxGeometryUniformComponents = */ 1024,
	/* .MaxGeometryVaryingComponents = */ 64,
	/* .MaxTessControlInputComponents = */ 128,
	/* .MaxTessControlOutputComponents = */ 128,
	/* .MaxTessControlTextureImageUnits = */ 16,
	/* .MaxTessControlUniformComponents = */ 1024,
	/* .MaxTessControlTotalOutputComponents = */ 4096,
	/* .MaxTessEvaluationInputComponents = */ 128,
	/* .MaxTessEvaluationOutputComponents = */ 128,
	/* .MaxTessEvaluationTextureImageUnits = */ 16,
	/* .MaxTessEvaluationUniformComponents = */ 1024,
	/* .MaxTessPatchComponents = */ 120,
	/* .MaxPatchVertices = */ 32,
	/* .MaxTessGenLevel = */ 64,
	/* .MaxViewports = */ 16,
	/* .MaxVertexAtomicCounters = */ 0,
	/* .MaxTessControlAtomicCounters = */ 0,
	/* .MaxTessEvaluationAtomicCounters = */ 0,
	/* .MaxGeometryAtomicCounters = */ 0,
	/* .MaxFragmentAtomicCounters = */ 8,
	/* .MaxCombinedAtomicCounters = */ 8,
	/* .MaxAtomicCounterBindings = */ 1,
	/* .MaxVertexAtomicCounterBuffers = */ 0,
	/* .MaxTessControlAtomicCounterBuffers = */ 0,
	/* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
	/* .MaxGeometryAtomicCounterBuffers = */ 0,
	/* .MaxFragmentAtomicCounterBuffers = */ 1,
	/* .MaxCombinedAtomicCounterBuffers = */ 1,
	/* .MaxAtomicCounterBufferSize = */ 16384,
	/* .MaxTransformFeedbackBuffers = */ 4,
	/* .MaxTransformFeedbackInterleavedComponents = */ 64,
	/* .MaxCullDistances = */ 8,
	/* .MaxCombinedClipAndCullDistances = */ 8,
	/* .MaxSamples = */ 4,

	/* .maxMeshOutputVerticesNV = */ 256,
	/* .maxMeshOutputPrimitivesNV = */ 512,
	/* .maxMeshWorkGroupSizeX_NV = */ 32,
	/* .maxMeshWorkGroupSizeY_NV = */ 1,
	/* .maxMeshWorkGroupSizeZ_NV = */ 1,
	/* .maxTaskWorkGroupSizeX_NV = */ 32,
	/* .maxTaskWorkGroupSizeY_NV = */ 1,
	/* .maxTaskWorkGroupSizeZ_NV = */ 1,
	/* .maxMeshViewCountNV = */ 4,

	/* .limits = */ {
		/* .nonInductiveForLoops = */ 1,
		/* .whileLoops = */ 1,
		/* .doWhileLoops = */ 1,
		/* .generalUniformIndexing = */ 1,
		/* .generalAttributeMatrixVectorIndexing = */ 1,
		/* .generalVaryingIndexing = */ 1,
		/* .generalSamplerIndexing = */ 1,
		/* .generalVariableIndexing = */ 1,
		/* .generalConstantMatrixVectorIndexing = */ 1,
	}
};

namespace ed
{
	std::string ShaderTranscompiler::Transcompile(ShaderLanguage inLang, const std::string &filename, int sType, const std::string &entry, std::vector<ShaderMacro> &macros, bool gsUsed, MessageStack *msgs, ProjectParser* project)
	{
		ed::Logger::Get().Log("Starting to transcompile a HLSL shader " + filename);

		//Load HLSL into a string
		std::ifstream file(filename);

		if (!file.is_open())
		{
			if (msgs != nullptr)
				msgs->Add(MessageStack::Type::Error, msgs->CurrentItem, "Failed to open file " + filename, -1, sType);
			file.close();
			return "errorFile";
		}

		std::string inputHLSL((std::istreambuf_iterator<char>(file)),
							std::istreambuf_iterator<char>());

		file.close();

		return ShaderTranscompiler::TranscompileSource(inLang, filename, inputHLSL, sType, entry, macros, gsUsed, msgs, project);
	}
	std::string ShaderTranscompiler::TranscompileSource(ShaderLanguage inLang, const std::string &filename, const std::string &inputHLSL, int sType, const std::string &entry, std::vector<ShaderMacro> &macros, bool gsUsed, MessageStack *msgs, ProjectParser* project)
	{
		const char* inputStr = inputHLSL.c_str();

		// create shader
		EShLanguage shaderType = EShLangVertex;
		if (sType == 1)
			shaderType = EShLangFragment;
		else if (sType == 2)
			shaderType = EShLangGeometry;
		else if (sType == 3)
			shaderType = EShLangCompute;

		glslang::TShader shader(shaderType);
		if (entry.size() > 0 && entry != "main") {
			shader.setEntryPoint(entry.c_str());
			shader.setSourceEntryPoint(entry.c_str());
		}
		shader.setStrings(&inputStr, 1);

		// set macros
		std::string preambleStr = "#extension GL_GOOGLE_include_directive : enable\n";
		for (auto& macro : macros) {
			if (!macro.Active)
				continue;
			preambleStr += "#define " + std::string(macro.Name) + " " + std::string(macro.Value) + "\n";
		}
		if (preambleStr.size() > 0)
			shader.setPreamble(preambleStr.c_str());

		// set up
		int sVersion = (sType == 3) ? 430 : 330;
		glslang::EShTargetClientVersion vulkanVersion = glslang::EShTargetVulkan_1_0;
		glslang::EShTargetLanguageVersion targetVersion = glslang::EShTargetSpv_1_0;

		shader.setEnvInput(inLang == ShaderLanguage::HLSL ? glslang::EShSourceHlsl : glslang::EShSourceGlsl, shaderType, glslang::EShClientVulkan, sVersion);
		shader.setEnvClient(glslang::EShClientVulkan, vulkanVersion);
		shader.setEnvTarget(glslang::EShTargetSpv, targetVersion);
		
		TBuiltInResource res = DefaultTBuiltInResource;
		EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

		const int defVersion = sVersion;

		// includer
		ed::HLSLFileIncluder includer;
		includer.pushExternalLocalDirectory(filename.substr(0, filename.find_last_of("/\\")));
		if (project != nullptr)
			for (auto& str : Settings::Instance().Project.IncludePaths)
				includer.pushExternalLocalDirectory(project->GetProjectPath(str));

		std::string processedShader;

		if (!shader.preprocess(&res, defVersion, ENoProfile, false, false, messages, &processedShader, includer))
		{
			if (msgs != nullptr) {
				msgs->Add(gl::ParseHLSLMessages(msgs->CurrentItem, sType, shader.getInfoLog()));
				msgs->Add(MessageStack::Type::Error, msgs->CurrentItem, "Shader preprocessing failed", -1, sType);
			}
			return "error";
		}

		// update strings
		const char *processedStr = processedShader.c_str();
		shader.setStrings(&processedStr, 1);

		// parse
		if (!shader.parse(&res, 100, false, messages))
		{
			if (msgs != nullptr)
				msgs->Add(gl::ParseHLSLMessages(msgs->CurrentItem, sType, shader.getInfoLog()));
			return "error";
		}

		// link
		glslang::TProgram prog;
		prog.addShader(&shader);

		if (!prog.link(messages))
		{
			if (msgs != nullptr)
				msgs->Add(MessageStack::Type::Error, msgs->CurrentItem, "Shader linking failed", -1, sType);
			return "error";
		}

		// convert to spirv
		std::vector<unsigned int> spv;
		spv::SpvBuildLogger logger;
		glslang::SpvOptions spvOptions;

		spvOptions.optimizeSize = false;
		spvOptions.disableOptimizer = true;

		glslang::GlslangToSpv(*prog.getIntermediate(shaderType), spv, &logger, &spvOptions);




		// Read SPIR-V from disk or similar.
		spirv_cross::CompilerGLSL glsl(std::move(spv));

		// Set some options.
		spirv_cross::CompilerGLSL::Options options;
		options.version = sVersion;

		glsl.set_common_options(options);

		auto active = glsl.get_active_interface_variables();

		// rename outputs
		spirv_cross::ShaderResources resources = glsl.get_shader_resources();
		std::string outputName = "outputVS";
		if (shaderType == EShLangFragment)
			outputName = "outputPS";
		else if (shaderType == EShLangGeometry)
			outputName = "outputGS";
		for (auto& resource : resources.stage_outputs)
		{
			uint32_t resID = glsl.get_decoration(resource.id, spv::DecorationLocation);
			glsl.set_name(resource.id, outputName + std::to_string(resID));
		}

		// rename inputs
		std::string inputName = "inputVS";
		if (shaderType == EShLangFragment)
			inputName = gsUsed ? "outputGS" : "outputVS";
		else if (shaderType == EShLangGeometry)
			inputName = "outputVS";
		for (auto& resource : resources.stage_inputs)
		{
			uint32_t resID = glsl.get_decoration(resource.id, spv::DecorationLocation);
			glsl.set_name(resource.id, inputName + std::to_string(resID));
		}

		// Compile to GLSL
		try {
			glsl.build_dummy_sampler_for_combined_images();
			glsl.build_combined_image_samplers();
		} catch (spirv_cross::CompilerError& e) {
			ed::Logger::Get().Log("An exception occured: " + std::string(e.what()), true);
			if (msgs != nullptr)
				msgs->Add(MessageStack::Type::Error, msgs->CurrentItem, "Transcompiling failed", -1, sType);
			return "error";
		}

		spirv_cross_util::inherit_combined_sampler_bindings(glsl);
		std::string source = "";
		try {
			source = glsl.compile();
		} catch (spirv_cross::CompilerError& e) {
			ed::Logger::Get().Log("Transcompiler threw an exception: " + std::string(e.what()), true);
			if (msgs != nullptr)
				msgs->Add(MessageStack::Type::Error, msgs->CurrentItem, "Transcompiling failed", -1, sType);
			return "error";
		}

		// WARNING: lost of hacks in the following code
		// remove all the UBOs when transcompiling from HLSL
		if (inLang == ShaderLanguage::HLSL) {
			std::stringstream ss(source);
			std::string line;
			source = "";
			bool inUBO = false;
			std::vector<std::string> uboNames;
			while (std::getline(ss, line)) {

				// i know, ewww, but idk if there's a function to do this (this = converting UBO
				// to separate uniforms)...
				if (line.find("layout(binding") != std::string::npos &&
					line.find("uniform") != std::string::npos &&
					line.find("sampler") == std::string::npos &&
					line.find("image") == std::string::npos &&
					line.find(" buffer ") == std::string::npos)
				{
					inUBO = true;
					continue;
				} else if (inUBO){
					if (line == "{")
						continue;
					else if (line[0] == '}') {
						inUBO = false;
						uboNames.push_back(line.substr(2, line.size() - 3));
						continue;
					} else {
						size_t playout = line.find(")");
						if (playout != std::string::npos)
							line.erase(0, playout+2);
						line = "uniform " + line;
					}
				}
				else { // remove all occurances of "ubo." substrings
					for (int i = 0; i < uboNames.size(); i++) {
						std::string what = uboNames[i] + ".";
						size_t n = what.length();
						for (size_t j = line.find(what); j != std::string::npos; j = line.find(what))
							line.erase(j, n);
					}
				}

				source += line + "\n";
			}
		} 
		else if (inLang == ShaderLanguage::VulkanGLSL) {
			std::stringstream ss(source);
			std::string line;
			source = "";
			bool inUBO = false;
			std::vector<std::string> uboNames;
			int deleteUboPos = 0, deleteUboLength = 0;
			std::string uboExt = (shaderType == 0) ? "VS" : (shaderType == 1 ? "PS" : "GS");
			while (std::getline(ss, line))
			{

				// i know, ewww, but idk if there's a function to do this (this = converting UBO
				// to separate uniforms)...
				if (line.find("layout(binding") != std::string::npos &&
					line.find("uniform") != std::string::npos &&
					line.find("sampler") == std::string::npos &&
					line.find("image") == std::string::npos &&
					line.find(" buffer ") == std::string::npos)
				{
					inUBO = true;
					deleteUboPos = source.size();
					
					size_t lineLastOf = line.find_last_of(' ');
					std::string name = line.substr(lineLastOf + 1, line.find('\n') - lineLastOf -1);
					std::string newLine = "uniform struct " + name + " {\n";
					deleteUboLength = newLine.size();
					source += newLine;
					continue;
				}
				else if (inUBO)
				{
					if (line == "{")
						continue;
					else if (line[0] == '}')
					{
						// i know this is yucky but are there glslang/spirv-cross functions for these?
						inUBO = false;
						std::string uboName = line.substr(2, line.size() - 3);
						bool isGenBySpirvCross = false;
						if (uboName[0] == '_') {
							isGenBySpirvCross = true;
							for (int i = 1; i < uboName.size(); i++) {
								if (!isdigit(uboName[i])) {
									isGenBySpirvCross = false;
									break;
								}
							}
						}

						if (isGenBySpirvCross) {
							uboNames.push_back(uboName); // only delete occurances if the structure name is generated by spirv-cross and not us
							
							// delete the declaration:
							source.erase(deleteUboPos, deleteUboLength);

							for (size_t loc = deleteUboPos; loc < source.size(); loc++) {
								source.insert(loc, "uniform ");
								loc = source.find_first_of(';', loc) + 1;
							}
						} else
							source += "} " + uboName + ";\n";

						continue;
					}
					/*
					TODO: do i need to remove layout(...)?
					else
					{
						size_t playout = line.find(")");
						if (playout != std::string::npos)
							line.erase(0, playout + 2);
					}
					*/
				}
				else
				{ // remove all occurances of "ubo." substrings
					for (int i = 0; i < uboNames.size(); i++)
					{
						std::string what = uboNames[i] + ".";
						size_t n = what.length();
						for (size_t j = line.find(what); j != std::string::npos; j = line.find(what))
							line.erase(j, n);
					}
				}

				source += line + "\n";
			}
		}

		ed::Logger::Get().Log("Finished transcompiling the shader");
		
		return source;
	}
	ShaderLanguage ShaderTranscompiler::GetShaderTypeFromExtension(const std::string &file)
	{
		std::vector<std::string> &hlslExts = Settings::Instance().General.HLSLExtensions;
		std::vector<std::string> &vkExts = Settings::Instance().General.VulkanGLSLExtensions;
		if (std::count(hlslExts.begin(), hlslExts.end(), file.substr(file.find_last_of('.') + 1)) > 0)
			return ShaderLanguage::HLSL;
		if (std::count(vkExts.begin(), vkExts.end(), file.substr(file.find_last_of('.') + 1)) > 0)
			return ShaderLanguage::VulkanGLSL;

		return ShaderLanguage::GLSL;
	}
}
