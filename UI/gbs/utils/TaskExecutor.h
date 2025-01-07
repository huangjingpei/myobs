#pragma once
#include <iostream>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <chrono>
#include <atomic>
#include <unordered_set>
#include <QDebug>


class TaskExecutor {
public:
    TaskExecutor(const std::string& name)
        : name(name), stop(false), taskIdCounter(0) {
        workerThread = std::thread([this] { this->run(); });
    }

    ~TaskExecutor() {
        {
            std::unique_lock<std::mutex> lock(taskMutex);
            stop = true;
        }
        taskCondition.notify_all();
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }

    //// 添加普通任务
    //template <typename Callable, typename... Args>
    //auto addTask(Callable&& func, Args&&... args) -> std::future<typename std::invoke_result<Callable(Args...)>::type> {
    //    using ReturnType = typename std::invoke_result<Callable(Args...)>::type;
    //    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
    //        std::bind(std::forward<Callable>(func), std::forward<Args>(args)...)
    //    );
    //    std::future<ReturnType> result = task->get_future();

    //    {
    //        std::unique_lock<std::mutex> lock(taskMutex);
    //        taskQueue.push([task]() { (*task)(); });
    //    }

    //    taskCondition.notify_one();
    //    return result;
    //}

        // 添加普通任务
    template<typename Callable, typename... Args>
    auto addTask(Callable &&func, Args &&...args)
	    -> std::future<typename std::invoke_result<Callable, Args...>::type>
    {
	    using ReturnType =
		    typename std::invoke_result<Callable,
						Args...>::type; // 确定返回类型
	    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
		    std::bind(std::forward<Callable>(func),
			      std::forward<Args>(args)...));
	    std::future<ReturnType> result = task->get_future();

	    {
		    std::unique_lock<std::mutex> lock(taskMutex);
		    taskQueue.push([task]() { (*task)(); });
	    }

	    taskCondition.notify_one();
	    return result;
    }

    // 添加定时任务，延迟执行
    int64_t addTimedTask(int delayMs, std::function<void()> task) {
        int64_t taskId = generateTaskId();
        auto execTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(delayMs);

        {
            std::unique_lock<std::mutex> lock(taskMutex);
            timedTasks.push({ execTime, taskId, delayMs, task });
        }

        taskCondition.notify_one();
        return taskId;
    }

    // 删除定时任务
    bool removeTimedTask(int64_t taskId) {
        std::unique_lock<std::mutex> lock(taskMutex);
        removedTasks.insert(taskId);  // 标记任务为已删除
        return true;
    }

private:
    std::string name;
    std::thread workerThread;
    std::mutex taskMutex;
    std::condition_variable taskCondition;

    using Task = std::function<void()>;
    std::queue<Task> taskQueue; // 普通任务队列

    struct TimedTask {
        std::chrono::steady_clock::time_point execTime;
        int64_t taskId;
        int delayMs;  // 保存延迟时间
        Task task;

        // 优先级队列排序比较函数
        bool operator>(const TimedTask& other) const {
            return execTime > other.execTime;
        }
    };

    std::priority_queue<TimedTask, std::vector<TimedTask>, std::greater<>> timedTasks; // 定时任务优先队列
    std::unordered_set<int64_t> removedTasks; // 用于标记已删除的任务ID

    std::atomic<bool> stop;
    std::atomic<int64_t> taskIdCounter;

    // 生成唯一任务ID
    int64_t generateTaskId() {
        return ++taskIdCounter;
    }

    // 线程运行逻辑
    void run() {
        while (true) {
            Task taskToRun;

            {
                std::unique_lock<std::mutex> lock(taskMutex);

                // 等待直到有任务可执行
                taskCondition.wait(lock, [this] { return !taskQueue.empty() || !timedTasks.empty() || stop; });

                if (stop && taskQueue.empty() && timedTasks.empty()) {
                    return; // 退出线程
                }

                auto now = std::chrono::steady_clock::now();

                // 处理定时任务
                while (!timedTasks.empty() && timedTasks.top().execTime <= now) {
                    auto topTask = timedTasks.top();
                    timedTasks.pop();

                    if (removedTasks.find(topTask.taskId) == removedTasks.end()) {
                        // 任务没有被删除，执行它
                        taskToRun = topTask.task;
                        // 重新计算下一次执行时间
                        topTask.execTime += std::chrono::milliseconds(topTask.delayMs);
                        timedTasks.push(topTask); // 将任务重新放入队列
                        break;
                    }
                    else {
                        // 任务已被标记删除，跳过
                        removedTasks.erase(topTask.taskId);
                    }
                }

                // 处理普通任务（如果当前没有定时任务需要执行）
                if (!taskToRun && !taskQueue.empty()) {
                    taskToRun = std::move(taskQueue.front());
                    taskQueue.pop();
                }
            }

            // 执行任务
            if (taskToRun) {
		auto start_time = std::chrono::steady_clock::now();
                taskToRun();
		auto end_time = std::chrono::steady_clock::now();
		auto execution_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time);
		if (execution_time.count() > 50) {
			qWarning("exec time is too long, spent %d (ms)\n", execution_time.count());
		}

            }
        }
    }
};
