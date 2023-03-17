/**
 * \author Dustin Roden
 * \date 08/24/22
 * \brief Provides a helper for binding to events in classes. Use lambdas to capture any calls.
 */

#pragma once

#include <functional>
#include <map>
#include <mutex>

class Event
{
	std::map<const wchar_t*, std::function<void()>> Subscribers;
	std::mutex NotifyMux;

public:
	// TODO: make this a variable list of params.
	Event();

	bool IsSubscribed(const wchar_t* FuncName);
	void Subscribe(const wchar_t* FuncName, const std::function<void()>& Func);
	void Notify(); // TODO: Notify with variable list of params matching function signature.
	void Unsubscribe(const wchar_t* FuncName);
};

