/// <summary>
/// 메모리 주소를 전부 사용안하는 것을 이용한 포인터 48비트 주소 체계에서만 사용가능
/// </summary>
/// <typeparam name="T"></typeparam>
/// 

#ifndef BUGAT_LOCKFREE_TAGGED_PTR_COMPRESS_INCLUDED
#define BUGAT_LOCKFREE_TAGGED_PTR_COMPRESS_INCLUDED

namespace bugat::lockfree
{
	template<typename T>
	class tagged_ptr
	{
	private:
		using compressed_ptr_t = std::uint64_t;
		using tag_t = std::uint16_t;
		static constexpr int tag_idx = 3;
		static constexpr compressed_ptr_t ptr_mask = (1LL << 48L) - 1;

	public:
		static constexpr tag_t MAX_TAG = (1 << 16) - 1;

	private:
		union pack_unit
		{
			compressed_ptr_t _value;
			tag_t _tag[4];
		};

		static compressed_ptr_t pack_ptr(T* ptr, tag_t tag)
		{
			pack_unit u;
			u._value = compressed_ptr_t(ptr);
			u._tag[tag_idx] = tag;
			return u._value;
		}

		static tag_t extract_tag(compressed_ptr_t ptr)
		{
			pack_unit u;
			u._value = ptr;
			return u._tag[tag_idx];
		}

	public:
		tagged_ptr() : _compressedPtr(0) {}
		explicit tagged_ptr(T* ptr, tag_t tag)
		{
			_compressedPtr = pack_ptr(ptr, tag);
		}

		void set(T* ptr, tag_t tag)
		{
			_compressedPtr = pack_ptr(ptr, tag);
		}

		T* get_ptr() const
		{
			return reinterpret_cast<T*>(_compressedPtr & ptr_mask);
		}

		tag_t get_tag()
		{
			return extract_tag(_compressedPtr);
		}

		tag_t get_next_tag()
		{
			return (extract_tag(_compressedPtr) + 1) % MAX_TAG;
		}

		T* operator->() const
		{
			return get_ptr();
		}

		T& operator*() const
		{
			return *get_ptr();
		}

		bool operator==(const tagged_ptr& p)
		{
			return _compressedPtr == p._compressedPtr;
		}

		bool operator!=(const tagged_ptr& p)
		{
			return _compressedPtr != p._compressedPtr;
		}

		operator bool() const
		{
			return get_ptr() != nullptr;
		}

	private:
		compressed_ptr_t _compressedPtr;
	};
}

#endif