#pragma once
#include "ts_queue.h"
#include "task.h"

namespace co
{

// 信号管理对象
// @线程安全
class BlockObject
{
protected:
    friend class Processer;
    std::size_t wakeup_;        // 当前信号数量
    std::size_t max_wakeup_;    // 可以积累的信号数量上限
    TSQueue<Task, false> wait_queue_;   // 等待信号的协程队列
    LFLock lock_;

public:
    explicit BlockObject(std::size_t init_wakeup = 0, std::size_t max_wakeup = -1);
    ~BlockObject();

    // 阻塞式等待信号
    void CoBlockWait();

    // 带超时的阻塞式等待信号
    // @returns: 是否成功等到信号
	bool CoBlockWaitTimed(MininumTimeDurationType timeo);

    template <typename R, typename P>
    bool CoBlockWaitTimed(std::chrono::duration<R, P> duration)
    {
        return CoBlockWaitTimed(std::chrono::duration_cast<MininumTimeDurationType>(duration));
    }

    template <typename DeadlineType>
    bool CoBlockWaitTimed(DeadlineType deadline)
    {
        auto now = std::chrono::system_clock::now();
        if (deadline < now)
            return CoBlockWaitTimed(MininumTimeDurationType(0));

        return CoBlockWaitTimed(std::chrono::duration_cast<MininumTimeDurationType>
                (deadline - std::chrono::system_clock::now()));
    }

    bool TryBlockWait();

    bool Wakeup();

    bool IsWakeup();

private:
    void CancelWait(Task* tk, uint32_t block_sequence);

    bool AddWaitTask(Task* tk);
};

} //namespace co
