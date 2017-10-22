#include <limits>
#include <algorithm>
#include <core/log.hpp>

namespace Physics {

	float saabb( 
		const float a_min[3], 
		const float a_max[3], 
		const float a_vel[3], 
		const float b_min[3], 
		const float b_max[3], 
		const float b_vel[3],
		int  	   coldata[3]
	){

		float invEntry[3];
		float invExit[3];
		float relVel[3];

		for (int i = 0; i < 3; ++i) {
			relVel[i] = ((a_vel[i] - b_vel[i]));
			if (relVel[i] > 0){
				invEntry[i] = b_min[i] - a_max[i];
				invExit [i] = b_max[i] - a_min[i];
			}else{
				invEntry[i] = b_max[i] - a_min[i];
				invExit [i] = b_min[i] - a_max[i];
			}
		}

		float entry[3];
		float exit[3];
		for (int i = 0; i < 3; ++i) {
			if (relVel[i] == 0){
				if (a_max[i] <= b_min[i] || a_min[i] >= b_max[i]){
					return 1; 
				}
				entry[i] = -std::numeric_limits<float>::infinity();
				exit[i]  =  std::numeric_limits<float>::infinity();
			}else{
				entry[i] = invEntry[i] / relVel[i];
				exit[i]  = invExit[i] /  relVel[i];
			}
		}

		// float entryTime = max(xEntry, yEntry);
		// float exitTime = min(xExit, yExit);
		float entryTime = *std::max_element(entry, entry+3);
		float exitTime  = *std::min_element(exit, exit+3);
		
		if(exitTime < entryTime || entryTime > 1 || entryTime < 0){
			return 1;
		}

		for (int i = 0; i < 3; ++i)
			coldata[i] = (entry[i] == entryTime) * (relVel[i] > 0 ? 1 : -1);

		return entryTime;
	}

}