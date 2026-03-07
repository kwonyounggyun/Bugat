/// <summary>
/// Double compare and swap Áö¿ø ½Ã
/// </summary>
/// <typeparam name="T"></typeparam>
 
#ifndef BUGAT_LOCKFREE_TAGGED_PTR_DCAS_INCLUDED
#define BUGAT_LOCKFREE_TAGGED_PTR_DCAS_INCLUDED

namespace bugat::lockfree
{
	template<typename T>
	class alignas(16) tagged_ptr
	{
		using tag_t = std::uint64_t;
	public:
		static constexpr std::uint64_t MAX_TAG = (1LL << 63);
		tagged_ptr() = default;
		explicit tagged_ptr(T* ptr, tag_t tag) : _ptr(ptr), _tag(tag)
		{
		}

		void set(T* ptr, tag_t tag)
		{
			_ptr = ptr;
			_tag = tag;
		}

		T* get_ptr()
		{
			return _ptr;
		}

		tag_t get_tag()
		{
			return _tag;
		}

		tag_t get_next_tag()
		{
			return (_tag + 1) % MAX_TAG;
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
			return _ptr == p._ptr && _tag == p._tag;
		}

		bool operator!=(const tagged_ptr& p)
		{
			return _ptr != p._ptr || _tag != p._tag;
		}

		operator bool() const
		{
			return get_ptr() != nullptr;
		}

	private:
		T* _ptr;
		tag_t _tag;
	};
}

#endif