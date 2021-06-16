
#ifndef PRO_SMARTBOX_APP_CIRCULARBUFFER_H
#define PRO_SMARTBOX_APP_CIRCULARBUFFER_H

#include <boost/circular_buffer.hpp>
#include <boost/utility.hpp>
#include <mutex>
#include <condition_variable>

/**
 * 实现循环缓冲区，这个循环缓冲区是基于BOOST库实现的， 实现的是可覆盖读写的循环缓冲区。
 * 可用在码流发送和视频原始帧读取。
 */

template <class T>
class CircularBuffer: public boost::noncopyable
{
public:
    typedef boost::circular_buffer<T> BufferType;
    typedef typename BufferType::size_type size_type;
    typedef typename BufferType::value_type value_type;

    /**
     * 构造函数
     * @param size 设置缓冲区的大小
     */
    explicit CircularBuffer(size_type size) : m_Qeque(size)
    {}

    /**
     * 在缓冲区中放入数据
     * @param data 数据
     */
    void putData(const value_type &data)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_Qeque.push_back(data);
        }

        m_cndNotEmpty.notify_one();
    }

    /**
     * 从缓冲区的尾部获得数据
     * @param data 数据
     */
    void getData(value_type &data)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_Qeque.empty())
        {
            m_cndNotEmpty.wait(lock);
        }

        data = m_Qeque.front();
        m_Qeque.pop_front();
    }

    /**
     * 返回当前循环缓冲区中元素的个数
     * @return
     */
    int getCurrentSize()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_Qeque.size();
    }

private:
    BufferType m_Qeque;
    std::mutex m_mutex;
    std::condition_variable m_cndNotEmpty;
};

#endif //PRO_SMARTBOX_APP_CIRCULARBUFFER_H
