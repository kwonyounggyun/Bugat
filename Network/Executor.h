#include <atomic>

//class ThreadExecutor 
//{
//	using executor_type = ThreadExecutor;
//    executor_type get_executor() const noexcept;
//    bool operator==(const ThreadExecutor&) const noexcept { return true; }
//    bool operator!=(const ThreadExecutor&) const noexcept { return false; }
//
//    template <typename F>
//    void execute(F&& f) const
//    {
//
//    }
//
//    void on_work_started() const {_workCount}
//    void on_work_finished() const;
//
//private:
//	std::atomic<int> _workCount{ 0 };
//};