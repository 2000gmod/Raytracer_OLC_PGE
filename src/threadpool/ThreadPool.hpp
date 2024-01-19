//
// Created by yo on 27-10-23.
//

#ifndef TEST1_THREADPOOL_HPP
#define TEST1_THREADPOOL_HPP

#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <memory>
#include <concepts>

namespace ll {
    typedef std::function<void()> Task;

    class ThreadPool {
        public:
            explicit ThreadPool(size_t numThreads = std::thread::hardware_concurrency());

            ThreadPool(const ThreadPool &) = delete;

            ~ThreadPool();

        private:
            std::vector<std::jthread> threads;
            std::queue<Task> tasks;
            std::condition_variable cv, doneCV;
            std::mutex queueMutex, workerMutex, doneMutex;
            bool stopped;
            bool paused;
            std::vector<bool> workflags;

            void WorkerFunc(int tid);

            bool AllThreadsDone();
            void Start();
            void JoinThreads();

        public:

            void Stop();

            void Pause();

            void Resume();

            void Clear();

            void WaitUntilEmpty();

            std::string StatusString();

            template <class F, class... Args>
            requires requires (const F f, const Args... args) {
                f(args...);
            }
            auto Submit(const F &func, const Args &... args) -> std::future<decltype(func(args...))> {
                using Res = decltype(func(args...));
                auto taskPromise = std::make_shared<std::promise<Res>>();
                auto taskFunc = [&func, &args...] () {
                    return func(args...);
                };

                Push([taskFunc, taskPromise] {
                    try {
                        if constexpr (std::is_void_v<Res>) {
                            taskFunc();
                            taskPromise->set_value();
                        } else {
                            taskPromise->set_value(taskFunc());
                        }
                    }
                    catch (...) {
                        try {
                            taskPromise->set_exception(std::current_exception());
                        }
                        catch (...) { }
                    }
                });
                return taskPromise->get_future();
            }

            template <class F>
            requires requires (const F f) {
                f();
            }
            auto Submit(const F &func) -> std::future<decltype(func())> {
                using Res = decltype(func());
                auto taskPromise = std::make_shared<std::promise<Res>>();

                Push([&taskFunc = func, taskPromise] {
                    try {
                        if constexpr (std::is_void_v<Res>) {
                            taskFunc();
                            taskPromise->set_value();
                        } else {
                            taskPromise->set_value(taskFunc);
                        }
                    }
                    catch (...) {
                        try {
                            taskPromise->set_exception(std::current_exception());
                        }
                        catch (...) { }
                    }
                });
                return taskPromise->get_future();
            }

        private:
            template <class F, class... Args>
            requires requires (const F f, const Args... args) {
                f(args...);
            }
            void Push(const F &func, const Args &... args) {
                static_assert(std::is_void_v<decltype(func(args...))>, "Use ll::ThreadPool::Submit when return type is not void");
                std::lock_guard lock(queueMutex);
                auto f = [&func, &args...] -> void {
                    func(args...);
                };
                tasks.push(f);
                cv.notify_one();
            }

            template <class F>
            requires requires (const F f) {
                f();
            }
            void Push(const F &func) {
                static_assert(std::is_void_v<decltype(func())>, "Use ll::ThreadPool::Submit when return type is not void");
                std::lock_guard lock(queueMutex);
                tasks.push(func);
                cv.notify_one();
            }
    };

} // ll

#endif //TEST1_THREADPOOL_HPP
