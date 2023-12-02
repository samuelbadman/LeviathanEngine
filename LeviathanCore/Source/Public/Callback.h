#pragma once

namespace LeviathanCore
{
	template<typename CallbackType>
	class Callback
	{
	private:
		std::vector<CallbackType> Callbacks = {};

	public:
		void Register(CallbackType callback)
		{
			Callbacks.emplace_back(callback);
		}

		void Deregister(CallbackType callback)
		{
			Callbacks.erase(std::remove(Callbacks.begin(), Callbacks.end(), callback));
		}

		template<typename... Args>
		void Call(Args... args)
		{
			const size_t callbackCount = Callbacks.size();
			for (size_t i = 0; i < callbackCount; ++i)
			{
				Callbacks[i](args...);
			}
		}
	};
}