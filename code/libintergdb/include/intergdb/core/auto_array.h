#ifndef AUTO_ARRAY_H_
#define AUTO_ARRAY_H_

template<typename T>
class auto_array
{
public:
    auto_array(T * data=0)
      : data_(data)
    {}
    auto_array(auto_array && other)
      : data_(other.release())
    {
        other.data_ = nullptr;
    }
    ~auto_array()
    {
        delete[] data_;
    }
    T * get()
    {
        return data_;
    }
    T * release()
    {
        T * data = data_;
        data_ = 0;
        return data;
    }
    void reset(T * data)
    {
        if (data!=data_) {
            delete [] data_;
            data_ = data;
        }
    }
private:
    T * data_;
};

#endif /* AUTO_ARRAY_H_ */
