#include "../core.h"
#include "../3rdparty/cJSON.h"

mat4 load_transform(cJSON *transform)
{
    mat4 model = identity<mat4>();
    if (!cJSON_IsArray(transform))
		return model;

	cJSON *operation;
	cJSON_ArrayForEach(operation, transform) {
		{
			cJSON *translate = cJSON_GetObjectItem(operation, "translate");
			if (cJSON_IsArray(translate) && cJSON_GetArraySize(translate) == 3) {
				float x = (float)(cJSON_GetArrayItem(translate, 0)->valuedouble);
				float y = (float)(cJSON_GetArrayItem(translate, 1)->valuedouble);
				float z = (float)(cJSON_GetArrayItem(translate, 2)->valuedouble);
				model = glm::translate(vec3(x, y, z)) * model;
				continue;
			}
		}
		{
			cJSON *scale = cJSON_GetObjectItem(operation, "scale");
			if (cJSON_IsArray(scale) && cJSON_GetArraySize(scale) == 3) {
				float x = (float)(cJSON_GetArrayItem(scale, 0)->valuedouble);
				float y = (float)(cJSON_GetArrayItem(scale, 1)->valuedouble);
				float z = (float)(cJSON_GetArrayItem(scale, 2)->valuedouble);
				model = glm::scale(vec3(x, y, z)) * model;
				continue;
			}
			if (cJSON_IsNumber(scale) && (scale->valuedouble != 0)) {
				float s = (float)(scale->valuedouble);
				model = glm::scale(vec3(s)) * model;
				continue;
			}
		}		
	}
	return model;
}
