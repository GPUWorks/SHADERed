#include "VariableValueEdit.h"
#include "Icons.h"
#include "../Objects/FunctionVariableManager.h"
#include "../Objects/CameraSnapshots.h"
#include "../Objects/Names.h"

#include <imgui/imgui.h>
#include <glm/glm.hpp>

namespace ed
{
	VariableValueEditUI::VariableValueEditUI()
	{
		Close();
	}
	bool VariableValueEditUI::Update()
	{
		if (m_var != nullptr) {
			FunctionShaderVariable state = m_var->Function;
			bool modified = false;

			ImGui::Text("%s ", m_var->Name);

			if (state != FunctionShaderVariable::None)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth()-(71+ImGui::GetStyle().WindowPadding.x*2));
			if (ImGui::Button((UI_ICON_EDIT "##" + std::string(m_var->Name)).c_str(), ImVec2(25, 0))) {
				m_var->Function = FunctionShaderVariable::None;
				modified = true;
			}

			if (state != FunctionShaderVariable::None)
				ImGui::PopStyleColor();
			else
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			ImGui::SameLine();
			if (ImGui::Button((UI_ICON_FX "##" + std::string(m_var->Name)).c_str(), ImVec2(25, 0))) {
				// find first function that is compatible with this value type
				for (int i = 0; i < (int)FunctionShaderVariable::Count; i++)
					if (FunctionVariableManager::HasValidReturnType(m_var->GetType(), (FunctionShaderVariable)i)) {
						FunctionVariableManager::AllocateArgumentSpace(m_var, (FunctionShaderVariable)i);
						break;
					}
			}

			if (state == FunctionShaderVariable::None)
				ImGui::PopStyleColor();

			if (m_var->Function == FunctionShaderVariable::None) {
				bool mres = m_drawRegular();
				modified = mres || modified;
			}
			else {
				bool mfunc = m_drawFunction();
				modified = mfunc || modified;
			}

			return modified;
		}

