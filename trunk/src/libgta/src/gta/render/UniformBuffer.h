/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef UNIFORMBUFFER_H_
#define UNIFORMBUFFER_H_

#include <gta/config.h>
#include "../gl.h"
#include <nxcommon/CString.h>
#include <nxcommon/math/Matrix3.h>
#include <nxcommon/math/Matrix4.h>
#include <nxcommon/math/Vector3.h>
#include <nxcommon/math/Vector4.h>
#include <map>

using std::map;



class AdvancedShaderProgram;


class UniformBuffer
{
public:
	class UniformData
	{
	public:
		virtual void apply(GLint location) const = 0;
	};

private:

	// **************************************
	// *									*
	// *			INTEGER DATA			*
	// *									*
	// **************************************

	class IntUniformData : public UniformData
	{
	public:
		IntUniformData(GLint value) : value(value) {}
		virtual void apply(GLint location) const { glUniform1i(location, value); }

	private:
		GLint value;
	};

	class Int2UniformData : public UniformData
	{
	public:
		Int2UniformData(GLint val1, GLint val2) : val1(val1), val2(val2) {}
		virtual void apply(GLint location) const { glUniform2i(location, val1, val2); }

	private:
		GLint val1, val2;
	};

	class Int3UniformData : public UniformData
	{
	public:
		Int3UniformData(GLint val1, GLint val2, GLint val3) : val1(val1), val2(val2), val3(val3) {}
		virtual void apply(GLint location) const { glUniform3i(location, val1, val2, val3); }

	private:
		GLint val1, val2, val3;
	};

	class Int4UniformData : public UniformData
	{
	public:
		Int4UniformData(GLint val1, GLint val2, GLint val3, GLint val4) : val1(val1), val2(val2), val3(val3), val4(val4) {}
		virtual void apply(GLint location) const { glUniform4i(location, val1, val2, val3, val4); }

	private:
		GLint val1, val2, val3, val4;
	};

	class IntArrayUniformData : public UniformData
	{
	public:
		IntArrayUniformData(GLsizei count, GLint* values) : count(count), values(values) {}
		~IntArrayUniformData() { delete[] values; }
		virtual void apply(GLint location) const { glUniform1iv(location, count, values); }

	private:
		GLsizei count;
		GLint* values;
	};

	class Int2ArrayUniformData : public UniformData
	{
	public:
		Int2ArrayUniformData(GLsizei count, GLint* values) : count(count), values(values) {}
		~Int2ArrayUniformData() { delete[] values; }
		virtual void apply(GLint location) const { glUniform2iv(location, count, values); }

	private:
		GLsizei count;
		GLint* values;
	};

	class Int3ArrayUniformData : public UniformData
	{
	public:
		Int3ArrayUniformData(GLsizei count, GLint* values) : count(count), values(values) {}
		~Int3ArrayUniformData() { delete[] values; }
		virtual void apply(GLint location) const { glUniform3iv(location, count, values); }

	private:
		GLsizei count;
		GLint* values;
	};

	class Int4ArrayUniformData : public UniformData
	{
	public:
		Int4ArrayUniformData(GLsizei count, GLint* values) : count(count), values(values) {}
		~Int4ArrayUniformData() { delete[] values; }
		virtual void apply(GLint location) const { glUniform4iv(location, count, values); }

	private:
		GLsizei count;
		GLint* values;
	};



	// **************************************
	// *									*
	// *			FLOAT DATA				*
	// *									*
	// **************************************

	class FloatUniformData : public UniformData
	{
	public:
		FloatUniformData(GLfloat value) : value(value) {}
		virtual void apply(GLint location) const { glUniform1f(location, value); }

	private:
		GLfloat value;
	};

	class Float2UniformData : public UniformData
	{
	public:
		Float2UniformData(GLfloat val1, GLfloat val2) : val1(val1), val2(val2) {}
		virtual void apply(GLint location) const { glUniform2f(location, val1, val2); }

	private:
		GLfloat val1, val2;
	};

	class Float3UniformData : public UniformData
	{
	public:
		Float3UniformData(GLfloat val1, GLfloat val2, GLfloat val3) : val1(val1), val2(val2), val3(val3) {}
		virtual void apply(GLint location) const { glUniform3f(location, val1, val2, val3); }

	private:
		GLfloat val1, val2, val3;
	};

	class Float4UniformData : public UniformData
	{
	public:
		Float4UniformData(GLfloat val1, GLfloat val2, GLfloat val3, GLfloat val4) : val1(val1), val2(val2), val3(val3), val4(val4) {}
		virtual void apply(GLint location) const { glUniform4f(location, val1, val2, val3, val4); }

