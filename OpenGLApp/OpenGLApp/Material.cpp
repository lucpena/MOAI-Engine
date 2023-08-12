#include "Material.h"

Material::Material()
{
    specularIntensity = 0;
    shineness = 0;
}

Material::Material(GLfloat sIntensity, GLfloat shine)
{
    specularIntensity = sIntensity;
    shineness = shine;
}

void Material::UseMaterial(GLuint specularIntensityLocation, GLuint shinenessLocation)
{
    glUniform1f(specularIntensityLocation, specularIntensity);
    glUniform1f(shinenessLocation, shineness);
}

Material::~Material()
{

}