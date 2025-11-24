#pragma once
#include "../Core/Event.h"
#include "../Core/NonCopyable.h"
#include <type_traits>

namespace bugat
{
	/*
	* 소수점 4자리까지만 보장
	*/
	class Param
	{
		static constexpr int32_t MAX = 1 << 30;
		static constexpr int32_t MIN = -1 * MAX;
		static constexpr int decimal = 10000;

	public:
		Param() : _value(0) {}
		Param(int32_t value) : _value(value) {}

		template<typename T>
		requires std::is_integral_v<T>
		void operator-=(T delta)
		{
			_value = _value - delta * decimal;
		}

		void operator-=(const Param& delta)
		{
			_value = _value - delta._value;
		}

		template<typename T>
		requires std::is_integral_v<T>
		void operator+=(T delta)
		{
			_value = _value + delta * decimal;
		}

		void operator+=(const Param& delta)
		{
			_value = _value + delta._value;
		}

		template<typename T>
		requires std::is_integral_v<T>
		void operator*=(T delta)
		{
			_value = _value * delta;
		}

		void operator*=(const Param& delta)
		{
			_value = _value * delta.GetFValue();
		}

		template<typename T>
		requires std::is_integral_v<T>
		void operator/=(T delta)
		{
			_value = _value / delta;
		}

		void operator/=(const Param& delta)
		{
			_value = _value / delta.GetFValue();
		}

		template<typename T>
		Param& operator-(T delta)
		{
			_value -= delta;
			return *this;
		}

		template<typename T>
		Param& operator+(T delta)
		{
			_value += delta;
			return *this;
		}

		template<typename T>
		Param& operator*(T delta)
		{
			_value *= delta;
			return *this;
		}

		template<typename T>
		Param & operator/(T delta)
		{
			_value /= delta;
			return *this;
		}

		int32_t GetValue() const { return _value / decimal; }
		float GetFValue() const { return _value / decimal; }

	protected:
		int32_t _value;
	};

	class EventParam : public NonCopyable
	{
	public:
		EventParam() {}
		EventParam(Param param) : _param(param) {}
		virtual ~EventParam() {}

		void operator=(const Param& newParam)
		{
			auto oldParam = _param;
			_param = newParam;
			LimitCheck(_param);
			OnChange(oldParam, newParam);
		}

		template<typename T>
		Param& operator-(T delta)
		{
			_param -= delta;
			return _param;
		}

		template<typename T>
		Param& operator+(T delta)
		{
			_param += delta;
			return _param;
		}

		template<typename T>
		Param& operator*(T delta)
		{
			_param *= delta;
			return _param;
		}

		template<typename T>
		Param& operator/(T delta)
		{
			_param /= delta;
			return *this;
		}

		Event<const Param&, const Param&> OnChange;
		Event<const Param&> LimitCheck;

	private:
		Param _param;
	};
}