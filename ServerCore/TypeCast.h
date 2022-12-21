#pragma once
#include "Types.h"

#pragma region TypeList

template<typename... T>
struct TypeList;

// 1. 2���� ����� �ؼ�
template<typename T, typename U>
struct TypeList<T, U>
{
	using Head = T;
	using Tail = U;
};

// n(>2)�� ����� �ؼ�(��������� ȣ��
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

// �ڷ����� ��ȯ�� ���������� üũ
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
		// 2. ��ȯ�� �����Ѵٸ�, MakeFrom�� from�� ��ȯ�ϴµ�
		// from�� to�� ��ȯ�����ϴٸ� Small�� ��ȯ�ϴ� Test�� ȣ��Ǿ� true
		// �ٸ� ���, Big�� ��ȯ�ϴ� Test�� ȣ��Ǿ� false
		exists = sizeof(Test(MakeFrom())) == sizeof(Small)
	};
};

#pragma endregion

#pragma region TypeCast

template<int32 V>
// 4. �̸� �̿��ϸ� ������ Ÿ��ó�� ���� ����� �� �ִ�. 
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
		// 3. �Ϲ����� �����ڶ�� �Ʒ��� ���� �ڵ带 ����� ���� ��
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
		// �׷��� �̵����� �ڵ�¥�� ������ �´µ� �ֳĸ� 
		// ������ Ÿ�ӿ� for���� Ȯ������ ���ϱ� �����̴�. 
		// �׷��� ������ Ÿ�ӿ� Ȯ���� �� �ֵ��� �ڵ带
		// �Ʒ��� ���� �����ϵ��� �Ѵ�.

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
		
		// �츮�� for���� ������ ���ų��� ȯ�濡 �ֱ� ������
		// ���⼭ ��������� for���� �����ϵ��� �Ѵ�. 

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
		// �� �̻� ȣ���� �� ����! for �� ���� ����
	}

	static inline bool CanConvert(int32 from, int32 to)
	{
		static type Conversion conversion;
		return s_convert[from][to];
	}

public:
	// Ÿ�� ĳ���� ���̺�
	static bool s_convert[length][length];
};

template<typename TL>
bool TypeConversion<TL>::s_convert[length][length];

// ��¼�� dynamic cast�� ���� from���� to���� ����ȯ�� �����ϰ� ����
template<typename To, typename From>
To TypeCast(From* ptr)
{
	if (ptr == nullptr)
		return nullptr;

	using TL = typename From::TL;

	// ��ü ���ο� _typeId �Ӽ��� �־�� �ؿ�
	// _typeId�� ���� �����ڿ��� ä����. 
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

	// ��ü ���ο� _typeId �Ӽ��� �־�� �ؿ�
	// _typeId�� ���� �����ڿ��� ä����. 
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