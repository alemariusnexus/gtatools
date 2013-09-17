/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef MATRIX4_H_
#define MATRIX4_H_

#include <gtaformats/config.h>
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"

class Matrix3;



/**	\brief Represents a 4x4 square matrix.
 *
 *	This class represents a 4x4 square matrix which can be used for various transforms (rotation, translation,
 *	scaling etc.).
 *
 *	Internally, the matrix is an array of 4x4 = 16 floats, which is stored in <b>column-major</b> ordering.
 *	This means that the vector (data[0], data[1], data[2], data[3]) is the first column, and not the first
 *	row. This is the same convention as used by OpenGL, so the matrix float array can be used in OpenGL
 *	without transposition.
 */
class Matrix4
{
public:
	const static Matrix4 Identity;

public:
	/**	\brief Constructs the identity matrix.
	 *
	 *	You should use the pre-built constant Matrix4::Identity to save the time needed for data
	 *	initialization.
	 */
	Matrix4();

	/**	\brief Copy constructor.
	 */
	Matrix4(const Matrix4& other);

	/**	\brief Constructs a matrix from individual elements.
	 *
	 * 	As the internal data array, the constructor is arranged in <b>column-major</b> order.
	 *
	 * 	@param m00 Element at row 0, column 0.
	 * 	@param m10 Element at row 1, column 0.
	 * 	@param m20 Element at row 2, column 0.
	 * 	@param m30 Element at row 3, column 0.
	 * 	@param m01 Element at row 0, column 1.
	 * 	@param m11 Element at row 1, column 1.
	 * 	@param m21 Element at row 2, column 1.
	 * 	@param m31 Element at row 3, column 1.
	 * 	@param m02 Element at row 0, column 2.
	 * 	@param m12 Element at row 1, column 2.
	 * 	@param m22 Element at row 2, column 2.
	 * 	@param m32 Element at row 3, column 2.
	 * 	@param m03 Element at row 0, column 3.
	 * 	@param m13 Element at row 1, column 3.
	 * 	@param m23 Element at row 2, column 3.
	 * 	@param m33 Element at row 3, column 3.
	 */
	Matrix4(float m00, float m10, float m20, float m30,
			float m01, float m11, float m21, float m31,
			float m02, float m12, float m22, float m32,
			float m03, float m13, float m23, float m33);

	/**	\brief Constructs a matrix from a float array containing it's elements.
	 *
	 * 	The array is expected to be in <b>column-major</b> order. A copy of the parameter is stored, so the
	 * 	original array can be deleted afterwards.
	 *
	 * 	@param data The column-major element array. Expected to contain 16 floats.
	 */
	Matrix4(const float* data) { memcpy(this->data, data, 16*4); }

	/** Constructs a 4x4 matrix from a 3x3 matrix.
	 *
	 * 	The 3x3 matrix is used to fill in the upper-left part of the new 4x4 matrix. The fourth row and column
	 * 	are initialized to the values (0, 0, 0, 1).
	 *
	 * 	@param other The matrix to be used in the upper-left part.
	 */
	Matrix4(const Matrix3& other);

	/**	\brief Returns the raw matrix data.
	 *
	 * 	This is not a copy, but the internal pointer of the object.
	 *
	 * 	@return The internal data pointer.
	 */
	const float* toArray() const { return data; }

	/**	\brief Returns the raw matrix data.
	 *
	 * 	This is not a copy, but the internal pointer of the object.
	 *
	 * 	@return The internal data pointer.
	 */
	float* toArray() { return data; }

	/**	\brief Performs matrix addition on this matrix.
	 *
	 * 	@param other The matrix to be added.
	 * 	@return A reference to this object.
	 */
	Matrix4& operator+=(const Matrix4& other);

	/**	\brief Performs matrix additon.
	 *
	 * 	@param rhv The matrix to be added.
	 * 	@param The resulting matrix.
	 */
	const Matrix4 operator+(const Matrix4& rhv) const { return Matrix4(*this) += rhv; }

	/**	\brief Performs matrix subtraction on this matrix.
	 *
	 * 	@param other The matrix to be subtracted fron this one.
	 * 	@return A reference to this object.
	 */
	Matrix4& operator-=(const Matrix4& other);

	/**	\brief Performs matrix subtraction.
	 *
	 * 	@param rhv The matrix to be subtracted from this one.
	 * 	@return The resulting matrix.
	 */
	const Matrix4 operator-(const Matrix4& rhv) const { return Matrix4(*this) -= rhv; }

	/**	\brief Multiplies this matrix component-wise by a scalar.
	 *
	 * 	@param rhv The scalar by which every element is multiplied.
	 * 	@return A reference to this object.
	 */
	Matrix4& operator*=(float rhv);

	/**	\brief Multiplies this matrix component-wise by a scalar.
	 *
	 * 	@param rhv The scalar by which every element is multiplied.
	 * 	@return The resulting matrix.
	 */
	const Matrix4 operator*(float rhv) const { return Matrix4(*this) *= rhv; }

	/**	\brief Performs matrix-matrix multiplication.
	 *
	 * 	@param rhv the matrix by which this one is multiplied.
	 * 	@return A reference to this object.
	 */
	Matrix4& operator*=(const Matrix4& rhv);

