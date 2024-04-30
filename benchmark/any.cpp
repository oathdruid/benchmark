#include "babylon/any.h"

#include "babylon/logging/interface.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/time/clock.h"

#include <any>
#include <list>
#include <unistd.h>
#include <vector>

ABSL_FLAG(uint64_t, times, 100, "");
ABSL_FLAG(uint64_t, num, 128, "");

using ::babylon::Any;
using ::babylon::TypeId;

template <typename T>
ABSL_ATTRIBUTE_NOINLINE
void analyse() {
    BABYLON_LOG(INFO) << TypeId<T>().ID << " size " << sizeof(T);
    BABYLON_LOG(INFO) << TypeId<T>().ID << " is trivially constructible? " << ::std::is_trivially_constructible<T>::value;
    BABYLON_LOG(INFO) << TypeId<T>().ID << " is trivially destructible? " << ::std::is_trivially_destructible<T>::value;
}

template <typename T>
ABSL_ATTRIBUTE_NOINLINE
void perf_construct_destruct() {
    ::std::vector<T> sv;
    auto num = ::absl::GetFlag(FLAGS_num);
    auto times = ::absl::GetFlag(FLAGS_times);
    sv.reserve(num);
    int64_t cused = 0;
    int64_t dused = 0;
    for (size_t i = 0; i < times; ++i) {
        auto begin = ::absl::GetCurrentTimeNanos();
        for (size_t i = 0; i < num; ++i) {
            sv.emplace_back();
        }
        auto end = ::absl::GetCurrentTimeNanos();
        cused += end - begin;

        begin = ::absl::GetCurrentTimeNanos();
        sv.clear();
        end = ::absl::GetCurrentTimeNanos();
        dused += end - begin;
    }
    BABYLON_LOG(INFO) << TypeId<T>().ID << " default construct / destruct use "
        << cused * 1.0 / times / num
        << " / " << dused * 1.0 / times / num;
}

template <typename T, typename U>
ABSL_ATTRIBUTE_NOINLINE
void perf_construct_destruct_with_value(const U& value) {
    ::std::vector<T> sv;
    auto num = ::absl::GetFlag(FLAGS_num);
    auto times = ::absl::GetFlag(FLAGS_times);
    sv.reserve(num);
    int64_t cused = 0;
    int64_t dused = 0;
    for (size_t i = 0; i < times; ++i) {
        auto begin = ::absl::GetCurrentTimeNanos();
        for (size_t i = 0; i < num; ++i) {
            sv.emplace_back(value);
        }
        auto end = ::absl::GetCurrentTimeNanos();
        cused += end - begin;

        begin = ::absl::GetCurrentTimeNanos();
        sv.clear();
        end = ::absl::GetCurrentTimeNanos();
        dused += end - begin;
    }
    BABYLON_LOG(INFO) << TypeId<T>().ID << " with " << TypeId<U>().ID
        << " construct / destruct use "
        << cused * 1.0 / times / num
        << " / " << dused * 1.0 / times / num;
}

template <typename T>
inline T* get_from_any(::std::any& any) noexcept {
    return &::std::any_cast<T&>(any);
}

template <typename T>
inline T* get_from_any(Any& any) noexcept {
    return any.get<T>();
}

template <typename T>
inline T* get_from_any(T& any) noexcept {
    return &any;
}

template <typename T, typename U>
ABSL_ATTRIBUTE_NOINLINE
void perf_get_pointer(const U& value) {
    ::std::vector<T> sv;
    auto num = ::absl::GetFlag(FLAGS_num);
    auto times = ::absl::GetFlag(FLAGS_times);
    sv.reserve(num);
    for (size_t i = 0; i < num; ++i) {
        sv.emplace_back(value);
    }
    int64_t used = 0;
    uint64_t sum = 0;
    for (size_t i = 0; i < times; ++i) {
        auto begin = ::absl::GetCurrentTimeNanos();
        for (size_t i = 0; i < num; ++i) {
            sum += (uint64_t)get_from_any<U>(sv[i]);
        }
        auto end = ::absl::GetCurrentTimeNanos();
        used += end - begin;
    }
    BABYLON_LOG(DEBUG) << sum;
    BABYLON_LOG(INFO) << TypeId<T>().ID << " with " << TypeId<U>().ID
        << " get pointer use "
        << used * 1.0 / times / num;
}

struct S {
    inline S(const ::std::string& o) noexcept :
        s {new ::std::string{o}} {}

    ::std::unique_ptr<::std::string> s;
};

int main(int argc, char** argv) {
    ::absl::ParseCommandLine(argc, argv);

    {
        auto cpu = ::sched_getcpu();
        ::cpu_set_t set;
        CPU_ZERO(&set);
        CPU_SET(cpu, &set);
        auto ret = ::sched_setaffinity(0, sizeof(set), &set);
        BABYLON_LOG(INFO) << "bind to cpu " << cpu << " ret " << ret;
    }

    analyse<::std::any>();
    analyse<Any>();
    analyse<::std::tuple<void*, void*>>();
    analyse<::std::shared_ptr<::std::string>>();
    analyse<::std::string>();
    analyse<::std::list<::std::string>>();
    analyse<::std::string*>();

    while (true) {
        BABYLON_LOG(INFO) << "=====================================================";
        perf_construct_destruct<::std::any>();
        perf_construct_destruct<::babylon::Any>();
        perf_construct_destruct<::std::tuple<void*, void*>>();

        BABYLON_LOG(INFO) << "=====================================================";
        perf_construct_destruct_with_value<::std::any>((uint64_t)0);
        perf_construct_destruct_with_value<::babylon::Any>((uint64_t)0);
        perf_construct_destruct_with_value<::std::tuple<void*, void*>>(::std::tuple<void*, void*>(nullptr, nullptr));

        BABYLON_LOG(INFO) << "=====================================================";
        perf_construct_destruct_with_value<::std::any>((void*)0);
        perf_construct_destruct_with_value<::babylon::Any>((void*)0);
        perf_construct_destruct_with_value<::std::tuple<void*, void*>>(::std::tuple<void*, void*>(nullptr, nullptr));

        BABYLON_LOG(INFO) << "=====================================================";
        perf_construct_destruct_with_value<::std::any>(::std::string("10086"));
        perf_construct_destruct_with_value<::babylon::Any>(::std::string("10086"));
        perf_construct_destruct_with_value<::std::tuple<void*, S>>(::std::make_tuple(nullptr, ::std::string("10086")));

        BABYLON_LOG(INFO) << "=====================================================";
        perf_get_pointer<::std::any>((uint64_t)0);
        perf_get_pointer<Any>((uint64_t)0);
        perf_get_pointer<uint64_t>((uint64_t)0);

        BABYLON_LOG(INFO) << "=====================================================";
        perf_get_pointer<::std::any>((void*)0);
        perf_get_pointer<Any>((void*)0);
        perf_get_pointer<void*>((void*)0);

        BABYLON_LOG(INFO) << "=====================================================";
        perf_get_pointer<::std::any>(::std::string("10086"));
        perf_get_pointer<Any>(::std::string("10086"));
        perf_get_pointer<::std::string>(::std::string("10086"));

        usleep(1000000);
    }
    return 0;
}
