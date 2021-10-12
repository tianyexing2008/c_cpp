
//
// 会在Function.h中包含多次，本头文件不能使用保护宏
#define FUNCTION_FUNCTION MACRO_JOIN(TFunction,FUNCTION_NUMBER)

//为了兼容原32位使用位保存类型加宏
#if (defined(_MSC_VER)&&defined(_WIN64))||(defined(__GNUC__)&&(__WORDSIZE == 64))
#define FUNCTION_USE_POINTER_OBJECT_TYPE
#endif

/// \class FUNCTION_FUNCTION
/// \brief 函数指针类模块
///
/// 支持普通函数指针和成员函数指针的保存，比较，调用等操作。对于成员函数，
/// 要求类只能是普通类或者是单继承的类，不能是多继承或虚继承的类。FUNCTION_FUNCTION是一个宏，
/// 根据参数个数会被替换成TFunctionN，用户通过TFunctionN<R, T1, T2, T3,..,TN>方式来使用，
/// R表示返回值类型，TN表示参数类型， N表示函数参数个数，目前最大参数为6。示例如下：
/// \code
/// int g(int x)
/// {
/// 	return x * 2;
/// }
/// class G
/// {
/// public:
/// 	int g(int x)
/// 	{
/// 		return x * 3;
/// 	}
/// }gg;
/// void test()
/// {
/// 	TFunction1<int, int> f1(g);
/// 	TFunction1<int, int> f2(&G::g, &gg);
/// 	assert(f1(1) = 2);
/// 	assert(f2(1) = 3);
/// 	f1 = f2;
/// 	assert(f1 == f2);
/// }
/// \endcode

template <FUNCTION_CLASS_TYPES>
class FUNCTION_FUNCTION
{
	class X{};

	enum FunctionType
	{
		typeEmpty,
		typeMember,
		typePointer,
		typeObject,
	};
	typedef R (X::*MEM_FUNCTION)(FUNCTION_TYPES);
	typedef R (*PTR_FUNCTION)(FUNCTION_TYPES);

	union
	{
		struct
		{
			MEM_FUNCTION proc;
			X* obj;
		}memFunction;
		PTR_FUNCTION ptrFunction;
	}m_function;

#ifdef FUNCTION_USE_POINTER_OBJECT_TYPE
	FunctionType m_type;
	const char* m_objectType;
#else//默认其他的都是32位，8位、16位和128位等暂不考虑
	FunctionType m_type:2;
	int m_objectType:30; //对象类型字符串，只有30位来表示，可怜
#endif
public:
	/// 缺省构造函数
	FUNCTION_FUNCTION( )
		:m_type(typeEmpty), m_objectType(0)
	{
	}

	/// 接受成员函数指针构造函数，将类的成员函数和类对象的指针绑定并保存。
	/// \param [in] f 类的成员函数
	/// \param [in] o 类对象的指针
	template<typename O>
		FUNCTION_FUNCTION(R(O::*f)(FUNCTION_TYPES), const O * o)
	{
		m_type = typeMember;
		m_function.memFunction.proc = horrible_cast<MEM_FUNCTION>(f); //what's safty, hei hei
		m_function.memFunction.obj = horrible_cast<X*>(o);
#ifdef FUNCTION_USE_POINTER_OBJECT_TYPE
		m_objectType = typeid(o).name();
#else
		m_objectType = (int)typeid(o).name() >> 2;
#endif
	}

	/// 接受普通函数指针构造函数，保存普通函数指针。
	/// \param [in] f 函数指针
	FUNCTION_FUNCTION(PTR_FUNCTION f)
	{
		m_type = typePointer;
		m_function.ptrFunction = f;
		m_objectType = 0;
	}

	/// 空构造函数，将函数指针设置为0
	/// \param [in] zero 只能为0，不能为其他值
	FUNCTION_FUNCTION(int zero)
	{
		m_type = typeEmpty;
		assert(zero == 0);
	}

	/// 赋值运算
	/// \param [in] f 源函数指针对象
	FUNCTION_FUNCTION& operator=(const FUNCTION_FUNCTION& f)
	{
		if (&f == this)
			return *this;
		m_function = f.m_function;
		m_type = f.m_type;
		m_objectType = f.m_objectType;
		return *this;
	}

	/// 将类的成员函数和类对象的指针绑定并保存。其他类型的函数指针可以=运算符和隐式转换直接完成。
	template<typename O>
	void bind(R(O::*f)(FUNCTION_TYPES), const O * o)
	{
		*this = FUNCTION_FUNCTION(f, o);
	}

	/// 判断函数指针是否为空
	bool empty() const
	{
		return (m_type == typeEmpty);
	}

	/// 比较两个函数指针是否相等
	bool operator==(const FUNCTION_FUNCTION& f) const
	{
		if(m_type != f.m_type)
		{
			return false;
		}
		if(m_type == typeMember)
		{
			return (m_function.memFunction.proc == f.m_function.memFunction.proc
				&& m_function.memFunction.obj == f.m_function.memFunction.obj);
		}
		else if(m_type == typePointer)
		{
			return (m_function.ptrFunction == f.m_function.ptrFunction);
		}
		else
		{
			return true;
		}
	}

