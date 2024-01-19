//
// Created by yo on 27-10-23.
//

#include "ThreadPool.hpp"
#include <iostream>

using namespace ll;

ThreadPool::ThreadPool(const size_t numThreads) {
    if (numThreads == 0) {
        throw std::invalid_argument("Tried to create ThreadPool with 0 threads.");
    }
    stopped = false;
    paused = true;
    for (auto i = 0; i < numThreads; i++) {
        workflags.emplace_back(false);
        threads.emplace_back(&ThreadPool::WorkerFunc, this, i);
    }
    Start();
}

ThreadPool::~ThreadPool() {
    if (stopped || paused || tasks.empty()) {
        stopped = true;
        cv.notify_all();
        return;
    }
    WaitUntilEmpty();
    Stop();
    JoinThreads();
}

void ThreadPool::Start() {
    stopped = false;
    paused = false;
    cv.notify_all();
}

void ThreadPool::JoinThreads() {
    for (auto &t : threads) {
        t.join();
    }
}

void ThreadPool::Stop() {
    stopped = true;
    cv.notify_all();
}

void ThreadPool::Pause() {
    paused = true;
}

void ThreadPool::WorkerFunc(int tid) {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock lock(workerMutex);
            cv.wait(lock, [this] { return (!tasks.empty() && !paused) || stopped; });
            if (stopped) {
                return;
            }

            std::lock_guard _(queueMutex);
            job = std::move(tasks.front());
            tasks.pop();

            workflags.at(tid) = true;
        }
        job();
        {
            std::lock_guard _(workerMutex);
            workflags.at(tid) = false;
        }
        cv.notify_one();
        if (tasks.empty()) doneCV.notify_all();
    }
}

void ThreadPool::Clear() {
    std::lock_guard lock(queueMutex);
    tasks = {};
}

void ThreadPool::WaitUntilEmpty() {
    std::unique_lock lock(doneMutex);
    doneCV.wait(lock, [this]{return tasks.empty() && AllThreadsDone();});
}

void ThreadPool::Resume() {
    paused = false;
    cv.notify_all();
}

bool ThreadPool::AllThreadsDone() {
    std::lock_guard _(workerMutex);
    for (const auto b : workflags) {
        if (b) return false;
    }
    return true;
}

std::string ThreadPool::StatusString() {
    std::lock_guard wm(workerMutex), qm(queueMutex);
    std::string ret = "[ThreadPool | Remaining Tasks: ";
    ret += std::to_string(tasks.size());
    ret += " | WF: [";
    for (const auto b : workflags) {
        ret += b ? "1 " : "0 ";
    }
    ret += "\b] N:" + std::to_string(threads.size()) + "]";
    return ret;
}
