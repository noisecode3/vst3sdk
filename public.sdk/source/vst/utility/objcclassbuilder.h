//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Helpers
// Filename    : public.sdk/source/vst/utility/objcclassbuilder.h
// Created by  : Steinberg, 06/2022
// Description : Objective-C class builder utilities
//
//-----------------------------------------------------------------------------
// LICENSE
// (c) 2022, Steinberg Media Technologies GmbH, All Rights Reserved
//-----------------------------------------------------------------------------
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of the Steinberg Media Technologies nor the names of its
//     contributors may be used to endorse or promote products derived from this
//     software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#pragma once

#include <objc/runtime.h>
#include <objc/message.h>
#include <tuple>
#include <string>
#include <optional>
#include <cmath>
#include <cassert>

//------------------------------------------------------------------------------------
namespace Steinberg {

//------------------------------------------------------------------------------------
template<typename T>
struct ObjCVariable
{
	ObjCVariable (__unsafe_unretained id obj, Ivar ivar) : obj (obj), ivar (ivar) {}
	ObjCVariable (ObjCVariable&& o) { *this = std::move (o); }

	ObjCVariable& operator= (ObjCVariable&& o)
	{
		obj = o.obj;
		ivar = o.ivar;
		o.obj = nullptr;
		o.ivar = nullptr;
		return *this;
	}

	T get () const 
	{
		return (__bridge T) (reinterpret_cast<intptr_t> (object_getIvar (obj, ivar)));
	}

	void set (const T& value)
	{
		object_setIvar (obj, ivar, (__bridge id) reinterpret_cast<id> (value));
	}

private:
	__unsafe_unretained id obj;
	Ivar ivar {nullptr};
};

//------------------------------------------------------------------------------------
struct ObjCInstance
{
	ObjCInstance (__unsafe_unretained id obj) : obj (obj) {}

	template<typename T>
	std::optional<ObjCVariable<T>> getVariable (const char* name) const
	{
		if (__strong auto ivar = class_getInstanceVariable (object_getClass (obj), name))
		{
			return {ObjCVariable<T> (obj, ivar)};
		}
		return {};
	}

	template<typename Func, typename... T>
	void callSuper (SEL selector, T... args) const
	{
		void (*f) (__unsafe_unretained id, SEL, T...) =
			(void (*) (__unsafe_unretained id, SEL, T...))objc_msgSendSuper;
		f (getSuper (), selector, args...);
	}

	template<typename Func, typename R, typename... T>
	R callSuper (SEL selector, T... args) const
	{
		R (*f)
		(__unsafe_unretained id, SEL, T...) =
			(R (*) (__unsafe_unretained id, SEL, T...))objc_msgSendSuper;
		return f (getSuper (), selector, args...);
	}

private:
	id getSuper () const
	{
		if (os.receiver == nullptr)
		{
			os.receiver = obj;
			os.super_class = class_getSuperclass (object_getClass (obj));
		}
		return (__bridge id) (&os);
	}

	__unsafe_unretained id obj;
	mutable objc_super os {};
};

//------------------------------------------------------------------------------------
struct ObjCClassBuilder
{
	ObjCClassBuilder& init (const char* name, Class baseClass);

	template<typename Func>
	ObjCClassBuilder& addMethod (SEL selector, Func imp);
	template<typename T>
	ObjCClassBuilder& addIvar (const char* name);

	ObjCClassBuilder& addProtocol (const char* name);
	ObjCClassBuilder& addProtocol (Protocol* proto);

	Class finalize ();

private:
	static Class generateUniqueClass (const std::string& inClassName, Class baseClass);

	template<typename Func>
	ObjCClassBuilder& addMethod (SEL selector, Func imp, const char* types);

	ObjCClassBuilder& addIvar (const char* name, size_t size, uint8_t alignment, const char* types);

	template<typename R, typename... T>
	static constexpr std::tuple<R, T...> functionArgs (R (*) (T...))
	{
		return std::tuple<R, T...> ();
	}

	template<typename... T>
	static constexpr std::tuple<T...> functionArgs (void (*) (T...))
	{
		return std::tuple<T...> ();
	}

	template<typename R, typename... T>
	static constexpr bool isVoidReturnType (R (*) (T...))
	{
		return false;
	}

	template<typename... T>
	static constexpr bool isVoidReturnType (void (*) (T...))
	{
		return true;
	}

	template<typename Proc>
	static std::string encodeFunction (Proc proc)
	{
		std::string result;
		if (isVoidReturnType (proc))
			result = "v";
		std::apply ([&] (auto&&... args) { ((result += @encode (decltype (args))), ...); },
					functionArgs (proc));
		return result;
	}

	Class cl {nullptr};
	Class baseClass {nullptr};
};

//------------------------------------------------------------------------
inline ObjCClassBuilder& ObjCClassBuilder::init (const char* name, Class bc)
{
	baseClass = bc;
	cl = generateUniqueClass (name, baseClass);
	return *this;
}

//------------------------------------------------------------------------------------
inline Class ObjCClassBuilder::generateUniqueClass (const std::string& inClassName, Class baseClass)
{
	std::string className (inClassName);
	int32_t iteration = 0;
	while (objc_lookUpClass (className.data ()) != nil)
	{
		iteration++;
		className = inClassName + "_" + std::to_string (iteration);
	}
	Class resClass = objc_allocateClassPair (baseClass, className.data (), 0);
	return resClass;
}

//-----------------------------------------------------------------------------
inline Class ObjCClassBuilder::finalize ()
{
	objc_registerClassPair (cl);

	auto res = cl;
	baseClass = cl = nullptr;
	return res;
}

//-----------------------------------------------------------------------------
template<typename Func>
inline ObjCClassBuilder& ObjCClassBuilder::addMethod (SEL selector, Func imp, const char* types)
{
	auto res = class_addMethod (cl, selector, IMP (imp), types);
	assert (res == true);
	(void)res;
	return *this;
}

//-----------------------------------------------------------------------------
template<typename Func>
ObjCClassBuilder& ObjCClassBuilder::addMethod (SEL selector, Func imp)
{
	return addMethod (selector, imp, encodeFunction (imp).data ());
}

//------------------------------------------------------------------------
template<typename T>
inline ObjCClassBuilder& ObjCClassBuilder::addIvar (const char* name)
{
	return addIvar (name, sizeof (T), static_cast<uint8_t> (std::log2 (sizeof (T))), @encode (T));
}

//-----------------------------------------------------------------------------
inline ObjCClassBuilder& ObjCClassBuilder::addIvar (const char* name, size_t size,
													uint8_t alignment, const char* types)
{
	auto res = class_addIvar (cl, name, size, alignment, types);
	assert (res == true);
	(void)res;
	return *this;
}

//-----------------------------------------------------------------------------
inline ObjCClassBuilder& ObjCClassBuilder::addProtocol (const char* name)
{
	if (auto protocol = objc_getProtocol (name))
		return addProtocol (protocol);
	return *this;
}

//-----------------------------------------------------------------------------
inline ObjCClassBuilder& ObjCClassBuilder::addProtocol (Protocol* proto)
{
	auto res = class_addProtocol (cl, proto);
	assert (res == true);
	(void)res;
	return *this;
}

//------------------------------------------------------------------------------------
} // Steinberg
