#include "Light.h"

Light::Light(glm::vec4 col, glm::vec3 pos) {
	lightColor = col;
	lightPos = pos;

	Light::lightShadow = new PointShadow(lightPos);
}

Light::~Light() {
	delete lightShadow;
}

void Light::linkShader(Shader sh) {
	sh.Activate();
	glUniform4f(glGetUniformLocation(sh.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(sh.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	glUniform3f(glGetUniformLocation(sh.ID, "light.position"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(sh.ID, "light.ambient"), ambient.x, ambient.y, ambient.z);
	glUniform3f(glGetUniformLocation(sh.ID, "light.diffuse"), diffuse.x, diffuse.y, diffuse.z);
	glUniform3f(glGetUniformLocation(sh.ID, "light.specular"), specular.x, specular.y, specular.z);
}