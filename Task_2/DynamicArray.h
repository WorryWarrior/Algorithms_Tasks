#pragma once

#include <cassert>
#include <stdexcept>

template <typename T>
class DynamicArray final
{
private:
	int capacity_;
	int size_;
	T* data_;
	constexpr static int initial_capacity_ = 8;
	constexpr static float growth_factor_ = 2.0f;
	
	void IncreaseSize()
	{
		capacity_ *= growth_factor_;
		T* tmp = static_cast<T*>(malloc(sizeof(T) * capacity_));

		/*if constexpr (std::is_move_constructible_v<T>)
		{*/
			for (int i = 0; i < size_; i++)
			{
				new (tmp + i) T(std::move(data_[i]));
			}
		/*}
		else
		{
			for (int i = 0; i < size_; i++)
			{
				new (tmp + i) T(data_[i]);
			}
		}*/

		
		ReleaseArray();
		data_ = tmp;
	}
	
public:
	DynamicArray() :DynamicArray(initial_capacity_) {}

	DynamicArray(int capacity) :capacity_(capacity)
	{
		assert(capacity > 0 && "Capacity must be a natural number");
		data_ = static_cast<T*>(malloc(sizeof(T) * capacity_));
		size_ = 0;
	}
	
	~DynamicArray()
	{
		ReleaseArray();
	}
	
	void ReleaseArray()
	{
		for (int i = 0; i < size_; ++i)
		{
			data_[i].~T();
		}
		
		free(data_);
	}

	// Copy constructor
	DynamicArray(const DynamicArray& arr) : DynamicArray(arr.capacity_)
	{
		for (int i = 0; i < arr.size_; ++i)
		{
			new (data_ + i) T(arr[i]);
			size_++;
		}
	}

	// Move constructor
	DynamicArray(DynamicArray&& arr) noexcept
	{
		data_ = arr.data_;
		size_ = arr.size_;
		capacity_ = arr.capacity_;

		arr.data_ = nullptr;
		arr.size_ = 0;
		arr.capacity_ = 0;
	}
	
	int Insert(const T& value)
	{
		if (size_ == capacity_)
		{
			IncreaseSize();
		}
		new (data_ + size_) T(value);
		size_++;
		return size_ - 1;
	}
	
	int Insert(int index, const T& value)
	{
		if (index > size_ || index < 0)
		{
			throw std::out_of_range("Target index was out of array bounds");
		}

		if (size_ == capacity_)
		{
			IncreaseSize();
		}

		/*if constexpr (std::is_move_constructible_v<T>)
		{*/
			for (int i = size_; i > index; --i)
			{
				new (data_ + i) T(std::move(data_[i - 1]));
				data_[i - 1].~T();
			}
		/*}
		else
		{
			for (int i = size_; i > index; i--)
			{
				new (data_ + i) T((data_[i - 1]));
				data_[i - 1].~T();
			}
		}*/
		new(data_ + index) T(value);
		size_++;
		return index;
	}

	// Remove from indexed position
	void Remove(int index)
	{
		if (index > size_ || index < 0)
		{
			throw std::out_of_range("Target index was out of array bounds");
		}

		data_[index].~T();
		/*if constexpr (std::is_move_constructible_v<T>)
		{*/
			for (int i = index; i < size_ - 1; ++i)
			{
				new (data_ + i) T(std::move(data_[i + 1]));
				data_[i + 1].~T();
			}
		/*}
		else
		{
			for (int i = index; i < size_ - 1; i++)
			{
				new (data_ + i) T(data_[i + 1]);
				data_[i + 1].~T();
			}
		}*/

		size_--;
	}


	T& operator[](int index)
	{
		return data_[index];
	}

	const T& operator[](int index) const
	{
		return data_[index];
	}

	int size() const
	{
		return size_;
	}
	
	class Iterator
	{
	private:
		DynamicArray<T>* owner_;
		int current_index_;
		bool reverse_traversal_;
		bool has_next_;
	public:
		Iterator(DynamicArray<T>* owner, const bool reverse_traversal)
		{
			owner_ = owner;
			reverse_traversal_ = reverse_traversal;
			has_next_ = owner_->size_ > 0;
			if (reverse_traversal_)
			{
				current_index_ = owner->size_ - 1;
			}
			else
			{
				current_index_ = 0;
			}
		}
		const T& get() const
		{
			return owner_->data_[current_index_];
		}

		void set(const T& value)
		{
			owner_->data_[current_index_] = value;
		}

		void next()
		{
			if (!has_next_)
				return;

			if (reverse_traversal_)
			{
				current_index_--;
				if (current_index_ == -1)
					has_next_ = false;
			}
			else
			{
				current_index_++;
				if (current_index_ == owner_->size_)
					has_next_ = false;
			}
		}

		bool hasNext() const
		{
			return has_next_;
		}
	};

	class ConstIterator
	{
	private:
		DynamicArray<T>* owner_;
		int current_index_;
		bool reverse_traversal_;
		bool has_next_;
	public:
		ConstIterator(DynamicArray<T>* owner, const bool reverse_traversal)
		{
			owner_ = owner;
			reverse_traversal_ = reverse_traversal;
			has_next_ = owner_->size_ > 0;
			if (reverse_traversal_)
			{
				current_index_ = owner->size_ - 1;
			}
			else
			{
				current_index_ = 0;
			}
		}
		const T& get() const
		{
			return owner_->data_[current_index_];
		}

		void next()
		{
			if (!has_next_)
				return;

			if (reverse_traversal_)
			{
				current_index_--;
				if (current_index_ == 0)
					has_next_ = false;
			}
			else
			{
				current_index_++;
				if (current_index_ == owner_->size_)
					has_next_ = false;
			}
		}

		bool hasNext() const
		{
			return has_next_;
		}
	};

	Iterator iterator()
	{
		Iterator iterator(this, false);
		return iterator;
	}

	const ConstIterator iterator() const
	{
		ConstIterator iterator(this, false);
		return iterator;
	}

	Iterator reversedIterator()
	{
		Iterator iterator(this, true);
		return iterator;
	}
	const ConstIterator reversedIterator() const
	{
		ConstIterator iterator(this, true);
		return iterator;
	}
};