	private:
		GLfloat val1, val2, val3, val4;
	};

	class FloatArrayUniformData : public UniformData
	{
	public:
		FloatArrayUniformData(GLsizei count, GLfloat* values) : count(count), values(values) {}
		~FloatArrayUniformData() { delete[] values; }
		virtual void apply(GLint location) const { glUniform1fv(location, count, values); }

	private:
		GLsizei count;
		GLfloat* values;
	};

	class Float2ArrayUniformData : public UniformData
	{
	public:
		Float2ArrayUniformData(GLsizei count, GLfloat* values) : count(count), values(values) {}
		~Float2ArrayUniformData() { delete[] values; }
		virtual void apply(GLint location) const { glUniform2fv(location, count, values); }

	private:
		GLsizei count;
		GLfloat* values;
	};

	class Float3ArrayUniformData : public UniformData
	{
	public:
		Float3ArrayUniformData(GLsizei count, GLfloat* values) : count(count), values(values) {}
		~Float3ArrayUniformData() { delete[] values; }
		virtual void apply(GLint location) const { glUniform3fv(location, count, values); }

	private:
		GLsizei count;
		GLfloat* values;
	};

	class Float4ArrayUniformData : public UniformData
	{
	public:
		Float4ArrayUniformData(GLsizei count, GLfloat* values) : count(count), values(values) {}
		~Float4ArrayUniformData() { delete[] values; }
		virtual void apply(GLint location) const { glUniform4fv(location, count, values); }

	private:
		GLsizei count;
		GLfloat* values;
	};



	// **************************************
	// *									*
	// *			MATRIX DATA				*
	// *									*
	// **************************************

	class Matrix2ArrayUniformData : public UniformData
	{
	public:
		Matrix2ArrayUniformData(GLsizei count, GLboolean transpose, GLfloat* values)
				: count(count), transpose(transpose), values(values) {}
		~Matrix2ArrayUniformData() { delete[] values; }
		virtual void apply(GLint location) const { glUniformMatrix2fv(location, count, transpose, values); }

	private:
		GLsizei count;
		GLboolean transpose;
		GLfloat* values;
	};

	class Matrix3ArrayUniformData : public UniformData
	{
	public:
		Matrix3ArrayUniformData(GLsizei count, GLboolean transpose, GLfloat* values)
				: count(count), transpose(transpose), values(values) {}
		~Matrix3ArrayUniformData() { delete[] values; }
		virtual void apply(GLint location) const { glUniformMatrix3fv(location, count, transpose, values); }

	private:
		GLsizei count;
		GLboolean transpose;
		GLfloat* values;
	};

	class Matrix4ArrayUniformData : public UniformData
	{
	public:
		Matrix4ArrayUniformData(GLsizei count, GLboolean transpose, GLfloat* values)
				: count(count), transpose(transpose), values(values) {}
		~Matrix4ArrayUniformData() { delete[] values; }
		virtual void apply(GLint location) const { glUniformMatrix4fv(location, count, transpose, values); }

	private:
		GLsizei count;
		GLboolean transpose;
		GLfloat* values;
	};

	class Matrix2x3ArrayUniformData : public UniformData
	{
	public:
		Matrix2x3ArrayUniformData(GLsizei count, GLboolean transpose, GLfloat* values)
				: count(count), transpose(transpose), values(values) {}
		~Matrix2x3ArrayUniformData() { delete[] values; }
		virtual void apply(GLint location) const { glUniformMatrix2x3fv(location, count, transpose, values); }

	private:
		GLsizei count;
		GLboolean transpose;
		GLfloat* values;
	};

	class Matrix3x2ArrayUniformData : public UniformData
	{
	public:
		Matrix3x2ArrayUniformData(GLsizei count, GLboolean transpose, GLfloat* values)
				: count(count), transpose(transpose), values(values) {}
		~Matrix3x2ArrayUniformData() { delete[] values; }
		virtual void apply(GLint location) const { glUniformMatrix3x2fv(location, count, transpose, values); }

	private:
		GLsizei count;
		GLboolean transpose;
		GLfloat* values;
	};

	class Matrix2x4ArrayUniformData : public UniformData
	{
	public:
		Matrix2x4ArrayUniformData(GLsizei count, GLboolean transpose, GLfloat* values)
				: count(count), transpose(transpose), values(values) {}
		~Matrix2x4ArrayUniformData() { delete[] values; }
		virtual void apply(GLint location) const { glUniformMatrix2x4fv(location, count, transpose, values); }

	private:
		GLsizei count;
		GLboolean transpose;
		GLfloat* values;
	};

