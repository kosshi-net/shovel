#ifndef SHOVEL_SAABB_INCLUDED
#define SHOVEL_SAABB_INCLUDED 1


namespace Physics {

	float saabb( 
		const float a_min[3], 
		const float a_max[3], 
		const float a_vel[3], 
		const float b_min[3], 
		const float b_max[3], 
		const float b_vel[3],
		int  	   coldata[3]
	);

}


#endif