#ifndef OBJECT_H
#define OBJECT_H

typedef int TId;

//所有有ID对象的基类
template<typename T> 
class Object
{
public:
	TId ID() const;
protected:
	Object();
	Object(TId id);
	virtual ~Object() = default;
private:
	static int s_count;
	TId m_id;
};

template<typename T>
TId Object<T>::ID() const
{
	return m_id;
}

//默认构造函数，递增count
template<typename T>
Object<T>::Object()
{
	m_id = s_count++;
}

//拷贝时的构造函数，不递增count
template<typename T>
Object<T>::Object(TId id):m_id(id){}

template<typename T>
int Object<T>::s_count = 0;

#endif  //OBJECT_H