	class Matrix4x2ArrayUniformData : public UniformData
	{
	public:
		Matrix4x2ArrayUniformData(GLsizei count, GLboolean transpose, GLfloat* values)
				: count(count), transpose(transpose), values(values) {}
		~Matrix4x2ArrayUniformData() { delete[] values; }
		virtual void apply(GLint location) const { glUniformMatrix4x2fv(location, count, transpose, values); }

	private:
		GLsizei count;
		GLboolean transpose;
		GLfloat* values;
	};

	class Matrix3x4ArrayUniformData : public UniformData
	{
	public:
		Matrix3x4ArrayUniformData(GLsizei count, GLboolean transpose, GLfloat* values)
				: count(count), transpose(transpose), values(values) {}
		~Matrix3x4ArrayUniformData() { delete[] values; }
		virtual void apply(GLint location) const { glUniformMatrix3x4fv(location, count, transpose, values); }

	private:
		GLsizei count;
		GLboolean transpose;
		GLfloat* values;
	};

	class Matrix4x3ArrayUniformData : public UniformData
	{
	public:
		Matrix4x3ArrayUniformData(GLsizei count, GLboolean transpose, GLfloat* values)
				: count(count), transpose(transpose), values(values) {}
		~Matrix4x3ArrayUniformData() { delete[] values; }
		virtual void apply(GLint location) const { glUniformMatrix4x3fv(location, count, transpose, values); }

	private:
		GLsizei count;
		GLboolean transpose;
		GLfloat* values;
	};

private:
	typedef map<CString, UniformData*> DataMap;
	typedef DataMap::iterator DataIterator;
	typedef DataMap::const_iterator ConstDataIterator;

public:
	void setUniformInt(const CString& name, GLint value)
			{ setData(name, new IntUniformData(value)); }
	void setUniformInt2(const CString& name, GLint val1, GLint val2)
			{ setData(name, new Int2UniformData(val1, val2)); }
	void setUniformInt3(const CString& name, GLint val1, GLint val2, GLint val3)
			{ setData(name, new Int3UniformData(val1, val2, val3)); }
	void setUniformInt4(const CString& name, GLint val1, GLint val2, GLint val3, GLint val4)
			{ setData(name, new Int4UniformData(val1, val2, val3, val4)); }

	void setUniformIntArray(const CString& name, GLsizei count, GLint* values)
			{ setData(name, new IntArrayUniformData(count, values)); }
	void setUniformInt2Array(const CString& name, GLsizei count, GLint* values)
			{ setData(name, new Int2ArrayUniformData(count, values)); }
	void setUniformInt3Array(const CString& name, GLsizei count, GLint* values)
			{ setData(name, new Int3ArrayUniformData(count, values)); }
	void setUniformInt4Array(const CString& name, GLsizei count, GLint* values)
			{ setData(name, new Int4ArrayUniformData(count, values)); }


	void setUniformFloat(const CString& name, GLfloat value)
			{ setData(name, new FloatUniformData(value)); }
	void setUniformFloat2(const CString& name, GLfloat val1, GLfloat val2)
			{ setData(name, new Float2UniformData(val1, val2)); }
	void setUniformFloat3(const CString& name, GLfloat val1, GLfloat val2, GLfloat val3)
			{ setData(name, new Float3UniformData(val1, val2, val3)); }
	void setUniformFloat4(const CString& name, GLfloat val1, GLfloat val2, GLfloat val3, GLfloat val4)
			{ setData(name, new Float4UniformData(val1, val2, val3, val4)); }

	void setUniformFloatArray(const CString& name, GLsizei count, GLfloat* values)
			{ setData(name, new FloatArrayUniformData(count, values)); }
	void setUniformFloat2Array(const CString& name, GLsizei count, GLfloat* values)
			{ setData(name, new Float2ArrayUniformData(count, values)); }
	void setUniformFloat3Array(const CString& name, GLsizei count, GLfloat* values)
			{ setData(name, new Float3ArrayUniformData(count, values)); }
	void setUniformFloat4Array(const CString& name, GLsizei count, GLfloat* values)
			{ setData(name, new Float4ArrayUniformData(count, values)); }

	void setUniformFloat3(const CString& name, const Vector3& vec)
			{ setUniformFloat3(name, vec.getX(), vec.getY(), vec.getZ()); }
	void setUniformFloat4(const CString& name, const Vector4& vec)
			{ setUniformFloat4(name, vec.getX(), vec.getY(), vec.getZ(), vec.getW()); }


