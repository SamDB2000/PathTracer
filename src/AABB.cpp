#include <AABB.h>

namespace path_tracer {
	
bool AABB::intersect(glm::vec3 rayPos, glm::vec3 rayDir) {
	// We find where the ray intersects with each line of the AABB
	// (Thinking of it as lines to the x,y,z axes)
	glm::vec3 min = bounds[0];
    glm::vec3 max = bounds[1];

	// Find the x intersection for the closer and further x lines
	float tx_min = (min.x - rayPos.x) / rayDir.x;
    float tx_max = (max.x - rayPos.x) / rayDir.x;
    if (tx_min > tx_max) std::swap(tx_min, tx_max);

	// Find the y intersection for the closer and further y lines
	float ty_min = (min.y - rayPos.y) / rayDir.y;
    float ty_max = (max.y - rayPos.y) / rayDir.y;
    if (ty_min > ty_max) std::swap(ty_min, ty_max);

	// No intersection
	if ((tx_min > ty_max) || (ty_min > tx_max)) {
        return false;
	}

	if (ty_min > tx_min) {
        tx_min = ty_min;
	}

	if (ty_max > tx_max) {
        tx_max = ty_max; 
	}

	// Find the y intersection for the closer and further z lines
	float tz_min = (min.z - rayPos.z) / rayDir.z;
    float tz_max = (max.z - rayPos.z) / rayDir.z;
    if (tz_min > tz_max) std::swap(tz_min, tz_max);

	// No intersection
	if ((tz_min > tz_max) || (tz_min > tx_max)) {
        return false;
	}

	// Not really necessary unless we find the spot of intersection
	if (tz_min > tx_min)
        tx_min = tz_min;

	if (tz_max < tx_max)
        tx_max = tz_max;

	return true;
}


}  // namespace path_tracer
