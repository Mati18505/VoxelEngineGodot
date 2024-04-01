#pragma once
#include "Math.h"
#include "VoxelTypes.h"

#define Array3D(x, y, z, width, height) ((y) * width * height) + ((z) * width) + (x)
namespace Voxel {
	template<typename T>
	class Array3d {
	public:
		Array3d(const Vector3i size)
			: size(size)
		{
			elements = new T[TotalSize()];
		}
		
		Array3d(const Array3d& other)
			: size(other.size)
		{
			elements = new T[TotalSize()];
			memcpy(elements, other.elements, TotalSize());
		}
	
		Array3d(Array3d&& other) noexcept
			: size(other.size)
		{
			elements = other.elements;
			other.elements = nullptr;
		}
	
		T& At(Vector3i index)
		{
			if(CheckBounds(index))
				throw new std::invalid_argument("Position out of range!");
			return elements[Array3D(index.x, index.y, index.z, size.x, size.z)];
		}

		const T& At(Vector3i index) const
		{
			if(CheckBounds(index))
				throw new std::invalid_argument("Position out of range!");
			return elements[Array3D(index.x, index.y, index.z, size.x, size.z)];
		}
	
		T* GetRawData() { return elements; }
	
		~Array3d()
		{
			delete[] elements;
		}
	
		const Vector3i& GetSize() const { return size; }
		int TotalSize() const { return size.x * size.y * size.z; }
	
		Array3d<T>& operator=(const Array3d<T>& other)
		{
			size = other.size;
			Reallocate(TotalSize());
			memcpy(elements, other.elements, TotalSize());
			return *this;
		}
		
		Array3d<T>& operator=(Array3d<T>&& other)
		{
			if (&other == this)
				return *this;
	
			size = other.size;
			delete[] elements;
			elements = other.elements;
			other.elements = nullptr;
	
			return *this;
		}
	
		T& operator[](Vector3i index)
		{
			return elements[Array3D(index.x, index.y, index.z, size.x, size.z)];
		}

		const T& operator[](Vector3i index) const
		{
			return elements[Array3D(index.x, index.y, index.z, size.x, size.z)];
		}
	
	private:
		T* elements;
		Vector3i size;
	
		//True if index out of range
		bool CheckBounds(Vector3i index) const {
			return (index.x >= size.x || index.x < 0 ||
					index.y >= size.y || index.y < 0 ||
					index.z >= size.z || index.z < 0);
		}
	
		void Reallocate(const int newSize)
		{
			delete[] elements;
			elements = new T[newSize];
		}
	};
}
