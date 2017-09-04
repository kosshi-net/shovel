#include <cstdio>
#include <physics/saabb.cpp>

int main()
{
	using namespace Physics;



	printf("%f\n",

		saabb(
			(const float[]){ 0.0f, 0.0f, 0.0f},
			(const float[]){ 1.0f, 1.0f, 1.0f},
			(const float[]){ 2.1f, 0.0f, 0.0f},

			(const float[]){ 2.0f, 0.0f, 0.0f},
			(const float[]){ 3.0f, 1.0f, 1.0f},
			(const float[]){ 1.0f, 0.0f, 0.0f}
		)

	);


	return 0;
}