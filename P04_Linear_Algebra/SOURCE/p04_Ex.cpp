#include <glm/glm.hpp>						// Opengl Mathematics
#include <glm/gtx/string_cast.hpp>			// glm::to_string()
#include <glm/gtc/matrix_access.hpp>		// glm::colum(), glm::row()
#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h>
using namespace glm;

#include <iostream>
using namespace std;

void
part1()
{
	// Vectors
	glm::vec<3, float>	v1(1, 2, 3);
	cout << "v1 = " << to_string(v1) << endl;
	glm::vec<3, float>	v2(3, 2, 1);
	cout << "v2 = " << to_string(v2) << endl;
	cout << endl;

	// Addition/subtraction
	cout << "v1 + v2 = " << to_string(v1 + v2) << endl;
	cout << "v1 - v2 = " << to_string(v1 - v2) << endl;

	// Negation
	cout << "-v1 = " << to_string(-v1) << endl;

	// Subtraction and Scalar multiplication
	cout << "v1 - 2*v2 = " << to_string(v1 - 2.0f * v2) << endl;

	// Dot product
	cout << "dot(v1, v2) = " << dot(v1, v2) << endl;

	// Cross product
	cout << "cross(v1, v2) = " << to_string(cross(v1, v2)) << endl;
}

void
part2()
{
	// Vectors
	glm::vec<3, float>	v1(1, 2, 3);
	cout << "v1 = " << to_string(v1) << endl;
	glm::vec<3, float>	v2(3, 2, 1);
	cout << "v2 = " << to_string(v2) << endl;

	//	GLM employs colum-major representation
	//	A1 = 1.0  2.0  3.0		A2 = 2.0  1.0  2.0 
	//		 2.0  3.0  2.0			 2.0  2.0  1.0
	//		 1.0  1.0  2.0			 1.0  1.0  1.0	
	glm::mat3	A1(1.0, 2.0, 1.0, 2.0, 3.0, 1.0, 3.0, 2.0, 2.0);
	cout << "A1 = " << to_string(A1) << endl;
	glm::mat3	A2(2.0, 2.0, 1.0, 1.0, 2.0, 1.0, 2.0, 1.0, 1.0);
	cout << "A2 = " << to_string(A2) << endl;
	cout << endl;

	// Element indexing: B[colum][row] in GLM.	Be careful!
	cout << "A1 + A2 = " << to_string(A1 + A2) << endl;			// Matrix addition
	cout << "A1 - A2 = " << to_string(A1 - A2) << endl;			// Matrix subtraction
	cout << "-A1 = " << to_string(-A1) << endl;					// Matrix Negation
	cout << "A1 -2*A2 = " << to_string(A1 -2.0f * A2) << endl;	// Matrix Subtraction and Scalar multiplication
	cout << "A1 x A2 = " << to_string(A1 * A2) << endl;			// Matrix-matrix multiplication (3x3 matrix) x (3x3 matrix)
	cout << "A2 x A1 = " << to_string(A2 * A1) << endl;			// Matrix-matrix multiplication (3x3 matrix) x (3x3 matrix)
	cout << "A1 x v1 = " << to_string(A1 * v1) << endl;			// Matrix-vector multiplication (3x3 matrix) x (3x1 vector)
	cout << "A2 x v2 = " << to_string(A2 * v2) << endl;			// Matrix-vector multiplication (1x3 vector) x (3x3 matrix)
}

int
main(int argc, char* argv[])
{
	int e = (argc < 2) ? 1 : atoi(argv[1]);

	switch (e)
	{
	case 1: part1(); break; // Vector operations
	case 2: part2(); break; // Matrix operations
	}

	system("pause");
	return 0;
}