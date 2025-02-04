#include "efpch.h"

#include "application/Application.h"

#include "Window.h"
#include "scene/Scene.h"
#include "application/InputManager.h"
#include "util/Time.h"
#include "geometry/prim/Quad.h"
#include "geometry/prim/Sphere.h"
#include "material/CheckerMaterial.h"
#include "scene/Light.h"

#include "application/ManyLightsDemo.h"

#include <imgui.h>

using namespace efgl;

static const int SCR_WIDTH = 1280;
static const int SCR_HEIGHT = 720;

class SandboxApplication : public Application {
public:
	SandboxApplication()
		: Application(Window::Init(SCR_WIDTH, SCR_HEIGHT, "Geodesic Sphere")),
		scl(1.0f), vSegments(30), hSegments(30), pos(glm::vec3(0.0f, 2.0f, -3.0f)),
		lightPos(glm::vec3(1.556f, 3.111f, -1.648f)), lightColor(glm::vec3(1.0f)),
		roughness(0.5), metallic(0.5), albedo(glm::vec3(125.0f))
	{
	}

	virtual void Init() override
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);

		sphere = MakeRef<Sphere>(nullptr, vSegments, hSegments);
		sphere->UploadData();

		bulb = MakeRef<Sphere>(nullptr, 30, 30);
		bulb->UploadData();

		shader = MakeRef<Shader>("shaders/debug/pbr_demo.glsl");
		camera = Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 0.0f, 0.0f));

		lightShader = MakeRef<Shader>("shaders/debug/lightbulb.glsl");

		InputManager::SetGLFWCallbacks(window, &camera);

	}

	virtual void OnRender() override
	{

		InputManager::ProcessInput(window, time.GetDeltaTime());

		// set background
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->Bind();

		shader->SetUniform("metallic", metallic);
		shader->SetUniform("albedo", glm::normalize(albedo));
		shader->SetUniform("roughness", roughness);

		glm::mat4 proj = camera.GetProjectionMatrix();
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f));

		shader->SetUniform("proj", proj);
		shader->SetUniform("view", view);
		shader->SetUniform("model", model);
		shader->SetUniform("viewPos", camera.Position);

		PointLight p;
		p.Position = lightPos;
		p.Ambient = Color(0.15f);
		p.Diffuse = Color(0.9f);
		p.Specular = Color(1.0f);

		p.Constant = 1.0f;
		p.Linear = 0.22f;
		p.Quadratic = 0.2f;

		p.Radius = 65;

		shader->SetUniform("light.position", glm::vec4(p.Position, 1.0f));
		shader->SetUniform("light.ambient", glm::vec4(p.Ambient, 1.0f));
		shader->SetUniform("light.color", glm::vec4(p.Diffuse, 1.0f));
		shader->SetUniform("light.constant", p.Constant);
		shader->SetUniform("light.linear", p.Linear);
		shader->SetUniform("light.quadratic", p.Quadratic);
		shader->SetUniform("light.radius", p.Radius);

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(scl));
		model = glm::translate(model, pos);

		shader->SetUniform("model", model);
		sphere->Draw(*shader);

		lightShader->Bind();

		lightShader->SetUniform("proj", proj);
		lightShader->SetUniform("view", view);

		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.1f));
		lightShader->SetUniform("model", model);
		bulb->Draw(*lightShader);
	}

	virtual void OnImGuiRender() override {
		ImGui::Begin("Geodesic Sphere");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		if (ImGui::CollapsingHeader("Controls")) {
			ImGui::Text("CONTROLS: WASD for movement, left-click+cursor for looking around, ESC for close");
		}

		if (ImGui::CollapsingHeader("PBR Parameters")) {
			ImGui::SliderFloat("Roughness", &roughness, 0, 1);
			ImGui::SliderFloat("Metallic", &metallic, 0, 1);
			ImGui::ColorPicker3("Albedo", glm::value_ptr(albedo));
			ImGui::SliderFloat3("Light color", glm::value_ptr(lightColor), 0, 1.5);
			ImGui::SliderFloat3("Light pos", glm::value_ptr(lightPos), -10, 10);
		}

		if (ImGui::CollapsingHeader("Sphere info")) {
			ImGui::SliderFloat("Scale", &scl, 0, 20);
			ImGui::SliderFloat3("Position", glm::value_ptr(pos), -5, 5);
			if (ImGui::SliderInt("# of vertical segments", &vSegments, 1, 100)) {
				sphere.reset();
				sphere = MakeRef<Sphere>(mat, vSegments, hSegments);
				sphere->UploadData();
			}
			if (ImGui::SliderInt("# of horizontal segments", &hSegments, 1, 100)) {
				sphere.reset();
				sphere = MakeRef<Sphere>(mat, vSegments, hSegments);
				sphere->UploadData();
			}
		}

		ImGui::End();
	}

	virtual void Exit() override {
		TextureManager::CleanUp();
	}

private:
	Ref<Sphere> sphere;
	Ref<Sphere> bulb;
	Ref<Shader> shader;
	Ref<Shader> lightShader;
	Camera camera;

	Ref<CheckerMaterial> mat;

	float scl;
	glm::vec3 pos;
	int vSegments, hSegments;

	glm::vec3 lightPos;

	glm::vec3 lightColor;

	glm::vec3 albedo;
	float metallic;
	float roughness;

	Time time;
};

int main() {
	SandboxApplication app;
	//ManyLightsDemo app;
	app.Run();

	return 0;
}