	/// 重载()运算符，可以以函数对象的形式来调用保存的函数指针。
#ifndef WIN_VC6
	inline R operator()(FUNCTION_TYPE_ARGS)
	{
		if(m_type == typeMember)
		{
			return (m_function.memFunction.obj->*m_function.memFunction.proc)(FUNCTION_ARGS);
		}
		else
		{
			return m_function.ptrFunction(FUNCTION_ARGS);
		}
	}
#else
	///< 解决vc6下因为return不能返回void值，导致编译不过问题
	template<typename x, typename y>
	struct SameType
	{
		#undef FUNCTIONTEMPLATESAMETYPE
		#define FUNCTIONTEMPLATESAMETYPE 0
	};

	template<typename T>
	struct SameType<T, T>
	{
		#undef FUNCTIONTEMPLATESAMETYPE
		#define FUNCTIONTEMPLATESAMETYPE 1
	};

	inline R operator()(FUNCTION_TYPE_ARGS)
	{
		if(m_type == typeMember)
		{
			SameType<R, void> test;
#if FUNCTIONTEMPLATESAMETYPE == 0
			return (m_function.memFunction.obj->*m_function.memFunction.proc)(FUNCTION_ARGS);
#else
			(m_function.memFunction.obj->*m_function.memFunction.proc)(FUNCTION_ARGS);
#endif
		}
		else
		{
			SameType<R, void> test;
#if FUNCTIONTEMPLATESAMETYPE == 0
			return m_function.ptrFunction(FUNCTION_ARGS);
#else
			m_function.ptrFunction(FUNCTION_ARGS);
#endif
		}
	}
#endif

	void * getObject()
	{
		return m_function.memFunction.obj;
	}

	const char* getObjectType()
	{
#ifdef FUNCTION_USE_POINTER_OBJECT_TYPE
		return m_objectType;
#else
		return (const char *)(m_objectType << 2);
#endif
	}

	/// 组件化转三代
	// operator Dahua::Infra::FUNCTION_FUNCTION<FUNCTION_CLASS_TYPE>() const
	// {
	// 	if(m_type == typeMember)
	// 	{
	// 		return Dahua::Infra::FUNCTION_FUNCTION<FUNCTION_CLASS_TYPE>(m_function.memFunction.proc, m_function.memFunction.obj);
	// 	}
	// 	else
	// 	{
	// 		return Dahua::Infra::FUNCTION_FUNCTION<FUNCTION_CLASS_TYPE>(m_function.ptrFunction);
	// 	}
	// }

	/// 三代转组件化
// 	FUNCTION_FUNCTION(Dahua::Infra::FUNCTION_FUNCTION<FUNCTION_CLASS_TYPE> func)
// 	{
// 		typedef Dahua::Infra::FUNCTION_FUNCTION<FUNCTION_CLASS_TYPE> Arch3Type;
// 		if (func.m_type == Arch3Type::typePointer)
// 		{
// 			m_type = typePointer;
// 			m_function.ptrFunction = func.m_function.ptrFunction;
// 			m_objectType = 0;
// 		}
// 		else if (func.m_type == Arch3Type::typeMember)
// 		{
// 			m_type = typeMember;
// 			m_function.memFunction.proc = horrible_cast<MEM_FUNCTION>(func.m_function.memFunction.proc);
// 			m_function.memFunction.obj = horrible_cast<X*>(func.m_function.memFunction.obj);
// #ifdef FUNCTION_USE_POINTER_OBJECT_TYPE
// 			m_objectType = func.m_objectType;
// #else
// 			m_objectType = (int)(func.m_objectType) >> 2;
// #endif
// 		}
// 		else if ((uint32_t)func.m_type >= (uint32_t)Arch3Type::typeReuse && func.m_type != Arch3Type::typeAllReuse)
// 		{
// 			m_type = typeMember;
// 			m_function.memFunction.proc = horrible_cast<MEM_FUNCTION>(func.m_function.memFunction.proc);
// 			m_function.memFunction.obj = horrible_cast<X*>(func.m_function.memFunction.obj);
// #ifdef FUNCTION_USE_POINTER_OBJECT_TYPE
// 			m_objectType = func.m_objectType;
// #else
// 			m_objectType = (int)(func.m_objectType) >> 2;
// #endif
// 		}
// 		else
// 		{
// 			m_type = typeEmpty;
// 			m_objectType = 0;
// 		}
// 	}

};

/// 定义宏，表示支持三代转组件化
#ifndef INFRA_FUNCTION_SUPPORT_ARCH3_TO_COMPONENT
#define INFRA_FUNCTION_SUPPORT_ARCH3_TO_COMPONENT
#endif

#undef FUNCTION_FUNCTION

#undef FUNCTION_USE_POINTER_OBJECT_TYPE