	/**	\brief Performs matrix-matrix multiplication.
	 *
	 * 	@param rhv the matrix by which this one is multiplied.
	 * 	@return The resulting matrix.
	 */
	const Matrix4 operator*(const Matrix4& rhv) const { return Matrix4(*this) *= rhv; }

	/**	\brief Performs matrix-vector multiplication.
	 *
	 * 	This is used to apply transformation to a vector.
	 *
	 * 	@param rhv The vector by which this matrix is multiplied.
	 * 	@return The resulting vector.
	 */
	const Vector4 operator*(const Vector4& rhv) const;

	/**	\brief Returns the vector containing elements of a specific row.
	 *
	 * 	@param i The row.
	 * 	@return The row vector.
	 */
	const Vector4 row(int i) const;

	/**	\brief Returns the vector containing elements of a specific column.
	 *
	 * 	@param i The column.
	 * 	@return The column vector.
	 */
	const Vector4 column(int i) const;

	/**	\brief Translates this matrix.
	 *
	 * 	This effectively multiplies this matrix by a translation matrix.
	 *
	 * 	@param x Translation along the x axis.
	 * 	@param x Translation along the y axis.
	 * 	@param x Translation along the z axis.
	 */
	void translate(float x, float y, float z) { *this *= translation(x, y, z); }

	/**	\brief Translates this matrix.
	 *
	 * 	This effectively multiplies this matrix by a translation matrix.
	 *
	 * 	@param vec The translation vector.
	 */
	void translate(const Vector3& vec) { *this *= translation(vec); }

	/**	\brief
	 *
	 */
	void rotate(float angle, const Vector3& vec) { *this *= rotation(angle, vec); }
	void rotate(float angle, float x, float y, float z) { *this *= rotation(angle, x, y, z); }
	void rotateX(float angle) { *this *= rotationX(angle); }
	void rotateY(float angle) { *this *= rotationY(angle); }
	void rotateZ(float angle) { *this *= rotationZ(angle); }
	void setTranslation(float x, float y, float z);
	void setTranslation(const Vector3& vec) { setTranslation(vec.getX(), vec.getY(), vec.getZ()); }
	void transpose();
	float determinant() const;
	void invert();
	const Matrix4 inverse() const { Matrix4 cpy(*this); cpy.invert(); return cpy; }

	void toEuler(float& x, float& y, float& z);

public:
	/**	\brief Builds a translation matrix.
	 *
	 * 	@param x Translation along the x axis.
	 * 	@param x Translation along the y axis.
	 * 	@param x Translation along the z axis.
	 * 	@return The translation matrix.
	 */
	static Matrix4 translation(float x, float y, float z);

	/**	\brief Builds a translation matrix.
	 *
	 * 	@param vec The translation vector.
	 * 	@return The translation matrix.
	 */
	static Matrix4 translation(const Vector3& vec) { return translation(vec.getX(), vec.getY(), vec.getZ()); }

	/**	\brief Builds a rotation matrix from axis-angle.
	 *
	 * 	The resulting matrix represents a rotation of the given angle around the given axis vector.
	 *
	 * 	@param angle The angle by which to rotate. It is given in radians.
	 * 	@param vec The axis vector around which to rotate.
	 * 	@return The rotation matrix.
	 */
	static Matrix4 rotation(float angle, const Vector3& vec);

	/**	\brief Builds a rotation matrix from axis-angle.
	 *
	 * 	The resulting matrix represents a rotation of the given angle around the given axis vector.
	 *
	 * 	@param a The angle by which to rotate. It is given in radians.
	 * 	@param x Translation around the x axis.
	 * 	@param y Translation around the y axis.
	 * 	@param z Translation around the z axis.
	 * 	@return The rotation matrix.
	 */
	static Matrix4 rotation(float a, float x, float y, float z) { return rotation(a, Vector3(x, y, z)); }

	/**	\brief Builds a rotation matrix around the X axis.
	 *
	 *	@param a The rotation angle given in radians.
	 *	@return The rotation matrix.
	 */
	static Matrix4 rotationX(float a);

	/**	\brief Builds a rotation matrix around the Y axis.
	 *
	 *	@param a The rotation angle given in radians.
	 *	@return The rotation matrix.
	 */
	static Matrix4 rotationY(float a);

	/**	\brief Builds a rotation matrix around the Z axis.
	 *
	 *	@param a The rotation angle given in radians.
	 *	@return The rotation matrix.
	 */
	static Matrix4 rotationZ(float a);

	static Matrix4 scale(const Vector3& s);
	static Matrix4 scale(float x, float y, float z) { return scale(Vector3(x, y, z)); }

	static Matrix4 lookAt(const Vector3& target, const Vector3& up);
	static Matrix4 fromQuaternion(const Quaternion& q);

	static Matrix4 fromQuaternionVector(const Vector3& pos, const Quaternion& rot);

	static Matrix4 frustum(float l, float r, float b, float t, float n, float f);
	static Matrix4 perspective(float fovy, float aspect, float n, float f);
	static Matrix4 ortho(float l, float r, float b, float t, float n, float f);

private:
	float data[16];
};

#endif /* MATRIX4_H_ */
