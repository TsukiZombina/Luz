#version 330 core

in vec4 Color;
in vec4 Normal;
in vec4 FragmentPosition;

uniform vec4 diffuseLightPosition;
uniform vec4 cameraPosition;

out vec4 color;

void main()
{
	vec4 ambientLightColor = vec4(0.0,0.0,0.7,1.0);
	vec4 diffuseLightColor = vec4(1.0,1.0,1.0,1.0);
	vec4 specularLightColor = vec4(1.0,1.0,1.0,1.0);
	//vec4 diffuseLightPosition = vec4(-2.0,2.0,0.0,1.0);
	vec4 diffuseLightDirection = diffuseLightPosition - FragmentPosition;
	diffuseLightDirection = normalize(diffuseLightDirection);
	float diffuseFactor = max(dot(Normal, diffuseLightDirection),0);
	
	vec4 specularLightDirection = reflect(-1 * (diffuseLightDirection), Normal);
	specularLightDirection = normalize(specularLightDirection);
	vec4 cameraDirection = cameraPosition - FragmentPosition;
	cameraDirection = normalize(cameraDirection);
	float specularFactor = max(dot(cameraDirection, specularLightDirection),0);
	specularFactor = pow(specularFactor, 2);

	color = Color*(ambientLightColor + diffuseLightColor * diffuseFactor + specularLightColor * specularFactor);
}