		return false;
	}
	bool VariableValueEditUI::m_drawRegular()
	{
		ImGui::PushItemWidth(-1);
		bool ret = false;

		switch (m_var->GetType()) {
			case ed::ShaderVariable::ValueType::Float4x4:
			case ed::ShaderVariable::ValueType::Float3x3:
			case ed::ShaderVariable::ValueType::Float2x2:
				for (int y = 0; y < m_var->GetColumnCount(); y++) {
					if (m_var->GetType() == ShaderVariable::ValueType::Float2x2) {
						if (ImGui::DragFloat2(("##valuedit" + std::string(m_var->Name) + std::to_string(y)).c_str(), m_var->AsFloatPtr(0, y), 0.01f))
							ret = true;
					} else if (m_var->GetType() == ShaderVariable::ValueType::Float3x3) {
						if (ImGui::DragFloat3(("##valuedit" + std::string(m_var->Name) + std::to_string(y)).c_str(), m_var->AsFloatPtr(0, y), 0.01f))
							ret = true;
					} else {
						if (ImGui::DragFloat4(("##valuedit" + std::string(m_var->Name) + std::to_string(y)).c_str(), m_var->AsFloatPtr(0, y), 0.01f))
							ret = true;
					}
				}
				break;
			case ed::ShaderVariable::ValueType::Float1:
				if (ImGui::DragFloat(("##valuedit" + std::string(m_var->Name)).c_str(), m_var->AsFloatPtr(), 0.01f))
					ret = true;
				break;
			case ed::ShaderVariable::ValueType::Float2:
				if (ImGui::DragFloat2(("##valuedit" + std::string(m_var->Name)).c_str(), m_var->AsFloatPtr(), 0.01f))
					ret = true;
				break;
			case ed::ShaderVariable::ValueType::Float3:
				if (ImGui::DragFloat3(("##valuedit" + std::string(m_var->Name)).c_str(), m_var->AsFloatPtr(), 0.01f))
					ret = true;
				break;
			case ed::ShaderVariable::ValueType::Float4:
				if (ImGui::DragFloat4(("##valuedit_" + std::string(m_var->Name)).c_str(), m_var->AsFloatPtr(), 0.01f))
					ret = true;
				break;
			case ed::ShaderVariable::ValueType::Integer1:
				if (ImGui::DragInt(("##valuedit" + std::string(m_var->Name)).c_str(), m_var->AsIntegerPtr(), 0.3f))
					ret = true;
				break;
			case ed::ShaderVariable::ValueType::Integer2:
				if (ImGui::DragInt2(("##valuedit" + std::string(m_var->Name)).c_str(), m_var->AsIntegerPtr(), 0.3f))
					ret = true;
				break;
			case ed::ShaderVariable::ValueType::Integer3:
				if (ImGui::DragInt3(("##valuedit" + std::string(m_var->Name)).c_str(), m_var->AsIntegerPtr(), 0.3f))
					ret = true;
				break;
			case ed::ShaderVariable::ValueType::Integer4:
				if (ImGui::DragInt4(("##valuedit" + std::string(m_var->Name)).c_str(), m_var->AsIntegerPtr(), 0.3f))
					ret = true;
				break;
			case ed::ShaderVariable::ValueType::Boolean1:
				if (ImGui::Checkbox(("##valuedit" + std::string(m_var->Name)).c_str(), m_var->AsBooleanPtr()))
					ret = true;
				break;
			case ed::ShaderVariable::ValueType::Boolean2:
				if (ImGui::Checkbox(("##valuedit1" + std::string(m_var->Name)).c_str(), m_var->AsBooleanPtr(0)))
					ret=true;
				ImGui::SameLine();
				if (ImGui::Checkbox(("##valuedit2" + std::string(m_var->Name)).c_str(), m_var->AsBooleanPtr(1)))
					ret=true;
				break;
			case ed::ShaderVariable::ValueType::Boolean3:
				if (ImGui::Checkbox(("##valuedit1" + std::string(m_var->Name)).c_str(), m_var->AsBooleanPtr(0)))
					ret=true;
				ImGui::SameLine();
				if (ImGui::Checkbox(("##valuedit2" + std::string(m_var->Name)).c_str(), m_var->AsBooleanPtr(1)))
					ret=true;
				ImGui::SameLine();
				if (ImGui::Checkbox(("##valuedit3" + std::string(m_var->Name)).c_str(), m_var->AsBooleanPtr(2)))
					ret=true;
				break;
			case ed::ShaderVariable::ValueType::Boolean4:
				if (ImGui::Checkbox(("##valuedit1" + std::string(m_var->Name)).c_str(), m_var->AsBooleanPtr(0)))
					ret=true;
				ImGui::SameLine();
				if (ImGui::Checkbox(("##valuedit2" + std::string(m_var->Name)).c_str(), m_var->AsBooleanPtr(1)))
					ret=true;
				ImGui::SameLine();
				if (ImGui::Checkbox(("##valuedit3" + std::string(m_var->Name)).c_str(), m_var->AsBooleanPtr(2)))
					ret=true;
				ImGui::SameLine();
				if (ImGui::Checkbox(("##valuedit4" + std::string(m_var->Name)).c_str(), m_var->AsBooleanPtr(3)))
					ret=true;
				break;
		}

		ImGui::PopItemWidth();

		return ret;
	}
	bool VariableValueEditUI::m_drawFunction()
	{
		bool ret = false;

		ImGui::Text("Function:");
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		const char* inputComboPreview = FUNCTION_NAMES[(int)m_var->Function];
		if (ImGui::BeginCombo(("##func" + std::string(m_var->Name)).c_str(), inputComboPreview)) {
			for (int n = 1; n < (int)FunctionShaderVariable::Count; n++) {
				bool is_selected = (n == (int)m_var->Function);
				if (FunctionVariableManager::HasValidReturnType(m_var->GetType(), (FunctionShaderVariable)n) && ImGui::Selectable(FUNCTION_NAMES[n], is_selected)) {
					FunctionVariableManager::AllocateArgumentSpace(m_var, (FunctionShaderVariable)n);
					ret = true;
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		
		ImGui::Columns(2, 0, false);
		
		ImGui::SetColumnWidth(0, 15+105);

		ImGui::Indent(15);
		switch (m_var->Function) {
			case FunctionShaderVariable::Pointer:
			{
				ImGui::Text("Variable:");
				ImGui::NextColumn();

				auto& varList = FunctionVariableManager::VariableList;
				if (ImGui::BeginCombo(("##ptrVars" + std::string(m_var->Name)).c_str(), m_var->Arguments)) {
					for (int n = 0; n < varList.size(); n++) {
						if (m_var == varList[n] || varList[n] == nullptr)
							break;

						if (m_var->GetType() != varList[n]->GetType())
							continue;


						bool is_selected = strcmp(m_var->Arguments, varList[n]->Name) == 0;
						if (ImGui::Selectable(varList[n]->Name, is_selected)) {
							strcpy(m_var->Arguments, varList[n]->Name);
							ret = true;
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				ImGui::NextColumn();
			} break;

			case FunctionShaderVariable::CameraSnapshot:
			{
				ImGui::Text("Camera:");
				ImGui::NextColumn();

				auto& camList = CameraSnapshots::GetList();
				if (ImGui::BeginCombo(("##ptrCams" + std::string(m_var->Name)).c_str(), m_var->Arguments)) {
					for (int n = 0; n < camList.size(); n++) {
						bool is_selected = strcmp(m_var->Arguments, camList[n].c_str()) == 0;
						if (ImGui::Selectable(camList[n].c_str(), is_selected)) {
							strcpy(m_var->Arguments, camList[n].c_str());
							ret = true;
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				ImGui::NextColumn();
			} break;

			case FunctionShaderVariable::MatrixLookAtLH:
			{
				ImGui::Text("Eye position:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat3(("##eyePosition" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 0), 0.01f))
					ret = true;
				ImGui::NextColumn();

				ImGui::Text("Focus position:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat3(("##focus" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 3), 0.01f))
					ret = true;
				ImGui::NextColumn();

				ImGui::Text("Up direction:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat3(("##eyeUp" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 6), 0.01f))
					ret = true;
				ImGui::NextColumn();
			} break;

			case FunctionShaderVariable::MatrixLookToLH:
			{
				ImGui::Text("Eye position:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat3(("##eyePosition" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 0), 0.01f))
					ret = true;
				ImGui::NextColumn();

				ImGui::Text("Eye direction:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat3(("##eyeDir" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 3), 0.01f))
					ret = true;
				ImGui::NextColumn();

				ImGui::Text("Up direction:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat3(("##eyeUp" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 6), 0.01f))
					ret = true;
				ImGui::NextColumn();
			} break;

			case FunctionShaderVariable::MatrixOrthographicLH:
			{
				ImGui::Text("View size:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat2(("##viewSize" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 0), 0.01f))
					ret = true;
				ImGui::NextColumn();

				ImGui::Text("Near Z:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat(("##nearZ" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 2), 0.01f))
					ret = true;
				ImGui::NextColumn();

				ImGui::Text("Far Z:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat(("##farZ" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 3), 0.01f))
					ret = true;
				ImGui::NextColumn();
			} break;

			case FunctionShaderVariable::MatrixPerspectiveFovLH:
			{
				ImGui::Text("FOV:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				float fov = glm::degrees(*FunctionVariableManager::LoadFloat(m_var->Arguments, 0));
				if (ImGui::SliderAngle(("##fovAngle" + std::string(m_var->Name)).c_str(), &fov, 0, 180))
					ret = true;
				*FunctionVariableManager::LoadFloat(m_var->Arguments, 0) = glm::radians(fov);
				ImGui::NextColumn();

				ImGui::Text("Aspect ratio:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat(("##aspectRatio" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 1), 0.01f))
					ret = true;
				ImGui::NextColumn();

				ImGui::Text("Near Z:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat(("##nearZ" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 2), 0.01f))
					ret = true;
				ImGui::NextColumn();

				ImGui::Text("Far Z:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat(("##farZ" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 3), 0.01f))
					ret = true;
				ImGui::NextColumn();
			} break;

			case FunctionShaderVariable::MatrixPerspectiveLH:
			{
				ImGui::Text("View size:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat2(("##viewSize" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 0), 0.01f))
					ret = true;
				ImGui::NextColumn();

				ImGui::Text("Near Z:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat(("##nearZ" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 2), 0.01f))
					ret = true;
				ImGui::NextColumn();

				ImGui::Text("Far Z:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat(("##farZ" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 3), 0.01f))
					ret = true;
				ImGui::NextColumn();
			} break;

			case FunctionShaderVariable::MatrixReflect:
			{
				ImGui::Text("Plane:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat4(("##plane" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 0), 0.01f))
					ret = true;
				ImGui::NextColumn();
			} break;

			case FunctionShaderVariable::MatrixRotationAxis:
			{
				ImGui::Text("Axis:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat3(("##vec" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 0), 0.01f))
					ret = true;
				ImGui::NextColumn();

				ImGui::Text("Angle:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat(("##angle" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 3), 0.01f))
					ret = true;
				ImGui::NextColumn();
			} break;

			case FunctionShaderVariable::MatrixRotationNormal:
			{
				ImGui::Text("Normal:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat3(("##vec" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 0), 0.01f))
					ret = true;
				ImGui::NextColumn();

				ImGui::Text("Angle:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat(("##angle" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 3), 0.01f))
					ret = true;
				ImGui::NextColumn();
			} break;

			case FunctionShaderVariable::MatrixRotationX:
			{
				ImGui::Text("Angle:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat(("##angle" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 0), 0.01f))
					ret = true;
				ImGui::NextColumn();
			} break;

			case FunctionShaderVariable::MatrixRotationY:
			{
				ImGui::Text("Angle:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat(("##angle" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 0), 0.01f))
					ret = true;
				ImGui::NextColumn();
			} break;

			case FunctionShaderVariable::MatrixRotationZ:
			{
				ImGui::Text("Angle:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat(("##angle" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 0), 0.01f))
					ret = true;
				ImGui::NextColumn();
			} break;

			case FunctionShaderVariable::MatrixScaling:
			{
				ImGui::Text("Scale:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat3(("##scale" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 0), 0.01f))
					ret = true;
				ImGui::NextColumn();
			} break;

			case FunctionShaderVariable::MatrixShadow:
			{
				ImGui::Text("Plane:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat4(("##plane" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 0), 0.01f))
					ret = true;
				ImGui::NextColumn();

				ImGui::Text("Light position:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat3(("##light" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 4), 0.01f))
					ret = true;
				ImGui::NextColumn();

				ImGui::Text("Point Light:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				bool isPoint = (bool)(*FunctionVariableManager::LoadFloat(m_var->Arguments, 7));
				if (ImGui::Checkbox(("##lightPoint" + std::string(m_var->Name)).c_str(), &isPoint))
					ret = true;
				*FunctionVariableManager::LoadFloat(m_var->Arguments, 7) = (float)isPoint;
				ImGui::NextColumn();
			} break;

			case FunctionShaderVariable::MatrixTranslation:
			{
				ImGui::Text("Position:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat3(("##position" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 0), 0.01f))
					ret = true;
				ImGui::NextColumn();
			} break;

			case FunctionShaderVariable::ScalarCos:
			{
				ImGui::Text("Value:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat(("##value" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 0), 0.01f))
					ret = true;
			} break;

			case FunctionShaderVariable::ScalarSin:
			{
				ImGui::Text("Value:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (ImGui::DragFloat(("##value" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 0), 0.01f))
					ret = true;
				ImGui::NextColumn();
			} break;

			case FunctionShaderVariable::VectorNormalize:
			{
				ImGui::Text("Position:");
				ImGui::NextColumn();

				ImGui::PushItemWidth(-1);
				if (m_var->GetType() == ShaderVariable::ValueType::Float2) {
					if (ImGui::DragFloat2(("##position" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 0), 0.01f))
						ret = true;
				} else if (m_var->GetType() == ShaderVariable::ValueType::Float3) {
					if (ImGui::DragFloat3(("##position" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 0), 0.01f))
						ret = true;
				} else if (m_var->GetType() == ShaderVariable::ValueType::Float4) {
					if (ImGui::DragFloat4(("##position" + std::string(m_var->Name)).c_str(), FunctionVariableManager::LoadFloat(m_var->Arguments, 0), 0.01f))
						ret = true;
				}

				ImGui::NextColumn();
			} break;
		}
		ImGui::Unindent(15);

		ImGui::Columns(1);

		return ret;
	}
}
