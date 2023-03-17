#include "Event.h"

Event::Event() = default;

bool Event::IsSubscribed(const wchar_t* FuncName) {
	const auto _ = Subscribers.find(FuncName);
	return _ != Subscribers.end();
}

void Event::Subscribe(const wchar_t* FuncName, const std::function<void()>& Func) {
	if(IsSubscribed(FuncName)) return;

	Subscribers.insert(std::pair<const wchar_t*, std::function<void()>>(FuncName, Func));
}

void Event::Notify() {
	std::unique_lock<std::mutex> lock(NotifyMux);

	for (auto i = Subscribers.begin(); i != Subscribers.end(); ++i) {
		i->second();
	}
}

void Event::Unsubscribe(const wchar_t* FuncName) {
	const auto _ = Subscribers.find(FuncName);
	Subscribers.erase(_);
}
