/*!
 * @brief	NoncopyÉ|ÉäÉVÅ[
 */
 
#ifndef _TKNONCOPYABLE_H_
#define _TKNONCOPYABLE_H_

namespace tkEngine2{
	struct Noncopyable{
		Noncopyable() = default;
	    Noncopyable(const Noncopyable&) = delete;
    	Noncopyable& operator=(const Noncopyable&) = delete;
	};
}

#endif // _TKNONCOPYABLE_H_