	void setUniformMatrix2Array(const CString& name, GLsizei count, GLfloat* values, GLboolean transpose = GL_FALSE)
			{ setData(name, new Matrix2ArrayUniformData(count, transpose, values)); }
	void setUniformMatrix3Array(const CString& name, GLsizei count, GLfloat* values, GLboolean transpose = GL_FALSE)
			{ setData(name, new Matrix3ArrayUniformData(count, transpose, values)); }
	void setUniformMatrix4Array(const CString& name, GLsizei count, GLfloat* values, GLboolean transpose = GL_FALSE)
			{ setData(name, new Matrix4ArrayUniformData(count, transpose, values)); }
	void setUniformMatrix2x3Array(const CString& name, GLsizei count, GLfloat* values, GLboolean transpose = GL_FALSE)
			{ setData(name, new Matrix2x3ArrayUniformData(count, transpose, values)); }
	void setUniformMatrix3x2Array(const CString& name, GLsizei count, GLfloat* values, GLboolean transpose = GL_FALSE)
			{ setData(name, new Matrix3x2ArrayUniformData(count, transpose, values)); }
	void setUniformMatrix2x4Array(const CString& name, GLsizei count, GLfloat* values, GLboolean transpose = GL_FALSE)
			{ setData(name, new Matrix2x4ArrayUniformData(count, transpose, values)); }
	void setUniformMatrix4x2Array(const CString& name, GLsizei count, GLfloat* values, GLboolean transpose = GL_FALSE)
			{ setData(name, new Matrix4x2ArrayUniformData(count, transpose, values)); }
	void setUniformMatrix3x4Array(const CString& name, GLsizei count, GLfloat* values, GLboolean transpose = GL_FALSE)
			{ setData(name, new Matrix3x4ArrayUniformData(count, transpose, values)); }
	void setUniformMatrix4x3Array(const CString& name, GLsizei count, GLfloat* values, GLboolean transpose = GL_FALSE)
			{ setData(name, new Matrix4x3ArrayUniformData(count, transpose, values)); }

	void setUniformMatrix2(const CString& name, GLfloat* values, GLboolean transpose = GL_FALSE)
			{ setUniformMatrix2Array(name, 1, values, transpose); }
	void setUniformMatrix3(const CString& name, GLfloat* values, GLboolean transpose = GL_FALSE)
			{ setUniformMatrix3Array(name, 1, values, transpose); }
	void setUniformMatrix4(const CString& name, GLfloat* values, GLboolean transpose = GL_FALSE)
			{ setUniformMatrix4Array(name, 1, values, transpose); }
	void setUniformMatrix2x3(const CString& name, GLfloat* values, GLboolean transpose = GL_FALSE)
			{ setUniformMatrix2x3Array(name, 1, values, transpose); }
	void setUniformMatrix3x2(const CString& name, GLfloat* values, GLboolean transpose = GL_FALSE)
			{ setUniformMatrix3x2Array(name, 1, values, transpose); }
	void setUniformMatrix2x4(const CString& name, GLfloat* values, GLboolean transpose = GL_FALSE)
			{ setUniformMatrix2x4Array(name, 1, values, transpose); }
	void setUniformMatrix4x2(const CString& name, GLfloat* values, GLboolean transpose = GL_FALSE)
			{ setUniformMatrix4x2Array(name, 1, values, transpose); }
	void setUniformMatrix3x4(const CString& name, GLfloat* values, GLboolean transpose = GL_FALSE)
			{ setUniformMatrix3x4Array(name, 1, values, transpose); }
	void setUniformMatrix4x3(const CString& name, GLfloat* values, GLboolean transpose = GL_FALSE)
			{ setUniformMatrix4x3Array(name, 1, values, transpose); }

	void setUniformMatrix3(const CString& name, const Matrix3& matrix, GLboolean transpose = GL_FALSE)
			{ GLfloat* data = new GLfloat[9]; memcpy(data, &matrix, 9*sizeof(float)); setUniformMatrix3(name, data, transpose); }
	void setUniformMatrix4(const CString& name, const Matrix3& matrix, GLboolean transpose = GL_FALSE)
			{ GLfloat* data = new GLfloat[16]; memcpy(data, &matrix, 16*sizeof(float)); setUniformMatrix4(name, data, transpose); }


	void setUniformVector3(const CString& name, const Vector3& vec) { setUniformFloat3(name, vec); }
	void setUniformVector4(const CString& name, const Vector4& vec) { setUniformFloat4(name, vec); }


	void apply(AdvancedShaderProgram* program) const;

private:
	void setData(const CString& name, UniformData* data);

private:
	DataMap data;
};

#endif /* UNIFORMBUFFER_H_ */
