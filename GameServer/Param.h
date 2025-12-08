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
		using ValueType = int32_t;
		static constexpr int32_t MAX = 1 << 30;
		static constexpr int32_t MIN = -1 * MAX;
		static constexpr int decimal = 10000;

	public:
		Param() : _value(0) {}
		Param(float value) : _value(static_cast<ValueType>(value * decimal)) {}

		template<typename T>
		requires std::is_integral_v<T> || std::is_floating_point_v<T>
		void operator-=(T delta)
		{
			(*this) -= Param(delta);
		}

		void operator-=(const Param& delta)
		{
			(*this) - delta;
		}

		template<typename T>
		requires std::is_integral_v<T> || std::is_floating_point_v<T>
		void operator+=(T delta)
		{
			(*this) += Param(delta);
		}

		void operator+=(const Param& delta)
		{
			(*this) + delta;
		}

		template<typename T>
		requires std::is_integral_v<T> || std::is_floating_point_v<T>
		void operator*=(T delta)
		{
			(*this) *= Param(delta);
		}

		void operator*=(const Param& delta)
		{
			(*this) * delta;
		}

		template<typename T>
		requires std::is_integral_v<T> || std::is_floating_point_v<T>
		void operator/=(T delta)
		{
			(*this) /= Param(delta);
		}

		void operator/=(const Param& delta)
		{
			(*this) / delta;
		}

		Param& operator-(const Param& delta)
		{
			int64_t value = _value - delta._value;
			if (value > MAX)
				value = MAX;
			else if (value < MIN)
				value = MIN;

			_value = static_cast<ValueType>(value);
			return *this;
		}

		Param& operator+(const Param& delta)
		{
			int64_t value = _value + delta._value;
			if (value > MAX)
				value = MAX;
			else if (value < MIN)
				value = MIN;

			_value = static_cast<ValueType>(value);
			return *this;
		}

		Param& operator*(const Param& delta)
		{
			auto v = delta.GetValue();
			int64_t value = static_cast<int64_t>((double)_value * v / decimal);
			if (value > MAX)
				value = MAX;
			else if (value < MIN)
				value = MIN;

			_value = static_cast<ValueType>(value);
			return *this;
		}

		Param & operator/(const Param& delta)
		{
			auto v = delta.GetValue();
			if (v == 0)
				v = 1;

			int64_t value = static_cast<int64_t>((double)_value / v / decimal);
			if (value > MAX)
				value = MAX;
			else if (value < MIN)
				value = MIN;

			_value = static_cast<ValueType>(value);
			return *this;
		}

		float GetValue() const { return (float)_value / decimal; }

	protected:
		ValueType _value;
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