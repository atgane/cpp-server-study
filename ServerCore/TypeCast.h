#pragma once
#include "Types.h"

#pragma region TypeList

template<typename... T>
struct TypeList;

// 1. 2개인 경우의 해석
template<typename T, typename U>
struct TypeList<T, U>
{
	using Head = T;
	using Tail = U;
};

// n(>2)의 경우의 해석(재귀적으로 호출
template<typename T, typename... U>
struct TypeList<T, U...>
{
	using Head = T;
	using Tail = TypeList<U...>;
};

#pragma endregion

#pragma region Length

template<typename T>
struct Length;

template<>
struct Length<TypeList<>>
{
	enum {value = 0 };
};

template<typename T, typename... U>
struct Length<TypeList<T, U... >>
{
	enum { value = 1 + Length<TypeList<U...>>::value };
};

#pragma endregion

#pragma region TypeAt

template<typename  TL, int32 index>
struct TypeAt;

template<typename Head, typename... Tail>
struct TypeAt<TypeList<Head, Tail...>, 0>
{
	using Result = Head;
};

template<typename Head, typename... Tail, int32 index>
struct TypeAt<TypeList<Head, Tail...>, index>
{
	using Result = typename TypeAt<TypeList<Tail...>, index - 1>::Result;
};

#pragma endregion

#pragma region IndexOf

template<typename TL, typename T>
struct IndexOf;

template< typename... Tail, typename T>
struct IndexOf<TypeList<T, Tail...>, T>
{
	enum { value = 0 };
};

template<typename T>
struct IndexOf<TypeList<>, T>
{
	enum { value = -1 };
};

template<typename Head, typename... Tail, typename T>
struct IndexOf<TypeList<Head, Tail... >, T>
{
private:
	enum { temp = IndexOf<TypeList<Tail...>, T>::value };

public:
	enum { value = (temp == -1) ? -1: temp - 1 };
};

#pragma endregion

// 자료형의 변환이 가능한지를 체크
#pragma region Conversion

template<typename From, typename To>
class Conversion
{
private:
	using Small = __int8;
	using Big = __int32;

	static Small Test(const To&) { return 0; }
	static Big Test(...) { return 0; }
	static From MakeFrom() { return 0; }

public:
	enum
	{
		// 2. 변환이 존재한다면, MakeFrom은 from을 반환하는데
		// from이 to로 반환가능하다면 Small을 반환하는 Test가 호출되어 true
		// 다른 경우, Big을 반환하는 Test가 호출되어 false
		exists = sizeof(Test(MakeFrom())) == sizeof(Small)
	};
};

#pragma endregion

#pragma region TypeCast

template<int32 V>
// 4. 이를 이용하면 정수를 타입처럼 만들어서 사용할 수 있다. 
struct int2Type 
{
	enum { value = V };
};

template<typename TL>
class TypeConversion
{
public:
	enum
	{
		length = Length<TL>::value
	};

	TypeConversion()
	{
		// 3. 일반적인 생성자라면 아래와 같은 코드를 만들고 싶은 것
		//for (int i = 0; i < length; i++)
		//{
		//	for (int j = 0; j < length; j++)
		//	{
		//		using FromType = typename TypeAt<TL, i>::Result;
		//		using ToType = typename TypeAt<TL, j>::Result;
		//		if (Conversion<const FromType*, const ToType*>::exists)
		//			s_convert[i][j] = true;
		//		else
		//			s_convert[i][j] = false;
		//	}
		//}
		// 그런데 이따구로 코드짜면 뒤지게 맞는데 왜냐면 
		// 컴파일 타임에 for문을 확인하지 못하기 때문이다. 
		// 그래서 컴파일 타임에 확인할 수 있도록 코드를
		// 아래와 같이 변경하도록 한다.

		MakeTable(int2Type<0>(), int2Type<0>());
	}

	template<int32 i, int32 j>
	static void MakeTable(int2Type<i>, int2Type<j>)
	{
		using FromType = typename TypeAt<TL, i>::Result;
		using ToType = typename TypeAt<TL, j>::Result;
		if (Conversion<const FromType*, const ToType*>::exists)
			s_convert[i][j] = true;
		else
			s_convert[i][j] = false;
		
		// 우리는 for문을 못쓰는 정신나간 환경에 있기 때문에
		// 여기서 재귀적으로 for문을 구현하도록 한다. 

		MakeTable(int2Type<i>(), int2Type<j + 1>());
	}

	template<int32 i>
	static void MakeTable(int2Type<i>, int2Type<length>)
	{
		MakeTable(int2Type<i + 1>(), int2Type<0>());
	}

	template<int32 j>
	static void MakeTable(int2Type<length>, int2Type<j>)
	{
		// 더 이상 호출할 게 없다! for 문 완전 구현
	}

	static inline bool CanConvert(int32 from, int32 to)
	{
		static type Conversion conversion;
		return s_convert[from][to];
	}

public:
	// 타입 캐스팅 테이블
	static bool s_convert[length][length];
};

template<typename TL>
bool TypeConversion<TL>::s_convert[length][length];

// 어쩌면 dynamic cast의 구현 from에서 to로의 형변환을 안전하게 진행
template<typename To, typename From>
To TypeCast(From* ptr)
{
	if (ptr == nullptr)
		return nullptr;

	using TL = typename From::TL;

	// 객체 내부에 _typeId 속성이 있어야 해요
	// _typeId를 먼저 생성자에서 채우자. 
	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
		return static_cast<to>(ptr);

	return nullptr;
}

template<typename To, typename From>
shared_ptr<To> TypeCast(shared_ptr<From> ptr)
{
	if (ptr == nullptr)
		return nullptr;

	using TL = typename From::TL;

	// 객체 내부에 _typeId 속성이 있어야 해요
	// _typeId를 먼저 생성자에서 채우자. 
	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
		return static_pointer_cast<to>(ptr);

	return nullptr;
}

template<typename To, typename From>
bool CanCast(From* ptr)
{
	if (ptr == nullptr)
		return false;
	using TL = typename From::TL;


	return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value);
}

template<typename To, typename From>
bool CanCast(shared_ptr<From> ptr)
{
	if (ptr == nullptr)
		return false;
	using TL = typename From::TL;


	return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value);
}

#pragma endregion

#define DECLARE_TL using TL = TL : int32 _typeId;
#define INT_TL(Type) _typeId\ = IndexOf<TL, Type>::value;