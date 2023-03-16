#include "threads.h"


template <typename F, typename... Args>
auto add_task(F&& f, Args&& ...args)
{
    std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    func();
    // auto task_ptr = std::make_shared<std::packaged_task<decltype<f(...args)>()>>(func);
    // svc = std::function([task_ptr]{
    //     (*task_ptr)();
    // });
    // return task_ptr->get_future();
}
void addd(int& output, int a, int b)
{
    output = a + b;
}
int main()
{
    int out = 0;
    add_task(addd, std::ref(out), 10, 20);
    std::cout<<out<<std::endl;
    return 0;
}